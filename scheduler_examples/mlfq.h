#ifndef MLFQ_H
#define MLFQ_H

#include "queue.h"

#define MLFQ_LEVELS 3
#define MLFQ_QUANTUM_L0 500
#define MLFQ_QUANTUM_L1 1000
#define MLFQ_QUANTUM_L2 2000

void mlfq_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task);

#endif // MLFQ_H
