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


pthread_mutex_t buffer_empty;
pthread_mutex_t buffer_in_use;
pthread_mutex_t buffer_ready;


void *threadProducer(void *param){
    int i;
    printf("iniciando lectura\n");
    for(i=0;i<(int) (long)param;i++){
        free(image);
        free(buffer);
        image = open(&width,&height,&channels,NULL,i);
        printf("width :%d\n",width);
        printf("height :%d\n",height);
        printf("channels :%d\n",channels);
        img_size = width * height * channels;
        buffer = malloc(img_size);
        for(unsigned char *p = image, *pg = buffer; p != image + img_size; p += 1, pg += 1) {
            *pg = (uint8_t) *p ;
        }

        writeImage(width, height, 3,buffer,i);
        //pthread_mutex_unlock(&buffer_ready);
    }
}

void processed_images(int number){
    int max,i;
    int *resultsClasifications;

    // como hay tres imagenes si se ingresa un valor mayor a 3 se guarda el 3 como maximo.

    pthread_t t_productor;
    pthread_t t_consumidor[10];

    pthread_mutex_init(&buffer_empty,NULL);
    pthread_mutex_init(&buffer_in_use,NULL);
    pthread_mutex_init(&buffer_ready,NULL);
    pthread_mutex_lock(&buffer_ready);


    if(number < 3){
        max = number;
    }else{
        max = 3;
    }
    resultsClasifications = (int*) malloc(max * sizeof(int));

    printf("procesando imagenes\n");

    // imagenes
    
    
    images = malloc(max * sizeof(unsigned char *));
    


    pthread_create(&t_productor,NULL,threadProducer,(void *) max);
    pthread_join(t_productor,NULL);
    printf("finalizando proceso\n");
    //for(i=0;i<max;i++){

        // obtenemos un arreglo con tres canales
        //unsigned char *image = open(&width,&height,&channels,images[i],i);

        // del mismo arreglo se realiza la transformacion a gris
        //unsigned char *grayImage = grayProccess(image, width, height, channels);

        // se transforma el arreglo a una matrix
        //unsigned char **matrix = createMatrix(grayImage,width,height);

        // se aplica el filtro lapleciano
        //unsigned char **matrixFilter = filterLapleciano(matrix,width,height,name_mask_laplaciana);

        // se aplica la binarizacion
        //binarization(matrixFilter,width, height,umbral_binary);

        // se clasifica y se obtiene una matrix por cada imagen
        //resultsClasifications[i]=classification(matrixFilter,width,height,umbral_classification);

        // se convierte de matrix a arreglo para usar la funcion de guardar la imagen
        //unsigned char *copyImg = matrixToArray(matrixFilter,width,height);

        // se guarda la imagen
        //writeImage(width, height, 1,copyImg,i);

        // se cierra el archivo
        //closeImage(image);
    //}

    // si se tiene la bandera de display se imprime
    //if(display != 0){
    //    printClassification(resultsClasifications,max);
    //}
    
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
