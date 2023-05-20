#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>

#define N 5 // Número de filósofos/palitos

typedef struct {
    int estado[N]; // Estado do filósofo
    sem_t sem[N];
    int deadlockFlag;
    int starvationFlag;
} MonitorFilosofos;

void pensar(int id) {
    printf("Filósofo %d está pensando.\n", id);
    sleep(rand() % 5); // Simula o tempo de pensamento
}

void comer(int id) {
    printf("Filósofo %d está comendo.\n", id);
    sleep(rand() % 5); // Simula o tempo de alimentação
}

void pegarPalitos(MonitorFilosofos* monitor, int id) {
    monitor->estado[id] = 1; // Filósofo está com fome

    while (monitor->estado[(id + 1) % N] == 2 || monitor->estado[(id + N - 1) % N] == 2) {
        // Aguarda até que os filósofos adjacentes não estejam comendo
        printf("Filósofo %d está esperando pelos palitos.\n", id);
        sem_wait(&monitor->sem[id]);
    }

    monitor->estado[id] = 2; // Filósofo adquiriu os palitos
}

void liberarPalitos(MonitorFilosofos* monitor, int id) {
    monitor->estado[id] = 0; // Filósofo está pensando

    // Sinaliza os filósofos adjacentes se estiverem esperando
    if (monitor->estado[(id + 1) % N] == 1 && monitor->estado[(id + N - 1) % N] != 2) {
        // Sinaliza o semáforo para o filósofo à direita
        sem_post(&monitor->sem[(id + 1) % N]);
    }

    if (monitor->estado[(id + 1) % N] != 2 && monitor->estado[(id + N - 1) % N] == 1) {
        // Sinaliza o semáforo para o filósofo à esquerda
        sem_post(&monitor->sem[(id + N - 1) % N]);
    }

    printf("Filósofo %d liberou os palitos.\n", id);
}

void filosofo(MonitorFilosofos* monitor, int id) {
    int count = 0;
    while (1) {
        pensar(id);
        pegarPalitos(monitor, id);
        comer(id);
        liberarPalitos(monitor, id);

        count++;

        // Verifica por deadlock
        if (count >= 2 * N && monitor->deadlockFlag == 0) {
            monitor->deadlockFlag = 1;
            printf("Possível deadlock detectado!\n");
        }

        // Verifica por starvation
        if (count >= 10 * N && monitor->starvationFlag == 0) {
            monitor->starvationFlag = 1;
            printf("Possível situação de starvation detectada!\n");
        }
    }
}

int main() {
    MonitorFilosofos monitor;
    int i;

    for (i = 0; i < N; i++) {
        monitor.estado[i] = 0; // Inicializa os estados dos filósofos
        sem_init(&monitor.sem[i], 0, 0); // Inicializa os semáforos
    }

    monitor.deadlockFlag = 0;
    monitor.starvationFlag = 0;

    for (i = 0; i < N; i++) {
        // Cria um processo separado para cada filósofo
        pid_t pid = fork();

        if (pid < 0) {
            // Ocorreu um erro
            perror("Falha na criação do processo");
            exit(1);
        } else if (pid == 0) {
            // O processo filho representa um filósofo
            filosofo(&monitor, i);
            exit(0);
        }
    }

    // Aguarda todos os processos filhos terminarem
    for (i = 0; i < N; i++) {
        wait(NULL);
    }

    for (i = 0; i < N; i++) {
        sem_destroy(&monitor.sem[i]); // Destroi os semáforos
    }

    return 0;
}
