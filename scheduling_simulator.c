#include "resource.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>

ready_queue *head, *current, *tail;
ucontext_t uc[TASKS_COUNT];
char stack[TASKS_COUNT][8192];
int high_priority = 0;
void auto_ready_queue()
{
    int i;
    for (i = 0; i < AUTO_START_TASKS_COUNT; i++)
    {
        current = (ready_queue *)malloc(sizeof(ready_queue));
        current->task.id = auto_start_tasks_list[i];
        if (i == 0)
        {
            current->previous = NULL;
            head = current;
        }
        else
        {
            current->previous = tail;
            tail->next = current;
        }
        current->next = NULL;
        tail = current;
    }
    current = head;
    while (current != NULL)
    {
        for (i = 0; i < TASKS_COUNT; i++)
        {
            if (current->task.id == task_const[i].id)
            {
                current->task.entry = task_const[i].entry;
                current->task.static_priority = task_const[i].static_priority;
                current->priority = task_const[i].static_priority;
                break;
            }
        }
        current = current->next;
    }
}
void print_ready()
{
    current = head;
    while (current != NULL)
    {
        printf("id:%d, priority:%d\n", current->task.id, current->task.static_priority);
        current = current->next;
    }
}
int main()
{
    for (int i = 0; i < TASKS_COUNT; i++)
    {
        getcontext(&uc[i]);
        uc[i].uc_stack.ss_sp = stack[i];
        uc[i].uc_stack.ss_size = sizeof(stack[i]);
        uc[i].uc_link = 0;
        uc[i].uc_stack.ss_flags = 0;
        makecontext(&uc[i], task_const[i].entry, 0);
    }
    auto_ready_queue();
    running = (struct state *)malloc(sizeof(struct state *));
    for (int i = 0; i < RESOURCES_COUNT; i++)
    {
        resource_record[i] = -1;
    }
    current = head;
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
            }
            else if (current == head)
            {
                ready_queue *pop = current;
                head = head->next;
                free(pop);
            }
            else
            {
                ready_queue *pop = current;
                current->previous->next = current->next;
                current->next->previous = current->previous;
                free(pop);
            }
            break;
        }
        current = current->next;
    }
    setcontext(&uc[running->task.id]);
    return 0;
}