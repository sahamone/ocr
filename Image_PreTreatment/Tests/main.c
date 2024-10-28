#include "main.h"
#include <string.h>

//Main file of the pre-treatment part of the OCR project

int main(int argc, char *argv[]) {
    if (argc != 3) {
        errx(EXIT_FAILURE, "Invalid number of arguments!\n./pre-treatment <all/rotate/blackwhite/grayscale/> <input_image_path>\n");
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        errx(EXIT_FAILURE, "SDL_Init failed: %s\n", SDL_GetError());
    }

    SDL_Surface *surface = load_image(argv[2]);
    if (!surface) {
        errx(EXIT_FAILURE, "Error while loading image: %s\n", IMG_GetError());
    }

	if (!strcmp(argv[1],"rotate"))
		rotate(surface, -25);

	if (!strcmp(argv[1],"grayscale") || !strcmp(argv[1],"all"))
		grayscaling(surface);

	if (!strcmp(argv[1],"blackwhite") || !strcmp(argv[1],"all"))
		blackWhite(surface);
   
	if (!strcmp(argv[1],"grayscale") || !strcmp(argv[1],"all"))
	{noise_reduction(surface, 100);
    printf("noise reduction\n");
	}
    

	save_image(surface, "result.png");//argv[2]);
    printf("image saved\n");

    SDL_FreeSurface(surface);
    SDL_Quit();

    printf("Pre-treatment done!\n");
    return EXIT_SUCCESS;
}
