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
 * @brief Cooperative multitasking scheduler API.
 */
#pragma once

#include <cms/monitor.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Represents a task function.
 *
 * The task function may be called multiple times by scheduler at
 * appropriate moments.
 *
 * @note Since the scheduler is cooperative, real-time achievement is up to
 * application implementation.
 *
 * @param state state that is preserved across multiple calls of this function.
 */
typedef void (*CmsTaskFn)(void* state);

/** Represents a scheduler. */
typedef struct CmsScheduler CmsScheduler;

/**
 * @brief Creates a cooperative scheduler.
 *
 * @note Although a scheduler can be created at any moment, it's recommended to
 * create one during application startup in order to prevent heap fragmentation.
 *
 * @note Usually a scheduler never stops once started. If a implementation
 * requires a scheduler to stop (through a call to {@link cms_scheduler_stop()},
 * and scheduler is no longer required, it must be destroyed (through a call to
 * {@link cms_scheduler_destroy()} in order to release allocated resources.
 *
 * @note Although multiples schedulers can be created, only one can be active
 * at a time.
 *
 * @param idleTaskfn Optional pointer to a function to be
 * called when scheduler is idle (i.e. there is no task ready to be called
 * during a scheduler cycle.
 *
 * @param idleTaskState Optional pointer to data which will be passed to idle
 * task on each call.
 *
 * @return Handler for created scheduler.
 */
CmsScheduler* cms_scheduler_create(CmsTaskFn idleTaskfn, void* idleTaskState);

/**
 * @brief Adds a task to a scheduler.
 *
 * Although tasks can added to a scheduler at any moment, it's recommended to
 * register tasks during application startup in order to prevent heap
 * fragmentation.
 *
 * @param scheduler Associated scheduler.
 *
 * @param taskFn Pointer to task function.
 *
 * @param taskState Optional pointer to data which will be passed to given task
 * on each call.
 *
 * @return Task internal monitor.
 */
CmsMonitor* cms_scheduler_add_task(CmsScheduler* scheduler, CmsTaskFn taskFn,
	void* taskState);

/**
 * @brief Destroys a scheduler.
 *
 * This function will release any allocated resources during scheduler creation.
 *
 * @param scheduler Handler for a scheduler.
 */
void cms_scheduler_destroy(CmsScheduler* scheduler);

/**
 * @brief Starts a scheduler.
 *
 * @note this function will not return until {@link cms_scheduler_stop()} is
 * called by either a {@link task.h task} or an {@link CmsSchedulerIdleFn idle
 * function}.
 *
 * @param scheduler Handler for the scheduler to be started.
 */
void cms_scheduler_start(CmsScheduler* scheduler);

/**
 * @brief Stops active scheduler.
 *
 * @note Current task will return immediately.
 */
void cms_scheduler_stop();

#ifdef __cplusplus
} // extern "C"
#endif
