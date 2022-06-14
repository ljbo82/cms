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
 * @brief System-specific API.
 */
#pragma once

#include <cms/defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns the system timestamp in milliseconds.
 *
 * @note The granularity of the value depends on the underlying system and may
 *       be larger than a millisecond.
 *
 * @return System timestamp in milliseconds. */
DLL_PUBLIC uint64_t cms_system_timestamp();

/**
 * @brief Process system events.
 *
 * This function will be called periodically by the scheduler in order to
 * allow external events to be processed.
 */
DLL_PUBLIC void cmd_system_process_events();

#ifdef __cplusplus
} // extern "C"
#endif
