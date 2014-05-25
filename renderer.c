#include "renderer.h"

#include <math.h>
#include <stdio.h>


void renderFrame( TD_Space * objects, TD_Camera * cam, TD_Screen * scr, unsigned int xstart, unsigned int ystart, unsigned xmax, unsigned int ymax )
{
    unsigned int x,y;
    unsigned char prct=0;

    for( y = ystart; y < ymax; y++ )
    {
        for( x = xstart; x < xmax; x++ )
        {
            //printf("%d;%d\n", x, y);
            if(prct < y*100/scr->h)
            {
                prct = y*100/scr->h;
                //printf("%#2d%%\b\b\b", prct < 100 ? prct : 99);

            }
            //printf("%d%%\n", y*100/600);
            scr->pxl[x][y] = getScreenPixel( objects, cam, x, y, scr->w, scr->h );
            //puts("done");


        }
    }
}

static double lowestDist = 11000000;
static char changed = 0;

TD_Pixel getScreenPixel( TD_Space * objects, TD_Camera * cam, unsigned int x, unsigned int y, unsigned int w, unsigned int h )
{

    double distance;
    int color=0;
    TD_Pixel pxl;

    /* I am creating a TD_Trace object, if you need informations look doc/situation1.ggb */

    TD_Trace traceObj;

    /* = FocalPoint */
    traceObj.point = cam->point;
    /*traceObj.point.x = traceObj.point.x + cam->f.x ;
    traceObj.point.y = traceObj.point.y + cam->f.y ;
    traceObj.point.z = traceObj.point.z + cam->f.z */;

    /* vectDir = (FocalPoint) - (ScreenXY) */

    traceObj.vectDir.x = (/*cam->point.x + */cam->f.x) - ( /*cam->point.x + */cam->x.x * ((int)(x-w/2)) + cam->y.x * ((int)(y-h/2)) ) ;
    traceObj.vectDir.y = (/*cam->point.y + */cam->f.y) - ( /*cam->point.y + */cam->x.y * ((int)(x-w/2)) + cam->y.y * ((int)(y-h/2)) ) ;
    traceObj.vectDir.z = (/*cam->point.z + */cam->f.z) - ( /*cam->point.z + */cam->x.z * ((int)(x-w/2)) + cam->y.z * ((int)(y-h/2)) ) ;

    //puts("Tracing the line...");
    distance = trace( &traceObj, objects );
    //printf("Distance : %f\n", distance);

    if( distance > 0 )
    {
        //puts("White pixel");
        //color = (distance-10)*255/102;
        color = (distance-1600)*0.5;

        if(distance < lowestDist)
        {
            changed = 1;
            lowestDist = distance;
        }



        if(color > 255)
        {
            color = 255;
        }
        else if(color < 0)
            color = 0;
        pxl.b = pxl.g = pxl.r = color;
        /*pxl.b = rand()%256;
        pxl.r = rand()%256;
        pxl.g = rand()%256;*/

    }
    else
    {
        //puts("Black pixel");
        pxl.b = pxl.g = pxl.r = 0;
    }

    if(changed)
    {
        changed = 0;
        //printf("%f\n", lowestDist);
    }


    return pxl;
}

double trace( TD_Trace * trace, TD_Space * objects )
{

    unsigned int i;
    double closestDist, dist;

    closestDist = -1;

    for( i = 0; i < objects->objCount; i++ )
    {

        //printf("Checking for object #%d\n", i);
        dist = traceHitObject( trace, objects->obj + i);

        if( dist >= 0 && (dist < closestDist || closestDist == -1) )
            closestDist = dist;

    }
    return closestDist;

}


double traceHitObject( TD_Trace * trace, TD_Object * object )
{

    unsigned int i;
    double closestDist, dist;

    closestDist = -1;

    for( i = 0; i < object->triangleCount; i++ )
    {

        //printf("Checking for triangle #%d\n", i);
        dist = traceHitTriangle( trace, object->triangle + i);

        if( dist >= 0 && (dist < closestDist || closestDist == -1) )
        {
            closestDist = dist;
        }

    }
    return closestDist;

}

