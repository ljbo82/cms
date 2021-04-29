/*
    Copyright 2021 Leandro José Britto de Oliveira

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
 */

#include "scheduler_private.h"

#include <cms/system.h>

#include <stdlib.h>

static _CmsSchedulerTask* __cms_scheduler_create_task(CmsTaskFn taskFn, void* state) {
	if (taskFn == NULL)
		return NULL;

	_CmsSchedulerTask* mNewTask = malloc(sizeof(_CmsSchedulerTask));
	mNewTask->internalMonitor.events = 0;
	mNewTask->waiting = false;
	mNewTask->monitor = NULL;
	mNewTask->waitTimestamp = 0;
	mNewTask->waitTimeout = 0;
	mNewTask->waitEvents = 0;
	mNewTask->allEvents = false;
	mNewTask->taskFn = taskFn;
	mNewTask->taskState = state;

	return mNewTask;
}

static bool __cms_scheduler_check_task_ready(_CmsSchedulerTask* task) {
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

CmsScheduler* cms_scheduler_create(CmsTaskFn idleTaskFn, void* idleTaskState) {
	CmsScheduler* mScheduler = malloc(sizeof(CmsScheduler));
	mScheduler->state = CMS_SCHEDULER_STATE_STOPPED;
	mScheduler->activeTaskNode = NULL;
	mScheduler->first = NULL;
	mScheduler->last = NULL;
	mScheduler->idleTaskFn = idleTaskFn;
	mScheduler->idleTaskState = idleTaskState;

	return mScheduler;
}

CmsMonitor* cms_scheduler_add_task(CmsScheduler* scheduler, CmsTaskFn taskFn, void* taskState) {
	if (scheduler == NULL || taskFn == NULL)
		return NULL;

	_CmsSchedulerTask* mTask = __cms_scheduler_create_task(taskFn, taskState);

	_CmsSchedulerTaskNode* mNode = malloc(sizeof(_CmsSchedulerTaskNode));
	mNode->next = NULL;
	mNode->task = mTask;

	if (scheduler->last == NULL) {
		// Adding first node
		scheduler->first = mNode;
	} else {
		// Appending a new node
		scheduler->last->next = mNode;
	}

	scheduler->last = mNode;

	return &mTask->internalMonitor;
}

void cms_scheduler_destroy(CmsScheduler* scheduler) {
	if (scheduler == NULL)
		return;

	_CmsSchedulerTaskNode* mNode = scheduler->first;
	while (mNode != NULL) {
		free(mNode->task);
		_CmsSchedulerTaskNode* mNextNode = mNode->next;
		free(mNode);
		mNode = mNextNode;
	}
}

void cms_scheduler_start(CmsScheduler* scheduler) {
	if (scheduler == NULL || scheduler->first == NULL || scheduler->state != CMS_SCHEDULER_STATE_STOPPED)
		return;

	CmsScheduler* mActiveSchedulerBackup = _activeScheduler;

	_activeScheduler = scheduler;
	_activeScheduler->state = CMS_SCHEDULER_STATE_RUNNING;

	_CmsSchedulerTaskNode* mActiveTaskNode = _activeScheduler->first;
	bool mFoundReadyTask = false;

	while(_activeScheduler->state == CMS_SCHEDULER_STATE_RUNNING) {
		_activeScheduler->activeTaskNode = mActiveTaskNode;
		_CmsSchedulerTask* mActiveTask = _activeScheduler->activeTaskNode->task;

		if (__cms_scheduler_check_task_ready(mActiveTask)) {
			mFoundReadyTask = true;
			if (setjmp(_activeScheduler->jmpBuf) == 0) {
				mActiveTask->taskFn(mActiveTask->taskState);
			} else {
				// Scheduler was stopped or task was put to wait...
				if (_activeScheduler->state == CMS_SCHEDULER_STATE_STOPPED) {
					break; // <-- Prevents idle task from being called when scheduler was stopped.
				}
			}
		}

		mActiveTaskNode = mActiveTaskNode->next;

		if (mActiveTaskNode == NULL) {
			// End of scheduler cycle...
			if (!mFoundReadyTask) {
				// Idle cycle detected...
				if (_activeScheduler->idleTaskFn != NULL) {
					_activeScheduler->state = CMS_SCHEDULER_STATE_IDLE; // <-- Setting idle state makes sense only when there is an idle task

					if (setjmp(_activeScheduler->jmpBuf) == 0) {
						_activeScheduler->idleTaskFn(_activeScheduler->idleTaskState);
						_activeScheduler->state = CMS_SCHEDULER_STATE_RUNNING;
					} else {
						// Idle task yielded or stopped the scheduler...
						if (_activeScheduler->state == CMS_SCHEDULER_STATE_STOPPED) {
							// Idle task stopped the scheduler...
							break;
						} else {
							// Idle task yielded...
							_activeScheduler->state = CMS_SCHEDULER_STATE_RUNNING;
						}
					}
				}
			}

			// Adjust state for next cycle...
			mFoundReadyTask = false;
			mActiveTaskNode = _activeScheduler->first;
		}
	}

	// Scheduler is stopping...
	_activeScheduler = mActiveSchedulerBackup; // <-- Restores previous active scheduler.
}

void cms_scheduler_stop() {
	if (_activeScheduler == NULL || _activeScheduler->state == CMS_SCHEDULER_STATE_STOPPED)
		return;

	_activeScheduler->state = CMS_SCHEDULER_STATE_STOPPED;
	longjmp(_activeScheduler->jmpBuf, 1);
}
