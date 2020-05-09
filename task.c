#include "task.h"
#include "resource.h"
#include <stdio.h>
#include <stdlib.h>

status_type activate_task(task_type id)
{
    status_type result = STATUS_OK;
    if (id == running->task.id)
    {
        result = STATUS_ERROR;
        return result;
    }
    current = head;
    while (current != NULL)
    {
        if (id == current->task.id)
        {
            result = STATUS_ERROR;
            return result;
        }
        current = current->next;
    }
    current = (ready_queue *)malloc(sizeof(ready_queue));
    current->task.id = id;
    for (int i = 0; i < TASKS_COUNT; i++)
    {
        if (id == task_const[i].id)
        {
            current->task.entry = task_const[i].entry;
            current->task.static_priority = task_const[i].static_priority;
            current->priority = task_const[i].static_priority;
            break;
        }
    }
    current->next = NULL;
    current->previous = tail;
    tail->next = current;
    tail = current;
    result = STATUS_OK;
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

status_type terminate_task(void)
{
    status_type result = STATUS_OK;
    for (int i = 0; i < RESOURCES_COUNT; i++)
    {
        if (running->task.id == resource_record[i])
        {
            result = STATUS_ERROR;
            return result;
        }
    }
    current = head;
    high_priority = 0;
    while (current != NULL)
    {
        if (current->task.static_priority > high_priority)
            high_priority = current->task.static_priority;
        current = current->next;
    }
    current = head;
    while (current != NULL)
    {
        if (current->task.static_priority == high_priority)
        {
            running->task = current->task;
            running->priority = current->priority;
            if (current == tail)
            {
                current->previous->next = NULL;
                tail = current->previous;
                ready_queue *pop = current;
                free(pop);
                setcontext(&uc[running->task.id]);
            }
            else if (current == head)
            {
                ready_queue *pop = current;
                head = head->next;
                free(pop);
                setcontext(&uc[running->task.id]);
            }
            else
            {
                ready_queue *pop = current;
                current->previous->next = current->next;
                current->next->previous = current->previous;
                free(pop);
                setcontext(&uc[running->task.id]);
            }
            break;
        }
        current = current->next;
    }
    return result;
}