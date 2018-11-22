#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "../include/commands.h"
#include "../include/PPMhandling.h"
#include "../include/updates.h"


int main(int argc, char *argv[]){
	Info imageInfo;
	int maxMem = 64; //maximum ram for the undo buffer in megabytes
	Pixel **undoBuffer = NULL;
	int bufferLen;
	SDL_Renderer *renderer = NULL;

	char command[50];
	char parameter[150];
	
	void *(*cmd_ptr)(char *, int , Info, Pixel **) = NULL;

	const Command commandList[20] = {{.id = "darken", .func = &Darken},
                           		{.id = "brighten", .func = &Lighten},
                           		{.id = "invert", .func = &Invert},
                           		{.id = "greyscale", .func = &Greyscale},
                           		{.id = "contrast", .func = &Contrast},
                                {.id = "exit", .func = &ExitProgram},
                                {.id = "save", .func = &Save},
                           		{.id = "end", .func = NULL}};

	if (argc == 2 || argc == 4 || argc == 5){
		FILE *fp;
		fp = fopen(argv[1], "r");

		if (fp != NULL){
			if (IsPPM(fp)){
				imageInfo = PPM_GetInfo(fp);
				bufferLen = (maxMem * 1024 *1024) / (sizeof(Pixel) * imageInfo.width * imageInfo.height);
				undoBuffer = (Pixel **)malloc(sizeof(Pixel **) * bufferLen); //lefoglal egy annyi elemű tömböt, ahányszor a kép belefér az általunk meghatározott maximális ramhasználatba
				for (int i = 0; i < bufferLen; i++){
					undoBuffer[i] = NULL;	
				}

				undoBuffer[0] = (Pixel *) malloc(sizeof(Pixel) * imageInfo.width * imageInfo.height); // FREEEEEEEEEEEEE

				if (undoBuffer[0] == NULL)
					printf("couldn't allocate memory");

				PPM_LoadImageToArray(fp, imageInfo, undoBuffer[0]);
			}
			else
				pushmsg("File is not a PPM");
			
			fclose(fp);
		}
		else 
			pushmsg("Error opening image");

		if (argc == 4 || argc == 5){
			char *commandin = argv[2];
			char *parameter = NULL;
			char *savepath;
			if (argc == 4){
				savepath = argv[3];
			}
			else{
				savepath = argv[4];
				parameter = argv[3];
			}

			cmd_ptr = InterpretCommand(commandList, commandin, &cmd_ptr);
			if (cmd_ptr != NULL)
				(*cmd_ptr)(parameter, 0, imageInfo, undoBuffer);
			Save(savepath, 1, imageInfo, undoBuffer);
			return 0;
		}
	}
	else{
		pushmsg("argument <image> or <image> <operation> or <image> <operation> <amount> required");
		exit(1);
	}

	printf("%d", bufferLen);

	while (1){ // ezt most itt szabad, mert van exit command
		char input[200] = "";

		int currStep = 0;
		while (undoBuffer[currStep] != NULL)
			currStep++;
		currStep -= 1;

		printf("%d\n", currStep );

		if (currStep == bufferLen){ //undoBuffer shiftelése, ha megtelik
			//TODO
		}

		if (renderer == NULL && undoBuffer[currStep] != NULL){
			renderer = SYSTEM_SDLSetup(imageInfo);
		}

		if (renderer != NULL){
			SDLupdate(renderer, imageInfo, undoBuffer[currStep]);
			SDLrender(renderer);
		}

		printf("Paint.PPM: ");

		GetCommand(input); //mégsem a scanf(" %s")-t használom mert picit furán viselkedett, ez most így jó
		SeparateCommandLine(input, command, parameter); //szétszedi space-nél a commandot és a paramétert


		cmd_ptr = InterpretCommand(commandList, command, &cmd_ptr);

		if (cmd_ptr != NULL)
			(*cmd_ptr)(parameter, currStep, imageInfo, undoBuffer);  	

	}

	free(undoBuffer);
}
