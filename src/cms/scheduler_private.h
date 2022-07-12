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

/**
 * @file
 * @brief Scheduler API internal/private data.
 */
#pragma once

#include "defs_private.h"

#include <cms/scheduler.h>

#include <setjmp.h>

/** @brief Node in a task list. */
typedef struct _cms_scheduler_task_node _cms_scheduler_task_node_t;

/** @brief Represents a scheduler. */
typedef struct _cms_scheduler _cms_scheduler_t;

/** @brief Represents a scheduler task. */
struct _cms_scheduler_task {
	/** @brief Is task waiting? */
	bool waiting;

	/** @brief Monitor on which this task is waiting. */
	cms_monitor_t* monitor;

	/** @brief Timestamp when wait was requested for this task. */
	uint64_t waitTimestamp;

	/** @brief Maximum waiting timeout. */
	uint64_t waitTimeout;

	/** @brief Events which may wake up this task. */
	cms_events_t waitEvents;

	/**
	 * @brief Defines if all events shall match at once in order to wake up
	 *        this task.
	 */
	bool allEvents;

	/** @brief Task function. */
	cms_task_fn taskFn;

	/** @brief Task data. */
	void* data;

	/** @brief Destructor function. */
	cms_destructor_fn destructor;
};

/** @brief Node in a task list. */
struct _cms_scheduler_task_node {
	/** @brief Next task in the list. */
	_cms_scheduler_task_node_t* next;

	/** @brief Task data. */
	cms_task_t* task;
};

/** @brief Represents a scheduler. */
struct _cms_scheduler {
	/** @brief First task node in the list. */
	_cms_scheduler_task_node_t* first;

	/** @brief Last task node in the list. */
	_cms_scheduler_task_node_t* last;

	/** @brief Scheduler jump context. */
	jmp_buf jmpBuf;

	/** @brief Active task for this scheduler. */
	_cms_scheduler_task_node_t* activeTaskNode;

	/** @brief Defines if the scheduler is running. */
	bool running;
};

/** @brief Active scheduler. */
extern PRIVATE _cms_scheduler_t* _scheduler;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} // extern "C"
#endif