double traceHitTriangle( TD_Trace * trace, TD_Triangle * triangle )
{

    /* Calculations made by XCas (see doc/situation2.xws) */

    double  a_x, a_y, a_z,
            b_x, b_y, b_z,
            c_x, c_y, c_z,
            d_x, d_y, d_z,
            e_x, e_y, e_z;
    double q,t ;

    TD_Point2d pt2;

    /* Point where the line hit in the coordinate system */
    TD_Vector pt;

    /* A is the point of the plane */
    a_x = triangle->pt1.x;
    a_y = triangle->pt1.y;
    a_z = triangle->pt1.z;

    /* B is the first vector of the plane */
    b_x = triangle->pt2.x - a_x;
    b_y = triangle->pt2.y - a_y;
    b_z = triangle->pt2.z - a_z;

    /* C is the second vector of the plane */
    c_x = triangle->pt3.x - a_x;
    c_y = triangle->pt3.y - a_y;
    c_z = triangle->pt3.z - a_z;

    /* D is the point of the trace */
    d_x = trace->point.x;
    d_y = trace->point.y;
    d_z = trace->point.z;

    /* E is the directionnal vector of the trace */
    e_x = trace->vectDir.x;
    e_y = trace->vectDir.y;
    e_z = trace->vectDir.z;

    q = b_x*c_y*e_z - b_x*c_z*e_y - b_y*c_x*e_z + b_y*c_z*e_x + b_z*c_x*e_y - b_z*c_y*e_x;



    /* the line and the plane are parallel (never hit) */
    if( q == 0.0)
        return -1;

    //puts("Not parallel");

    t = (a_x*b_y*c_z-a_x*b_z*c_y-a_y*b_x*c_z+a_y*b_z*c_x+a_z*b_x*c_y-a_z*b_y*c_x-b_x*c_y*d_z+b_x*c_z*d_y+b_y*c_x*d_z-b_y*c_z*d_x-b_z*c_x*d_y+b_z*c_y*d_x) / q;
    pt2.x = (-a_x*c_y*e_z+a_x*c_z*e_y+a_y*c_x*e_z-a_y*c_z*e_x-a_z*c_x*e_y+a_z*c_y*e_x-c_x*d_y*e_z+c_x*d_z*e_y+c_y*d_x*e_z-c_y*d_z*e_x-c_z*d_x*e_y+c_z*d_y*e_x) / q;
    pt2.y = (a_x*b_y*e_z-a_x*b_z*e_y-a_y*b_x*e_z+a_y*b_z*e_x+a_z*b_x*e_y-a_z*b_y*e_x+b_x*d_y*e_z-b_x*d_z*e_y-b_y*d_x*e_z+b_y*d_z*e_x+b_z*d_x*e_y-b_z*d_y*e_x) / q;

    if( t > 0 && pt2.x >= 0 && pt2.x <= 1 && pt2.y <= 1-pt2.x+0.0000000000000001 && pt2.y >= 0)
    {

        //puts("Hit in the triangle");

        pt.x = (d_x + e_x * t);
        pt.y = (d_y + e_y * t);
        pt.z = (d_z + e_z * t);


        return sqrt( (pt.x-d_x)*(pt.x-d_x) + (pt.y-d_y)*(pt.y-d_y) + (pt.z-d_z)*(pt.z-d_z) );

    }
    else
    {
        //printf("%f / %f\n", (a_x*b_y*c_z-a_x*b_z*c_y-a_y*b_x*c_z+a_y*b_z*c_x+a_z*b_x*c_y-a_z*b_y*c_x-b_x*c_y*d_z+b_x*c_z*d_y+b_y*c_x*d_z-b_y*c_z*d_x-b_z*c_x*d_y+b_z*c_y*d_x), q);
        //printf("%f/%f=%f\n", a_x*b_y*c_z-a_x*b_z*c_y-a_y*b_x*c_z+a_y*b_z*c_x+a_z*b_x*c_y-a_z*b_y*c_x-b_x*c_y*d_z+b_x*c_z*d_y+b_y*c_x*d_z-b_y*c_z*d_x-b_z*c_x*d_y+b_z*c_y*d_x, q, t);

        //printf("  %f\n  %f\n  %f\n", pt.x, pt.y, pt.z);
        return -1;
    }

}
