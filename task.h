#ifndef TASK_H
#define TASK_H

#include "typedefine.h"
#include "config.h"
#include <ucontext.h>

status_type activate_task(task_type id);
status_type terminate_task(void);
extern ucontext_t uc[TASKS_COUNT];
extern char stack[TASKS_COUNT][8192];
#endif /* TASK_H */