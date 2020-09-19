/*
 * Copyright 2020 Leandro José Britto de Oliveira
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

typedef struct CmsTaskNode CmsTaskNode;
typedef struct CmsTaskNodeList CmsTaskNodeList;

struct CmsTaskNode {
    CmsTaskNode* next;
    CmsTask* task;
};

struct CmsTaskNodeList {
    CmsTaskNode* first;
    CmsTaskNode* last;
};

CmsTaskNodeList taskNodeList = { .first = NULL, .last = NULL };
CmsTaskNode* currentTaskNode = NULL;

CmsTask* cms_task_create(CmsTaskFn taskFn, void* state) {
    if (taskFn == NULL)
        return NULL;

    CmsTask* newTask = malloc(sizeof(CmsTask));
    newTask->monitor.events = 0;

    CmsTaskPrivate* private = (CmsTaskPrivate*)malloc(sizeof(CmsTaskPrivate));
    private->waiting = false;
    private->monitor = &newTask->monitor;
    private->waitTimestamp = 0;
    private->waitEvents = 0;
    private->allEvents = false;
    private->taskFn = taskFn;
    private->state = state;

    newTask->__private__ = private;

    CmsTaskNode* newNode = malloc(sizeof(CmsTaskNode));
    newNode->next = NULL;
    newNode->task = newTask;

    if (taskNodeList.last == NULL) {
        // Adding first node
        taskNodeList.first = newNode;
    } else {
        // Appending a new node
        taskNodeList.last->next = newNode;
    }
    
    taskNodeList.last = newNode;

    return newTask;
}

void cms_task_start_scheduler() {
    if (taskNodeList.first == NULL)
        abort();

    currentTaskNode = taskNodeList.first;

    while(true) {
        CmsTaskPrivate* currentTaskPrivate = (CmsTaskPrivate*)currentTaskNode->task->__private__;

        bool wakeUp;
        
        if (!currentTaskPrivate->waiting) {
            wakeUp = true;
        } else {
            uint64_t now = cms_system_timestamp();

            // Check for delay/wait timeout:
            wakeUp = (now - currentTaskPrivate->waitTimestamp) >= currentTaskPrivate->waitTimeout;

            // Check for notifications:
            if (!wakeUp && currentTaskPrivate->monitor != NULL && currentTaskPrivate->waitEvents != 0)
                wakeUp = cms_monitor_check_events(currentTaskPrivate->monitor, currentTaskPrivate->waitEvents, currentTaskPrivate->allEvents, false);
        }

        if (wakeUp)
            currentTaskPrivate->taskFn(currentTaskPrivate->state);

        currentTaskNode = currentTaskNode->next;
        
        if (currentTaskNode == NULL)
            currentTaskNode = taskNodeList.first;
    }
}

CmsTask* cms_task_get_current() {
    return currentTaskNode == NULL ? NULL : currentTaskNode->task;
}
