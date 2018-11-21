#ifndef COMMANDS_H
#define COMMANDS_H

#include "PPMhandling.h"
#include <stdbool.h>

typedef struct Command{
	char id[20];
	void *func;
}Command;

void GetCommand(char *);
void SeparateCommandLine(char *input, char *command, char *argument);
bool Darken(char *amount,  int currStep, Info boolimgInfo, Pixel **undoBuffer);
bool Lighten(char *amount,  int currStep, Info boolimgInfo, Pixel **undoBuffer);
bool Invert(char *mustTakeArgument,  int currStep, Info boolimgInfo, Pixel **undoBuffer);
bool Greyscale(char *mustTakeArgument,  int currStep, Info boolimgInfo, Pixel **undoBuffer);
bool Contrast(char *mustTakeArgument,  int currStep, Info boolimgInfo, Pixel **undoBuffer);
bool Save(char *path, Pixel **img, Info boolimgInfo);
bool ExitProgram(char *mustTakeArgument, Pixel **img, Info boolimgInfo);

#endif