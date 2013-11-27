#include "TGALoader.h"

tgaInfo* tgaCreate(short int width, short int height, unsigned char bpp, unsigned char* data){
	tgaInfo* info = (tgaInfo *)malloc(sizeof(tgaInfo));
	if (info == NULL)
		return(NULL);
	
	info->width = width;
	info->height = height;
	info->pixelDepth = bpp*8;
	unsigned int total = info->height * info->width * bpp;
	info->imageData = (unsigned char *)malloc(sizeof(unsigned char)*total);
	memcpy(info->imageData, data, sizeof(unsigned char)*total);
	info->type = 3;
	info->status = 0;
	return info;
}

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
	info->imageData = (unsigned char *)malloc(sizeof(unsigned char) * total);

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

	// We want origin in the lower left corner, flip image vertically	
	for(int y=0; y < info->height/2;y++){
		for(int x=0; x < info->width;x++){
			unsigned int i0 = mode*(x+y*info->width);
			unsigned int i1 = mode*(x+(info->height-y-1)*info->width);
			
			unsigned char px00 = (unsigned char)info->imageData[i0];
			unsigned char px01 = (unsigned char)info->imageData[i0+1];
			unsigned char px02 = (unsigned char)info->imageData[i0+2];
			
			unsigned char px10 = (unsigned char)info->imageData[i1];
			unsigned char px11 = (unsigned char)info->imageData[i1+1];
			unsigned char px12 = (unsigned char)info->imageData[i1+2];
			
			info->imageData[i0] = px10;
			info->imageData[i0+1] = px11;
			info->imageData[i0+2] = px12;
			
			info->imageData[i1] = px00;
			info->imageData[i1+1] = px01;
			info->imageData[i1+2] = px02;
		}
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

int tgaSave(const char *filename, tgaInfo* img) {

	unsigned char cGarbage = 0, type,mode,aux;
	short int iGarbage = 0;
	int i;
	FILE *file;

	unsigned char *imageData;

	mode = img->pixelDepth / 8;
	// total is the number of bytes to write
	int total = img->height * img->width * mode;
	// allocate memory for image pixels
	imageData = (unsigned char *)malloc(sizeof(unsigned char) * total);
	memcpy(imageData,img->imageData, sizeof(unsigned char) * total);		
	
	// open file and check for errors
	file = fopen(filename, "wb");
	if (file == NULL) {
		return(TGA_ERROR_FILE_OPEN);
	}
	
	// compute image type: 2 for RGB(A), 3 for greyscale
	if ((img->pixelDepth == 24) || (img->pixelDepth == 32))
		type = 2;
	else
		type = 3;

	// write the header
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);

	fwrite(&type, sizeof(unsigned char), 1, file);

	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&cGarbage, sizeof(unsigned char), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);
	fwrite(&iGarbage, sizeof(short int), 1, file);

	fwrite(&img->width, sizeof(short int), 1, file);
	fwrite(&img->height, sizeof(short int), 1, file);
	fwrite(&img->pixelDepth, sizeof(unsigned char), 1, file);

	fwrite(&cGarbage, sizeof(unsigned char), 1, file);

	// convert the image data from RGB(a) to BGR(A)
	if (mode >= 3)
		for (i=0; i < img->width * img->height * mode ; i+= mode) {
			aux = imageData[i];
			imageData[i] = imageData[i+2];
			imageData[i+2] = aux;
		}

	// save the image data
	fwrite(imageData, sizeof(unsigned char), img->width * img->height * mode, file);
	fclose(file);
	free(imageData);

	return(TGA_OK);
}

void tgaDestroy(tgaInfo *info){
	if (info != NULL) {
		free(info->imageData);
		free(info);
	}
}
