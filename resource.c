#include "resource.h"
#include <stdio.h>
#include <stdlib.h>

int resource_record[RESOURCES_COUNT];
struct state *running;
status_type get_resource(resource_type id)
{
    status_type result = STATUS_OK;
    if (resource_record[id] != -1)
    {
        result = STATUS_ERROR;
        return result;
    }
    else
    {
        resource_record[id] = running->task.id;
        if (running->task.static_priority < resources_priority[id])
        {
            running->task.static_priority = resources_priority[id];
        }
    }
    return result;
}

status_type release_resource(resource_type id)
{

    status_type result = STATUS_OK;
    if (running->task.id != resource_record[id])
    {
        //printf("running id:%d, resource id:%d\n", running->task.id, resource_record[id]);
        result = STATUS_ERROR;
        return result;
    }
    else
    {
        int high_resource = -1;
        resource_record[id] = -1;
        for (int i = 0; i < RESOURCES_COUNT; i++)
        {
            if (running->task.id == resource_record[i])
            {
                if (resources_priority[i] > high_resource)
                    high_resource = resources_priority[i];
            }
        }
        //printf("running priority:%d\n", running->priority);
        if (high_resource == -1)
            running->task.static_priority = running->priority;
        else
            running->task.static_priority = high_resource;
    }
    //printf("%d\n", running->priority);
    current = head;
    high_priority = 0;
    while (current != NULL)
    {
        if (current->task.static_priority > high_priority)
            high_priority = current->task.static_priority;
        current = current->next;
    }
    if (running->task.static_priority < high_priority)
    {
        int run_id = running->task.id;
        current = head;
        while (current != NULL)
        {
            if (current->task.static_priority == high_priority)
            {
                ready_queue *temp = (ready_queue *)malloc(sizeof(ready_queue));
                temp->task = running->task;
                temp->priority = running->priority;
                temp->previous = NULL;
                running->task = current->task;
                running->priority = current->priority;
                if (current == tail)
                {
                    current->previous->next = NULL;
                    tail = current->previous;
                    ready_queue *pop = current;
                    free(pop);
                    temp->next = head;
                    head->previous = temp;
                    head = temp;
                    swapcontext(&uc[run_id], &uc[running->task.id]);
                }
                else if (current == head)
                {
                    ready_queue *pop = current;
                    head = head->next;
                    free(pop);
                    temp->next = head;
                    head->previous = temp;
                    head = temp;
                    swapcontext(&uc[run_id], &uc[running->task.id]);
                }
                else
                {
                    ready_queue *pop = current;
                    current->previous->next = current->next;
                    current->next->previous = current->previous;
                    free(pop);
                    temp->next = head;
                    head->previous = temp;
                    head = temp;
                    swapcontext(&uc[run_id], &uc[running->task.id]);
                }
                break;
            }
            current = current->next;
        }
    }
    return result;
}