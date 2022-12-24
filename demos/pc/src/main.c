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
#include <cms.h>

#include <stdio.h>

#define EVT1     (1 << 0)
#define EVT2     (1 << 1)
#define EVT_STOP (1 << 2)

static cms_monitor_t __listenTaskMonitor = { 0 };

static void __destructor(void* data) {
	printf("destructor\n");;
}

static void __led_task(void* data) {
	static bool ledState = false;
	printf("[LED] %s\n", ledState ? "ON" : "OFF");
	ledState = !ledState;
	cms_task_delay(1000);
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

	printf("[NOTIFY] counter: %d\n", counter);
	counter++;
	cms_task_delay(1000);
}

static void __listen_task(void* data) {
	if (cms_monitor_check_events(&__listenTaskMonitor, EVT1, false, false))
		printf("[LISTEN] Detected EVT1\n");

	if (cms_monitor_check_events(&__listenTaskMonitor, EVT2, false, false))
		printf("[LISTEN] Detected EVT2\n");

	if (cms_monitor_check_events(&__listenTaskMonitor, EVT_STOP, false, false)) {
		printf("[LISTEN] Detected EVT_STOP\n");
		cms_scheduler_stop(false);
	}

	cms_monitor_clear_events(&__listenTaskMonitor, CMS_MONITOR_ALL_EVENTS);

	cms_monitor_wait(&__listenTaskMonitor, EVT1 | EVT2 | EVT_STOP, UINT64_MAX, false);
}

int main() {
	cms_scheduler_create_task(__listen_task, NULL, __destructor);
	cms_scheduler_create_task(__led_task, NULL, __destructor);
	cms_scheduler_create_task(__notify_task, NULL, __destructor);
	cms_scheduler_start();
}
