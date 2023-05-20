# Problema do Jantar dos Filósofos

Este código em C implementa o problema do jantar dos filósofos usando um monitor como mecanismo de sincronização. O Jantar dos filosofos é um problema clássico de sincronização que demonstra alocação de recursos e prevenção de deadlocks.

## Funcionamento do Código

O código simula um grupo de filósofos sentados ao redor de uma mesa circular. Cada filósofo alterna entre pensar e comer. Para evitar deadlocks, os filósofos devem adquirir dois palitos (representados pelo array `estado`) para comer. O monitor garante que apenas um filósofo pode pegar os palitos por vez.

### Funções Principais

- `pensar`: Simula o filósofo pensando, exibindo uma mensagem na tela e dormindo por um período de tempo aleatório.

- `comer`: Simula o filósofo comendo, exibindo uma mensagem na tela e dormindo por um período de tempo aleatório.

- `pegarPalitos`: Representa o filósofo tentando pegar os palitos. Atualiza o estado do filósofo e, para evitar deadlock, aguarda até que os filósofos adjacentes não estejam comendo.

- `liberarPalitos`: Libera os palitos após o filósofo terminar de comer. Atualiza o estado do filósofo e sinaliza os filósofos adjacentes, se estiverem esperando pelos palitos.

- `filosofo`: Representa o comportamento de um filósofo. Alterna entre pensar, pegar e liberar os palitos, e verifica a ocorrência de deadlocks e situações de starvation.

### Execução

A função `main` é responsável por inicializar o monitor, criar um processo separado para cada filósofo, aguardar a finalização dos processos e limpar os recursos utilizados.

## Compilação e Execução

Para compilar o código, execute o seguinte comando no terminal:

'''
gcc -o filosofos filosofos.c -lpthread
'''

Em seguida, execute o programa com o comando:

'''
./filosofos
'''

## Observações

- O número de filósofos é definido pela constante `N` no início do código.
- O código utiliza semáforos para sincronizar as ações dos filósofos e garantir acesso exclusivo aos palitos compartilhados.
- Após a execução, são exibidas as ações de cada filósofo (pensar, esperar pelos palitos, comer) e quaisquer deadlocks ou situações de starvation são detectados e relatados.
