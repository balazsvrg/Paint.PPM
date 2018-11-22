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

void *InterpretCommand(const Command *commandList, char *command, void *cmd_ptr){
	for (int i = 0; commandList[i].func != NULL; i++){
		if(strcmp(command, commandList[i].id) == 0){
			cmd_ptr = commandList[i].func;
		}
	}												  
	
	if (cmd_ptr == NULL)
		pushmsg("invalid command");

	return cmd_ptr;
}

bool Darken(char *amount, int currStep, Info imgInfo, Pixel **undoBuffer){
	int value = atoi(amount);
	PPM_NextStep(imgInfo, currStep, undoBuffer);
	if (value >= 0 && value <= 255){
		PPM_Darken(undoBuffer[currStep +1], value, imgInfo);
	}
	else
		pushmsg("cannot Darken by that amount");

	return true;
}

bool Lighten(char *amount,  int currStep, Info imgInfo, Pixel **undoBuffer){
	int value = atoi(amount);
	PPM_NextStep(imgInfo, currStep, undoBuffer);
	if (value >= 0 && value <= 255){
		PPM_Lighten(undoBuffer[currStep +1], value, imgInfo);
	}
	else
		pushmsg("cannot Lighten by that amount");
	return true;
}

bool Invert(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel **undoBuffer){
	PPM_NextStep(imgInfo, currStep, undoBuffer);
	PPM_Invert(undoBuffer[currStep +1], imgInfo);
	return true;
}

bool Greyscale(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel **undoBuffer){
	PPM_NextStep(imgInfo, currStep, undoBuffer);
	PPM_Greyscale(undoBuffer[currStep +1], imgInfo);
	return true;
}

bool Contrast(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel **undoBuffer){
	PPM_NextStep(imgInfo, currStep, undoBuffer);
	PPM_Contrast(undoBuffer[currStep +1],imgInfo);
	return true;
}

bool Save(char *path, int currStep, Info imgInfo, Pixel **undoBuffer){
	if (strcmp(path, "") != 0){
		FILE *fp;
		fp = fopen(path, "w");
		fprintf(fp, "P6\n %d %d\n%d\n",imgInfo.width, imgInfo.height, imgInfo.maxColorVal);
		for (int i = 0; i <imgInfo.height * imgInfo.width; i++){
			fputc(undoBuffer[currStep][i].r, fp);
			fputc(undoBuffer[currStep][i].g, fp);
			fputc(undoBuffer[currStep][i].b, fp);
		}

		fclose(fp);
		return false;
	}

	else{
		pushmsg("path needed to save");
		return false;
	}
}

bool ExitProgram(char *mustTakeArgument, Pixel **img, Info imgInfo){ //kilép a programból (megszakítja a végtelen ciklust)

	if (*img != NULL)
		free(*img);

	exit(0);
}