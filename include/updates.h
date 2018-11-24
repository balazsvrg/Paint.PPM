#ifndef SYSTEM_H
#define SYSTEM_H

#ifdef __linux
#include <SDL2/SDL.h>
#endif

#include "PPMhandling.h"


void pushmsg(char *);

#ifdef __linux
SDL_Renderer *SYSTEM_SDLSetup(Info imageInfo);
void SDLupdate(SDL_Renderer *renderer, Info imageInfo, Pixel *img);
void SDLrender(SDL_Renderer *renderer);
#endif
#endif