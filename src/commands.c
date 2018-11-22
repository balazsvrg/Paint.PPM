#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/commands.h"
#include "../include/PPMhandling.h"
#include "../include/updates.h"

extern bool run;

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
	cmd_ptr = NULL;
	for (int i = 0; commandList[i].func != NULL; i++){
		if(strcmp(command, commandList[i].id) == 0){
			cmd_ptr = commandList[i].func;
		}
	}												  
	
	if (cmd_ptr == NULL)
		pushmsg("invalid command");

	return cmd_ptr;
}

void Darken(char *amount, int currStep, Info imgInfo, Pixel **undoBuffer){
	int value = atoi(amount);
	PPM_NextStep(imgInfo, currStep, undoBuffer);
	if (value >= 0 && value <= 255){
		PPM_Darken(undoBuffer[currStep], value, imgInfo);
	}
	else
		pushmsg("cannot Darken by that amount");

	}

void Lighten(char *amount,  int currStep, Info imgInfo, Pixel **undoBuffer){
	int value = atoi(amount);
	PPM_NextStep(imgInfo, currStep, undoBuffer);
	if (value >= 0 && value <= 255){
		PPM_Lighten(undoBuffer[currStep], value, imgInfo);
	}
	else
		pushmsg("cannot Lighten by that amount");
	}

void Invert(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel **undoBuffer){
	PPM_NextStep(imgInfo, currStep, undoBuffer);
	PPM_Invert(undoBuffer[currStep], imgInfo);
	}

void Greyscale(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel **undoBuffer){
	PPM_NextStep(imgInfo, currStep, undoBuffer);
	PPM_Greyscale(undoBuffer[currStep], imgInfo);
	}

void Contrast(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel **undoBuffer){
	PPM_NextStep(imgInfo, currStep, undoBuffer);
	PPM_Contrast(undoBuffer[currStep],imgInfo);
}

void Undo(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel **undoBuffer){
	if (currStep != 1)
		undoBuffer[currStep -1] = NULL;

	else
		pushmsg("undo buffer is empty");
	
}

void Save(char *path, int currStep, Info imgInfo, Pixel **undoBuffer){
	if (strcmp(path, "") != 0){
		FILE *fp;
		fp = fopen(path, "w");

		fprintf(fp, "P6\n %d %d\n%d\n",imgInfo.width, imgInfo.height, imgInfo.maxColorVal);
		for (int i = 0; i <imgInfo.height * imgInfo.width; i++){
			fputc(undoBuffer[currStep-1][i].r, fp);
			fputc(undoBuffer[currStep-1][i].g, fp);
			fputc(undoBuffer[currStep-1][i].b, fp);
		}

		fclose(fp);
		
	}

	else{
		pushmsg("path needed to save");
		
	}
}

void ExitProgram(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel **undoBuffer){ //kilép a programból (megszakítja a végtelen ciklust)

	for (int i = 0; i <= currStep; ++i)
	{
		free(undoBuffer[i]);
	}

	run = false;
	
}