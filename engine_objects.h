#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define TD_ObjectCount unsigned int

typedef struct TD_Point
{

  float x;
  float y;
  float z;

} TD_Point;

typedef struct TD_Point2d
{

  double x;
  double y;

} TD_Point2d;

typedef struct TD_Vector
{

    double x;
    double y;
    double z;

} TD_Vector;

typedef struct TD_Trace
{

    TD_Point point;
    TD_Vector vectDir;

} TD_Trace;

typedef struct TD_Poly
{

    TD_ObjectCount pointsCount;
    TD_Point *points;


} TD_Poly;

typedef struct TD_Triangle
{
    TD_Point pt1;
    TD_Point pt2;
    TD_Point pt3;
} TD_Triangle;


typedef struct TD_Object
{

    TD_ObjectCount triangleCount;
    TD_Triangle *triangle;

} TD_Object;

typedef struct TD_ObjectPoly
{

    TD_ObjectCount polyCount;
    TD_Poly *poly;

} TD_ObjectPoly;



typedef struct TD_Camera
{

    TD_Point point;
    TD_Vector x;
    TD_Vector y;
    TD_Vector f;

} TD_Camera;

typedef struct TD_Pixel
{

    unsigned char r;
    unsigned char g;
    unsigned char b;

} TD_Pixel;

typedef struct TD_Space
{

    TD_Object *obj;
    TD_ObjectCount objCount;

} TD_Space;


typedef struct TD_SpacePoly
{

    TD_ObjectPoly *obj;
    TD_ObjectCount objCount;

} TD_SpacePoly;



typedef struct TD_Screen
{
    TD_Pixel **pxl;
    unsigned int w;
    unsigned int h;

} TD_Screen;

void new_point( TD_Point *point, float x, float y, float z );
void new_vector( TD_Vector *vector, double x, double y, double z );

#endif // ENGINE_H_INCLUDED
