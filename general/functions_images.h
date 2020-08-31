#ifndef FUNCTION_IMAGE_H
#define FUNCTION_IMAGE_H

unsigned char *open(int *width,int *height,int *channels,unsigned char *image,int i);

void writeImage(int width, int height, int channels,unsigned char **image,int i);

void grayProccess(unsigned char **buffer, unsigned char **buffer_gray,int start, int end,int width, int channels);

unsigned char **createMatrix(unsigned char *img,int width,int height);

unsigned char* matrixToArray(unsigned char **matrix,int widht, int height);

void filterLapleciano(unsigned char **buffer_gray, unsigned char **buffer_laplaciano,int start,int end, int width, char *fileName);

void  binarization(unsigned char **buffer_laplaciano,int start,int end, int width, int umbral_binary);

// int classification(unsigned char **buffer_laplaciano,int start,int end, int width, int umbral_classification, int count);

void printClassification(int count,int n_image, int height, int width, int umbral_classification);

void closeImage(unsigned char* image);

#endif