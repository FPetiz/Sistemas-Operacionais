#include <stdio.h>
#include <stdlib.h>
#define HAVE_STRUCT_TIMESPEC
#include <pthread.h> 
#include <semaphore.h>
#include <unistd.h> 
#include <string.h>
#include <time.h> /*-- para quando formos gerar o tempo de cada um
ou recebemos o tempo pelo txt também? */
#define TRUE 1
#define FALSE 0
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

// typedef struct ClienteNaFila {
//     Cliente cliente;
// } ClienteNaFila;

// ClienteNaFila *clientesNaFila[80];

Cliente listaNaoAtendidos[MAX_THREADS];

sem_t capacetes;
sem_t karts;
sem_t mutex;
sem_t m_fila;

// Protótipos das funções
void* threadCrianca(void* crianca);
void* threadAdolescente(void* adolescente);
void* threadAdulto(void* adulto);
void retiraFila(Cliente* piloto);
void c_pegaRecursos(Cliente* piloto);
void a_pegaRecursos(Cliente* piloto);
void addTempo();

int main() {

    // Variáveis 
    Cliente piloto[MAX_THREADS];
    Cliente pilotoFila[MAX_THREADS];
    int numThreads = 0, atual = 0;
    int expediente = 2;
    int pessoas = 0;

    pthread_t tCrianca[MAX_THREADS], tAdolescente[MAX_THREADS], tAdulto[MAX_THREADS];

    // Inicializa semáforo - não sei porque seria 0 ou 1 -- 0 ou 1 indica se vai ser compartilhado ou não
    sem_init(&capacetes, 0, NUM_CAPACETES);
    sem_init(&karts, 0, NUM_KARTS);
    sem_init(&mutex, 0, 1);
    sem_init(&m_fila, 0, 1);

    // Abrindo o arquivo
    FILE* file = fopen( "Clientes-somente-nome.txt", "r" ); // O meu por algum motivo só lê o arquivo se eu colocar o caminho completo
    if ( file == NULL ) {
        perror( "\nErro: nao foi possivel abrir o arquivo.\n" );
        return EXIT_FAILURE;
    }

    int total = numThreads;

    srand(time(NULL));
    // For para simular o expediente
    for (int hora = 0; hora < expediente; ++hora) {

        // pessoas = rand() % 10 + 1;
        pessoas = 10;
        printf("\nTotal de pessoas %d\n", pessoas);
        // Leitura do arquivo e obtenção de dados -- Transformei os whiles em fors
        for (numThreads = 0; numThreads < pessoas; numThreads++) {
            // fscanf(file, "%s %d %d", piloto[numThreads].nome, &piloto[numThreads].idade, &piloto[numThreads].tempoDeAluguel);
            fscanf(file, "%s", piloto[numThreads].nome);
            piloto[numThreads].idade = rand() % 15 + 8;
            piloto[numThreads].tempoDeAluguel = rand() % 25 + 5;
            piloto[numThreads].tempoDeEspera = 0;  
            piloto[numThreads].atendido = FALSE;

            if (numThreads >= pessoas) {
                fprintf(stderr, "\nUltrapassou o limite de threads\n");
                break;
            }
        }
        
        // Criação de thread conforme a idade do piloto -- Transformei os whiles em fors
        for (numThreads = 0; numThreads < pessoas; ++numThreads) {
            if (piloto[numThreads].idade < 14) {
                pthread_create(&tCrianca[numThreads], NULL, threadCrianca, &piloto[numThreads]);
                pthread_detach(tCrianca[numThreads]); // Isso faz com não seja necessário usar o join e as horas não dependem das threads terminarem
            } else if (piloto[numThreads].idade < 18) {
                pthread_create(&tAdolescente[numThreads], NULL, threadAdolescente, &piloto[numThreads]);
                pthread_detach(tAdolescente[numThreads]);
            } else {
                pthread_create(&tAdulto[numThreads], NULL, threadAdulto, &piloto[numThreads]);
                pthread_detach(tAdulto[numThreads]);
            }
        }

        sleep(20);
        printf("\nFim da hora %d\n", hora);

    }
    
    sleep(10);
    // Destroi os semáforos
    sem_destroy( &capacetes );
    sem_destroy( &karts );
    
    float media = (float) totalWaitTime / clientesAtendidos;

    // printf("\nClientes na fila:\n");
    // for (int i = 0; i < filaTotal; i++) {
    //     if (clientesNaFila[i]->cliente.atendido == FALSE)
    //         printf("\nNome: %s\nIdade: %d\nTempo de espera: %d\n", clientesNaFila[i]->cliente.nome, clientesNaFila[i]->cliente.idade, clientesNaFila[i]->cliente.tempoDeEspera);
    // }

    // Fecha arquivo de nomes 
    fclose( file );

    // Cria arquivo relatório
    FILE* relatorio = fopen( "relatorio-final.txt", "w" );
    if ( relatorio == NULL ) {
        perror( "\nErro ao abrir o arquivo de relatorio.\n" );
        return EXIT_FAILURE;
    }

    fprintf( relatorio, "\nRelatorio final\n" );
    fprintf( relatorio, "Total de clientes atendidos: %d\n", clientesAtendidos);
    fprintf( relatorio, "Tempo medio de espera: %.2f\n", media );
    fprintf( relatorio, "Capacetes usados: %d\n", capacetes );
    fprintf( relatorio, "Karts usados: %d\n", karts );
    for ( int i = 0; i < clientesNaoAtendidos; i++ ) {
        fprintf( relatorio, "Nome: %s, Idade: %d, Tempo de Espera: %d\n",
        listaNaoAtendidos[i].nome, listaNaoAtendidos[i].idade, listaNaoAtendidos[i].tempoDeEspera );
    }

    return 0;
}


