#ifndef DATA_H
#define DATA_H
#include <stdint.h>
// #include <stdlib.h>
// variables de opciones

int number_of_images = -1;
char* name_mask_laplaciana;
int umbral_binary = -1;
int umbral_classification = -1;
int display = 0; // boleano de 0: falso, 1: verdadero

// variables del procesamiento de imagen

int width, height, channels;
unsigned char *image;
unsigned char **images;

int size_buffer;

unsigned char **buffer;
unsigned char **buffer_gray;
unsigned char **buffer_laplaciano;

size_t img_size;



#endif

