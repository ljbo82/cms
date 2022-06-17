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

#include "scheduler_private.h"

#include <cms/monitor.h>
#include <cms/system.h>

#include <stdlib.h>

DLL_PUBLIC void cms_monitor_wait(cms_monitor_t* monitor, cms_events_t events, uint64_t millis, bool allEvents) {
	cms_task_t* activeTask = cms_scheduler_get_active_task();

	if (activeTask == NULL)
		return;

	activeTask->waiting = true;
	activeTask->monitor = monitor;
	activeTask->waitTimestamp = cms_system_timestamp();
	activeTask->waitTimeout = millis;
	activeTask->waitEvents = monitor == NULL ? 0 : events;
	activeTask->allEvents = monitor == NULL ? false : allEvents;

	longjmp(_scheduler->jmpBuf, 1);
}

DLL_PUBLIC void cms_monitor_notify(cms_monitor_t* monitor, cms_events_t events, bool append) {
	if (append) {
		monitor->events |= events;
	} else {
		monitor->events = events;
	}
}

DLL_PUBLIC void cms_monitor_clear_events(cms_monitor_t* monitor, cms_events_t events) {
	monitor->events &= ~events;
}

DLL_PUBLIC bool cms_monitor_check_events(cms_monitor_t* monitor, cms_events_t events, bool allEvents, bool clear) {
	cms_events_t matchedEvents = (monitor->events & events);

	if (matchedEvents != 0) {
		if (allEvents && matchedEvents != events) {
			matchedEvents = 0;
		}
	}

	if (matchedEvents != 0 && clear)
		cms_monitor_clear_events(monitor, matchedEvents);

	return matchedEvents != 0;
}
