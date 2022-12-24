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
 * @brief Cooperative multitasking scheduler API.
 */
#pragma once

#include "monitor.h"

/**
 * @brief Represents a task function.
 *
 * The task function may be called multiple times by scheduler at
 * appropriate moments.
 *
 * @note Since the scheduler is cooperative, real-time achievement is up to
 *       application implementation.
 *
 * @param data Data defined during task creation.
 */
typedef void (*cms_task_fn)(void* data);

/**
 * @brief Represents a destructor function.
 *
 * @param data Data to be destroyed.
 */
typedef void (*cms_destructor_fn)(void* data);

/** @brief Task handler. */
typedef struct _cms_scheduler_task cms_task_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Adds a new task to the scheduler.
 *
 * Although tasks can added to a scheduler at any moment, it's recommended to
 * register tasks during application startup in order to prevent heap
 * fragmentation.
 *
 * @param taskFn Pointer to task function.
 *
 * @param taskData Optional data which will be passed to given task function
 *        on each call.
 *
 * @param destructor Optional function to be called when task is being
 *        destroyed in order to release allocated resources (usually
 *        {@code taskData}).
 *
 * @return Created task handler or {@code NULL} on failure.
 */
cms_task_t* cms_scheduler_create_task(
	cms_task_fn taskFn,
	void* taskData,
	cms_destructor_fn destructor
);

/**
 * @brief Starts the scheduler.
 *
 * A scheduler is intended to execute indefintely until a
 * {@link cms_scheduler_stop signal was emitted}.
 *
 * When a scheduler finishes its execution, all assciated tasks will be
 * destroyed releasing all allocated resources.
 *
 * @return {@code true} if scheduler was started and was stopped (NOTE:
 *         This function will not return until {@link cms_scheduler_stop()} was
 *         called). If schedule is already running or scheduler has no
 *         associated tasks, this function will do nothing and will
 *         {@code false}.
 */
bool cms_scheduler_start();

/**
 * @brief Returns the active task.
 *
 * @return Active task handler. If scheduler is not running, returns
 *         {@code NULL}.
 */
cms_task_t* cms_scheduler_get_active_task();

/**
 * @brief Stops scheduler.
 *
 * @param now Defines if scheduler shall stop immediately.
 */
void cms_scheduler_stop(bool now);

#ifdef __cplusplus
} // extern "C"
#endif
