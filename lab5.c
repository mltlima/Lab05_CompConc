/* Disciplina: Computacao Concorrente */
/* Miguel Lima Tavares 119161571 */
/* Lab 05 */


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>



/* Variaveis globais */
int nThreads;
int *vetor;
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

    for(int i = 1; i < nThreads; i *= 2){
		int j = id - i;

		if(j >= 0){
            aux = vetor[j];
        } 
		barreira();

		if(j >= 0){
            vetor[id] += aux;
        } 
		barreira();
	}


   	pthread_exit(NULL);
}

//--------------------------------------------------------------



/* Funcao principal */
int main(int argc, char *argv[]) {

  pthread_t *threads;
  int id[nThreads];
  int potencia;



  /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
  pthread_mutex_init(&x_mutex, NULL);
  pthread_cond_init (&x_cond, NULL);



  //recebe e valida os parametros de entrada (dimensao do vetor(potencia de 2), numero de threads)
    if(argc < 2) {
        fprintf(stderr, "Digite: %s <potencia de 2>\n", argv[0]);
        return 1; 
    }
    potencia = atoi(argv[1]);
    nThreads = (2 << (potencia+1) );//dimensao do vetor bitshit potencia de 2 para matriz quadrada 2^potencia x 2^potencia





    //Aloca espaço para as threads
    threads = (pthread_t *) malloc(sizeof(pthread_t) * nThreads);
	if(threads == NULL) {
    	fprintf(stderr, "ERRO - malloc\n");
    	return 2;
	}





  // Cria vetor aleatorio de tamanho especificado
  srand(time(NULL));//seed para geracao de numeros aleatorios
  alocarMatrix(nThreads);
  int count = 0;
  printf("\nVetor \n");
    for (int i = 0; i < nThreads ; i++) {
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
    for (int i = 0; i < nThreads ; i++) {
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
