/*
 * Copyright 2020 Leandro José Britto de Oliveira
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
 * @brief Monitor API
 */
#pragma once

#include <cms/defs.h>

#define CMS_MONITOR_ALL_EVENTS UINT8_MAX

/**
 * @brief Causes the current task to wait upon events on given monitor.
 * 
 * @note current task function will return immediately.
 * 
 * @param _monitor monitor. Passing NULL implies a delay (task will wake up only after given interval expired).
 * @param _events waiting events (will be ignored if 'monitor' is NULL).
 * @param _millis maximum timeout in milliseconds on current task waiting for events/delay on given monitor.
 * @param _allEvents defines if either all events at once, or any of given events is a valid condition (will be 
 * ignored if 'monitor' is NULL).
 */
#define cms_monitor_wait(_monitor, _events, _millis, _allEvents) \
{ __cms_monitor_wait__(_monitor, _events, _millis, _allEvents); return; }

#ifdef __cplusplus
extern "C" {
#endif

/** Event bit flags. */
typedef uint8_t cms_events_t;

typedef struct CmsMonitor CmsMonitor;

/**
 * @internal
 * @brief Flags the current task to wait upon events on given monitor.
 * 
 * @note this function shall not be called directly. Instead, use macro {@link cms_monitor_wait()}.
 * 
 * @param monitor monitor. Passing NULL implies a delay (task will wake up only after given interval expired).
 * @param events waiting events (will be ignored if 'monitor' is NULL).
 * @param millis maximum timeout in milliseconds on current task waiting for events/delay on given monitor.
 * @param allEvents defines if either all events at once, or any of given events is a valid condition (will be 
 * ignored if 'monitor' is NULL).
 */
void __cms_monitor_wait__(CmsMonitor* monitor, cms_events_t events, uint64_t millis, bool allEvents);

/** 
 * @brief Represents a monitor.
 * 
 * A monitor is an entity used for synchronization among multiple tasks trough its internal event bitmask.
 */
struct CmsMonitor {
    /** Currently flagged events. */
    cms_events_t events;
};

/**
 * @brief Notifies all tasks waiting for events on given monitor.
 * 
 * @param monitor monitor.
 * @param events events to be set in given monitor.
 * @param append defines if monitor events are preserved and new ones (defined in 'events') will be appended. Passing
 * false will force monitor to have exactly given events.
 */
void cms_monitor_notify(CmsMonitor* monitor, cms_events_t events, bool append);

/**
 * @brief Clears events in given monitor.
 * 
 * @param monitor monitor.
 * @param events events in monitor which shall be cleared.
 */
void cms_monitor_clear_events(CmsMonitor* monitor, cms_events_t events);

/**
 * @brief Check if given events are present on given monitor.
 * 
 * @param monitor monitor.
 * @param events events to be identified on given monitor (either all or any).
 * @param allEvents defines if all events shall match at once (true) or if any of given events is enough (false).
 * @param clear defines if 'events' shall be cleared from monitor if this function returns true.
 * 
 * @return a boolean indicating if either 1) one or more events were matched, or 2) all events matched at once in given
 * monitor.
 */
bool cms_monitor_check_events(CmsMonitor* monitor, cms_events_t events, bool allEvents, bool clear);

#ifdef __cplusplus
} // extern "C"
#endif
