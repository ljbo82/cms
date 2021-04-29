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

#include <cms/task.h>

void cms_task_delay(uint64_t millis) {
	cms_monitor_wait(NULL, 0, millis, false);
}

void cms_task_yield() {
	cms_task_delay(0);
}

CmsMonitor* cms_task_get_monitor() {
	return &_activeScheduler->activeTaskNode->task->internalMonitor;
}
