#ifndef FUNCTION_IMAGE_H
#define FUNCTION_IMAGE_H

unsigned char *open(int *width,int *height,int *channels,unsigned char *image,int i);

void writeImage(int width, int height, int channels,unsigned char *image, int i);

unsigned char *grayProccess(unsigned char *image, int width, int height,int channels);

unsigned char **createMatrix(unsigned char *img,int width,int height);

unsigned char* matrixToArray(unsigned char **matrix,int widht, int height);

unsigned char** filterLapleciano(unsigned char** matrix,int width, int height,char *fileName);

void  binarization(unsigned char** matrix, int w, int h, int umbral_binary);

int classification(unsigned char** matrix, int w, int h, int umbral_classification);

void printClassification(int* arrayResults,int size);

void closeImage(unsigned char* image);

#endif