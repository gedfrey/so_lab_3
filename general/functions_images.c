#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_resize.h"
#include "stb/stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "data_images.h"


// Entradas: numero de filas y columnas
// Funcionamiento: Funcion que crea una matrix( reserva espacio en memoria)
// Salida: una matrix

int** createArray(int n){
    int** array;
    int i;
    
    array = (int** ) malloc (n*sizeof(int *));
    for(i=0;i<n;i++){
        array[i] = (int*) malloc (n*sizeof(int*));
    }

    return array;
}

// Entradas: numero de columnas y filas, el arreglo a imprimir
// Funcionamiento: imprime una matrix

void printArray(int** array,int n){
    int i,j;
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            printf("%d ",array[i][j]);
        }
        printf("\n");
    }
}

// Entradas: nombre del archivo, y cantidad de filas y columnas
// Funcionamiento: lee un archivo, reserva memoria, asigna valores y retorna la matrix
// Salida: una matrix

int** readFile(char name[],int num){
    FILE* file = fopen(name,"rt");
    if(file == NULL){
        printf("Error al abrir el archivo de mascara: %s\n",name);
        exit(1);
    }
    int i,j;
    int** matriz = createArray(num);
    if(file != NULL){
        for(i=0;i<num;i++){
            for(j=0;j<num;j++){
                fscanf(file,"%d",&matriz[i][j]);
            }
        }
    }
    return matriz;
}

// Entradas: una matrix de los pixeles, un filtro que posee enteros, la posición x y y a procesar, el min y un max para no salir del borde
// Funcionamiento: aplica el filtro y entrega el resultado según la posición indicada 
// Salida: el resultado de aplicar el filtro

int calculateFilter(unsigned char **matrix, int **filtro, int y, int x, int min, int maxy, int maxx){
    int i,j,k,l,positionI,positionJ, sum;

    sum = 0;
    positionI = y - 1;
    positionJ = x - 1;

    for(i=positionI, k = 0;k < 3; i++, k++){
        for(j=positionJ, l= 0;l < 3;j++,l++){
            if ( i >= 0 && i <= maxy - 1 && j >= 0 && j <= maxx - 1){
                sum = sum + (matrix[i][j] * filtro[k][l]);
            }
        }
    }

    return sum;
}

// Entradas: una matrix de pixeles , el ancho y el alto
// Funcionamiento: a partir de la matrix de pixeles se lleva a un array de pixeles para guardar la imagen
// Salida: un arreglo de pixeles

unsigned char* matrixToArray(unsigned char **matrix,int widht, int height){
    int i,j;
    size_t img_size = width * height * 1;
    unsigned char *img = malloc(img_size);
    unsigned char *p = img;

    for(i=0;i < height;i++){

        for(j=0;j < width; j++){
            *p = (uint8_t) matrix[i][j];
            p++;
        }
    }

    return img;
}

// Entradas: un arreglo de pixeles, el ancho y el alto de la imagen
// Funcionamiento: a partir de un arreglo de pixeles se lleva a una matrix
// Salida: una matrix de pixeles

unsigned char **createMatrix(unsigned char *img,int w,int h){
    size_t height = h;
    size_t row = w;
    unsigned char **array2d = malloc(height * sizeof(unsigned char *));
    int i,j;
    unsigned char *copyImg = img;

    for(i=0;i<height;i++){
        array2d[i] = (unsigned char *) malloc(row);
    }

    for(i=0;i < height;i++){

        for(j=0;j < width; j++){
            array2d[i][j] = *copyImg;
            copyImg++;
        }
    }

    return array2d;
}

// Entradas: un arreglo con los resultados de la clasificación y el numero de imagenes leidas
// Funcionamiento: a partir del arreglo imprime el resultado de aplicar la clasificacion
// Salida:

void printClassification(int* arrayResults,int size){

    int i;
    char number_to_string[10];
    char path_full[100];
    printf("|\t image\t |\t nearly black\t |\n");
    printf("|----------------------------------------|\n");
    for(i=0;i<size;i++){
        strcpy(path_full,"imagen_");
        snprintf(number_to_string, 100,"%d",i+1); 
        strcat(path_full,number_to_string);
        if(arrayResults[i] == 1){
            printf("|\t %s\t |\t yes\t|\n",path_full);
        }else{
            printf("|\t %s\t |\t no\t|\n",path_full);
        }
    }
}



