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
#include <cms/task_private.h>
#include <cms/system.h>

#include <stdlib.h>

CmsTask* cms_task_create(CmsTaskFn taskFn, void* state) {
    if (taskFn == NULL)
        return NULL;

    CmsTask* newTask = malloc(sizeof(CmsTask));
    newTask->monitor.events = 0;

    _CmsTaskData* data = (_CmsTaskData*)malloc(sizeof(_CmsTaskData));
    data->waiting = false;
    data->monitor = &newTask->monitor;
    data->waitTimestamp = 0;
    data->waitEvents = 0;
    data->allEvents = false;
    data->taskFn = taskFn;
    data->state = state;

    newTask->__data = data;

    _CmsTaskNode* newNode = malloc(sizeof(_CmsTaskNode));
    newNode->next = NULL;
    newNode->task = newTask;

    if (_taskNodeList.last == NULL) {
        // Adding first node
        _taskNodeList.first = newNode;
    } else {
        // Appending a new node
        _taskNodeList.last->next = newNode;
    }
    
    _taskNodeList.last = newNode;

    return newTask;
}

void cms_task_start_scheduler() {
    if (_taskNodeList.first == NULL)
        abort();

    _currentTaskNode = _taskNodeList.first;

    while(true) {
        _CmsTaskData* currentTaskData = (_CmsTaskData*)_currentTaskNode->task->__data;

        bool wakeUp;
        
        if (!currentTaskData->waiting) {
            wakeUp = true;
        } else {
            uint64_t now = cms_system_timestamp();

            // Check for delay/wait timeout:
            wakeUp = (now - currentTaskData->waitTimestamp) >= currentTaskData->waitTimeout;

            // Check for notifications:
            if (!wakeUp && currentTaskData->monitor != NULL && currentTaskData->waitEvents != 0)
                wakeUp = cms_monitor_check_events(currentTaskData->monitor, currentTaskData->waitEvents, currentTaskData->allEvents, false);
        }

        if (wakeUp) {
        	if (setjmp(_jmpBuf) == 0) {
        		currentTaskData->taskFn(currentTaskData->state);
        	}
        }

        _currentTaskNode = _currentTaskNode->next;
        
        if (_currentTaskNode == NULL)
            _currentTaskNode = _taskNodeList.first;
    }
}

void cms_task_delay(uint64_t millis) {
	cms_monitor_wait(NULL, 0, millis, false);
}
