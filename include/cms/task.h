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
 * @brief Task API.
 */
#pragma once

#include <cms/monitor.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Causes the current task to wait for at least given interval.
 *
 * @note Current task function will return immediately to the active scheduler.
 *
 * @note Calling this function from an idle task ignores the interval and
 * yields to the scheduler (see {@link cms_monitor_wait()}.
 *
 * @param millis Delay interval in milliseconds. Passing zero is equivalent
 * to {@link cms_task_yield()}.
 */
void cms_task_delay(uint64_t millis);

/** @brief Causes the current task function to return the active scheduler. */
void cms_task_yield();

/**
 * @brief Returns the internal monitor associated with active task.
 *
 * @return The internal monitor associated with active task.
 */
CmsMonitor* cms_task_get_monitor();

#ifdef __cplusplus
} // extern "C"
#endif
