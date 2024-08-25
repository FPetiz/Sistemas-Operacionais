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
void c_pegaRecursos(Cliente* piloto);
void a_pegaRecursos(Cliente* piloto);
void addTempo();

int main() {

    // Variáveis 
    Cliente piloto[MAX_PILOTOS];
    Cliente pilotoFila[MAX_THREADS];
    int numThreads = 0, atual = 0;
    int expediente = 2;
    int pessoas = 0;
    int aux = 0;

    pthread_t tCrianca[MAX_THREADS], tAdolescente[MAX_THREADS], tAdulto[MAX_THREADS];

    // Inicializa semáforo - não sei porque seria 0 ou 1 -- 0 ou 1 indica se vai ser compartilhado ou não
    sem_init(&capacetes, 0, NUM_CAPACETES);
    sem_init(&karts, 0, NUM_KARTS);
    sem_init(&mutex, 0, 1);
    sem_init(&m_fila, 0, 1);

    // Abrindo o arquivo
    FILE* file = fopen( "F:\\Code\\Sistemas-Operacionais\\Clientes-somente-nome.txt", "r" ); // O meu por algum motivo só lê o arquivo se eu colocar o caminho completo
    if ( file == NULL ) {
        perror( "\nErro: nao foi possivel abrir o arquivo.\n" );
        return EXIT_FAILURE;
    }

    int totalClientes = 0;

    srand(time(NULL));
    // For para simular o expediente
    for (int hora = 0; hora < expediente; ++hora) {

        // pessoas = rand() % 10 + 1;
        pessoas = 10;
        printf("\nTotal de pessoas %d\n", pessoas);
        // Leitura do arquivo e obtenção de dados -- Transformei os whiles em fors
        for (numThreads = 0; numThreads < pessoas; numThreads++) {
            // fscanf(file, "%s %d %d", piloto[numThreads].nome, &piloto[numThreads].idade, &piloto[numThreads].tempoDeAluguel);
            fscanf(file, "%s", piloto[numThreads + aux].nome);
            piloto[numThreads + aux].idade = rand() % 15 + 8;
            piloto[numThreads + aux].tempoDeAluguel = rand() % 25 + 5;
            piloto[numThreads + aux].tempoDeEspera = 0;  
            piloto[numThreads + aux].atendido = FALSE;

            if (numThreads >= pessoas) {
                fprintf(stderr, "\nUltrapassou o limite de threads\n");
                break;
            }
        }
        
        // Criação de thread conforme a idade do piloto -- Transformei os whiles em fors
        for (numThreads = 0; numThreads < pessoas; ++numThreads) {
            if (piloto[numThreads + aux].idade < 14) {
                pthread_create(&tCrianca[numThreads], NULL, threadCrianca, &piloto[numThreads + aux]);
                pthread_detach(tCrianca[numThreads]); // Isso faz com não seja necessário usar o join e as horas não dependem das threads terminarem
            } else if (piloto[numThreads + aux].idade < 18) {
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
    
    float media = (float) totalWaitTime / clientesAtendidos;

    // Fecha arquivo de nomes 
    fclose( file );

    // Cria arquivo relatório
    FILE* relatorio = fopen( "F:\\Code\\Sistemas-Operacionais\\relatorio-final.txt", "w+" );
    if ( relatorio == NULL ) {
        perror( "\nErro ao abrir o arquivo de relatorio.\n" );
        return EXIT_FAILURE;
    }

    for( int i = 0; i < totalClientes; i++ ) {
        if ( piloto[i].atendido == FALSE ) {
            printf( "\nNome: %s, Idade: %d, Tempo de Espera: %d\n", piloto[i].nome, piloto[i].idade, piloto[i].tempoDeEspera );
        }
    }

    fprintf( relatorio, "Relatorio final\n" );
    fprintf( relatorio, "\nTotal de clientes atendidos: %d\n", clientesAtendidos);
    fprintf( relatorio, "Tempo medio de espera: %.2f\n", media );
    fprintf( relatorio, "Capacetes usados: %d\n", capacetesUsados );
    fprintf( relatorio, "Karts usados: %d\n", kartsUsados );

    fprintf( relatorio, "\nClientes que não foram atendidos:\n" );
    for( int i = 0; i < totalClientes; i++ ) {
        if ( piloto[i].atendido == FALSE ) {
            fprintf( relatorio, "\nNome: %s, Idade: %d, Tempo de Espera: %d\n", piloto[i].nome, piloto[i].idade, piloto[i].tempoDeEspera );
        }
    }

    fclose( relatorio );
    return 0;
}


void* threadCrianca( void* crianca ) {
    sem_wait(&m_fila);
    Cliente* piloto = (Cliente*) crianca;
    printf( "\nThread crianças" );
    printf( "\nNome: %s\nIdade: %d\nTempo: %d\n", piloto->nome, piloto->idade, piloto->tempoDeAluguel );

    // A pessoa entra e vai para a fila
    ++fila;
    sem_post(&m_fila);
    c_pegaRecursos(piloto);

    // A pessoa pega os recursos

    // A pessoa usa os recursos pelo tempo gerado aleatóriamente
    sleep( piloto->tempoDeAluguel );

    // A pessoa libera os recursos
    sem_post( &karts );
    sem_post( &capacetes );

    // A pessoa sai da fila e é contabilizada
    sem_wait(&m_fila);
    --fila;
    ++clientesAtendidos;
    piloto->atendido = TRUE;
    sem_post(&m_fila);
    
    printf( "\n%s saiu da pista", piloto->nome );
}

void* threadAdolescente ( void* adolescente) {
    sem_wait(&m_fila);
    Cliente* piloto = (Cliente*) adolescente;
    printf( "\nThread adolescentes" );

    printf( "\nNome: %s\nIdade: %d\nTempo: %d\n", piloto->nome, piloto->idade, piloto->tempoDeAluguel );

    // A pessoa entra e vai para a fila
    ++fila;

    sem_post(&m_fila);
    // A pessoa pega os recursos
    c_pegaRecursos(piloto);

    // A pessoa usa os recursos pelo tempo gerado aleatóriamente
    sleep( piloto->tempoDeAluguel );

    // A pessoa libera os recursos
    sem_post( &karts );
    sem_post( &capacetes );

    // A pessoa sai da fila e é contabilizada
    sem_wait(&m_fila);
    --fila;
    ++clientesAtendidos;
    piloto->atendido = TRUE;
    sem_post(&m_fila);

    printf( "\n%s saiu da pista", piloto->nome );
}

void* threadAdulto( void* adulto ) {
    sem_wait(&m_fila);
    Cliente* piloto = (Cliente*) adulto;
    printf( "\nThread adultos" );

    printf( "\nNome: %s\nIdade: %d\nTempo: %d\n", piloto->nome, piloto->idade, piloto->tempoDeAluguel );

    
    // clientesNaFila[filaTotal]->cliente = *piloto;
    ++fila;
    // ++filaTotal;
    sem_post(&m_fila);

    a_pegaRecursos(piloto);


    sleep( piloto->tempoDeAluguel );

    sem_post( &capacetes );
    sem_post( &karts );

    sem_wait(&m_fila);
    --fila;
    ++clientesAtendidos;
    piloto->atendido = TRUE;
    sem_post(&m_fila);

    printf( "\n%s saiu da pista", piloto->nome );
}

// Pega recursos para crianças e adolescentes
void c_pegaRecursos(Cliente* piloto) {

    // Enquanto não conseguir pegar os recursos, a pessoa fica esperando
    while(1){

        // Se não conseguir pegar um capacete, a pessoa espera
        if (sem_trywait(&capacetes) != 0){

            sleep(1);
            sem_wait(&m_fila);
            piloto->tempoDeEspera += 1;
            printf("\nTempo de espera de %s: %d\n", piloto->nome ,piloto->tempoDeEspera);
            // clientesNaFila[filaTotal]->cliente = *piloto;
            sem_post(&m_fila);
            addTempo();
            continue;
        }

        // Se conseguir pegar um capacete, ele é contabilizado
        sem_wait(&mutex);
        ++capacetesUsados;      
        sem_post(&mutex);

        // Se não conseguir pegar um kart, a pessoa espera
        if (sem_trywait(&karts) != 0){
            sem_post(&capacetes);
            sleep(1);
            sem_wait(&m_fila);
            piloto->tempoDeEspera += 1;
            printf("\nTempo de espera de %s: %d\n", piloto->nome ,piloto->tempoDeEspera);
            // clientesNaFila[filaTotal]->cliente = *piloto;
            sem_post(&m_fila);
            addTempo();
            continue;
        } 

        // Se conseguir pegar um kart, ele é contabilizado
        sem_wait(&mutex);
        ++kartsUsados;
        // piloto->atendido = TRUE;
        sem_post(&mutex);
        break;
        
    }
}

// Pega recursos para adultos
void a_pegaRecursos(Cliente* piloto) {

    // Enquanto não conseguir pegar os recursos, a pessoa fica esperando
    while(1){
        
        // Se não conseguir pegar um kart, a pessoa espera
        if (sem_trywait(&karts) != 0){
            sleep(1);
            sem_wait(&m_fila);
            piloto->tempoDeEspera += 1;
            printf("\nTempo de espera de %s: %d\n", piloto->nome ,piloto->tempoDeEspera);
            sem_post(&m_fila);
            addTempo();
            continue;
        }

        // Se conseguir pegar um kart, ele é contabilizado
        sem_wait(&mutex);
        ++kartsUsados;      
        sem_post(&mutex); 

        // Se não conseguir pegar um capacete, a pessoa espera
        if (sem_trywait(&capacetes) != 0){
            sem_post(&karts);
            sleep(1);
            sem_wait(&m_fila);
            piloto->tempoDeEspera += 1;
            printf("\nTempo de espera de %s: %d\n", piloto->nome ,piloto->tempoDeEspera);
            sem_post(&m_fila);
            addTempo();
            continue;
        } 

        // Se conseguir pegar um capacete, ele é contabilizado
        sem_wait(&mutex);
        ++capacetesUsados;
        sem_post(&mutex);
        break;
    }
}

// Função adiciona ao tempo total de espera por todas as pessoas
void addTempo() {
    sem_wait(&mutex);
    ++totalWaitTime;
    sem_post(&mutex);
}