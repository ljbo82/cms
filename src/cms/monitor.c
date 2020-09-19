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
#include <cms/monitor.h>
#include <cms/task_private.h>
#include <cms/system.h>

#include <stdlib.h>

void __cms_monitor_wait__(CmsMonitor* monitor, cms_events_t events, uint64_t millis, bool allEvents) {
    CmsTask* task;
    if ((task = cms_task_get_current()) == NULL) // Scheduler is not running
        abort();

    CmsTaskPrivate* cmsTaskPrivate = (CmsTaskPrivate*)(task->__private__);

    cmsTaskPrivate->waiting = true;
    cmsTaskPrivate->monitor = monitor;
    cmsTaskPrivate->waitTimestamp = cms_system_timestamp();
    cmsTaskPrivate->waitTimeout = millis;
    cmsTaskPrivate->waitEvents = monitor == NULL ? 0 : events;
    cmsTaskPrivate->allEvents = monitor == NULL ? false : allEvents;
}

void cms_monitor_notify(CmsMonitor* monitor, cms_events_t events, bool append) {
    if (append) {
        monitor->events |= events;
    } else {
        monitor->events = events;
    }
}

void cms_monitor_clear_events(CmsMonitor* monitor, cms_events_t events) {
    monitor->events &= ~events;
}

bool cms_monitor_check_events(CmsMonitor* monitor, cms_events_t events, bool allEvents, bool clear) {
    bool matched = false;

    if (monitor->events != 0) {
        if (allEvents) {
            matched = ((monitor->events & events) == events);
        } else {
            matched = ((monitor->events & events) != 0);
        }

        if (matched && clear)
            cms_monitor_clear_events(monitor, events);
    }

	return matched;
}
