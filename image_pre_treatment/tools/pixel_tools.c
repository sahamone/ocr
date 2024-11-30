#include "pixel_tools.h"
// This file contain all the tools function
// for letter-detection script detailed under

/*
This function return the rgb value
of a specific pixel according to 
the surface given in parameter.
RGB are given by ref
@param surface surface to process
@param x x coordinate of the pixel
@param y y coordinate of the pixel
*/
Uint32 GetPixelColor(SDL_Surface *surface, int x, int y) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    return pixels[(y * surface->w) + x];
}

/*
This function set the rgb value of
a specific pixel according to 
the surface given in parameter
@param surface surface to process
@param x x coordinate of the pixel
@param y y coordinate of the pixel
@param color color to set
*/
void SetPixelColor(SDL_Surface *surface,
        int x, int y, Uint32 r, Uint32 g, Uint32 b) {

    Uint32 *pixels = (Uint32 *)surface->pixels;
    pixels[(y * surface->w) + x] = SDL_MapRGB(surface->format, r, g, b);
}



/*	Function : PixelIsOnSide
 *
 *	--------------------------
 *
 *	Returns 1 if pixel is on a side of
 *	given width and height.
 *
 *	@param pixel_index	: index of target pixel in surface
 *	@param width		: associated surface's width
 *	@param height		: associated surface's height
 *
 *	@return				: 1 if on a side, else 0.
 */
long PixelIsOnSide(long pixel_index, long width, long height)
{
	long pixel_col = pixel_index%width;
	
    return 
        (pixel_col <= 1) ||
        (pixel_index <= width) ||
        (pixel_index >= (width * (height-1)));
}



/*	Function : PixelIsOnSideRange4
 *
 *	--------------------------
 *
 *	Returns 1 if pixel is within 4 pixels 
 *	of a side of given width and height.
 *
 *	@param pixel_index	: index of target pixel in surface
 *	@param width		: associated surface's width
 *	@param height		: associated surface's height
 *
 *	@return				: 1 if within 4 pixels of a side, else 0.
 */
long PixelIsOnSideRange4(long pixel_index, long width, long height)
{
	long pixel_col = pixel_index%width;
	
    return 
        (pixel_col <= 3) ||
        (pixel_col + 4 >= width) ||
        (pixel_index <= width*4) ||
        (pixel_index >= (width * (height-4)));
}
