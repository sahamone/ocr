#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
//#include "transpose_image.c"
#include "solve.c"

#include "predict.c"

int main()
{

    pro_grid();
    pro_word();
    
    printf("la");
    pro_solv();
    
    
    return 1;
}