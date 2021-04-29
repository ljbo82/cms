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
 * @brief Monitor API
 */
#pragma once

#include <cms/defs.h>

#define CMS_MONITOR_ALL_EVENTS UINT8_MAX

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Event bit flags. */
typedef uint8_t cms_events_t;

typedef struct CmsMonitor CmsMonitor;

/**
 * @brief Represents a monitor.
 *
 * A monitor is an entity used for synchronization among multiple tasks trough
 * its internal event bitmask.
 */
struct CmsMonitor {
    /** @brief Currently flagged events. */
    cms_events_t events;
};

/**
 * @brief Flags the current task to wait upon events on given monitor.
 * 
 * @note Current task function will return immediately to the active scheduler.
 * 
 * @note Calling this function from an idle task is equivalent to call
 * {@link cms_task_yield()} because it does not make sense to put idle tasks
 * to wait for either a monitor or a delay (idle tasks are called only when
 * there is no ready tasks in a scheduler cycle).
 *
 * @param monitor Monitor. Passing NULL implies a delay (task will wake up
 * only after given interval expired. If given interval is zero task yields).
 *
 * @param events Waiting events (will be ignored if 'monitor' is NULL).
 *
 * @param millis Maximum timeout in milliseconds on current task waiting for
 * events/delay on given monitor.
 *
 * @param allEvents Defines if either all events at once, or any of given
 * events is a valid condition (will be ignored if 'monitor' is NULL).
 */
void cms_monitor_wait(CmsMonitor* monitor, cms_events_t events,
	uint64_t millis, bool allEvents
);

/**
 * @brief Notifies all tasks waiting for events on given monitor.
 * 
 * @param monitor Monitor.
 *
 * @param events Events to be set in given monitor.
 *
 * @param append Defines if monitor events are preserved and new ones
 * (defined in 'events') will be appended. Passing false will force monitor
 * to have exactly given events.
 */
void cms_monitor_notify(CmsMonitor* monitor, cms_events_t events, bool append);

/**
 * @brief Clears events in given monitor.
 * 
 * @param monitor Monitor.
 *
 * @param events Events in monitor which shall be cleared.
 */
void cms_monitor_clear_events(CmsMonitor* monitor, cms_events_t events);

/**
 * @brief Check if given events are present on given monitor.
 * 
 * @param monitor Monitor.
 *
 * @param events Events to be identified on given monitor (either all or any).
 *
 * @param allEvents Defines if all events shall match at once (true) or if
 * any of given events is enough (false).
 *
 * @param clear Defines if 'events' shall be cleared from monitor if this
 * function returns true (i.e. one or more events were matched).
 * 
 * @return A boolean indicating if either 1) one or more events were matched,
 * or 2) all events matched at once in given monitor.
 */
bool cms_monitor_check_events(CmsMonitor* monitor, cms_events_t events,
	bool allEvents, bool clear
);

#ifdef __cplusplus
} // extern "C"
#endif
