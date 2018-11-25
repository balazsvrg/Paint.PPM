#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __linux
#include <SDL2/SDL.h>
#endif

#include "../include/commands.h"
#include "../include/PPMhandling.h"
#include "../include/updates.h"


bool run = true;

int main(int argc, char *argv[]){
	Info imageInfo;
	int maxMem = 64; /*maximum ram az UndoBuffernek, megabyteban*/
	Pixel **undoBuffer = NULL;
	int bufferLen;
	int currStep;


	#ifdef __linux
	SDL_Renderer *renderer = NULL;
	#endif

	char command[50];
	char parameter[150];

	void *(*cmd_ptr)(char *, int , Info, Pixel **) = NULL; /*ez mutat minden kommand beírása után a megfelelő függvényre*/

	const Command commandList[20] = {{.id = "darken", .func = &Darken}, 
                           		{.id = "brighten", .func = &Lighten},
                           		{.id = "invert", .func = &Invert},
                           		{.id = "greyscale", .func = &Greyscale},
                           		{.id = "contrast", .func = &Contrast},
                           		{.id = "blur", .func = &Blur},
                           		{.id = "undo", .func = &Undo},
                                {.id = "exit", .func = &ExitProgram},
                                {.id = "save", .func = &Save},
                                {.id = "help", .func = &Help},
                           		{.id = "end", .func = NULL}};


	if ((argc == 2 && strcmp(argv[1], "help") != 0) || argc == 4 || argc == 5){
		FILE *fp;
		fp = fopen(argv[1], "r");

		if (fp != NULL){
			if (IsPPM(fp)){
				imageInfo = PPM_GetInfo(fp);
				imageInfo.size = imageInfo.width * imageInfo.height;
				bufferLen = (maxMem * (1024 * 1024)) / (imageInfo.size * 3);
				undoBuffer = (Pixel **)malloc(sizeof(Pixel **) * bufferLen); /*lefoglal egy annyi elemű tömböt, ahányszor a kép belefér az általunk meghatározott maximális ramhasználatba*/
				if (undoBuffer == NULL){
					pushmsg("couldn't allocate memory");
					return 1;
				}
				for (int i = 0; i < bufferLen; i++){
					undoBuffer[i] = NULL;
				}
                Pixel *img = (Pixel *) malloc(sizeof(Pixel) * imageInfo.size);
				undoBuffer[0] = img;

				if (undoBuffer[0] == NULL){
					pushmsg("couldn't allocate memory");
					return 1;
				}

			PPM_LoadImageToArray(fp, imageInfo, undoBuffer[0]);
			}
			else
				pushmsg("File is not a PPM");

			fclose(fp);
		}
		else{
			pushmsg("Error opening image");
			exit(1);
		}

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
			if (cmd_ptr != NULL){
				(*cmd_ptr)(parameter, 1, imageInfo, undoBuffer);
			}
			Save(savepath, 2, imageInfo, undoBuffer);

			free(undoBuffer[0]);
			free(undoBuffer[1]);
			free(undoBuffer);
			return 0;
		}
	}

	else if (argc == 2 && strcmp(argv[1], "help") == 0){
		Help(NULL, 0, imageInfo, undoBuffer);
		exit(0);
	}

	else{
		pushmsg("arguments <path> or <path> <command> <savepath> or <path> <command> <parameter> <savepath> required");
		exit(1);
	}

	while (run){ // ezt most itt szabad, mert van exit command
		char input[200] = "";

		currStep = 0;
		while (undoBuffer[currStep] != NULL)
			currStep++;


		#ifdef __linux
		if (renderer == NULL && undoBuffer[currStep-1] != NULL){
			renderer = SYSTEM_SDLSetup(imageInfo);
		}

		if (renderer != NULL){
			SDLupdate(renderer, imageInfo, undoBuffer[currStep-1]);
			SDLrender(renderer);
		}
		#endif

		if (currStep == bufferLen -2){ //undoBuffer shiftelése, ha megtelik
			free(undoBuffer[0]);
			for (int i = 0; i < currStep - 1; ++i)
			{
				undoBuffer[i] = undoBuffer [i + 1];
			}

			undoBuffer[currStep-1] = NULL;
			currStep -= 1;
		}

		printf("Paint.PPM: ");

		GetCommand(input); //mégsem a scanf(" %s")-t használom mert picit furán viselkedett, ez most így jó
		SeparateCommandLine(input, command, parameter); //szétszedi space-nél a commandot és a paramétert

		cmd_ptr = InterpretCommand(commandList, command, &cmd_ptr);

		if (cmd_ptr != NULL)
			(*cmd_ptr)(parameter, currStep, imageInfo, undoBuffer);

		strcpy(command, "");
		strcpy(parameter, "");

	}
	for (int i = 0; i <= currStep; ++i){
		free(undoBuffer[i]);
	}
	free(undoBuffer);
	return 0;
}
