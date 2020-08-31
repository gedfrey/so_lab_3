#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "data.h"
#include "general.h"
#include "functions_images.h"
#include <pthread.h>

int count;

int count_ready_thread;

pthread_mutex_t mutex_classification;

pthread_barrier_t barrier_gray;

pthread_barrier_t barrier_laplaciana;

pthread_barrier_t barrier_binarization; 

// Estructura para las hebras

typedef struct {
    int start,id, end,n_image;

} threadConsumer;

threadConsumer *threads_consumer_data;


// Entradas: una matrix de pixeles, el inicio , el fin y el ancho de la imagen
// Funcionamiento: cada hebra ejecuta por cada fila asignada la clasificacion

int classification(unsigned char **buffer_laplaciano,int start,int end, int width){
    int i,j;

    pthread_mutex_lock(&mutex_classification);

    count_ready_thread++;

    for(i=start;i<end;i++){
        for(j=0;j<width;j++){
            if(buffer_laplaciano[i][j] == 0){
                count += 1;
            }
        }
    }

    pthread_mutex_unlock(&mutex_classification);

    return 0;
}

// Entradas: el numero de la imagen
// Funcionamiento: esta hebra lee la image y la guarda en una matrix


void *producer(void *param){
    
    int id = (intptr_t) param;
    int i,j;
    count = 0;
    count_ready_thread = 0;
    image = open(&width,&height,&channels,NULL,id);
    buffer = malloc(height * sizeof(unsigned char *));
    buffer_gray = malloc(height * sizeof(unsigned char *));
    buffer_laplaciano = malloc(height * sizeof(unsigned char *));


    unsigned char *copyImg = image;
    
    for(i=0;i<height;i++){
        buffer[i] = (unsigned char *) malloc(width * 3);
    }

    for(i=0;i<height;i++){
        buffer_gray[i] = (unsigned char *) malloc(width);
    }

    for(i=0;i<height;i++){
        buffer_laplaciano[i] = (unsigned char *) malloc(width);
    }


    for(i=0;i < height;i++){

        for(j=0;j < width * 3; j++){
            buffer[i][j] = *copyImg;
            copyImg++;
        }
    }

    pthread_exit(NULL);

}

// Entradas: el numero de la imagen
// Funcionamiento: genera el pipeline para cada hebra, la hebra tiene asignada un numero de fila, al final imprime y guarda la imagen

void *consumer(void *param){
    
    int j = (intptr_t) param;

    grayProccess(buffer,buffer_gray,threads_consumer_data[j].start,threads_consumer_data[j].end, width, channels);

    pthread_barrier_wait(&barrier_gray);

    filterLapleciano(buffer_gray,buffer_laplaciano,threads_consumer_data[j].start,threads_consumer_data[j].end, width, name_mask_laplaciana);

    pthread_barrier_wait(&barrier_laplaciana);

    binarization(buffer_laplaciano,threads_consumer_data[j].start,threads_consumer_data[j].end, width, umbral_binary);

    pthread_barrier_wait(&barrier_binarization);

    classification(buffer_laplaciano,threads_consumer_data[j].start,threads_consumer_data[j].end, width);


    if(count_ready_thread == n_threads){

        if(display == 1){
            printClassification(count,threads_consumer_data[j].n_image,height,width,umbral_classification);
        }

        writeImage(width, height, 1,buffer_laplaciano,threads_consumer_data[j].n_image);
    }

    pthread_exit(NULL);
}


// Entradas: la cantidad de imagenes
// Funcion: es la que se encarga de procesar las imagenes creadno la hebra productora y las hebras consumidoras

