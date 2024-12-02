#include "pre_treatment.h"
#include "rotation/rotation.h"

void PRT_Heavy(SDL_Surface* surface)
{
	PRT_Grayscaling(surface);
	PRT_AverageDenoising(surface);
	PRT_BlackWhite(surface);
	PRT_AverageDenoisingBlackWhite(surface);
	PRT_SimpleDenoising(surface);
	PRT_xy_denoising(surface);
}

void PRT_Medium(SDL_Surface* surface)
{
	PRT_Grayscaling(surface);
	PRT_BlackWhite(surface);
	PRT_AverageDenoisingBlackWhite(surface);
	PRT_SimpleDenoising(surface);
	PRT_xy_denoising(surface);
}

void PRT_Light(SDL_Surface* surface)
{
	PRT_Grayscaling(surface);
	PRT_BlackWhite(surface);
	//PRT_AverageDenoisingBlackWhite(surface);
	//PRT_SimpleDenoising(surface);
	//PRT_xy_denoising(surface);
}

void PRT_Rotate(SDL_Surface* surface, int angle)
{
	rotate(surface, angle);
}