void* threadCrianca( void* crianca ) {
    Cliente* piloto = (Cliente*) crianca;
    printf( "\nThread crianças" );
    printf( "\nNome: %s\nIdade: %d\nTempo: %d\n", piloto->nome, piloto->idade, piloto->tempoDeAluguel );

    sem_wait(&m_fila);
    // clientesNaFila[fila].cliente = *piloto;
    ++fila;
    sem_post(&m_fila);

    c_pegaRecursos(piloto);

    sleep( piloto->tempoDeAluguel );

    sem_post( &karts );
    sem_post( &capacetes );

    sem_wait(&m_fila);
    // retiraFila(piloto);
    --fila;
    ++clientesAtendidos;
    sem_post(&m_fila);
    
    printf( "\nThread criança finalizada" );
}

void* threadAdolescente ( void* adolescente) {
    Cliente* piloto = (Cliente*) adolescente;
    printf( "\nThread adolescentes" );

    printf( "\nNome: %s\nIdade: %d\nTempo: %d\n", piloto->nome, piloto->idade, piloto->tempoDeAluguel );

    sem_wait(&m_fila);
    // clientesNaFila[fila].cliente = *piloto;
    ++fila;
    sem_post(&m_fila);

    c_pegaRecursos(piloto);

    sleep( piloto->tempoDeAluguel );

    sem_post( &karts );
    sem_post( &capacetes );

    sem_wait(&m_fila);
    // retiraFila(piloto);
    --fila;
    ++clientesAtendidos;
    sem_post(&m_fila);

    printf( "\nThread adolescente finalizada" );
}

void* threadAdulto( void* adulto ) {
    Cliente* piloto = (Cliente*) adulto;
    printf( "\nThread adultos" );

    printf( "\nNome: %s\nIdade: %d\nTempo: %d\n", piloto->nome, piloto->idade, piloto->tempoDeAluguel );

    sem_wait(&m_fila);
    // clientesNaFila[filaTotal]->cliente = *piloto;
    ++fila;
    // ++filaTotal;
    sem_post(&m_fila);

    a_pegaRecursos(piloto);
    Cliente* piloto = (Cliente*) adulto;
    printf( "\nThread adultos" );

    printf( "\nNome: %s\nIdade: %d\nTempo: %d\n", piloto->nome, piloto->idade, piloto->tempoDeAluguel );

    sem_wait(&m_fila);
    // clientesNaFila[filaTotal]->cliente = *piloto;
    ++fila;
    // ++filaTotal;
    sem_post(&m_fila);

    a_pegaRecursos(piloto);

    sleep( piloto->tempoDeAluguel );
    sleep( piloto->tempoDeAluguel );

    sem_post( &capacetes );
    sem_post( &karts );

    sem_wait(&m_fila);
    // retiraFila(piloto);
    --fila;
    ++clientesAtendidos;
    sem_post(&m_fila);

    printf( "\nThread adultos finalizada" );
}

// void retiraFila(Cliente* piloto) {
//     for (int i = 0; i < fila; i++) {
//         if (strcmp(clientesNaFila[i]->cliente.nome, piloto->nome) == 0) {
//             for (int j = i; j < fila - 1; j++) {
//                 clientesNaFila[j] = clientesNaFila[j + 1];
//             }
//             --fila;
//             break;
//         }
//     }

//     ++clientesAtendidos;
// }

void c_pegaRecursos( Cliente* piloto ) {
    while(1){

        if ( sem_trywait(&capacetes) != 0 ){

            sleep(1);
            ++piloto->tempoDeEspera;
            sem_wait(&m_fila);
            // clientesNaFila[filaTotal]->cliente = *piloto;
            sem_post(&m_fila);
            addTempo();
            continue;
        }

        sem_wait(&mutex);
        ++capacetesUsados;      
        sem_post(&mutex);

        if ( sem_trywait(&karts) != 0 ) {
            sem_post(&capacetes);
            sleep(1);
            ++piloto->tempoDeEspera;
            sem_wait(&m_fila);
            // clientesNaFila[filaTotal]->cliente = *piloto;
            sem_post(&m_fila);
            addTempo();
            continue;
        } 

        sem_wait(&mutex);
        ++kartsUsados;
        piloto->atendido = TRUE;
        sem_post(&mutex);
        break;
        
        if ( piloto->atendido == FALSE ) {
            listaNaoAtendidos[clientesNaoAtendidos++] = *piloto;
        }
    }
}

void a_pegaRecursos( Cliente* piloto ) {
    while(1){
        if ( sem_trywait(&karts) != 0 ) {
            sleep(1);
            ++piloto->tempoDeEspera;
            sem_wait(&m_fila);
            // clientesNaFila[filaTotal]->cliente = *piloto;
            sem_post(&m_fila);
            addTempo();
            continue;
        }

        sem_wait(&mutex);
        ++kartsUsados;      
        sem_post(&mutex); 

        if ( sem_trywait(&capacetes ) != 0) {
            sem_post(&karts);
            sleep(1);
            ++piloto->tempoDeEspera;
            sem_wait(&m_fila);
            // clientesNaFila[filaTotal]->cliente = *piloto;
            sem_post(&m_fila);
            addTempo();
            continue;
        } 

        sem_wait(&mutex);
        ++capacetesUsados;
        piloto->atendido = TRUE;
        sem_post(&mutex);
        break;

        if (piloto->atendido) {

        }
    }
}

void addTempo(){
    sem_wait(&mutex);
    ++totalWaitTime;
    sem_post(&mutex);
}