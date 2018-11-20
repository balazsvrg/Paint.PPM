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
void Darken(char *amount,  int currStep, Info imgInfo, Pixel ***undoBuffer);
void Lighten(char *amount,  int currStep, Info imgInfo, Pixel ***undoBuffer);
void Invert(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel ***undoBuffer);
void Greyscale(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel ***undoBuffer);
void Contrast(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel ***undoBuffer);
void Save(char *path, Pixel **img, Info imgInfo);
void ExitProgram(char *mustTakeArgument, Pixel **img, Info imgInfo);

#endif