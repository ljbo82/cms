/*
Copyright (c) 2022 Leandro José Britto de Oliveira

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "scheduler_private.h"

#include <cms/system.h>

#include <stdlib.h>

static bool __is_task_ready(_cms_scheduler_task_t* task) {
	bool mWakeUp;

	if (!task->waiting) {
		mWakeUp = true;
	} else {
		uint64_t mNow = cms_system_timestamp();

		// Check for delay/wait timeout:
		mWakeUp = (mNow - task->waitTimestamp) >= task->waitTimeout;

		// Check for notifications:
		if (!mWakeUp && task->monitor != NULL && task->waitEvents != 0)
			mWakeUp = cms_monitor_check_events(task->monitor, task->waitEvents, task->allEvents, false);
	}

	if (mWakeUp) {
		task->waiting = false;
		task->monitor = NULL;
		task->waitTimestamp = 0;
		task->waitTimeout = 0;
		task->waitEvents = 0;
		task->allEvents = false;
	}

	return mWakeUp;
}

static void __do_cleanup() {
	_cms_scheduler_task_node_t* mNode = _scheduler->first;
	while (mNode != NULL) {
		if (mNode->task->destructor)
			mNode->task->destructor(mNode->task->data);

		free(mNode->task);
		_cms_scheduler_task_node_t* mNextNode = mNode->next;
		free(mNode);
		mNode = mNextNode;
	}

	_scheduler->first          = NULL;
	_scheduler->last           = NULL;
	_scheduler->activeTaskNode = NULL;
}

DLL_PUBLIC cms_monitor_t* cms_scheduler_create_task (cms_task_fn taskFn, void* taskData, cms_destructor_fn destructor) {
	_cms_scheduler_task_t* mTask = malloc(sizeof(_cms_scheduler_task_t));
	if (taskFn == NULL)
		return NULL;

	mTask->internalMonitor.events = 0;
	mTask->waiting = false;
	mTask->monitor = NULL;
	mTask->waitTimestamp = 0;
	mTask->waitTimeout = 0;
	mTask->waitEvents = 0;
	mTask->allEvents = false;
	mTask->taskFn = taskFn;
	mTask->data = taskData;
	mTask->destructor = destructor;

	_cms_scheduler_task_node_t* mNode = malloc(sizeof(_cms_scheduler_task_node_t));
	mNode->next = NULL;
	mNode->task = mTask;

	if (_scheduler->last == NULL) {
		// Adding first node
		_scheduler->first = mNode;
	} else {
		// Appending a new node
		_scheduler->last->next = mNode;
	}

	_scheduler->last = mNode;

	return &mTask->internalMonitor;
}

DLL_PUBLIC bool cms_scheduler_start() {
	if (_scheduler->running || _scheduler->first == NULL)
		return false;

	_scheduler->running = true;

	_cms_scheduler_task_node_t* mActiveTaskNode = _scheduler->first;

	while(_scheduler->running) {
		_scheduler->activeTaskNode = mActiveTaskNode;
		_cms_scheduler_task_t* mActiveTask = _scheduler->activeTaskNode->task;

		if (__is_task_ready(mActiveTask)) {
			if (setjmp(_scheduler->jmpBuf) == 0) {
				mActiveTask->taskFn(mActiveTask->data);
			}
		}

		mActiveTaskNode = mActiveTaskNode->next;

		if (mActiveTaskNode == NULL) {
			// End of scheduler cycle, adjust state for next cycle...
			mActiveTaskNode = _scheduler->first;

			// Allows system to process extern events...
			cmd_system_process_events();
		}
	}

	__do_cleanup();

	return true;
}

DLL_PUBLIC void cms_scheduler_stop() {
	_scheduler->running = false;
}
