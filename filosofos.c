#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

// Definindo constantes
#define NUM_FILOSOFOS 5
#define CICLOS_STARVATION 10

// Enumeração para representar os estados dos filósofos
typedef enum {
    PENSANDO,
    FAMINTO,
    COMENDO
} Estado;

// Estrutura para representar o monitor
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t condicao[NUM_FILOSOFOS];
    Estado estados[NUM_FILOSOFOS];
    int encerrar;
} Monitor;

// Variáveis globais para controle de deadlock e starvation
int deadlock_flag;
int starvation_flag;
Monitor global_monitor;

// Funções para manipular as flags de deadlock e starvation
void set_deadlock_flag() {
    deadlock_flag = 1;
}

void set_starvation_flag() {
    starvation_flag = 1;
}

int is_deadlock_detected() {
    return deadlock_flag;
}

int is_starvation_detected() {
    return starvation_flag;
}

// Protótipo da função testar
void testar(Monitor *monitor, int filosofo);

// Função que simula o filósofo pensando
void pensar(int filosofo) {
    printf("Filósofo %d está pensando.\n", filosofo);
    sleep(rand() % 3);
}

// Função que simula o filósofo comendo
void comer(int filosofo) {
    printf("Filósofo %d está comendo.\n", filosofo);
    sleep(rand() % 3);
}

// Função que representa o filósofo tentando pegar os palitos
void pegar_palitos(Monitor *monitor, int filosofo) {
    pthread_mutex_lock(&monitor->mutex);
    monitor->estados[filosofo] = FAMINTO;
    testar(monitor, filosofo);
    while (monitor->estados[filosofo] != COMENDO)
        pthread_cond_wait(&monitor->condicao[filosofo], &monitor->mutex);
    pthread_mutex_unlock(&monitor->mutex);
}

// Função que libera os palitos após o filósofo terminar de comer
void liberar_palitos(Monitor *monitor, int filosofo) {
    pthread_mutex_lock(&monitor->mutex);
    monitor->estados[filosofo] = PENSANDO;
    testar(monitor, (filosofo + NUM_FILOSOFOS - 1) % NUM_FILOSOFOS);
    testar(monitor, (filosofo + 1) % NUM_FILOSOFOS);
    pthread_mutex_unlock(&monitor->mutex);
}

// Função que verifica se o filósofo pode comer e atualiza seu estado
void testar(Monitor *monitor, int filosofo) {
    int vizinho_esquerdo = (filosofo + NUM_FILOSOFOS - 1) % NUM_FILOSOFOS;
    int vizinho_direito = (filosofo + 1) % NUM_FILOSOFOS;

    if (monitor->estados[filosofo] == FAMINTO &&
        monitor->estados[vizinho_esquerdo] != COMENDO &&
        monitor->estados[vizinho_direito] != COMENDO) {
        monitor->estados[filosofo] = COMENDO;
        pthread_cond_signal(&monitor->condicao[filosofo]);
    }
}

// Função que encerra o programa quando o sinal SIGINT é recebido
void encerrar_programa(int signal) {
    printf("\nEncerrando o programa...\n");
    exit(0);
}

// Função que representa o comportamento de um filósofo
void *filosofo(void *arg) {
    // Obtém o ID do filósofo a partir do argumento
    int filosofo = *(int *)arg;

    // Obtém uma referência ao monitor global
    Monitor *monitor = &global_monitor;

    // Variável para controlar o tempo de fome do filósofo
    time_t start_hungry_time;  

    while (1) {
        // O filósofo está pensando
        pensar(filosofo);  

        // O filósofo tenta pegar os palitos
        pegar_palitos(monitor, filosofo);  

        // Bloqueia o mutex do monitor
        pthread_mutex_lock(&monitor->mutex);  

        // Verifica se o filósofo está faminto por muito tempo (starvation)
        if (monitor->estados[filosofo] == FAMINTO) {
            if (start_hungry_time == 0) {
                // Inicia a contagem do tempo de fome
                start_hungry_time = time(NULL);  
            } else if (time(NULL) - start_hungry_time > CICLOS_STARVATION) {
                // Se o tempo de fome exceder o limite, define a flag de starvation
                set_starvation_flag();  
            }
        } else {
            // Reseta a contagem do tempo de fome
            start_hungry_time = 0;  
        }

        int deadlock_detected = 1;
        for (int i = 0; i < NUM_FILOSOFOS; i++) {
            if (monitor->estados[i] != FAMINTO) {
                // Verifica se há algum filósofo não faminto
                deadlock_detected = 0;  
                break;
            }
        }
        if (deadlock_detected) {
            // Se todos os filósofos estiverem famintos, define a flag de deadlock
            set_deadlock_flag();  
        }

        // Libera o mutex do monitor
        pthread_mutex_unlock(&monitor->mutex);  

        if (is_deadlock_detected()) {
            printf("Deadlock detectado!\n");
            break;
        }

        if (is_starvation_detected()) {
            printf("Starvation detectada (%d ciclos)!\n", CICLOS_STARVATION);
            break;
        }

        // O filósofo está comendo
        comer(filosofo);  

        // O filósofo libera os palitos
        liberar_palitos(monitor, filosofo);  

    }

    return NULL;
}


int main() {
    // Arrays para armazenar as threads e IDs dos filósofos
    pthread_t filosofos[NUM_FILOSOFOS];
    int id[NUM_FILOSOFOS];

    int i;

    // Instância do monitor
    Monitor monitor;

    // Inicialização do mutex do monitor
    pthread_mutex_init(&monitor.mutex, NULL);

    // Loop para inicialização das condições do monitor
    for (i = 0; i < NUM_FILOSOFOS; i++) {
        pthread_cond_init(&monitor.condicao[i], NULL);
    }

    // Inicialização de flags
    deadlock_flag = 0;
    starvation_flag = 0;
    monitor.encerrar = 0;

    // Configuração do tratamento do sinal SIGINT
    signal(SIGINT, encerrar_programa);

    // Inicialização dos atributos das threads
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);

    // Definição da prioridade desejada para as threads (ajustar conforme necessário)
    struct sched_param param;
    param.sched_priority = 1;

    // Atribuição do monitor global
    global_monitor = monitor;

    for (i = 0; i < NUM_FILOSOFOS; i++) {
        id[i] = i;
        // Definição dos atributos de thread (prioridade)
        pthread_attr_setschedparam(&thread_attr, &param);
        // Criação das threads dos filósofos
        pthread_create(&filosofos[i], &thread_attr, filosofo, (void *)&id[i]);
    }

    for (i = 0; i < NUM_FILOSOFOS; i++){
        // Aguarda o término de todas as threads dos filósofos
        pthread_join(filosofos[i], NULL);
    }

    // Destruir o mutex do monitor
    pthread_mutex_destroy(&monitor.mutex);

    // Destruir as condições do monitor
    for (i = 0; i < NUM_FILOSOFOS; i++){
        pthread_cond_destroy(&monitor.condicao[i]);
    }

    // Destruir os atributos das threads
    pthread_attr_destroy(&thread_attr);

    return 0;
}