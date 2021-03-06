#ifndef COMMANDS_H
#define COMMANDS_H

#include "PPMhandling.h"
#include <stdbool.h>

typedef struct Command{ /*a kommandlista elemeinek tipusa*/
    char id[20];
    void *func;
}Command;

void GetCommand(char *);
void SeparateCommandLine(char *input, char *command, char *argument);
void *InterpretCommand(const Command *commandList, char *command, void *cmd_ptr);
void Darken(char *amount,  int currStep, Info boolimgInfo, Pixel **undoBuffer);
void Lighten(char *amount,  int currStep, Info boolimgInfo, Pixel **undoBuffer);
void Invert(char *mustTakeArgument,  int currStep, Info boolimgInfo, Pixel **undoBuffer);
void Greyscale(char *mustTakeArgument,  int currStep, Info boolimgInfo, Pixel **undoBuffer);
void Contrast(char *mustTakeArgument,  int currStep, Info boolimgInfo, Pixel **undoBuffer);
void Blur(char *amount,  int currStep, Info imgInfo, Pixel **undoBuffer);
void Undo(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel **undoBuffer);
void Save(char *path, int currStep, Info imgInfo, Pixel **undoBuffer);
void Help(char *NotUsed,  int NotUsed2, Info NotUsed3, Pixel **NotUsed4);
void ExitProgram(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel **undoBuffer);

#endif