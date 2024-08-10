#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <semaphore.h>
#include <unistd.h> 
#include <string.h>
// #include <sys/syscall.h>
#include <time.h> /*-- para quando formos gerar o tempo de cada um
ou recebemos o tempo pelo txt também? */

// Constantes
#define MAX_THREADS 10
#define NUM_KARTS 3
#define NUM_CAPACETES 3

// Variáveis globais para relatório
int capacetesUsados = 0;
int kartsUsados = 0;
int clientesAtenditos = 0;
int clientesNaoAtendidos = 0;
int fila = 0;

//Estrutura que define o cliente
typedef struct Cliente {
    char nome[50];
    int idade;
    int tempoDeAluguel;
} Cliente;

sem_t capacetes;
sem_t karts;
// precisamos de mutex??? - Da pra usar semáforo para controlar o acesso a variáveis compartilhadas

// Protótipos das funções
void* threadCrianca(void* crianca);
void* threadAdolescente(void* adolescente);
void* threadAdulto(void* adulto);

int main() {

    // Variáveis - não sei se essa primeira precisa ser global
    Cliente piloto[MAX_THREADS];
    int numThreads = 0, atual = 0;
    int expediente = 2;
    int pessoas = 0;

    pthread_t tCrianca[MAX_THREADS], tAdolescente[MAX_THREADS], tAdulto[MAX_THREADS];

    // Inicializa semáforo - não sei porque seria 0 ou 1 -- 0 ou 1 indica se vai ser compartilhado ou não
    sem_init(&capacetes, 1, NUM_KARTS);
    sem_init(&karts, 1, NUM_CAPACETES);

    // Abrindo o arquivo
    FILE* file = fopen( "F:\\Code\\SO\\Sistemas-Operacionais\\Clientes-somente-nome.txt", "r" ); // O meu por algum motivo só lê o arquivo se eu colocar o caminho completo
    if ( file == NULL ) {
        perror( "\nErro: nao foi possivel abrir o arquivo.\n" );
        return EXIT_FAILURE;
    }

    int total = numThreads;

    srand(time(NULL));
    // For para simular o expediente
    for (int hora = 0; hora < expediente; ++hora) {

        pessoas = rand() % 10 + 1;
        printf("\nTotal de pessoas %d\n", pessoas);
        // Leitura do arquivo e obtenção de dados -- Transformei os whiles em fors
        for (numThreads = 0; numThreads < pessoas; numThreads++) {
            // fscanf(file, "%s %d %d", piloto[numThreads].nome, &piloto[numThreads].idade, &piloto[numThreads].tempoDeAluguel);
            fscanf(file, "%s", piloto[numThreads].nome);
            piloto[numThreads].idade = rand() % 22 + 8;
            piloto[numThreads].tempoDeAluguel = rand() % 40 + 20;

            if (numThreads >= pessoas) {
                fprintf(stderr, "\nUltrapassou o limite de threads\n");
                break;
            }
        }
        
        
        // Criação de thread conforme a idade do piloto -- Transformei os whiles em fors
        for (numThreads = 0; numThreads < pessoas; ++numThreads) {
            if (piloto[numThreads].idade < 14) {
                pthread_create(&tCrianca[numThreads], NULL, threadCrianca, &piloto[numThreads]);
            } else if (piloto[numThreads].idade < 18) {
                pthread_create(&tAdolescente[numThreads], NULL, threadAdolescente, &piloto[numThreads]);
            } else {
                pthread_create(&tAdulto[numThreads], NULL, threadAdulto, &piloto[numThreads]);
            }
        }

        // Bloqueia novas chamadas até que a thread especificada seja concluída
        for ( int i = 0; i < numThreads; i++ ) {
            // for (int j = 0; j < numThreads; j++) { // Porque esse segundo for?
                pthread_join(tCrianca[i], NULL);
                pthread_join(tAdolescente[i], NULL);
                pthread_join(tAdulto[i], NULL);
        // 
        }
        sleep(30);
        printf("\nFim da hora %d\n", hora);
    }
    
    // Destroi os semáforos
    sem_destroy( &capacetes );
    sem_destroy( &karts );
    
    printf( "\nRelatório:\n" );
    printf( "\nCapacetes usados: %d", capacetesUsados );
    printf( "\nKarts usados: %d", kartsUsados );
    printf( "\nClientes atendidos: %d", clientesAtenditos );
    printf( "\nFila: %d", fila );

    // Fecha arquivo txt
    fclose( file );
    return 0;
}


void* threadCrianca( void* crianca ) {

    Cliente* piloto = (Cliente*) crianca;
    printf( "\nThread crianças" );
    printf( "\nNome: %s\n", piloto->nome );

    ++fila;
    sem_wait( &capacetes );
    ++capacetesUsados;
    sem_wait( &karts);
    ++kartsUsados;

    sleep( piloto->tempoDeAluguel );
    //sleep
    // sleep(1);

    sem_post( &karts );
    sem_post( &capacetes );
    --fila;
    ++clientesAtenditos;

    printf( "\nThread criança finalizada" );
}

void* threadAdolescente ( void* adolescente) {
    Cliente* piloto = (Cliente*) adolescente;
    printf( "\nThread adolescentes" );

    printf( "\nNome: %s\n", piloto->nome );

    ++fila;
    sem_wait( &capacetes );
    ++capacetesUsados;
    sem_wait( &karts);
    ++kartsUsados;

    sleep( piloto->tempoDeAluguel );
    //sleep
    // sleep(2);

    sem_post( &karts );
    sem_post( &capacetes );
    --fila;
    ++clientesAtenditos;

    printf( "\nThread adolescente finalizada" );
}

void* threadAdulto( void* adulto ) {
    Cliente* piloto = (Cliente*) adulto;
    printf( "\nThread adultos" );

    printf( "\nNome: %s\n", piloto->nome );

    ++fila;
    sem_wait( &karts );
    ++kartsUsados;
    sem_wait( &capacetes );
    ++capacetesUsados;

    sleep( piloto->tempoDeAluguel );
    //sleep
    // sleep(3);

    sem_post( &capacetes );
    sem_post( &karts );
    --fila;
    ++clientesAtenditos;

    printf( "\nThread adulto finalizada" );
}