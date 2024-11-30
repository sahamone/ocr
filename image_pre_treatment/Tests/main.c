#include "main.h"

//Main file of the pre-treatment part of the OCR project
int main(int argc, char *argv[]) {
    if (argc != 3) {
        errx(EXIT_FAILURE, "Invalid number of arguments!\n
				./pre-treatment 
				<all/rotate/blackwhite/grayscale/>
				<input_image_path>\n");
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        errx(EXIT_FAILURE, "SDL_Init failed: %s\n", SDL_GetError());
    }

    SDL_Surface *surface = Tools_LoadImage(argv[2]);
    if (!surface) {
        errx(EXIT_FAILURE, "Error while loading image: %s\n", IMG_GetError());
    }

	if (!strcmp(argv[1],"rotate"))
		rotate(surface, -25);

	if (!strcmp(argv[1],"grayscale") || !strcmp(argv[1],"all"))
		PRT_Grayscaling(surface);

	if (!strcmp(argv[1],"heavy"))
		PRT_Heavy(surface);
	if (!strcmp(argv[1],"medium"))
		PRT_Medium(surface);

	if (!strcmp(argv[1],"light"))
		PRT_Light(surface);


	if (!strcmp(argv[1],"dgray"))
	{
		PRT_Grayscaling(surface);
		PRT_AverageDenoising(surface);
		PRT_BlackWhite(surface);
		PRT_AverageDenoisingBlackWhite(surface);
		PRT_SimpleDenoising(surface);
		PRT_xy_denoising(surface);
		PRT_AverageDenoisingBlackWhite(surface);
		printf("noised reduction\n");
	}

	if (!strcmp(argv[1],"blackwhite") || !strcmp(argv[1],"all"))
		PRT_BlackWhite(surface);
   
	if (!strcmp(argv[1],"base"))
	{
		PRT_Grayscaling(surface);		
		PRT_BlackWhite(surface);
	}   

	Tools_SaveImage(surface, "result.png");//argv[2]);
    
	if (LOG_LEVEL)
		printf("Image (result.png) saved\n");

    SDL_FreeSurface(surface);
    SDL_Quit();

    if (LOG_LEVEL)
		printf("Pre-treatment done!\n");
    return EXIT_SUCCESS;
}
