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


//pthread_mutex_t buffer_empty;
pthread_mutex_t buffer_in_use;
pthread_mutex_t buffer_ready;
pthread_cond_t buffer_empty;
pthread_mutex_t buffer_complete;

typedef struct {
    int start,id, end;

} threadConsumer;

threadConsumer *threads_consumer_data;

void *producer(void *param){
   
    int i,j;

    image = open(&width,&height,&channels,NULL,(long)(int) param);
    buffer = malloc(height * sizeof(unsigned char *));
    unsigned char *copyImg = image;
    
    for(i=0;i<height;i++){
        buffer[i] = (unsigned char *) malloc(width * 3);
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

    //printf("%d\n",j);
    printf("hebra id: %d, hebra inicio: %d, hebra fin: %d\n",threads_consumer_data[j].id,threads_consumer_data[j].start,threads_consumer_data[j].end);

}

void processed_images(int number){
    // m: cantidad_total_filas_imagen/ cantidad_de_hebras
    int max,i,j,m;
    int *resultsClasifications;

    int threads_size = 10;
    
    // como hay tres imagenes si se ingresa un valor mayor a 3 se guarda el 3 como maximo.

    pthread_t t_productor;
    pthread_t t_consumidor[10];

    //pthread_cond_init(&buffer_empty,NULL);
    //pthread_mutex_init(&buffer_in_use,NULL);
    //pthread_mutex_init(&buffer_ready,NULL);
    //pthread_mutex_lock(&buffer_ready);


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

        // fin de cada hebra

        for(j=0; j < threads_size; j++){
            pthread_join(t_consumidor[j],NULL);
        }

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
