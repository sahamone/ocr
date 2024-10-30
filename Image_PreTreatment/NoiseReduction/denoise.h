#ifndef DENOISE_H
#define DENOISE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <err.h>
#include "../Tools/tools.h"
#include "../../Common/ocr.h"
#include "patterns.h"


long OnSide(long index, long width, long height);
void denoise_simple(SDL_Surface *surface);


#endif
