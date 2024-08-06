#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <pthread.h>
//#include <semaphore.h>
//#include <sys/syscall.h>
/*#include <time.h> -- para quando formos gerar o tempo de cada um
ou recebemos o tempo pelo txt também? */
#define MAX_THREADS 5

typedef struct Cliente{
    char nome[50];
    int idade;
    int tempo; // Coloquei em int, mas podemos mudar para float dependendo do que for melhor
} Cliente;

// Globais
//sem_t capacetes;
//sem_t karts;

//void* threadC(void* crianca);
//void* threadA(void* adulto);

void andaKart(char *nome, int tempo); 

int main() {

    // Abrindo o arquivo
    FILE* file; 
    file = fopen("F:/Code/SO/Sistemas-Operacionais/Clientes.txt", "r"); // Eu só consegui fazer isso funcionar colocando todo o caminho do arquivo, achar uma maneira de corrigir isso depois
    if ( file == NULL ) {
        perror("\nErro: nao foi possivel abrir o arquivo.\n");
        return EXIT_FAILURE;
    }

    Cliente piloto[MAX_THREADS];
    int threads = 0;

    //sem_init(&capacetes, 1, 10);
    //sem_init(&karts, 1, 10);
    
    // Leitura do arquivo e obtenção de dados
    while( threads < MAX_THREADS ) {
        fscanf(file, "%s %d %d", piloto[threads].nome, &piloto[threads].idade, &piloto[threads].tempo);
        threads++;

        if ( threads > MAX_THREADS ) {
            fprintf(stderr, "\nUltrapassou o limite de threads\n");
            break;
        }
    } 
    
    // Não sei se é realmente necessário mas isso é uma lista de clientes com ponteiro para o endereço de cada um
    Cliente *priori[MAX_THREADS];
/*
    pthread_t tCrianca, tAdulto;
    pthread_create(&tCrianca, NULL, threadC, NULL);
    pthread_create(&tAdulto, NULL, threadA, NULL);
*/
    // Fiz isso para poder fazer a verificação de prioridade, senão começava em uma posição inválida (MAX_THREADS)
    // --threads;
    int i = 0;
    threads = 0;
    // Quem sabe a gente com essa informação de prioridade não faça uma lista de prioridades e vá atendendo conforme a prioridade?
    while( threads < MAX_THREADS ) {
        if ( piloto[threads].idade < 14 && piloto[threads].idade > 6 ) {
            printf("\n%s tem maior prioridade para capacete e baixa para kart.", piloto[threads].nome);
            //pthread_join(tCrianca, NULL);
            priori[i] = &piloto[threads];
            ++i;
            // andaKart(piloto[threads].nome, piloto[threads].tempo);
        
        } else if ( piloto[threads].idade > 13 && piloto[threads].idade < 18 ) {
            printf("\n%s tem media prioridade para capacete e baixa para kart.", piloto[threads].nome);
            priori[i] = &piloto[threads];
            ++i;
            // andaKart(piloto[threads].nome, piloto[threads].tempo);

        }else {
            printf("\n%s tem baixa prioridade para capacete e alta para kart.", piloto[threads].nome);
            priori[i] = &piloto[threads];
            ++i;
             //pthread_join(tAdulto, NULL);
            // andaKart(piloto[threads].nome, piloto[threads].tempo);
        }
        sleep(1);
        ++threads;
    } 

    // Exibindo os dados pra checar se está tudo certo
    for(int i = 0; i < MAX_THREADS; ++i) {
        printf("\n%s %d %d", priori[i]->nome, priori[i]->idade, priori[i]->tempo);
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

// Função para simular o tempo de cada um andando de kart
void andaKart(char *nome, int tempo) {
    printf("\n%s andando...", nome);
    sleep(tempo);
}
