/*
 * Copyright 2021 Leandro José Britto de Oliveira
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

#include <cms/scheduler.h>
#include <cms/task.h>

#include <iostream>

#define EVT1 (1 << 0)
#define EVT2 (1 << 1)

CmsScheduler* scheduler = nullptr;

static void led_task(void* state) {
	static bool mLedState = false;
	std::cout << "[LED] " << (mLedState ? "ON" : "OFF") << std::endl;
	mLedState = !mLedState;
	cms_task_delay(1000);
}

static void notify_task(void* state) {
	CmsMonitor* mListenTaskMonitor = (CmsMonitor*)state;

	static int mNotifyState = 0;
	if (mNotifyState != 0 && mNotifyState % 10 == 0)
		cms_monitor_notify(mListenTaskMonitor, EVT1, true);

	if (mNotifyState != 0 && mNotifyState % 12 == 0)
		cms_monitor_notify(mListenTaskMonitor, EVT2, true);

	if (mNotifyState != 0 && mNotifyState % 15 == 0)
		cms_monitor_notify(mListenTaskMonitor, EVT1 | EVT2, true);

	std::cout << "[NOTIFY] counter: " << mNotifyState << std::endl;
	mNotifyState++;
	cms_task_delay(2000);
}

static void listen_task(void* s) {
	CmsMonitor* mListenTaskMonitor = cms_task_get_monitor();

	if (cms_monitor_check_events(mListenTaskMonitor, EVT1, false, false))
		std::cout << "[LISTEN] Notified EVT1" << std::endl;

	if (cms_monitor_check_events(mListenTaskMonitor, EVT2, false, false))
		std::cout << "[LISTEN] Notified EVT2" << std::endl;

	cms_monitor_clear_events(mListenTaskMonitor, CMS_MONITOR_ALL_EVENTS);

	cms_monitor_wait(mListenTaskMonitor, EVT1 | EVT2, UINT64_MAX, false);
}

int main() {
	scheduler = cms_scheduler_create(nullptr, nullptr);
	CmsMonitor* mListenTaskMonitor = cms_scheduler_add_task(scheduler, listen_task, nullptr);
    cms_scheduler_add_task(scheduler, led_task, nullptr);
    cms_scheduler_add_task(scheduler, notify_task, mListenTaskMonitor);
	cms_scheduler_start(scheduler);
	cms_scheduler_destroy(scheduler);
	abort();
}
