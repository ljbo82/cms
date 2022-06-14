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

#include <cms/cms.h>

#include <iostream>

#define EVT1 (1 << 0)
#define EVT2 (1 << 1)

static void led_task(void* data) {
	static bool mLedState = false;
	std::cout << "[LED] " << (mLedState ? "ON" : "OFF") << std::endl;
	mLedState = !mLedState;
	cms_task_delay(1000);
}

static void notify_task(void* data) {
	cms_monitor_t* mListenTaskMonitor = (cms_monitor_t*)data;

	static int mNotifyState = 0;

	if (mNotifyState != 0 && mNotifyState % 10 == 0)
		cms_monitor_notify(mListenTaskMonitor, EVT1, true);

	if (mNotifyState != 0 && mNotifyState % 12 == 0)
		cms_monitor_notify(mListenTaskMonitor, EVT2, true);

	if (mNotifyState != 0 && mNotifyState % 15 == 0)
		cms_monitor_notify(mListenTaskMonitor, EVT1 | EVT2, true);

	std::cout << "[NOTIFY] counter: " << mNotifyState << std::endl;
	mNotifyState++;
	cms_task_delay(1000);
}

static void listen_task(void* data) {
	cms_monitor_t* mListenTaskMonitor = cms_task_get_monitor();

	if (cms_monitor_check_events(mListenTaskMonitor, EVT1, false, false))
		std::cout << "[LISTEN] Notified EVT1" << std::endl;

	if (cms_monitor_check_events(mListenTaskMonitor, EVT2, false, false))
		std::cout << "[LISTEN] Notified EVT2" << std::endl;

	cms_monitor_clear_events(mListenTaskMonitor, CMS_MONITOR_ALL_EVENTS);

	cms_monitor_wait(mListenTaskMonitor, EVT1 | EVT2, UINT64_MAX, false);
}

int main() {
	cms_monitor_t* mListenTaskMonitor = cms_scheduler_create_task(listen_task, nullptr, nullptr);
	cms_scheduler_create_task(led_task, nullptr, nullptr);
	cms_scheduler_create_task(notify_task, mListenTaskMonitor, nullptr);
	cms_scheduler_start();
	abort();
}
