#ifndef PPMHANDLING_H
#define PPMHANDLING_H

#include <stdbool.h>

typedef struct{
	unsigned char r, g, b;
}Pixel;

typedef struct{
	int width, height, maxColorVal;
}Info;

typedef struct{
	int histR[256];
	int histG[256];
	int histB[256];
}Histogram;

bool IsPPM(FILE *imgfile);
Info PPM_GetInfo(FILE *imgfile);
void PPM_LoadImageToArray(FILE *imgfile, Info imgInfo, Pixel *image);
void PPM_NextStep(Info imgInfo, int currStep, Pixel ***undoBuffer);
void PPM_Darken(Pixel *img, int amount, Info imgInfo);
void PPM_Lighten(Pixel *img, int amount, Info imgInfo);
void PPM_Invert(Pixel*img, Info imgInfo);
void PPM_Greyscale(Pixel *img, Info imgInfo);
void PPM_Contrast(Pixel *img, Info imgInfo);

#endif