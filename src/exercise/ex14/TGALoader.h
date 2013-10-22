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

tgaInfo* tgaLoad(const char *filename){
	FILE *file;
	tgaInfo *info;
	int mode,total;

	// allocate memory for the info struct and check!
	info = (tgaInfo *)malloc(sizeof(tgaInfo));
	if (info == NULL)
		return(NULL);


	// open the file for reading (binary mode)
	file = fopen(filename, "rb");
	if (file == NULL) {
		info->status = TGA_ERROR_FILE_OPEN;
		return(info);
	}
	
	// Load header info
	unsigned char cGarbage;
	short int iGarbage;

	fread(&cGarbage, sizeof(unsigned char), 1, file);
	fread(&cGarbage, sizeof(unsigned char), 1, file);

	// type must be 2 or 3
	fread(&info->type, sizeof(unsigned char), 1, file);

	fread(&iGarbage, sizeof(short int), 1, file);
	fread(&iGarbage, sizeof(short int), 1, file);
	fread(&cGarbage, sizeof(unsigned char), 1, file);
	fread(&iGarbage, sizeof(short int), 1, file);
	fread(&iGarbage, sizeof(short int), 1, file);

	fread(&info->width, sizeof(short int), 1, file);
	fread(&info->height, sizeof(short int), 1, file);
	fread(&info->pixelDepth, sizeof(unsigned char), 1, file);

	fread(&cGarbage, sizeof(unsigned char), 1, file);
	
	// check for errors when loading the header
	if (ferror(file)) {
		info->status = TGA_ERROR_READING_FILE;
		fclose(file);
		return(info);
	}

	// check if the image is color indexed
	if (info->type == 1) {
		info->status = TGA_ERROR_INDEXED_COLOR;
		fclose(file);
		return(info);
	}
	// check for other types (compressed images)
	if ((info->type != 2) && (info->type !=3)) {
		info->status = TGA_ERROR_COMPRESSED_FILE;
		fclose(file);
		return(info);
	}

	// mode equals the number of image components
	mode = info->pixelDepth / 8;
	// total is the number of bytes to read
	total = info->height * info->width * mode;
	// allocate memory for image pixels
	info->imageData = (unsigned char *)malloc(sizeof(unsigned char) * 
															total);

	// check to make sure we have the memory required
	if (info->imageData == NULL) {
		info->status = TGA_ERROR_MEMORY;
		fclose(file);
		return(info);
	}
	
	//Load img
	int i;
	unsigned char aux;

	// mode equal the number of components for each pixel
	mode = info->pixelDepth / 8;
	// total is the number of bytes we'll have to read
	total = info->height * info->width * mode;
	
	fread(info->imageData,sizeof(unsigned char),total,file);

	// mode=3 or 4 implies that the image is RGB(A). However TGA
	// stores it as BGR(A) so we'll have to swap R and B.
	if (mode >= 3)
		for (i=0; i < total; i+= mode) {
			aux = info->imageData[i];
			info->imageData[i] = info->imageData[i+2];
			info->imageData[i+2] = aux;
		}
	
	// check for errors when reading the pixels
	if (ferror(file)) {
		info->status = TGA_ERROR_READING_FILE;
		fclose(file);
		return(info);
	}
	fclose(file);
	info->status = TGA_OK;
	return(info);
}

void tgaDestroy(tgaInfo *info){
	if (info != NULL) {
		free(info->imageData);
		free(info);
	}
}
