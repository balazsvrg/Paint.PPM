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
    Info imageInfo;                 /*indó a kép méretéről és maximum színértékéről (csak 8bites színkódolást enged a program)*/
    int maxMem = 64;                /*maximum ram az UndoBuffernek, megabyteban*/
    Pixel **undoBuffer = NULL;      /*eltárolja a lépéseket (maximum MAXMEM memórián)*/  
    int bufferLen;                  /*enni elemű lehet az undoBuffer*/
    int currStep;                   /*hanyadik elemét szerkesztjuk az undobuffernek*/

    /*parancssoros futtatás változói*/
    char command[50];               /*az inputrol kapott kommand*/
    char parameter[50];             /*a kommand paramétere (ha szükséges)*/
    char input[200] = "";           /*itt tároljuk az input sztringet*/

    void *(*cmd_ptr)(char *, int , Info, Pixel **) = NULL; /*ez mutat minden kommand beírása után a megfelelő függvényre*/

    #ifdef __linux                  /*Az SDL linuxon jobban tűri ha megáll a program futása(input)*/
    SDL_Renderer *renderer = NULL;  /*és nem ez a legfontosabb feature a programnak ezért wondowson */
    #endif                          /*nem indítom el az SDL ablakot*/

    const Command commandList[20] = {{.id = "darken", .func = &Darken},     /*******KOMMANDLISTA**********/
                                {.id = "brighten", .func = &Lighten},       /*a command szövege alapján  */
                                {.id = "invert", .func = &Invert},          /*értéket rendel a cmd_ptrhez*/
                                {.id = "greyscale", .func = &Greyscale},
                                {.id = "contrast", .func = &Contrast},
                                {.id = "blur", .func = &Blur},
                                {.id = "undo", .func = &Undo},
                                {.id = "exit", .func = &ExitProgram},
                                {.id = "save", .func = &Save},
                                {.id = "help", .func = &Help},
                                {.id = "end", .func = NULL}};

    /*Leelenőrzi hogy a program megfelelő indítási paramétereket kapott,
     *kiválasztja a kapott paraméterek alapján, hogy milyen formában fusson le*/
    if (((argc == 2 && strcmp(argv[1], "help") != 0) && strcmp(argv[1], "save") &&
    strcmp(argv[1], "exit") && strcmp(argv[1], "undo")) || argc == 4 || argc == 5){ /*ha helyes formátumban kapjuk az indítást, megnyitja a megadott fájlt és brtölti az undobufferbe*/
        FILE *fp;
        fp = fopen(argv[1], "r");

        if (fp != NULL){
            if (IsPPM(fp)){
                imageInfo = PPM_GetInfo(fp);

                if(imageInfo.maxColorVal != 255){
                    pushmsg("only 8bit encoding supported");
                    return 1;
                }

                bufferLen = (maxMem * (1024 * 1024)) / (imageInfo.size * 3);
                undoBuffer = (Pixel **)malloc(sizeof(Pixel **) * bufferLen); /*lefoglal egy annyi elemű tömböt, ahányszor a kép belefér az általunk meghatározott maximális ramhasználatba*/
                
                if (undoBuffer == NULL){
                    pushmsg("couldn't allocate memory");
                    return 1;
                }
                
                for (int i = 0; i < bufferLen; i++){ /*NULL-ra inicializálja az undoBuffert*/
                    undoBuffer[i] = NULL;
                }
               
                Pixel *img = (Pixel *) malloc(sizeof(Pixel) * imageInfo.size); 
                undoBuffer[0] = img;

                if (undoBuffer[0] == NULL){
                    pushmsg("couldn't allocate memory");
                    return 1;
                }

            PPM_LoadImageToArray(fp, imageInfo, undoBuffer[0]);/*feltolti az undobuffer első képét a fájl tartalmával*/
            }
            else
                pushmsg("File is not a PPM");

            fclose(fp);
        }
        else{
            pushmsg("Error opening image");
            return 1;
        }

        if (argc == 4 || argc == 5){     /*egyetlen kommanddal duttatja a programot és elmenti egyből a változtatást*/
            char *commandin = argv[2];   /*a paraméterként kapott kommandot a program neve mellé kell írni*/     
            char *parameter = NULL;      /*csak akkor van paraméter ha a kommand azt igényli*/

            char *savepath;              /*kommandtól függően vagy 3. vagy 4. paraméter*/
            if (argc == 4){
                savepath = argv[3];
            }
            else{
                savepath = argv[4];
                parameter = argv[3];
            }

            cmd_ptr = InterpretCommand(commandList, commandin, &cmd_ptr);   /*elhelyezi a cmd-ptr-ben az inputként kapott kommand címét*/
            if (cmd_ptr != NULL){
                (*cmd_ptr)(parameter, 1, imageInfo, undoBuffer);            /*meghívja a kommandhoz tartozó függvényt*/
            }
            Save(savepath, 2, imageInfo, undoBuffer);

            free(undoBuffer[0]);   /*egyetlen kommand esetén biztosan csak a megnyitott kép*/
            free(undoBuffer[1]);   /*és a kommand által módosított kép*/
            free(undoBuffer);      /*lesz az undoBuffer-en*/
            return 0;
        }
    }

    else if (argc == 2 && strcmp(argv[1], "help") == 0){ /*help kommandos futtatás esetén csak kiprinteli a helptextet*/
        Help(NULL, 0, imageInfo, undoBuffer);
        return 0;
    }

    else if (argc == 2 && (strcmp(argv[1], "save") || strcmp(argv[1], "exit") || strcmp(argv[1], "undo"))){ /*és néhány kommandot nincs értelme így futtatni*/
        pushmsg("command not supported in one-line-execution");
        return 1;
    }

    else{ /*helytelen formátumban megadott futtatással sem érdemes törődni*/
        pushmsg("arguments <path> or <path> <command> <savepath> or <path> <command> <parameter> <savepath> required");
        return 1;
    }

    while (run){ /*ez a kommandsoros futtatás, ahol megnyílik a program parancssora a shell-ben*/

        currStep = 0;
        while (undoBuffer[currStep] != NULL)
            currStep++;


        #ifdef __linux  /*szintén nem futtat windowson SDL kódot*/
        if (renderer == NULL && undoBuffer[currStep-1] != NULL){
           renderer = SYSTEM_SDLSetup(imageInfo);
        }

        if (renderer != NULL){
            SDLupdate(renderer, imageInfo, undoBuffer[currStep-1]);
            SDLrender(renderer);
        }
        #endif

        if (currStep == bufferLen -2){ /*undoBuffer shiftelése, ha megtelik*/
            free(undoBuffer[0]);
            for (int i = 0; i < currStep - 1; ++i)
            {
                undoBuffer[i] = undoBuffer [i + 1];
            }

            undoBuffer[currStep-1] = NULL;
            currStep -= 1;
        }

        printf("Paint.PPM: ");

        GetCommand(input); /*mégsem a scanf(" %s")-t használom mert picit furán viselkedett, ez most így jó*/
        SeparateCommandLine(input, command, parameter); /*szétszedi space-nél a commandot és a paramétert*/

        cmd_ptr = InterpretCommand(commandList, command, &cmd_ptr); /*elhelyezi a cmd-ptr-ben az inputként kapott kommand címét*/

        if (cmd_ptr != NULL)
            (*cmd_ptr)(parameter, currStep, imageInfo, undoBuffer);

        strcpy(command, "");
        strcpy(parameter, "");
        strcpy(input, "");

    }

    for (int i = 0; i <= currStep; ++i){
        free(undoBuffer[i]);
    }
    free(undoBuffer);
    return 0;
}
