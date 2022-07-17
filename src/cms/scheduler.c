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

static _cms_scheduler_t __scheduler = {
	.first          = NULL,
	.last           = NULL,
	.activeTaskNode = NULL,
	.running        = false
};

static bool __is_task_ready(cms_task_t* task) {
	bool ready;

	if (!task->waiting) {
		ready = true;
	} else {
		uint64_t now = cms_system_timestamp();

		// Check for delay/wait timeout:
		ready = (now - task->waitTimestamp) >= task->waitTimeout;

		// Check for notifications:
		if (!ready && task->monitor != NULL && task->waitEvents != 0)
			ready = cms_monitor_check_events(task->monitor, task->waitEvents, task->allEvents, false);
	}

	if (ready) {
		task->waiting = false;
		task->monitor = NULL;
		task->waitTimestamp = 0;
		task->waitTimeout = 0;
		task->waitEvents = 0;
		task->allEvents = false;
	}

	return ready;
}

static void __do_cleanup() {
	_cms_scheduler_task_node_t* taskNode = _scheduler->first;
	while (taskNode != NULL) {
		if (taskNode->task->destructor)
			taskNode->task->destructor(taskNode->task->data);

		free(taskNode->task);
		_cms_scheduler_task_node_t* nextNode = taskNode->next;
		free(taskNode);
		taskNode = nextNode;
	}

	_scheduler->first          = NULL;
	_scheduler->last           = NULL;
	_scheduler->activeTaskNode = NULL;
}

_cms_scheduler_t* _scheduler = &__scheduler;

PUBLIC cms_task_t* CALL cms_scheduler_create_task (cms_task_fn taskFn, void* taskData, cms_destructor_fn destructor) {
	if (taskFn == NULL)
		return NULL;

	cms_task_t* task = malloc(sizeof(cms_task_t));

	if (task == NULL)
		return NULL;

	task->waiting = false;
	task->monitor = NULL;
	task->waitTimestamp = 0;
	task->waitTimeout = 0;
	task->waitEvents = 0;
	task->allEvents = false;
	task->taskFn = taskFn;
	task->data = taskData;
	task->destructor = destructor;

	_cms_scheduler_task_node_t* taskNode = malloc(sizeof(_cms_scheduler_task_node_t));
	if (taskNode == NULL) {
		free(task);
		return NULL;
	}

	taskNode->next = NULL;
	taskNode->task = task;

	if (_scheduler->last == NULL) {
		// Adding first node
		_scheduler->first = taskNode;
	} else {
		// Appending a new node
		_scheduler->last->next = taskNode;
	}

	_scheduler->last = taskNode;

	return task;
}

PUBLIC bool CALL cms_scheduler_start() {
	if (_scheduler->running || _scheduler->first == NULL)
		return false;

	_scheduler->running = true;

	_cms_scheduler_task_node_t* activeTaskNode = _scheduler->first;

	while(_scheduler->running) {
		_scheduler->activeTaskNode = activeTaskNode;
		cms_task_t* mActiveTask = _scheduler->activeTaskNode->task;

		if (__is_task_ready(mActiveTask)) {
			if (setjmp(_scheduler->jmpBuf) == 0) {
				mActiveTask->taskFn(mActiveTask->data);
			}
		}

		activeTaskNode = activeTaskNode->next;

		if (activeTaskNode == NULL && _scheduler->running) {
			// End of scheduler cycle, adjust state for next cycle...
			activeTaskNode = _scheduler->first;

			// Allows system to process extern events...
			cmd_system_process_events();
		}
	}

	__do_cleanup();

	return true;
}

PUBLIC cms_task_t* CALL cms_scheduler_get_active_task() {
	return _scheduler->activeTaskNode == NULL ? NULL : _scheduler->activeTaskNode->task;
}

PUBLIC void CALL cms_scheduler_stop(bool now) {
	if (!_scheduler->running)
		return;

	_scheduler->running = false;

	if (now)
		longjmp(_scheduler->jmpBuf, 1);
}
