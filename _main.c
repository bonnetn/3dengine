#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <math.h>

#include "object_creator.h"
#include "renderer.h"
#include "objfileloader.h"

int main( int argc, char** argv )
{


    int i;
    float coef;
    unsigned int j;
    TD_Space space;

    TD_Screen scr;
    TD_Pixel **pxl;/*[SCREEN_WIDTH][SCREEN_HEIGHT];*/
    TD_Camera cam;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture* texture;

    Uint32 pixels[SCREEN_HEIGHT*SCREEN_WIDTH];

    SDL_Event event;

    TD_Object fileObj;

    char loop = 1;

    loadObjFile( "cube.obj", &fileObj);

    new_point( &cam.point, 0, 0, 2 );
    new_vector( &cam.x, 0.01, 0, 0 );
    new_vector( &cam.y, 0, 0.01, 0 );

    coef =  1500;
    new_vector( &cam.f, -(cam.x.y*cam.y.z-cam.y.y*cam.x.z)*coef, -(cam.x.z*cam.y.x-cam.y.z*cam.x.x)*coef, -(cam.x.x*cam.y.y-cam.y.x*cam.x.y)*coef );

    printf("DirVect : %f;%f;%f\n", cam.f.x, cam.f.y, cam.f.z);

    pxl = malloc( SCREEN_WIDTH * sizeof(TD_Pixel*) );

    if( pxl == NULL )
    {
        puts("Failed to allocate.");
        exit(EXIT_FAILURE);
    }

    for( i = 0; i < SCREEN_WIDTH; i++)
    {
        pxl[i] = malloc( SCREEN_HEIGHT * sizeof(TD_Pixel) );
        if( pxl[i] == NULL )
        {
            puts("Failed to allocate.");
            exit(EXIT_FAILURE);
        }
    }

    scr.pxl = (TD_Pixel **) pxl;
    scr.w = SCREEN_WIDTH;
    scr.h = SCREEN_HEIGHT;

    puts("Objects created & initialized !");

    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
    {

        puts( "Could not initialize SDL");
        return EXIT_FAILURE;

    }


    puts("Rendering frame...");
    renderFrame( &space, &cam, &scr );
    puts("Rendering completed!");



    if( SDL_CreateWindowAndRenderer( SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL, &window, &renderer ))
    {
        printf("Failed to create the window : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_SetWindowTitle( window, "Moteur de rendu 3D");

    texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    for(j = 0; j < SCREEN_WIDTH*SCREEN_HEIGHT; j++)
    {
        TD_Pixel px = scr.pxl[(j%SCREEN_WIDTH)][(j/SCREEN_WIDTH)];
        pixels[j] = px.r;
        pixels[j] = pixels[j] << 8 | px.g;
        pixels[j] = pixels[j] << 8 | px.b;
        pixels[j] = pixels[j] << 8;

    }

    SDL_UpdateTexture( texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));

    SDL_RenderClear(renderer);
    SDL_RenderCopy( renderer, texture, NULL, NULL);
    SDL_RenderPresent( renderer );



    while( loop )

    {
        SDL_PollEvent( &event );
        if(event.type == SDL_QUIT)
        {
            loop = 0;

        }

    }

    puts("Freeing the variables...");
    for( i =0; i < SCREEN_WIDTH; i++ )
    {
        free(pxl[i]);
    }
    free(pxl);

    puts("done !");

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    free(fileObj.triangle);
    atexit(SDL_Quit);

    return 0;
}
