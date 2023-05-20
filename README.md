# Problema do Jantar dos Filósofos

Este projeto implementa uma solução para o problema clássico do Jantar dos Filósofos usando a linguagem de programação C e a biblioteca Pthreads. O objetivo é garantir que os filósofos possam comer e pensar sem causar deadlocks ou starvation.

## Funcionamento do Código

O código simula um grupo de filósofos sentados ao redor de uma mesa circular. Cada filósofo alterna entre pensar e comer. Para evitar deadlocks, os filósofos devem adquirir dois palitos (representados pelo array `estado`) para comer. O monitor garante que apenas um filósofo pode pegar os palitos por vez.

### Funções Principais

- `pensar`: Simula o filósofo pensando, exibindo uma mensagem na tela e dormindo por um período de tempo aleatório.

- `comer`: Simula o filósofo comendo, exibindo uma mensagem na tela e dormindo por um período de tempo aleatório.

- `pegar_palitos`: Representa o filósofo tentando pegar os palitos. Atualiza o estado do filósofo e, para evitar deadlock, aguarda até que os filósofos adjacentes não estejam comendo.

- `liberar_palitos`: Libera os palitos após o filósofo terminar de comer. Atualiza o estado do filósofo e sinaliza os filósofos adjacentes, se estiverem esperando pelos palitos.

- `filosofo`: Representa o comportamento de um filósofo. Alterna entre pensar, pegar e liberar os palitos, e verifica a ocorrência de deadlocks e situações de starvation.

### Execução

Cada filósofo é representado por uma thread e segue o seguinte ciclo:

#1 - Pensar por um tempo aleatório.
#2 - Tentar pegar os palitos.
#3 - Comer por um tempo aleatório.
#4 - Liberar os palitos.

A struct `Monitor` é responsável por verificar se um filósofo faminto pode comer, levando em consideração o estado de seus vizinhos. Se ambos os vizinhos não estiverem comendo, o filósofo faminto pode começar a comer.

A função `main` é responsável por inicializar o monitor, criar um processo separado para cada filósofo, aguardar a finalização dos processos e limpar os recursos utilizados.

## Resultados

A solução proposta evita deadlocks e starvation. O programa monitora continuamente o estado dos filósofos e encerra a execução caso detecte um deadlock ou starvation.
Problemas Encontrados
O principal desafio foi implementar o monitor de forma a evitar deadlocks e starvation. Para isso, foi necessário garantir que um filósofo só pudesse comer se seus vizinhos não estivessem comendo. Além disso, foi adicionado um mecanismo para detectar starvation, encerrando o programa caso um filósofo fique faminto por um número específico de ciclos.

## Problemas Encontrados
O principal desafio foi implementar o monitor de forma a evitar deadlocks e starvation. Para isso, foi necessário garantir que um filósofo só pudesse comer se seus vizinhos não estivessem comendo. Além disso, foi adicionado um mecanismo para detectar starvation, encerrando o programa caso um filósofo fique faminto por um número específico de ciclos.

## Compilação e Execução

Para compilar o código, execute o seguinte comando no terminal:

```
gcc -o filosofos filosofos.c -lpthread
```

Em seguida, execute o programa com o comando:

```
./filosofos
```
O programa exibirá o estado dos filósofos em tempo real. Para encerrar a execução, pressione `Ctrl+C`.

## Observações

- O número de filósofos é definido pela constante `N` no início do código.
- A flag de starvation apenas será acionada se um filósofo ficar sem comer por mais ciclos do que a quantidade dada pela constante `CICLOS_STARVATION`.
- Após a execução, são exibidas as ações de cada filósofo (pensar, esperar pelos palitos, comer) e quaisquer deadlocks ou situações de starvation são detectados e relatados.
