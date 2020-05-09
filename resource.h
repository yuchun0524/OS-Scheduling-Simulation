#ifndef RESOURCE_H
#define RESOURCE_H

#include "typedefine.h"
#include "task.h"
#include "config.h"

status_type get_resource(resource_type id);
status_type release_resource(resource_type id);
extern int resource_record[RESOURCES_COUNT];
struct state
{
    task_const_type task;
    int priority;
};
extern struct state *running;
#endif /* RESOURCE_H */