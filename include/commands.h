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
void *InterpretCommand(const Command *commandList, char *command, void *cmd_ptr);
bool Darken(char *amount,  int currStep, Info boolimgInfo, Pixel **undoBuffer);
bool Lighten(char *amount,  int currStep, Info boolimgInfo, Pixel **undoBuffer);
bool Invert(char *mustTakeArgument,  int currStep, Info boolimgInfo, Pixel **undoBuffer);
bool Greyscale(char *mustTakeArgument,  int currStep, Info boolimgInfo, Pixel **undoBuffer);
bool Contrast(char *mustTakeArgument,  int currStep, Info boolimgInfo, Pixel **undoBuffer);
bool Undo(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel **undoBuffer);
bool Save(char *path, int currStep, Info imgInfo, Pixel **undoBuffer);
bool ExitProgram(char *mustTakeArgument,  int currStep, Info imgInfo, Pixel **undoBuffer);

#endif