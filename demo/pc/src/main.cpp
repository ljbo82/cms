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

#include <cms.h>

#include <iostream>

#define EVT1     (1 << 0)
#define EVT2     (1 << 1)
#define EVT_STOP (1 << 2)

static cms_monitor_t __listenTaskMonitor = { 0 };

static void CALL __destructor(void* data) {
	std::cout << "destructor" << std::endl;
}

static void CALL __led_task(void* data) {
	static bool ledState = false;
	std::cout << "[LED] " << (ledState ? "ON" : "OFF") << std::endl;
	ledState = !ledState;
	cms_task_delay(1000);
}

static void CALL __notify_task(void* data) {
	static int counter = 0;

	if (counter != 0 && counter % 10 == 0)
		cms_monitor_notify(&__listenTaskMonitor, EVT1, true);

	if (counter != 0 && counter % 12 == 0)
		cms_monitor_notify(&__listenTaskMonitor, EVT2, true);

	if (counter != 0 && counter % 15 == 0)
		cms_monitor_notify(&__listenTaskMonitor, EVT1 | EVT2, true);

	if (counter == 40)
		cms_monitor_notify(&__listenTaskMonitor, EVT_STOP, true);

	std::cout << "[NOTIFY] counter: " << counter << std::endl;
	counter++;
	cms_task_delay(1000);
}

static void CALL __listen_task(void* data) {
	if (cms_monitor_check_events(&__listenTaskMonitor, EVT1, false, false))
		std::cout << "[LISTEN] Detected EVT1" << std::endl;

	if (cms_monitor_check_events(&__listenTaskMonitor, EVT2, false, false))
		std::cout << "[LISTEN] Detected EVT2" << std::endl;

	if (cms_monitor_check_events(&__listenTaskMonitor, EVT_STOP, false, false)) {
		std::cout << "[LISTEN] Detected EVT_STOP" << std::endl;
		cms_scheduler_stop(false);
	}

	cms_monitor_clear_events(&__listenTaskMonitor, CMS_MONITOR_ALL_EVENTS);

	cms_monitor_wait(&__listenTaskMonitor, EVT1 | EVT2 | EVT_STOP, UINT64_MAX, false);
}

int main() {
	cms_scheduler_create_task(__listen_task, nullptr, __destructor);
	cms_scheduler_create_task(__led_task, nullptr, __destructor);
	cms_scheduler_create_task(__notify_task, nullptr, __destructor);
	cms_scheduler_start();
}
