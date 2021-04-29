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

/**
 * @file
 * @brief Scheduler API internal/private data.
 */
#pragma once

#include <cms/scheduler.h>

#include <setjmp.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Task data. */
typedef struct _CmsSchedulerTask _CmsSchedulerTask;

/** @brief Task data. */
struct _CmsSchedulerTask {
	/** @brief Internal monitor. */
	CmsMonitor internalMonitor;

	/** @brief Is task waiting? */
	bool waiting;

	/** @brief Monitor on which this task is waiting. */
    CmsMonitor*  monitor;

    /** @brief Timestamp when wait was requested for this task. */
    uint64_t waitTimestamp;

    /** @brief Maximum waiting timeout. */
    uint64_t waitTimeout;

    /** @brief Events which may wake up this task. */
    cms_events_t waitEvents;

    /**
     * @brief Defines if all events shall match at once in order to wake up
     * this task.
     */
    bool allEvents;

    /** @brief Task function. */
    CmsTaskFn taskFn;

    /** @brief Task state. */
    void* taskState;
};

/** @brief Node in a task list. */
typedef struct _CmsSchedulerTaskNode _CmsSchedulerTaskNode;

/** @brief Node in a task list. */
struct _CmsSchedulerTaskNode {
	/** @brief Next task in the list. */
	_CmsSchedulerTaskNode* next;

    /** @brief Task data. */
    _CmsSchedulerTask* task;
};

/** @brief Scheduler state. */
typedef enum _CmsSchedulerState {
	CMS_SCHEDULER_STATE_STOPPED = 0,
	CMS_SCHEDULER_STATE_RUNNING = 1,
	CMS_SCHEDULER_STATE_IDLE = 2
}_CmsSchedulerState;

// Documented in public header
struct CmsScheduler {
	/** @brief Scheduler state. */
	_CmsSchedulerState state;

	/** @brief Idle task function. */
	CmsTaskFn idleTaskFn;

	/** @brief Idle task state. */
	void* idleTaskState;

	/** @brief First task node in the list. */
	_CmsSchedulerTaskNode* first;

	/** @brief Last task node in the list. */
	_CmsSchedulerTaskNode* last;

    /** @brief Scheduler jump context. */
    jmp_buf jmpBuf;

    /** @brief Active task for this scheduler. */
    _CmsSchedulerTaskNode* activeTaskNode;
};

/** @brief Active scheduler. */
extern CmsScheduler* _activeScheduler;

#ifdef __cplusplus
} // extern "C"
#endif
