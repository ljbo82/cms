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
 * @note Current task function will return immediately to the scheduler.
 *
 * @param millis Delay interval in milliseconds. Passing zero is equivalent
 *        to {@link cms_task_yield()}.
 */
DLL_PUBLIC void cms_task_delay(
	uint64_t millis
);

/** @brief Causes the current task function to return to the scheduler. */
DLL_PUBLIC void cms_task_yield();

/**
 * @brief Returns the internal monitor associated with active task.
 *
 * @return The internal monitor associated with active task.
 */
DLL_PUBLIC cms_monitor_t* cms_task_get_monitor();

#ifdef __cplusplus
} // extern "C"
#endif
