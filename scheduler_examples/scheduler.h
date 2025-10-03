#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include "queue.h"

typedef enum {
    SCHED_FIFO = 0,
    SCHED_SJF,
    SCHED_RR,
    SCHED_MLFQ
} scheduler_t;

void scheduler_dispatch(scheduler_t alg, uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task);

#endif // SCHEDULER_H
