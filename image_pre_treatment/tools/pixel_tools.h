#ifndef PIXEL_TOOLS_H
#define PIXEL_TOOLS_H

#include <SDL2/SDL_stdinc.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

Uint32 GetPixel(Uint32* const pixels,
        int const line, int const row, int const width);
int SetPixel(Uint32* pixels,
        int const line, int const row,
       Uint8 const r, Uint8 const g, Uint8 const b);
long PixelIsOnSide(long pixel_index, long width, long height);
long PixelIsOnSideRange4(long pixel_index, long width, long height);

#endif
