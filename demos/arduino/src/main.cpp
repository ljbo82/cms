/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/
#include "_watchdog.h"

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
	cms_task_delay(250);
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
	Serial.print("\n");
	counter++;
	cms_task_delay(1000);
}

static void __listen_task(void* data) {
	if (cms_monitor_check_events(&__listenTaskMonitor, EVT1, false, false))
		Serial.print("[LISTEN] Detected EVT1\n");

	if (cms_monitor_check_events(&__listenTaskMonitor, EVT2, false, false))
		Serial.print("[LISTEN] Detected EVT2\n");

	bool stop = cms_monitor_check_events(&__listenTaskMonitor, EVT_STOP, false, false);

	if (stop) {
		Serial.print("[LISTEN] Detected EVT_STOP (watchdog will reset the board)\n");
		while(true) {
			// Never returns to the scheduler (watchdog will timeout).
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

	Serial.print("Firmware is ready!\n");
}

void loop() {
	cms_scheduler_start();
}
