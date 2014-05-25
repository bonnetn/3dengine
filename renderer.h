#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED


#include "engine_objects.h"

void renderFrame( TD_Space * objects, TD_Camera * cam, TD_Screen * scr, unsigned int xstart, unsigned int ystart, unsigned xmax, unsigned int ymax);

TD_Pixel getScreenPixel( TD_Space * objects, TD_Camera * cam, unsigned int x, unsigned int y, unsigned int w, unsigned int h );

double trace( TD_Trace * trace, TD_Space * objects );

double traceHitObject( TD_Trace * trace, TD_Object * object );

double traceHitTriangle( TD_Trace * trace, TD_Triangle * triangle );




#endif // RENDERER_H_INCLUDED
