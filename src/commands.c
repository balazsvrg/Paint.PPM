#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../include/commands.h"
#include "../include/PPMhandling.h"
#include "../include/updates.h"

extern bool run;

/*ez a modul az inputként kapott kommandok kezelésével foglakozik
 *néhány kommand viselkedése itt van leírva, ezek azok amelyek nem
 *módosítanak a képen (az a PPMhandling.c feladata). Amik képfeldolgozó
 *kommandok, azoknak csak ellenőrzi a paramétereit, és továbbadja
 *a feladatot a PPMhandling.c-nek*/



void GetCommand(char *input){ //Beolvas egy sornyi karaktert és stringként elmenti (nem hagy \n-t a stdin-en)
    char c;
    int index = 0;
    bool run = true;

    while(run){
        c = getchar();

        if (c == '\n'){ /*ez az a kódrészlet amiért erre sahát függvényt használok*/
            input[index] = '\0'; /*kicseréli a \n -t \0-ra!!*/
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
    for (int i = 0; commandList[i].func != NULL; i++){ /*kikeresi a kommandlistából az inputtal egyező kommandot*/
        if(strcmp(command, commandList[i].id) == 0){
            cmd_ptr = commandList[i].func;
        }
    }                                                 
    
    if (cmd_ptr == NULL)                               /*és persze hibát is keres*/
        pushmsg("invalid command");

    return cmd_ptr;
}

void Darken(char *amount, int currStep, Info imgInfo, Pixel **undoBuffer){
    int value = atoi(amount);                               /*egységítés miatt minden kommand sztring paramétert kap*/
    if (value >= 0 && value <= 255){                        /*megfelelő paraméterre indul*/
        PPM_NextStep(imgInfo, currStep, undoBuffer);        /*az összes függvény így működik szóval a többit nem részletezem*/
        PPM_Darken(undoBuffer[currStep], value, imgInfo);
    }
    else
        pushmsg("invalid argument [1-255] needed");

    }

void Lighten(char *amount,  int currStep, Info imgInfo, Pixel **undoBuffer){
    int value = atoi(amount);
    if (value >= 0 && value <= 255){
        PPM_NextStep(imgInfo, currStep, undoBuffer);
        PPM_Lighten(undoBuffer[currStep], value, imgInfo);
    }
    else
        pushmsg("invalid argument [1-255] needed");
    }

void Invert(char *NotUsed,  int currStep, Info imgInfo, Pixel **undoBuffer){
    if (strcmp(NotUsed, "") == 0){
        PPM_NextStep(imgInfo, currStep, undoBuffer);
        PPM_Invert(undoBuffer[currStep], imgInfo);
    }
    else
        pushmsg("Invert takes no arguments");
}

void Greyscale(char *NotUsed,  int currStep, Info imgInfo, Pixel **undoBuffer){
    if (strcmp(NotUsed, "") == 0){
        PPM_NextStep(imgInfo, currStep, undoBuffer);
        PPM_Greyscale(undoBuffer[currStep], imgInfo);
    }
    else
        pushmsg("Greyscale takes no arguments");
}

void Contrast(char *NotUsed,  int currStep, Info imgInfo, Pixel **undoBuffer){
    if (strcmp(NotUsed, "") == 0){
        PPM_NextStep(imgInfo, currStep, undoBuffer);
        PPM_Contrast(undoBuffer[currStep],imgInfo);
    }
    else
        pushmsg("Contrast takes no arguments");
}

void Blur(char *amount,  int currStep, Info imgInfo, Pixel **undoBuffer){
    int size = atoi(amount);
    if (size > 1 && size <= 10){
        PPM_NextStep(imgInfo, currStep, undoBuffer);
        PPM_Blur(size, undoBuffer[currStep],imgInfo);
    }
    else
        pushmsg("invalid argument [1-10] needed");
}

void Undo(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel **undoBuffer){
    if (currStep != 1){
        free(undoBuffer[currStep-1]);
        undoBuffer[currStep -1] = NULL;
    }

    else
        pushmsg("undo buffer is empty");
    
}

void Save(char *path, int currStep, Info imgInfo, Pixel **undoBuffer){
    if (strcmp(path, "") != 0 && currStep > 1){
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

    else
        pushmsg("path needed to save");
}

void Help(char *NotUsed,  int NotUsed2, Info NotUsed3, Pixel **NotUsed4){ //kiprinteli a help fájl tartalmát
    FILE *fp;
    #ifdef __linux                  /*valamiért nem lehet megegyezni a sorvége jelen szóval*/
    fp = fopen("helptextlin", "r");
    #endif
    #ifndef __linux                 /*egyszerűen csak két külön fájl van linuxra és windowsra*/
    fp = fopen("helptextwin.txt", "r");
    #endif
    if (fp == NULL)
        pushmsg("cannot open helpfile");

    else{
        putchar('\n');
        char c = fgetc(fp);
        while(c != EOF){
            putchar(c);
            c = fgetc(fp);
        }
        putchar('\n');

        fclose(fp);
    }
}

void ExitProgram(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel **undoBuffer){ //kilép a programból (megszakítja a "végtelen" ciklust)    
    run = false;
    
}