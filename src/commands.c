#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/commands.h"
#include "../include/PPMhandling.h"
#include "../include/updates.h"

void GetCommand(char *input){ //Beolvas egy sornyi karaktert és stringként elmenti (nem hagy \n-t a stdin-en)
	char c;
	int index = 0;
	bool run = true;

	while(run){
		c = getchar();

		if (c == '\n'){
			input[index] = '\0';
			run = false;
		}

		else
			input[index] = c;

		index++;
	}
}

void SeparateCommandLine(char *input, char *command, char *argument){ //Szétszedi az input sztringet egy commandra meg a paraméterre (szóközzel elválasztva)
	char *tmp;
	tmp = strtok(input, " '\n'");
	if (tmp != NULL){
		strcpy(command, tmp);
	}

	tmp = strtok(NULL, " \n");
	if (tmp != NULL){
		strcpy(argument, tmp);
	}
}

void Darken(char *amount, int currStep, Info imgInfo, Pixel ***undoBuffer){
	printf("I work");
	int value = atoi(amount);
	printf("I work too");
	PPM_NextStep(imgInfo, currStep, undoBuffer);
	printf("next step does too");
	if (value >= 0 && value <= 255){
		PPM_Darken(*undoBuffer[currStep +1], value, imgInfo);
	}
	else
		pushmsg("cannot Darken by that amount");
}

void Lighten(char *amount,  int currStep, Info imgInfo, Pixel ***undoBuffer){
	int value = atoi(amount);
	PPM_NextStep(imgInfo, currStep, undoBuffer);
	if (value >= 0 && value <= 255){
		PPM_Lighten(*undoBuffer[currStep +1], value, imgInfo);
	}
	else
		pushmsg("cannot Lighten by that amount");
}

void Invert(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel ***undoBuffer){
	PPM_NextStep(imgInfo, currStep, undoBuffer);
	PPM_Invert(*undoBuffer[currStep +1], imgInfo);
}

void Greyscale(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel ***undoBuffer){
	PPM_NextStep(imgInfo, currStep, undoBuffer);
	PPM_Greyscale(*undoBuffer[currStep +1], imgInfo);
}

void Contrast(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel ***undoBuffer){
	PPM_NextStep(imgInfo, currStep, undoBuffer);
	PPM_Contrast(*undoBuffer[currStep +1],imgInfo);
}

void Save(char *path, Pixel **img, Info imgInfo){
	FILE *fp;
	fp = fopen(path, "w");
	fprintf(fp, "P6\n %d %d\n%d\n",imgInfo.width, imgInfo.height, imgInfo.maxColorVal);
	for (int i = 0; i <imgInfo.height * imgInfo.width; i++){
		fputc(img[i]->r, fp);
		fputc(img[i]->g, fp);
		fputc(img[i]->b, fp);
	}

	fclose(fp);
}

void ExitProgram(char *mustTakeArgument, Pixel **img, Info imgInfo){ //kilép a programból (megszakítja a végtelen ciklust)

	if (*img != NULL)
		free(*img);

	exit(0);
}