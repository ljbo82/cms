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

#include "watchdog_private.h"

#include <cms.h>

#include <Arduino.h>

#define EVT1     (1 << 0)
#define EVT2     (1 << 1)
#define EVT_STOP (1 << 2)

static cms_monitor_t __listenTaskMonitor = { 0 };

static void __led_task(void* data) {
	static bool ledState = false;
	digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
	ledState = !ledState;
	cms_task_delay(500);
}

static void __notify_task(void* data) {
	static int counter = 0;
	if (counter != 0 && counter % 10 == 0)
		cms_monitor_notify(&__listenTaskMonitor, EVT1, true);

	if (counter != 0 && counter % 12 == 0)
		cms_monitor_notify(&__listenTaskMonitor, EVT2, true);

	if (counter != 0 && counter % 15 == 0)
		cms_monitor_notify(&__listenTaskMonitor, EVT1 | EVT2, true);

	if (counter == 40)
		cms_monitor_notify(&__listenTaskMonitor, EVT_STOP, true);

	Serial.print("[NOTIFY] Counter: ");
	Serial.print(counter);
	Serial.print("\r\n");
	counter++;
	cms_task_delay(1000);
}

static void __listen_task(void* data) {
	if (cms_monitor_check_events(&__listenTaskMonitor, EVT1, false, false))
		Serial.print("[LISTEN] Detected EVT1\r\n");

	if (cms_monitor_check_events(&__listenTaskMonitor, EVT2, false, false))
		Serial.print("[LISTEN] Detected EVT2\r\n");

	bool stop = cms_monitor_check_events(&__listenTaskMonitor, EVT_STOP, false, false);

	if (stop) {
		Serial.print("[LISTEN] Detected EVT_STOP (watchdog will reset the board)\r\n");
		while(true) {
			// Never returns to the scheduler (which allows watchdog to be reset).
		}
	} else {
		cms_monitor_clear_events(&__listenTaskMonitor, CMS_MONITOR_ALL_EVENTS);
		cms_monitor_wait(&__listenTaskMonitor, EVT1 | EVT2 | EVT_STOP, UINT64_MAX, false);
	}
}

static void __watchdog_reset_task(void* data) {
	_watchdog_reset();
	cms_task_delay(500);
}

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(9600);

	cms_scheduler_create_task(__listen_task, nullptr, nullptr);
	cms_scheduler_create_task(__led_task, nullptr, nullptr);
	cms_scheduler_create_task(__notify_task, nullptr, nullptr);
	cms_scheduler_create_task(__watchdog_reset_task, nullptr, nullptr);

	_watchdog_init();

	Serial.print("Firmware is ready!\r\n");
}

void loop() {
	cms_scheduler_start();
}
