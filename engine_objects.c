#include "engine_objects.h"

void new_point( TD_Point *point, float x, float y, float z )
{

    point->x = x;
    point->y = y;
    point->z = z;

}

void new_vector( TD_Vector *vect, double x, double y, double z )
{

    vect->x = x;
    vect->y = y;
    vect->z = z;

}


