#include "mlfq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msg.h"
#include <unistd.h>

static queue_t mlfq_queues[MLFQ_LEVELS];
static int mlfq_inited = 0;

static uint32_t mlfq_quantum_for_level(uint8_t lvl) {
    switch (lvl) {
        case 0: return MLFQ_QUANTUM_L0;
        case 1: return MLFQ_QUANTUM_L1;
        default: return MLFQ_QUANTUM_L2;
    }
}

static void mlfq_init_if_needed(queue_t *rq) {
    if (mlfq_inited) return;
    memset(mlfq_queues, 0, sizeof(mlfq_queues));
    mlfq_queues[0] = *rq;
    mlfq_inited = 1;
}

static pcb_t *mlfq_dequeue_highest() {
    for (int i = 0; i < MLFQ_LEVELS; ++i)
        if (mlfq_queues[i].head)
            return dequeue_pcb(&mlfq_queues[i]);
    return NULL;
}

void mlfq_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    mlfq_init_if_needed(rq);

    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;
        (*cpu_task)->quantum_used_ms += TICKS_MS;

        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {
            msg_t msg = {
                .pid = (*cpu_task)->pid,
                .request = PROCESS_REQUEST_DONE,
                .time_ms = current_time_ms
            };
            if (write((*cpu_task)->sockfd, &msg, sizeof(msg_t)) != sizeof(msg_t))
                perror("write");

            free(*cpu_task);
            *cpu_task = NULL;
        } else {
            uint8_t lvl = (*cpu_task)->mlfq_level;
            uint32_t q = mlfq_quantum_for_level(lvl);

            if ((*cpu_task)->quantum_used_ms >= q) {
                uint8_t new_lvl = (lvl + 1 < MLFQ_LEVELS) ? lvl + 1 : lvl;
                pcb_t *to_enqueue = *cpu_task;
                *cpu_task = NULL;
                to_enqueue->next = NULL;
                to_enqueue->quantum_used_ms = 0;
                to_enqueue->mlfq_level = new_lvl;
                enqueue_pcb(&mlfq_queues[new_lvl], to_enqueue);
            }
        }
    }

    if (*cpu_task == NULL) {
        *cpu_task = mlfq_dequeue_highest();
    }
}
