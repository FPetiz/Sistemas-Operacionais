#include <stdio.h>
#include <stdlib.h>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h> 
#include <semaphore.h>
#include <unistd.h> 
#include <string.h>
#include <time.h> 
#define TRUE 1
#define FALSE 0

// Constantes
#define MAX_THREADS 10
#define NUM_KARTS 10
#define NUM_CAPACETES 10
#define MAX_PILOTOS 80

// Variáveis globais para relatório
int capacetesUsados = 0;
int kartsUsados = 0;
int clientesAtendidos = 0;
int clientesNaoAtendidos = 0;
int fila = 0;
int totalWaitTime = 0;

//Estrutura que define o cliente
typedef struct Cliente {
    char nome[50];
    int idade;
    int tempoDeAluguel;
    int tempoDeEspera;
    int atendido;
} Cliente;

// Lista de clientes não atendidos 
Cliente listaNaoAtendidos[MAX_THREADS];

// Semáforos 
sem_t capacetes;
sem_t karts;
sem_t mutex;
sem_t m_fila;

// Protótipos das funções
void* threadCrianca(void* crianca);
void* threadAdolescente(void* adolescente);
void* threadAdulto(void* adulto);
void addTempo();

int main() {

    // Variáveis 
    Cliente piloto[MAX_PILOTOS];
    int numThreads = 0;
    int expediente = 8;
    int pessoas = 0;
    int aux = 0;

    // Define arrays de threads
    pthread_t tCrianca[MAX_THREADS], tAdolescente[MAX_THREADS], tAdulto[MAX_THREADS];

    // Inicializa semáforo 
    sem_init(&capacetes, 0, NUM_CAPACETES);
    sem_init(&karts, 0, NUM_KARTS);
    sem_init(&mutex, 0, 1);
    sem_init(&m_fila, 0, 1);

    // Abrindo o arquivo que contém os nomes dos clietes
    FILE* file = fopen( "Clientes.txt", "r" ); 
    if ( file == NULL ) {
        perror( "\nErro: nao foi possivel abrir o arquivo.\n" );
        return EXIT_FAILURE;
    }

    int totalClientes = 0;

    srand(time(NULL));
    // For para simular o expediente
    for ( int hora = 0; hora < expediente; ++hora ) {

        // Pessoas a cada hora
        pessoas = rand() % 9 + 1;
        
        printf("\nTotal de pessoas %d\n", pessoas);
        // Leitura do arquivo e obtenção de dados 
        for ( numThreads = 0; numThreads < pessoas; numThreads++ ) {
            fscanf(file, "%s", piloto[numThreads + aux].nome);
            piloto[numThreads + aux].idade = rand() % 17 + 8;
            piloto[numThreads + aux].tempoDeAluguel = rand() % 40 + 20;
            piloto[numThreads + aux].tempoDeEspera = 0;  
            piloto[numThreads + aux].atendido = FALSE;

            if ( numThreads >= pessoas ) {
                fprintf(stderr, "\nUltrapassou o limite de threads\n");
                break;
            }
        }
        
        // Criação de thread conforme a idade do piloto 
        for ( numThreads = 0; numThreads < pessoas; ++numThreads ) {
            if ( piloto[numThreads + aux].idade < 14 ) {
                pthread_create(&tCrianca[numThreads], NULL, threadCrianca, &piloto[numThreads + aux]);
                pthread_detach(tCrianca[numThreads]); // Isso faz com não seja necessário usar o join e as horas não dependem das threads terminarem
            } else if ( piloto[numThreads + aux].idade < 18 ) {
                pthread_create(&tAdolescente[numThreads], NULL, threadAdolescente, &piloto[numThreads + aux]);
                pthread_detach(tAdolescente[numThreads]);
            } else {
                pthread_create(&tAdulto[numThreads], NULL, threadAdulto, &piloto[numThreads + aux]);
                pthread_detach(tAdulto[numThreads]);
            }
        }
        
        aux += numThreads;
        totalClientes += pessoas;

        sleep(20);
        printf("\nFim da hora %d\n", hora);

    }
    
    sleep(1);
    // Destroi os semáforos
    sem_destroy( &capacetes );
    sem_destroy( &karts );
    sem_destroy( &mutex );
    sem_destroy;( &m_fila );
    
    // Calcula média
    float media = (float) totalWaitTime / clientesAtendidos;

    // Fecha arquivo de nomes 
    fclose( file );

    // Cria arquivo relatório
    FILE* relatorio = fopen( "relatorio-final.txt", "w+" );
    if ( relatorio == NULL ) {
        perror( "\nErro ao abrir o arquivo de relatorio.\n" );
        return EXIT_FAILURE;
    }

    // Imprime clientes não atendidos
    for( int i = 0; i < totalClientes; i++ ) {
        if ( piloto[i].atendido == FALSE ) {
            printf( "\nNome: %s, Idade: %d, Tempo de Espera: %d\n", piloto[i].nome, piloto[i].idade, piloto[i].tempoDeEspera );
        }
    }

    // Escreve no relatório
    fprintf( relatorio, "Relatorio final\n" );
    fprintf( relatorio, "\nTotal de clientes atendidos: %d\n", clientesAtendidos );
    fprintf( relatorio, "Tempo medio de espera: %.2f\n", media );
    fprintf( relatorio, "Capacetes usados: %d\n", capacetesUsados );
    fprintf( relatorio, "Karts usados: %d\n", kartsUsados );
    fprintf( relatorio, "Total de clientes: %d\n", totalClientes );
    fprintf( relatorio, "\nClientes que não foram atendidos:\n" );
    for( int i = 0; i < totalClientes; i++ ) {
        if ( piloto[i].atendido == FALSE ) {
            fprintf( relatorio, "\nNome: %s, Idade: %d, Tempo de Espera: %d\n", piloto[i].nome, piloto[i].idade, piloto[i].tempoDeEspera );
        }
    }

    // Fecha arquivo relatório
    fclose( relatorio );
    return 0;
}

