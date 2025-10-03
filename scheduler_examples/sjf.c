#include "sjf.h"
#include "queue.h"
#include "msg.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @brief Shortest Job First (SJF) scheduling algorithm.
 *
 * Escolhe a tarefa com o menor tempo total de execução (time_ms) da ready queue.
 * Quando uma tarefa termina, envia uma mensagem DONE para a aplicação.
 *
 * @param current_time_ms Tempo atual do simulador (em ms)
 * @param rq Ponteiro para a fila de tarefas prontas
 * @param cpu_task Ponteiro duplo para a tarefa atualmente em execução
 */
void sjf_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    // 1️⃣ Atualizar tarefa em execução
    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;

        // Verifica se terminou
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
        }
    }

    //  Se o CPU está livre, procura o processo com menor tempo_ms
    if (*cpu_task == NULL && rq && rq->head != NULL) {
        queue_elem_t *cur = rq->head;
        queue_elem_t *prev = NULL;
        queue_elem_t *shortest_prev = NULL;
        queue_elem_t *shortest_elem = rq->head;

        // Percorre a fila à procura do processo mais curto
        while (cur) {
            if (cur->pcb->time_ms < shortest_elem->pcb->time_ms) {
                shortest_elem = cur;
                shortest_prev = prev;
            }
            prev = cur;
            cur = cur->next;
        }

        // Remove o elemento encontrado da fila
        if (shortest_prev == NULL) {
            rq->head = shortest_elem->next;  // estava no início
        } else {
            shortest_prev->next = shortest_elem->next;
        }

        if (rq->tail == shortest_elem)
            rq->tail = shortest_prev;

        shortest_elem->next = NULL;

        // Atribui o PCB ao CPU e liberta o elemento da fila
        *cpu_task = shortest_elem->pcb;
        free(shortest_elem);
    }
}
