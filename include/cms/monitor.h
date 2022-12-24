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
 *
 * @brief Monitor API.
 */
#pragma once

#include "defs.h"

#define CMS_MONITOR_ALL_EVENTS UINT8_MAX

/** @brief Event bit flags. */
typedef uint8_t cms_events_t;

typedef struct cms_monitor cms_monitor_t;

/**
 * @brief Represents a monitor.
 *
 * A monitor is an entity used for synchronization among multiple tasks trough
 * its internal event bitmask.
 */
struct cms_monitor {
    /** @brief Currently flagged events. */
    cms_events_t events;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Flags the current task to wait upon events on given monitor.
 *
 * @note Execution will immediately return to the scheduler.
 *
 * @param monitor Monitor. Passing {@code NULL} implies a delay (task will
 *        wake up only after given interval expired. If given interval is
 *        zero task yields).
 *
 * @param events Waiting events (will be ignored if 'monitor' is {@code NULL}).
 *
 * @param millis Maximum timeout in milliseconds on current task waiting for
 *        events/delay on given monitor.
 *
 * @param allEvents Defines if either all events at once, or any of given
 *        events is a valid condition (will be ignored if 'monitor' is NULL).
 */
void cms_monitor_wait(
	cms_monitor_t* monitor,
	cms_events_t events,
	uint64_t millis,
	bool allEvents
);

/**
 * @brief Notifies a monitor for events.
 *
 * @param monitor Monitor.
 *
 * @param events Events to be set in given monitor.
 *
 * @param append Defines if current monitor events should be preserved and
 *        new ones (defined by the bits in 'events') will be appended.
 *        Passing {@code false} will force the monitor to have exactly
 *        given events.
 */
void cms_monitor_notify(
	cms_monitor_t* monitor,
	cms_events_t events,
	bool append
);

/**
 * @brief Clears events in given monitor.
 *
 * @param monitor Monitor.
 *
 * @param events Events in monitor that must be cleared. In order to clear
 *        all events use {@link CMS_MONITOR_ALL_EVENTS}.
 */
void cms_monitor_clear_events(
	cms_monitor_t* monitor,
	cms_events_t events
);

/**
 * @brief Check if given events are present on given monitor.
 *
 * @param monitor Monitor.
 *
 * @param events Events to be identified on given monitor (either all or any).
 *
 * @param allEvents Defines if all events shall match at once ({@code true})
 *        or if any of given events is enough ({@code false}).
 *
 * @param clear Defines if matched {@code events} shall be cleared from
 *        monitor if this function returns {@code true} (i.e. one or more
 *        events were matched).
 *
 * @return A boolean indicating if either 1) one or more events were matched,
 *         ({@code allEvents == false}or 2) all events matched at once
 *         ({@code allEvents = true}) in given monitor.
 */
bool cms_monitor_check_events(
	cms_monitor_t* monitor,
	cms_events_t events,
	bool allEvents,
	bool clear
);

#ifdef __cplusplus
} // extern "C"
#endif