void* threadCrianca( void* crianca ) {
    Cliente* piloto = (Cliente*) crianca;
    printf( "\nThread crianças" );
    printf( "\nNome: %s\nIdade: %d\nTempo: %d\n", piloto->nome, piloto->idade, piloto->tempoDeAluguel );

    // A pessoa entra e vai para a fila
    sem_wait(&m_fila);
    ++fila;
    sem_post(&m_fila);

    // Enquanto não conseguir pegar os recursos, a pessoa fica esperando
    while(1){

        // Se não conseguir pegar um capacete, a pessoa espera
        if ( sem_trywait(&capacetes) != 0 ) {

            sleep(1);
            piloto->tempoDeEspera += 1;
            printf("\nTempo de espera de %s: %d\n", piloto->nome ,piloto->tempoDeEspera);
            addTempo();
            continue;
        }

        printf("\nCapacete pego por %s\n", piloto->nome);
        // Se conseguir pegar um capacete, ele é contabilizado
        sem_wait(&mutex);
        ++capacetesUsados;      
        sem_post(&mutex);

        // Se não conseguir pegar um kart, a pessoa espera
        if ( sem_trywait(&karts) != 0 ){
            sleep(1);
            piloto->tempoDeEspera += 1;
            printf("\nTempo de espera de %s: %d\n", piloto->nome ,piloto->tempoDeEspera); 
            addTempo();
            continue;
        } 

        printf("\nKart pego por %s\n", piloto->nome);
        // Se conseguir pegar um kart, ele é contabilizado
        sem_wait(&mutex);
        ++kartsUsados;
        piloto->atendido = TRUE;
        ++clientesAtendidos;
        sem_post(&mutex);
        break;
        
    }

    // A pessoa usa os recursos pelo tempo gerado aleatóriamente
    sleep( piloto->tempoDeAluguel );

    // A pessoa libera os recursos
    sem_post( &karts );
    sem_post( &capacetes );

    // A pessoa sai da fila e é contabilizada
    sem_wait(&m_fila);
    --fila;
    sem_post(&m_fila);
    
    printf( "\n%s saiu da pista", piloto->nome );
}

