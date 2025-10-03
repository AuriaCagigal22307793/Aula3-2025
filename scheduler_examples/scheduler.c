#include "scheduler.h"
#include "fifo.h"
#include "sjf.h"
#include "rr.h"
#include "mlfq.h"

void scheduler_dispatch(scheduler_t alg, uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    switch (alg) {
    case SCHED_FIFO:
        fifo_scheduler(current_time_ms, rq, cpu_task);
        break;
    case SCHED_SJF:
        sjf_scheduler(current_time_ms, rq, cpu_task);
        break;
    case SCHED_RR:
        rr_scheduler(current_time_ms, rq, cpu_task);
        break;
    case SCHED_MLFQ:
        mlfq_scheduler(current_time_ms, rq, cpu_task);
        break;
    default:
        fifo_scheduler(current_time_ms, rq, cpu_task);
    }
}
