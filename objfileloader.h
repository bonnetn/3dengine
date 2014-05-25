#ifndef OBJFILELOADER_H_INCLUDED
#define OBJFILELOADER_H_INCLUDED

#include "engine_objects.h"

int loadObjFile( char * fileName, TD_Space *space );
void freeObjFile( TD_Space * space );

#endif // OBJFILELOADER_H_INCLUDED
