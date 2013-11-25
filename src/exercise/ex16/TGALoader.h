#ifndef TGA_LOADER_H
#define TGA_LOADER_H

#define	TGA_ERROR_FILE_OPEN				-5
#define TGA_ERROR_READING_FILE			-4
#define TGA_ERROR_INDEXED_COLOR			-3
#define TGA_ERROR_MEMORY				-2
#define TGA_ERROR_COMPRESSED_FILE		-1
#define TGA_OK							 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int status;
	unsigned char type, pixelDepth;
	short int width, height;
	unsigned char *imageData;
}tgaInfo;

tgaInfo* tgaCreate(short int width, short int height, unsigned char bpp, unsigned char* data);
tgaInfo* tgaLoad(const char *filename);
int tgaSave(const char *filename, tgaInfo* img);
void tgaDestroy(tgaInfo *info);

#endif //TGA_LOADER_H
