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
 * @brief System-specific API.
 */
#pragma once

#include <cms/defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns system timestamp in milliseconds.
 *
 * @note The granularity of the value depends on the underlying system and may
 * be larger than a millisecond.
 *
 * @return System timestamp in milliseconds. */
uint64_t cms_system_timestamp();

#ifdef __cplusplus
} // extern "C"
#endif
