#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <semaphore.h>
#include <unistd.h> 
#include <string.h>
#include <sys/syscall.h>
/*#include <time.h> -- para quando formos gerar o tempo de cada um
ou recebemos o tempo pelo txt também? */

// Constantes
#define MAX_THREADS 5
#define NUM_KARTS 10
#define NUM_CAPACETES 10

//Estrutura que define o cliente
typedef struct Cliente {
    char nome[50];
    int idade;
    int tempoDeAluguel;
} Cliente;

sem_t capacetes;
sem_t karts;
// precisamos de mutex???

int capacetesUsados;
int kartsUsados;
int clientesAtenditos;
int clientesNaoAtendidos;
//int tempoDeEspera;

// Protótipos das funções
void* threadCrianca(void* crianca);
void* threadAdolescente(void* adolescente);
void* threadAdulto(void* adulto);

int main() {

    // Variáveis - não sei se essa primeira precisa ser global
    Cliente piloto[MAX_THREADS];
    int numThreads = 0;

    pthread_t tCrianca[MAX_THREADS], tAdoleacente[MAX_THREADS], tAdulto[MAX_THREADS];

    // Inicializa semáforo - não sei porque seria 0 ou 1
    sem_init(&capacetes, 0, NUM_KARTS);
    sem_init(&karts, 0, NUM_CAPACETES);

    // Abrindo o arquivo
    FILE* file = fopen( "Clientes.txt", "r" );
    if ( file == NULL ) {
        perror( "\nErro: nao foi possivel abrir o arquivo.\n" );
        return EXIT_FAILURE;
    }

    // Leitura do arquivo e obtenção de dados
    while( threads < MAX_THREADS ) {
        fscanf( file, "%s %d %d", piloto[numThreads].nome, &piloto[numThreads].idade,
                        &clientes[numThreads].tempoDeAluguel == 3 );
        numThreads++;

        if ( numThreads > MAX_THREADS ) {
            fprintf( stderr, "\nUltrapassou o limite de threads\n" );
            break;
        }
    } 
    
    int total = numThreads;
    
    // Criação de thread conforme a idade do piloto
    while( numThreads > 0 ) {
        if ( piloto[numThreads].idade < 14 ) {
            pthread_create( &tCrianca[numThreads], NULL, threadCrianca, NULL ); // o que é esse último NULL?
        } else if ( piloto[numThreads].idade < 18 ) {
            pthread_create( &tAdolescente[numThreads], NULL, threadAdolescente, NULL );
        }else {
            pthread_create( &tAdulto[numThreads], NULL, threadAdulto, NULL );
            
        }
        numThreads--;
    } 

    // Bloqueia novas chamadas até que a thread especificada seja concluída
    for ( int i = 0; i < total; i++ ) {
        if ( piloto[i].idade < 14 ) {
            pthread_join( tCrianca, NULL );
        } else if ( piloto[i].idade < 18 ) {
            pthread_join( tAdoleacente, NULL );
        } else {
            pthread_join( tAdulto, NULL );
        }
    }
    
    // Destroi os semáforos
    sem_destroy( &capacetes );
    sem_destroy( &karts );
    
    // Fecha arquivo txt
    fclose( file );
    return 0;
}


void* threadCrianca( void* crianca ) {

    sem_wait( &capacetes );
    sem_wait( &karts);

    capacetesUsados++;
    kartsUsados++;
    clientesAtenditos++;

    //sleep

    sem_post( &karts );
    sem_post( &capacetes );
}

void* threadAdolescente () {
    sem_wait( &capacetes );
    sem_wait( &karts);

    capacetesUsados++;
    kartsUsados++;
    clientesAtenditos++;

    //sleep

    sem_post( &karts );
    sem_post( &capacetes );
}

void* threadAdulto( void* adulto ) {
    sem_wait( &karts );
    sem_wait( &capacetes );

    capacetesUsados++;
    kartsUsados++;
    clientesAtenditos++;

    //sleep

    sem_post( &capacetes );
    sem_post( &karts );
}