void processed_images(int number){
    // m: cantidad_total_filas_imagen/ cantidad_de_hebras
    int max,i,j,m,barrier_gray_c, barrier_laplaciana_c, barrier_binarization_c;

    pthread_t t_productor;

    pthread_t *t_consumidor;

    t_consumidor = malloc (n_threads * sizeof(pthread_t));

    pthread_mutex_init(&mutex_classification,NULL);

    barrier_gray_c = pthread_barrier_init(&barrier_gray, NULL, n_threads);

    if (barrier_gray_c) {
        fprintf(stderr, "pthread_barrier_init: %s\n", strerror(barrier_gray_c));
        exit(1);
    }

    barrier_laplaciana_c = pthread_barrier_init(&barrier_laplaciana, NULL, n_threads);

    if (barrier_laplaciana_c) {
        fprintf(stderr, "pthread_barrier_init: %s\n", strerror(barrier_laplaciana_c));
        exit(1);
    }

    barrier_binarization_c = pthread_barrier_init(&barrier_binarization, NULL, n_threads);

    if (barrier_binarization_c) {
        fprintf(stderr, "pthread_barrier_init: %s\n", strerror(barrier_binarization_c));
        exit(1);
    }

    if(number < 3){
        max = number;
    }else{
        max = 3;
    }
    
    images = malloc(max * sizeof(unsigned char *));
    
    // generando data hebras

    threads_consumer_data = malloc(n_threads * sizeof(threadConsumer));

    // procesando las imagenes

    

    for(i=0; i < max;i++){


        pthread_create(&t_productor,NULL,producer,(void *) (intptr_t) i);
        pthread_join(t_productor,NULL);
        m = height / n_threads;

        for(j=0; j < n_threads; j++){
            threads_consumer_data[j].id = j;
            threads_consumer_data[j].start = j * m;
            threads_consumer_data[j].n_image = i;
            if(j == n_threads - 1){
                threads_consumer_data[j].end = height;
            }else{
                threads_consumer_data[j].end = j * m + m;
            }

            pthread_create(&t_consumidor[j],NULL,consumer,(void *) (intptr_t) j);



        }

        // fin de cada hebra y esperamos cada uno

        for(j=0; j < n_threads; j++){
            pthread_join(t_consumidor[j],NULL);
        }

    }
    
}

// Entradas: sin entradas
// Funcionamiento: valida que los valores ingresados sean correctos

void validate(){
    if(number_of_images == -1){
        printf("Debes ingresar un numero de imagenes\n");
        exit(1);
    }

    if(umbral_binary == -1){
        printf("Debes ingresar un umbral de binarion\n");
        exit(1);
    }

    if(umbral_binary < 0 || umbral_binary > 255){
        printf("Debes ingresar un umbral entre 0 a 255\n");
        exit(1);
    }

    if(umbral_classification == -1){
        printf("Debes ingresar un umbral de clasificación\n");
        exit(1);
    }

    if(umbral_classification < 0 || umbral_classification > 100){
        printf("Debes ingresar un umbral de clasificación entre 0 a 100\n");
        exit(1);
    }
}

// Entradas: cantidad de argumentos y un arreglo de argumentos ingresados por consola
// Funcionamiento: procesa los argumentos ingresados por consola, posteriormente ejecuta la función de validacion y ejecuta el proceso
// 


void getArguments(int argc, char *argv[]){
    int opt;

    while((opt = getopt(argc, argv, "c:h:m:u:n:b")) != -1){
        switch (opt)
        {
        case 'c':
            number_of_images = atoi(optarg);
            if(number_of_images <= 0){
                printf("Error, el numero de imagenes no puede ser 0 o menor \n");
                exit(1);
            }
            break;
        case 'm':
            name_mask_laplaciana = optarg;
            if(name_mask_laplaciana[0] == '\0' || name_mask_laplaciana[0] == '-' || optarg == NULL){
                printf("Error, el nombre de la mascara laplaciana esta vacio \n");
                exit(1);
            }
            break;
        case 'u':
            umbral_binary = atoi(optarg);
            if(umbral_binary < 0){
                printf("Error, el numero debe ser mayor o igual a 0 para el umbral binario \n");
                exit(1);
            }
            break;
        case 'n':
            umbral_classification = atoi(optarg);
            if(umbral_classification < 0){
                printf("Error, el numero debe ser mayor o igual a 0 para el umbral de clasificación \n");
                exit(1);
            }
            break;
        case 'b':
            display = 1;
            break;

        case 'h':
            n_threads = atoi(optarg);
            if(n_threads < 1){
                printf("El numero de hebras mayor que 0\n");
                exit(1);
            }
            break;
        case '?':
            printf("Error bandera ingresada desconocida \n");
            break;
        }

    }

    validate();
    processed_images(number_of_images);
}
