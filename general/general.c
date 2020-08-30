#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "data.h"
#include "general.h"
#include "functions_images.h"
#include <pthread.h>

// Entradas: number representa la cantidad de imagenes a procesar
// Funcionamiento: Esta funcion genera el flujo de procesamiento de imagen desde su apertura hasta la impresión

int count;

//pthread_mutex_t buffer_empty;
// pthread_mutex_t buffer_in_use;
// pthread_mutex_t buffer_ready;
// pthread_cond_t buffer_empty;
// pthread_mutex_t buffer_complete;

pthread_mutex_t mutex_classification;

pthread_barrier_t barrier_gray;

pthread_barrier_t barrier_laplaciana;

pthread_barrier_t barrier_binarization; 

typedef struct {
    int start,id, end;

} threadConsumer;

threadConsumer *threads_consumer_data;


// Entradas: una matrix de pixeles, el ancho , el largo y el umbral de classificacion
// Funcionamiento: por cada posición busca los pixeles negros y los cuenta, posteriormente realiza el calculo
// del porcentaje con la cantidad total de pixeles, que se calcula por el ancho y el largo
// Salidas: un numero que indica el porcentaje de pixeles negros

int classification(unsigned char **buffer_laplaciano,int start,int end, int width, int count){
    int i,j;

    pthread_mutex_lock(&mutex_classification);

    for(i=start;i<end;i++){
        for(j=0;j<width;j++){
            if(buffer_laplaciano[i][j] == 0){
                count += 1;
            }
        }
    }

    pthread_mutex_unlock(&mutex_classification);
  
    // if( ((count * 100)/ total) >= umbral_classification){
    //     return 1;
    // }

    // return 0;
    

}


void *producer(void *param){
   
    int i,j;
    count = 0;
    image = open(&width,&height,&channels,NULL,(long)(int) param);
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


    // size_t img_size = width * height * channels;
    // unsigned char *img = malloc(img_size);
    // unsigned char *p = img;

    // for(i=0;i < height;i++){

    //     for(j=0;j < width * 3; j++){
    //         *p = (uint8_t) buffer[i][j];
    //         p++;
    //     }
    // }

    // writeImage(width, height, 3,img,(long)(int) param);

}

void *consumer(void * param){
    
    int j = (long) (int) param;

    // printf("hebra id: %d, hebra inicio: %d, hebra fin: %d\n",threads_consumer_data[j].id,threads_consumer_data[j].start,threads_consumer_data[j].end);

    grayProccess(buffer,buffer_gray,threads_consumer_data[j].start,threads_consumer_data[j].end, width, channels);

    pthread_barrier_wait(&barrier_gray);

    filterLapleciano(buffer_gray,buffer_laplaciano,threads_consumer_data[j].start,threads_consumer_data[j].end, width, name_mask_laplaciana);

    pthread_barrier_wait(&barrier_laplaciana);

    binarization(buffer_laplaciano,threads_consumer_data[j].start,threads_consumer_data[j].end, width, umbral_binary);

    pthread_barrier_wait(&barrier_binarization);

    classification(buffer_laplaciano,threads_consumer_data[j].start,threads_consumer_data[j].end, width,count);

    // size_t img_size = width * height * 1;
    // unsigned char *img = malloc(img_size);
    // unsigned char *p = img;

    // int k;

    // for(k=0;k < height;k++){

    //     for(j=0;j < width ; j++){
    //         *p = (uint8_t) buffer_gray[k][j];
    //         p++;
    //     }
    // }

    // writeImage(width, height, 1,img,0);


}

void processed_images(int number){
    // m: cantidad_total_filas_imagen/ cantidad_de_hebras
    int max,i,j,m,barrier_gray_c, barrier_laplaciana_c, barrier_binarization_c,total;
    int *resultsClasifications;

    int threads_size = 10;
    
    // como hay tres imagenes si se ingresa un valor mayor a 3 se guarda el 3 como maximo.

    pthread_t t_productor;
    pthread_t t_consumidor[10];

    pthread_mutex_init(&mutex_classification,NULL);

    barrier_gray_c = pthread_barrier_init(&barrier_gray, NULL, threads_size);

    if (barrier_gray_c) {
        fprintf(stderr, "pthread_barrier_init: %s\n", strerror(barrier_gray_c));
        exit(1);
    }

    barrier_laplaciana_c = pthread_barrier_init(&barrier_laplaciana, NULL, threads_size);

    if (barrier_laplaciana_c) {
        fprintf(stderr, "pthread_barrier_init: %s\n", strerror(barrier_laplaciana_c));
        exit(1);
    }

    barrier_binarization_c = pthread_barrier_init(&barrier_binarization, NULL, threads_size);

    if (barrier_binarization_c) {
        fprintf(stderr, "pthread_barrier_init: %s\n", strerror(barrier_binarization_c));
        exit(1);
    }

    if(number < 3){
        max = number;
    }else{
        max = 3;
    }

    resultsClasifications = (int*) malloc(max * sizeof(int));

    
    images = malloc(max * sizeof(unsigned char *));
    
    // generando data hebras

    threads_consumer_data = malloc(threads_size * sizeof(threadConsumer));


    printf("procesando imagenes\n");

    // procesando las imagenes

    

    for(i=0; i < max;i++){


        pthread_create(&t_productor,NULL,producer,(void *) i);
        pthread_join(t_productor,NULL);
        m = height / threads_size;
        printf("valor de height : %d\n",height);
        printf("valor de m : %d\n",m);
        
        // inicio de cada hebra

        for(j=0; j < threads_size; j++){
            threads_consumer_data[j].id = j;
            threads_consumer_data[j].start = j * m;

            if(j == threads_size - 1){
                threads_consumer_data[j].end = height;
            }else{
                threads_consumer_data[j].end = j * m + m;
            }

            printf("creando hebra consumidora\n");

            pthread_create(&t_consumidor[j],NULL,consumer,(void *) j);
            //pthread_join(t_consumidor[j],NULL);



        }

        // for(j=0; j < threads_size; j++){
        //     printf(" hebra id: %d, hebra inicio: %d, hebra fin: %d\n",threads_consumer_data[j].id,threads_consumer_data[j].start,threads_consumer_data[j].end);
        // }

        // fin de cada hebra y esperamos cada uno

        for(j=0; j < threads_size; j++){
            pthread_join(t_consumidor[j],NULL);
        }

        // clasificacion

        total = width * height;

        if( ((count * 100)/ total) >= umbral_classification){
            printf("es nearlyblack\n");
            // return 1;
        }else{
            printf("no es nearlyblack\n");
        }   

        // return 0;


        // se imprime la imagen

        size_t img_size = width * height * 1;
        unsigned char *img = malloc(img_size);
        unsigned char *p = img;

        int k;

        for(k=0;k < height;k++){

            for(j=0;j < width ; j++){
                *p = (uint8_t) buffer_laplaciano[k][j];
                p++;
            }
        }

        writeImage(width, height, 1,img,i);

    }

    printf("finalizando proceso\n");
    
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

    //printf(" argc :%d\n",argc);

    while((opt = getopt(argc, argv, "c:m:u:n:b")) != -1){
        switch (opt)
        {
        case 'c':
            // printf("valor %s",optarg);
            number_of_images = atoi(optarg);
            if(number_of_images <= 0){
                printf("Error, el numero de imagenes no puede ser 0 o menor \n");
                exit(1);
            }
            break;
        case 'm':
            name_mask_laplaciana = optarg;
            //printf("%s",optarg);
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
        case '?':
            printf("Error bandera ingresada desconocida \n");
            break;
        }




    }

    validate();
    processed_images(number_of_images);
}