void* threadAdolescente ( void* adolescente ) {
    Cliente* piloto = (Cliente*) adolescente;
    printf( "\nThread adolescentes" );
    printf( "\nNome: %s\nIdade: %d\nTempo: %d\n", piloto->nome, piloto->idade, piloto->tempoDeAluguel );

    // A pessoa entra e vai para a fila
    sem_wait(&m_fila);
    ++fila;
    sem_post(&m_fila);

    // Enquanto não conseguir pegar os recursos, a pessoa fica esperando
    while(1){

        // Se não conseguir pegar um capacete, a pessoa espera
        if ( sem_trywait(&capacetes) != 0 ) {

            sleep(1);
            piloto->tempoDeEspera += 1;
            printf("\nTempo de espera de %s: %d\n", piloto->nome ,piloto->tempoDeEspera);
            addTempo();
            continue;
        }

        printf("\nCapacete pego por %s\n", piloto->nome);
        // Se conseguir pegar um capacete, ele é contabilizado
        sem_wait(&mutex);
        ++capacetesUsados;      
        sem_post(&mutex);

        // Se não conseguir pegar um kart, a pessoa espera
        if ( sem_trywait(&karts) != 0 ) {
            sleep(1);
            piloto->tempoDeEspera += 1;
            printf("\nTempo de espera de %s: %d\n", piloto->nome ,piloto->tempoDeEspera);
            addTempo();
            continue;
        } 

        printf("\nKart pego por %s\n", piloto->nome);
        // Se conseguir pegar um kart, ele é contabilizado
        sem_wait(&mutex);
        ++kartsUsados;
        piloto->atendido = TRUE;
        ++clientesAtendidos;
        sem_post(&mutex);
        break;
        
    }

    sleep( piloto->tempoDeAluguel );
    // A pessoa libera os recursos
    sem_post( &karts );
    sem_post( &capacetes );

    // A pessoa sai da fila e é contabilizada
    sem_wait(&m_fila);
    --fila;
    sem_post(&m_fila);

    printf( "\n%s saiu da pista", piloto->nome );
}

void* threadAdulto( void* adulto ) {
    Cliente* piloto = (Cliente*) adulto;
    
    printf( "\nThread adultos" );
    printf( "\nNome: %s\nIdade: %d\nTempo: %d\n", piloto->nome, piloto->idade, piloto->tempoDeAluguel );

    sem_wait(&m_fila);
    ++fila;
    sem_post(&m_fila);

    // Enquanto não conseguir pegar os recursos, a pessoa fica esperando
    while(1){
        
        // Se não conseguir pegar um kart, a pessoa espera
        if (sem_trywait(&karts) != 0){
            sleep(1);
            piloto->tempoDeEspera += 1;
            printf("\nTempo de espera de %s: %d\n", piloto->nome ,piloto->tempoDeEspera);
            addTempo();
            continue;
        }

        // Se conseguir pegar um kart, ele é contabilizado
        printf("\nKart pego por %s\n", piloto->nome);
        sem_wait(&mutex);
        ++kartsUsados;      
        sem_post(&mutex); 

        // Se não conseguir pegar um capacete, a pessoa espera
        if (sem_trywait(&capacetes) != 0){
            sleep(1);
            piloto->tempoDeEspera += 1;
            printf("\nTempo de espera de %s: %d\n", piloto->nome ,piloto->tempoDeEspera);
            addTempo();
            continue;
        } 
        printf("\nCapacete pego por %s\n", piloto->nome);
        // Se conseguir pegar um capacete, ele é contabilizado
        sem_wait(&mutex);
        ++capacetesUsados;
        piloto->atendido = TRUE;
        ++clientesAtendidos;
        sem_post(&mutex);
        break;
    }

    sleep( piloto->tempoDeAluguel );

    sem_post( &capacetes );
    sem_post( &karts );

    sem_wait(&m_fila);
    --fila;
    sem_post(&m_fila);

    printf( "\n%s saiu da pista", piloto->nome );
}

// Função adiciona ao tempo total de espera por todas as pessoas
void addTempo() {
    sem_wait(&mutex);
    ++totalWaitTime;
    sem_post(&mutex);
}