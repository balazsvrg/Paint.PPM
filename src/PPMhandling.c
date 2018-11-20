#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../include/updates.h"
#include "../include/PPMhandling.h"

bool IsPPM(FILE *imgfile){ //jelez, ha rossz formátumú képet próbálnánk megnyitni (igen, tényleg Magic Number a szakszerű megfogalmazása)
	char magicNumber;
	return (fscanf(imgfile, "P%c\n", &magicNumber) == 1 && magicNumber == '6');
}

Info PPM_GetInfo(FILE *imgfile){ //visszatér a PPM méreteivel, és a maximális színértékkel
	Info imageInfo;
	fscanf(imgfile, "%d%d%d\n", &imageInfo.width, &imageInfo.height, &imageInfo.maxColorVal);
	return imageInfo;
}

void PPM_LoadImageToArray(FILE *imgfile, Info imgInfo, Pixel *img){ //lefoglal egy tömböt, és feltölti a kép pixeleinek értékével
	for (int i = 0; i < imgInfo.width; ++i){
		for (int j = 0; j < imgInfo.height; ++j){
			img[i * imgInfo.height + j].r = fgetc(imgfile);
			img[i * imgInfo.height + j].g = fgetc(imgfile);
			img[i * imgInfo.height + j].b = fgetc(imgfile);
		}
	}
}

void PPM_NextStep(Info imgInfo, int currStep, Pixel ***undoBuffer){
	*undoBuffer[currStep + 1] = (Pixel *) malloc(sizeof(Pixel) * imgInfo.width * imgInfo.height);
	for (int i = 0; i < imgInfo.width; ++i)
		for (int j = 0; j < imgInfo.height; ++j)
			*undoBuffer[currStep + 1][i * imgInfo.height + j] = *undoBuffer[currStep][i * imgInfo.height + j];
}

void PPM_Darken(Pixel *img, int amount, Info imgInfo){
	for (int i = 0; i < imgInfo.height; i++){
		for (int j = 0; j < imgInfo.width; ++j){
			int index = i * imgInfo.width + j;
			img[index].r = img[index].r >= amount ? img[index].r - amount : 0;
			img[index].g = img[index].g >= amount ? img[index].g - amount : 0;
			img[index].b = img[index].b >= amount ? img[index].b - amount : 0;
		}
	}
}

void PPM_Lighten(Pixel *img, int amount, Info imgInfo){
	for (int i = 0; i < imgInfo.height; i++){
		for (int j = 0; j < imgInfo.width; ++j){
			int index = i * imgInfo.width + j;
			img[index].r = img[index].r <= 255 - amount ? img[index].r + amount : 255;
			img[index].g = img[index].g <= 255 - amount ? img[index].g + amount : 255;
			img[index].b = img[index].b <= 255 - amount ? img[index].b + amount : 255;
		}
	}
}

void PPM_Invert(Pixel *img, Info imgInfo){
	for (int i = 0; i < imgInfo.height; i++){
		for (int j = 0; j < imgInfo.width; ++j){
			int index = i * imgInfo.width + j;
			img[index].r = 255 - img[index].r;
			img[index].g = 255 - img[index].g;
			img[index].b = 255 - img[index].b;
		}
	}
}

void PPM_Greyscale(Pixel *img, Info imgInfo){
	for (int i = 0; i < imgInfo.height; i++){
		for (int j = 0; j < imgInfo.width; ++j){
			int index = i * imgInfo.width + j;
			int greyValue = (img[index].r + img[index].g + img[index].b) / 3;
			img[index].r = img[index].g = img[index].b = greyValue;
		}
	}
}

void PPM_Contrast(Pixel *img, Info imgInfo){ 
	int min = 0;
	int max = 255;
	Histogram histogram;

	for (int i = 0; i < 255; i++){
		histogram.histR[i] = 0;
		histogram.histG[i] = 0;
		histogram.histB[i] = 0;
	}


	for (int i = 0; i < imgInfo.height; i++){
		for (int j = 0; j < imgInfo.width; ++j){
			int index = i * imgInfo.width + j;
			histogram.histR[img[index].r] += 1;
			histogram.histG[img[index].g] += 1;
			histogram.histB[img[index].b] += 1;
		}
	}
	while (((((histogram.histR[min] + histogram.histG[min] + histogram.histB[min])/3) +    			
		(histogram.histR[min +1] + histogram.histG[min +1] + histogram.histB[min +1])/3) + 			
		(histogram.histR[min +2] + histogram.histG[min +2] + histogram.histB[min +2])/3)/3 < 50)	
		min++;

	while (((((histogram.histR[max] - histogram.histG[max] - histogram.histB[max])/3) + 
		(histogram.histR[max -1] - histogram.histG[max -1] - histogram.histB[max -1])/3) + 
		(histogram.histR[max -2] - histogram.histG[max -2] - histogram.histB[max -2])/3)/3 < 50)
		max--;

	if (max - min != 0){
		for (int i = 0; i < imgInfo.height; i++){
			for (int j = 0; j < imgInfo.width; ++j){
				int index = i * imgInfo.width + j;
				int redVal =  255 * (img[index].r - min) / (max - min);
				int bluVal = 255 * (img[index].g - min) / (max - min);
				int greVal = 255 * (img[index].b - min) / (max - min);

				if (redVal < 0)		   img[index].r = 0;
				else if (redVal > 255) img[index].r = 255;
				else 				   img[index].r = redVal;

				if (bluVal < 0)		   img[index].g = 0;
				else if (bluVal > 255) img[index].g = 255;
				else 				   img[index].g = bluVal;

				if (greVal < 0)		   img[index].b = 0;
				else if (greVal > 255) img[index].b = 255;
				else 				   img[index].b = greVal;
			}
		}
	}
	else
		pushmsg("Contrast cannot be stretched, as this is a monochrome picture");
}

