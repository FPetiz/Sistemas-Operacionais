#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <semaphore.h>
#include <unistd.h> 
#include <string.h>
//#include <sys/syscall.h>
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
    //float tempo; ??
} Cliente;

sem_t capacetes;
sem_t karts;
// precisamos de mutex???

//void* threadC(void* crianca);
//void* threadA(void* adulto);

int main() {

    // Variáveis - não sei se essa primeira precisa ser global
    Cliente piloto[MAX_THREADS];
    int numThreads = 0;

    pthread_t tCrianca[MAX_THREADS], tAdoleacente[MAX_THREADS], tAdulto[MAX_THREADS];

    // Inicializa semáforo - não sei porque seria 0 ou 1
    sem_init(&capacetes, 1, NUM_KARTS);
    sem_init(&karts, 1, NUM_CAPACETES);

    // Abrindo o arquivo
    FILE* file = fopen("Clientes.txt", "r");
    if ( file == NULL ) {
        perror( "\nErro: nao foi possivel abrir o arquivo.\n" );
        return EXIT_FAILURE;
    }

    // Leitura do arquivo e obtenção de dados
    while( threads < MAX_THREADS ) {
        fscanf(file, "%s %d", piloto[threads].nome, &piloto[threads].idade);
        threads++;

        if ( numThreads > MAX_THREADS ) {
            fprintf( stderr, "\nUltrapassou o limite de threads\n" );
            break;
        }
    } 
    
/*
    pthread_t tCrianca, tAdulto;
    pthread_create(&tCrianca, NULL, threadC, NULL);
    pthread_create(&tAdulto, NULL, threadA, NULL);
*/
    
    while( threads > 0 ) {
        if ( piloto[threads].idade < 14 && piloto[threads].idade > 6 ) {
            printf("\n%s tem maior prioridade para capacete e beixa para kart.", piloto[threads].nome);
            //pthread_join(tCrianca, NULL);
        
        } else if ( piloto[threads].idade > 13 && piloto[threads].idade < 18 ) {
            printf("\n%s tem media prioridade para capacete e baixa para kart.", piloto[threads].nome);

        }else {
            printf("\n%s tem baixa prioridade para capacete e alta para kart.", piloto[threads].nome);
             //pthread_join(tAdulto, NULL);
        }
        threads--;
    } 

/*  
    sem_destroy(&capacetes);
    sem_destroy(&karts);
*/
    
    fclose( file );
    return 0;
}

/*
void* threadC(void* crianca) {

}


void* threadA(void* adulto) {

}
*/