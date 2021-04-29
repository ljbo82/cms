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

#include "scheduler_private.h"

#include <cms/monitor.h>
#include <cms/system.h>

#include <stdlib.h>

void cms_monitor_wait(CmsMonitor* monitor, cms_events_t events, uint64_t millis, bool allEvents) {
	if (_activeScheduler == NULL)
		return;

	if (_activeScheduler->state == CMS_SCHEDULER_STATE_IDLE) {
		// Calling from idle task...
		longjmp(_activeScheduler->jmpBuf, 1);
	}

    _CmsSchedulerTask* activeTask = _activeScheduler->activeTaskNode->task;

    activeTask->waiting = true;
    activeTask->monitor = monitor;
    activeTask->waitTimestamp = cms_system_timestamp();
    activeTask->waitTimeout = millis;
    activeTask->waitEvents = monitor == NULL ? 0 : events;
    activeTask->allEvents = monitor == NULL ? false : allEvents;

    longjmp(_activeScheduler->jmpBuf, 1);
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
