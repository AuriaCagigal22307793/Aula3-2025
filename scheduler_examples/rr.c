#include "rr.h"
#include <stdio.h>
#include <stdlib.h>
#include "msg.h"
#include <unistd.h>

static uint32_t rr_quantum_elapsed_ms = 0;

/**
 * Round-Robin (RR)
 * Executa cada processo por 500ms e volta ao fim da fila se não terminar.
 */
void rr_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;
        rr_quantum_elapsed_ms += TICKS_MS;

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
            rr_quantum_elapsed_ms = 0;
        }
        else if (rr_quantum_elapsed_ms >= RR_QUANTUM_MS) {
            rr_quantum_elapsed_ms = 0;
            pcb_t *to_enqueue = *cpu_task;
            *cpu_task = NULL;
            to_enqueue->next = NULL;
            enqueue_pcb(rq, to_enqueue);
        }
    }

    if (*cpu_task == NULL) {
        *cpu_task = dequeue_pcb(rq);
        rr_quantum_elapsed_ms = 0;
    }
}
