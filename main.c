#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <math.h>

#include "renderer.h"
#include "objfileloader.h"

typedef struct argsThread
{
    TD_Space *objects;
    TD_Camera * cam;
    TD_Screen * scr;
    unsigned int xstart;
    unsigned int ystart;
    unsigned int xmax;
    unsigned int ymax;

} argsThread;

//void renderFrame( TD_Space * objects, TD_Camera * cam, TD_Screen * scr, unsigned int xstart, unsigned int ystart, unsigned xmax, unsigned int ymax);


int threadFn(void* data);

int main( int argc, char** argv )
{


    int i,i2;
    float coef;
    unsigned int j;

    TD_Screen scr;
    TD_Pixel **pxl;/*[SCREEN_WIDTH][SCREEN_HEIGHT];*/
    TD_Camera cam;
    TD_Space spaceLoaded;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture* texture;
    SDL_Event event;
    SDL_Thread *thread[4];
    char * path;

    argsThread args[4];

    Uint32 *pixels;

    char loop = 1;

    if(argc < 12 && argc != 1)
    {
        printf("ERROR: Missing argument... (%d/12)\n", argc);
        return EXIT_FAILURE;

    }
    if( argc != 1 )
        path = argv[11];

    else
        path = "teapot.obj";


    puts("======[ 3D Engine ]=====\n");

    printf("  [ ] Loading the OBJ file...");
    if(loadObjFile( path, &spaceLoaded) < 0)
    {
        printf("\r  /!\\\n      Could not open the file. (%s)\n", path);
        exit(EXIT_FAILURE);
    }
    puts("\r  [x");

    //spaceLoaded.obj[0].triangleCount = spaceLoaded.obj[0].triangleCount/4;

    printf("  [ ] Creating the camera...");
    if(argc != 1)
    {
        new_point( &cam.point, strtod(argv[1], NULL), strtod(argv[2], NULL), strtod(argv[3], NULL) );
        new_vector( &cam.x, strtod(argv[4], NULL), strtod(argv[5], NULL), strtod(argv[6], NULL) );
        new_vector( &cam.y, strtod(argv[7], NULL), strtod(argv[8], NULL), strtod(argv[9], NULL) );
        coef = strtod(argv[10], NULL);
        new_vector( &cam.f, -(cam.x.y*cam.y.z-cam.y.y*cam.x.z)*coef, -(cam.x.z*cam.y.x-cam.y.z*cam.x.x)*coef, -(cam.x.x*cam.y.y-cam.y.x*cam.x.y)*coef );

    }
    else
    {
        new_point( &cam.point, 1000, 1000, 1000 );
        new_vector( &cam.x, -0.1, 0, 0.1 );
        new_vector( &cam.y, -0.1, 0.1, -0.1 );
        new_vector( &cam.f, -400, -400, -400 );
        //coef =  20000;
    }
    printf(" - DirVect: [%.2f;%.2f;%.2f]", cam.f.x, cam.f.y, cam.f.z);
    puts("\r  [x");


    printf("  [ ] Allocating the screen...");
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

    puts("\r  [x");

    printf("  [ ] Initializing the SDL library...");
    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
    {

        puts( "Could not initialize SDL");
        return EXIT_FAILURE;

    }
    puts("\r  [x");


    args[3].objects = &spaceLoaded;
    args[3].cam = &cam;
    args[3].scr = &scr;

    args[2] = args[3];
    args[1] = args[3];
    args[0] = args[3];



    args[0].xstart = scr.w*0/4;
    args[0].ystart = 0;
    args[0].xmax = scr.w*1/4;
    args[0].ymax = scr.h;

    thread[0] = SDL_CreateThread( threadFn, "TD_Thread0", (void*) (args + 0));


    args[1].xstart = scr.w*1/4;
    args[1].ystart = 0;
    args[1].xmax = scr.w*2/4;
    args[1].ymax = scr.h;

    thread[1] = SDL_CreateThread( threadFn, "TD_Thread1", (void*) (args + 1));

    args[2].xstart = scr.w*2/4;
    args[2].ystart = 0;
    args[2].xmax = scr.w*3/4;
    args[2].ymax = scr.h;

    thread[2] = SDL_CreateThread( threadFn, "TD_Thread2", (void*) (args + 2));


    args[3].xstart = scr.w*3/4;
    args[3].ystart = 0;
    args[3].xmax = scr.w*4/4;
    args[3].ymax = scr.h;

    thread[3] = SDL_CreateThread( threadFn, "TD_Thread3", (void*) (args + 3));


    printf("  [ ] Rendering the 3D object...");
    SDL_WaitThread(thread[0], NULL);
    SDL_WaitThread(thread[1], NULL);
    SDL_WaitThread(thread[2], NULL);
    SDL_WaitThread(thread[3], NULL);



    //renderFrame( &spaceLoaded, &cam, &scr, 0, 0, scr.w, scr.h );
    puts("\r  [x");




    printf("  [ ] Creating the window...");
    if( SDL_CreateWindowAndRenderer( SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL, &window, &renderer ))
    {
        printf("Failed to create the window : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_SetWindowTitle( window, "Moteur de rendu 3D");
    puts("\r  [x");

    printf("  [ ] Filling the screen...");

    pixels = malloc( sizeof(Uint32) * (SCREEN_HEIGHT*SCREEN_WIDTH) );
    if(pixels == NULL)
    {
        puts("Failed to allocate the pixel variable.");
        exit(EXIT_FAILURE);
    }

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

    free(pixels);

    puts("\r  [x");
    printf("  [ ] Waiting...");

    while( loop )

    {
        SDL_PollEvent( &event );
        if(event.type == SDL_QUIT)
        {
            loop = 0;

        }

    }

    puts("\r  [x");
    printf("  [ ] Freeing the objects...");

    freeObjFile( &spaceLoaded );

    for( i =0; i < SCREEN_WIDTH; i++ )
    {
        free(pxl[i]);
    }
    free(pxl);

    puts("\r  [x");

    printf("  [ ] Closing the window & SDL...");

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    atexit(SDL_Quit);

    puts("\r  [x");
    puts("\n======[   Done!   ]=====\n");

    return 0;
}

int threadFn(void* data)
{

    argsThread *args = (argsThread *) data;
    renderFrame( args->objects, args->cam, args->scr, args->xstart, args->ystart, args->xmax, args->ymax );
    return 0;

}
