#ifndef SYSTEM_H
#define SYSTEM_H

#include <SDL2/SDL.h>
#include "PPMhandling.h"


void pushmsg(char *);
SDL_Renderer *SYSTEM_SDLSetup(Info imageInfo);
void SDLupdate(SDL_Renderer *renderer, Info imageInfo, Pixel *img);
void SDLrender(SDL_Renderer *renderer);

#endif