// Entradas: una matrix de pixeles, el ancho, largo de la imagen, el umbral de binarizacion
// Funcionamiento: va revisando por cada pixel si este es mayor al umbral se deja como 255 de lo contrario como 0


void  binarization(unsigned char **buffer_laplaciano,int start,int end, int width, int umbral_binary){
    int i,j;
    
    for(i=start;i<end;i++){
        for(j=0;j<width;j++){
            if(buffer_laplaciano[i][j] > umbral_binary){
                buffer_laplaciano[i][j] = (uint8_t) 255;
            }else{
                buffer_laplaciano[i][j] = (uint8_t) 0;
            }
        }
    }
}

// Entradas: una matrix de pixeles , el ancho, largo, y el nombre del archivo que se usa para aplicar el filtro
// Funcionamiento: por cada posición en la matrix se aplica el filtro con la función calculateFilter y se agrega en esa posición
// Salida: retorna el filtro aplicado

unsigned char** filterLapleciano(unsigned char **buffer_gray, unsigned char **buffer_laplaciano,int start,int end, int width, char *fileName){
    int i,j;
    
    char path_full[100] = "input/";
    strcat(path_full,fileName);
    int **filter = readFile(path_full,3);

    for(i=start;i<end;i++){
        for(j=0;j<width;j++){
            buffer_laplaciano[i][j] = (uint8_t) calculateFilter(buffer_gray,filter,i,j,0,height,width);
        }
    }

}

// Entradas: un arreglo de pixeles con tres canales, ancho, alto, y canales de la imagen
// Funcionamiento: por cada posición del arreglo y dependiendo de la cantidad de canales que en este caso se consideran 3 se calcula
// la escala de grises cada tres canales y se guarda en otro arreglo.
// Salida: se obtiene un arreglo de pixeles con un solo canal
// fuente: https://solarianprogrammer.com/2019/06/10/c-programming-reading-writing-images-stb_image-libraries/


unsigned char *grayProccess(unsigned char **buffer, unsigned char **buffer_gray,int start, int end,int width, int channels){

    int i,j,k;
    for(i=start;i<end;i++){
        for(j=0,k=0;j<width;k = k + 3,j++){
            buffer_gray[i][j] = ( ( buffer[i][k] * 0.3) + (buffer[i][k + 1] *0.59) + (buffer[i][k + 2] * 0.11) );
        }
    }

}

// Entrada: puntero de ancho, alto, canales y arreglo de imagen, i indica un numero para crear la imagen
// ejemplo: i = 0 imagen_1
// Funcionamiento: se utiliza la libreria de stbi y se carga la imagen
// Salida: retorna un arreglo de pixeles con tres canales( esto puede variar según la imagen)


unsigned char *open(int *width,int *height,int *channels,unsigned char *image, int i){

    // Crear path para abrir imagenes
    // las imagenes estan en la carpeta images
    char number_to_string[10];
    char path_full[100];
    strcpy(path_full,"images/imagen_");
    snprintf(number_to_string, 100,"%d",i+1); 
    strcat(path_full,number_to_string);
    strcat(path_full,".jpg");
    image = stbi_load(path_full,width,height,channels,0);
    return image;
}

// Entrada: ancho, alto y canales usados( en este caso siempre es 1), arreglo de pixeles, i indica el numero de la imagen
// Funcionamiento: se utiliza la libreria de stbi y se almacena la imagen


void writeImage(int width, int height, int channels,unsigned char *image,int i){
    char number_to_string[10];
    char path_full_output[100];
    strcpy(path_full_output,"output/imagen_");
    snprintf(number_to_string, 100,"%d",i+1); 
    strcat(path_full_output,number_to_string);
    strcat(path_full_output,".jpg");
    stbi_write_jpg(path_full_output,width, height, channels, image, 100);
}

void closeImage(unsigned char* image){
    stbi_image_free(image);
}