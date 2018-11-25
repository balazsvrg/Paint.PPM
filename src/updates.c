#include <stdio.h>

#ifdef __linux
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#endif

#include "../include/PPMhandling.h"

void pushmsg(char *msg){ /*kiír a stdout-ra egy Paint.PPN taggal*/
    fprintf(stderr, "Paint.PPM: %s\n", msg);
}

#ifdef __linux /*szintén SDL-es függvények szóval windowson nem kéne lefordítani*/
SDL_Renderer *SYSTEM_SDLSetup(Info imageInfo){
    SDL_Window *window = SDL_CreateWindow("Paint.PPM", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, imageInfo.width, imageInfo.height, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    SDL_RenderClear(renderer);
    return renderer;
}

void SDLupdate(SDL_Renderer *renderer, Info imageInfo, Pixel *img){
    for (int i = 0; i < imageInfo.height; i++)
        for (int j = 0; j < imageInfo.width; j++)
            pixelRGBA(renderer, j, i, img[imageInfo.width * i + j].r, 
                                      img[imageInfo.width * i + j].g, 
                                      img[imageInfo.width * i + j].b, 255);
}

void SDLrender(SDL_Renderer *renderer){
    SDL_RenderPresent(renderer);
}
#endif