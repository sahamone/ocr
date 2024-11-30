#include "rotation.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#define NOT_IN_IMAGE -1

double absf(double x)
{
	if (x < 0)
		return -x;
	return x;
}



/*	Function : getXFromPixelNb
 *
 *	--------------------------
 *
 *	Computes X coordinate from pixel number of 
 *	an image of given width
 *
 *	x		: pixel number in image's pixels array
 *  width	: image's width
 *
 *	returns	: coordinate on X axis (from LEFT to RIGHT) 
 */
int getXFromPixelNb(int x, unsigned int width)
{
	return x % width - (width / 2);
}



 /*	Function : getYFromPixelNb
 *
 *	--------------------------
 *
 *	Computes Y coordinate from pixel number of 
 *	an image of given width and height
 *
 *	x		: pixel number in image pixels' array
 *	width	: image's width
 *	height	: image's heigth
 *
 *	returns	: coordinate on Y axis (from TOP to BOTTOM) 
 */
int getYFromPixelNb(int x, unsigned int width, unsigned int height)
{
	return x / width - (height / 2);
}



 /*	Function : getPixelNbFromXY
 *
 *	--------------------------
 *
 *	Computes  coordinate from pixel number of 
 *	an image of given width and height.
 *	If coordinates are not in image, returns NOT_IN_IMAGE.
 *
 *	x		: pixel's X axis cootdinate (from LEFT to RIGHT)
 *	y		: pixel's Y axis coordinate (from TOP to BOTTOM)
 *	width	: image's width
 *	height	: image's heigth
 *
 *	returns	: pixel's index in image array, 
 *			or NOT_IN_IMAGE.
 */
long getPixelNbFromXY(int x, int y, unsigned int width, unsigned int height)
{	
	long retour_x = (x + width/2);
	if (retour_x < 0 || retour_x >= width)
		return NOT_IN_IMAGE;

	long retour_y = (y + height/2);
	if (retour_y < 0 || retour_y >= height)
		return NOT_IN_IMAGE;

	return (retour_y * width) + retour_x;
}


 /*	Function : rotate
 *
 *	--------------------------
 *
 *	Rotates a surface of <angle> degrees
 *
 *	surface	: surface to rotate
 *	angle	: angle (degrees) to rotate the surface with
 *
 *	returns	: void
 */
void rotate(SDL_Surface *surface, int angle) 
{   
	if (LOG_LEVEL)
		printf("---------------\nStarting rotation...\n");
	
	double newAngle = (double)angle / 180 * M_PI;
	
	SDL_LockSurface(surface);
	
	if (LOG_LEVEL > 1)	{printf("Radian angle: %f\n", newAngle);}

	unsigned int initW = surface->w,	initH = surface->h;
	unsigned int newW = initH * absf(sin(newAngle)) +
		initW * absf(cos(newAngle));
	unsigned int newH = initH + absf(cos(newAngle)) +
		initW * absf(sin(newAngle));
	
	if (LOG_LEVEL > 1){
		printf("Initial width : %i, initial height : %i\n",
				initW, initH);
		printf("New width : %i, new height : %i\n",
				newW, newH);}

	SDL_Surface* newOne = SDL_CreateRGBSurface(0,
			newW,newH,32,0,0,0,0);

	Uint32* pixelDepart = surface->pixels;
	Uint32* pixelPtr = newOne->pixels;
	unsigned long pixelCount = newW * newH;

	for(unsigned long pixelNb=0; pixelNb < pixelCount; pixelNb++)
	{	
		int x_depart = 
			getXFromPixelNb(pixelNb, newW) *
			cos(newAngle) 
			+
			getYFromPixelNb(pixelNb, newW, newH) *
			sin(newAngle);
        int y_depart = 
			-getXFromPixelNb(pixelNb, newW)*
			sin(newAngle) 
			+
			getYFromPixelNb(pixelNb, newW, newH) *
			cos(newAngle);
	
		long pixelDepartNb = getPixelNbFromXY(x_depart, y_depart,
				initW, initH);
		
		if (pixelDepartNb == NOT_IN_IMAGE) 
		{
			pixelPtr[pixelNb] = 0;//SDL_MapRGBA(newOne->format, 0, 0, 0, 0);
		}
		else
		{	
			pixelPtr[pixelNb] = pixelDepart[pixelDepartNb];
		}
	}
	*surface = *newOne;
	SDL_UnlockSurface(surface);
	
	if (LOG_LEVEL)
		printf("Image successfully rotated !\n---------------\n");
}


