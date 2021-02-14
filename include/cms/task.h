/*
 * Copyright 2021 Leandro José Britto de Oliveira
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file
 * @brief Task API.
 */
#pragma once

#include <cms/defs.h>
#include <cms/monitor.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Represents a task function.
 * 
 * The task function will be called multiple times by scheduler at appropriate moments.
 * 
 * @note The scheduler DOES NOT ensure real time.
 *
 * @param state state that is preserved across multiple calls of this function.
 */
typedef void (*CmsTaskFn)(void* state);

typedef struct CmsTask CmsTask;

/** 
 * @brief Task handler.
 * 
 * @note A pointer to CmsTask can be safely cast to a pointer to {@link CmsMonitor}.
 */
struct CmsTask {
    CmsMonitor monitor;
    void* __data;
};

/** 
 * @brief Creates a task handler an adds it to the scheduler.
 * 
 * @param taskFn task function which will be called multiple times by scheduler.
 * @param state state that is preserved across multiple calls of this function.
 * 
 * @note The scheduler DOES NOT ensure real time.
 */
CmsTask* cms_task_create(CmsTaskFn taskFn, void* state);

/**
 * @brief Starts the scheduler.
 * 
 * @note Once started, scheduler never returns.
 */
void cms_task_start_scheduler();

/**
 * Causes the current task to wait for at least given interval.
 *
 * @note current task function will return immediately.
 *
 * @param millis delay interval in milliseconds.
 */
void cms_task_delay(uint64_t millis);

#ifdef __cplusplus
} // extern "C"
#endif
