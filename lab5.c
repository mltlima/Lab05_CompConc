/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Implementação e uso de sincronização por barreira */

/***** Condicao logica da aplicacao: a cada iteracao, as threads fazem uma parte do processamento e só podem continuar depois que todas as threads completaram essa iteração. ****/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>



/* Variaveis globais */
int nThreads;
int *vetor;
int tam;
int potencia;
int bloqueadas = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

//--------------------------------------------------------------

//Alocacao na memoria usando malloc
//inicia a matriz com valores aleatorios
//variavel tamanho matriz quadrada NxN
void alocarMatrix(int tamanho) {

    
    //Alocacao de um vetor
    vetor = (int*) malloc(tamanho * sizeof(int));

    if (vetor == NULL) {
        printf ("Erro ao alocar memoria");
        return ;
    }

    // aloca memoria para cada vetor da matriz
    for (int i = 0; i < tamanho; i++) {
        vetor[i] = (int*) malloc(tamanho * sizeof(int));

        if (vetor[i] == NULL) {
            printf ("Memoria insuficiente");
            return ;
        }
        

    // inicializa matriz com numeros entre -50 e 50
    vetor[i] = (rand() % (2 * 51)) - 51;
        
    }
}
//--------------------------------------------------------------

//funcao barreira
void barreira() {
    pthread_mutex_lock(&x_mutex); //inicio secao critica
    if (bloqueadas == (nThreads-1)) { 
      //ultima thread a chegar na barreira
      pthread_cond_broadcast(&x_cond);
      bloqueadas=0;
    } else {
      bloqueadas++;
      pthread_cond_wait(&x_cond, &x_mutex);
    }
    pthread_mutex_unlock(&x_mutex); //fim secao critica
}

//--------------------------------------------------------------



//funcao das threads
void * tarefa (void*args){


    int id = (int)args;
    int aux;
    //printf("Thread %d iniciada\n",id);
    
    int tamBloco = tam/nThreads;  //tamanho do bloco de cada thread 
    int ini = id * tamBloco; //elemento inicial do bloco da thread
    int fim; //elemento final(nao processado) do bloco da thread
    if(id == nThreads-1) fim = tam;
    else fim = ini + tamBloco; //trata o resto se houver
    for(int i=ini; i<fim; i++){
    barreira();
        aux = vetor[ini];
        //printf("ini %d\n",vetor[ini]);
        //printf("ini + 1 %d\n",vetor[ini+1]);
        vetor[ini + 1] += aux;
    barreira();
    }
    
/*
    for(int i = 1; i < nThreads; i *= 2){
		int j = id - i;
		if(j >= 0) aux = vetor[j];
        //printf("teste %d\n",vetor[-1]);
		barreira();

		if(j >= 0) vetor[id] += aux;

		barreira();
	}
*/

   	pthread_exit(NULL);
}

//--------------------------------------------------------------



/* Funcao principal */
int main(int argc, char *argv[]) {

  pthread_t threads[nThreads];
  int id[nThreads];
  //int tam, potencia;

  /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
  pthread_mutex_init(&x_mutex, NULL);
  pthread_cond_init (&x_cond, NULL);
/*
  //recebe e valida os parametros de entrada (dimensao do vetor(potencia de 2), numero de threads)
    if(argc < 3) {
        fprintf(stderr, "Digite: %s <dimensao sequencia> <numero threads>\n", argv[0]);
        return 1; 
    }
    potencia = atoi(argv[1]);
    tam = (2 << (potencia+1) );//dimensao do vetor bitshit potencia de 2
    printf("tamanho do vetor %d\n",tam);
    nThreads = atoi(argv[2]);
*/

    potencia = 2;
    tam = (2 << (potencia+1) );
    nThreads = 4;


  // Cria vetor aleatorio de tamanho especificado
  srand(time(NULL));//seed para geracao de numeros aleatorios
  alocarMatrix(tam);
  int count = 0;
  printf("\nVetor \n");
    for (int i = 0; i < tam ; i++) {
        printf("%d ", vetor[i]);
        count ++;
        if (count == potencia * potencia){
            printf("\n");
            count = 0;
        }
        
    }


    //Cria as threads 
    for(int i=0;i<nThreads;i++) {
    //printf("id i = %d\n",id[i]);
    pthread_create(&threads[i], NULL, tarefa, (void *) i+1);
  }

  //Espera todas as threads completarem
  for (int i = 0; i < nThreads; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("\nResultado \n");
    for (int i = 0; i < tam ; i++) {
        printf("%d ", vetor[i]);
        count ++;
        if (count == potencia * potencia){
            printf("\n");
            count = 0;
        }
        
    }

  printf ("FIM.\n");

  //Desaloca variaveis e termina
  pthread_mutex_destroy(&x_mutex);
  pthread_cond_destroy(&x_cond);
  return 0;

}
