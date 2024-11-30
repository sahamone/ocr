#include "simple_denoising.h"
#include "patterns.h"
#include <stdlib.h>
#include <string.h>

#define IS_BLACK 0
#define IS_WHITE 255



/*	Function : SimpleDenoising
 *
 *	--------------------------
 *
 *	Denoise a black/white surface by removing 
 *	pixels without (or only one) neighbour,
 *	or if it matches some predifined patterns.
 *
 *	@param surface	: target surface
 *
 *	@return			: void 
 */
void PRT_SimpleDenoising(SDL_Surface *surface)
{
	SDL_LockSurface(surface);
	pid_t parentId = getpid();
	pid_t pid = fork();

	if (pid == 0)
	{
		int status = 0;
		if (LOG_LEVEL >= 1)
		{
			printf("%s\nStarting average denoising...\n",
				LOG_SEPARATOR);
			waitpid(parentId, &status, 0);
 			printf("Average denoising successfully finished !\n%s\n",
					LOG_SEPARATOR);
		}
		_exit(EXIT_SUCCESS);
	}

	/* pseudo code
	 * chq pixel :
	 *	 SI pixel pas sur un bord et blanc :
	 *		on regarde les pixels autour 
	 *		si (aucun || 1 seul) est blanc :
	 *			pixel devient noir
	 *	
	 */ 

	long pixel_count = (surface->w) * (surface->h);
	Uint32* pixels = surface->pixels;
	Uint32* new_pixels = malloc(sizeof(Uint32) * pixel_count);
	long W = surface->w, H = surface->h;

	Uint8 color = 0;

	for (long pixel_index = 0; pixel_index < pixel_count; pixel_index++)
	{
		SDL_GetRGB(pixels[pixel_index],
				surface->format, &color, &color, &color);
		new_pixels[pixel_index] = pixels[pixel_index];
		if (!PixelIsOnSide(pixel_index, W, H) &&
				color == IS_WHITE &&
				WhiteNeighboursCount(pixel_index, surface) <= 1)
		{
			new_pixels[pixel_index] = SDL_MapRGB(surface->format,
					0, 0, 0);
			continue;
		}
		if (!PixelIsOnSideRange4(pixel_index, W, H) &&
				color ==  255)
		{
			Patterns_ReplaceLosange(surface,
					pixel_index, 1);
			Patterns_ReplaceBlob(surface,
					pixel_index);
		}
		//if (!PixelIsOnSide(pixel_index, W, H) &&
		//color == IS_WHITE &&
		//WhiteNeighboursCount(pixel_index, surface) >= 8)
		//{
		//	new_pixels[pixel_index] = SDL_MapRGB(surface->format,
		//	255, 255, 255);
		//}
	}
	
	memcpy(pixels, new_pixels, pixel_count*sizeof(Uint32));
	free(new_pixels);
	SDL_UnlockSurface(surface);
}
