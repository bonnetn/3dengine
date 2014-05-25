#include "objfileloader.h"

#include <stdio.h>
#include <stdlib.h>

static void countPointAndTriangles( FILE * f, int *pointCount, int *triangleCount )
{
    char readChar = 'a';
    int phaseTri, phasePt;
    int p,t;

    p = t = 0;

    while( readChar != EOF )
    {
        if(phasePt == 3)
        {
            p++;
            phasePt = 0;
        }
        if(phaseTri == 3)
        {
            t++;
            phaseTri = 0;
        }


        readChar = fgetc(f);
        if( (phasePt == 0 && readChar == '\n') || (phasePt == 1 && readChar == 'v') || (phasePt == 2 && readChar == ' ') )
            phasePt++;
        else
            phasePt = 0;

        if( (phaseTri == 0 && readChar == '\n') || (phaseTri == 1 && readChar == 'f') || (phaseTri == 2 && readChar == ' ') )
            phaseTri++;
        else
            phaseTri = 0;

    }

    *pointCount = p;
    *triangleCount = t;
    rewind(f);


}

int loadObjFile( char * fileName, TD_Object *object )
{
    FILE *f;
    int pointsC, trianglesC, i, triCursor;
    TD_Point *points;
    TD_Triangle *triangles;
    char readChar, phase;
    int a,b,c;

    readChar = 'a';
    i = triCursor = phase = 0;

    f = fopen( fileName, "r" );
    if( f == NULL )
    {
        puts("Failed to open the file.");
        return -1;
    }

    countPointAndTriangles( f, &pointsC, &trianglesC);

    points = malloc( sizeof(TD_Point) * pointsC );
    triangles = malloc( sizeof(TD_Triangle) * trianglesC );

    if( points == NULL || triangles == NULL)
    {
        printf("Could not allocate pointer");
        exit(EXIT_FAILURE);
    }

    while( readChar != EOF )
    {
        if(phase == 3)
        {
            fscanf(f, " %f %f %f", &(points[i].x), &(points[i].y), &(points[i].z));
            i++;
            phase = 0;
        }

        readChar = fgetc(f);
        if( (phase == 0 && readChar == '\n') || (phase == 1 && readChar == 'v') || (phase == 2 && readChar == ' ') )
            phase++;
        else
            phase = 0;

    }
    rewind(f);
    phase = 0;
    i = 0;
    readChar = 'a';

    while( readChar != EOF )
    {
        if(phase == 3)
        {
            fscanf(f, " %d%*s %d%*s %d", &a,&b,&c);

            triangles[i].pt1 = points[a-1];
            triangles[i].pt2 = points[b-1];
            triangles[i].pt3 = points[c-1];

            i++;
            phase = 0;
        }

        readChar = fgetc(f);
        if( (phase == 0 && readChar == '\n') || (phase == 1 && readChar == 'f') || (phase == 2 && readChar == ' ') )
            phase++;
        else
            phase = 0;

    }

    for( i = 0; i < trianglesC;i++)
    {

        printf("====[%d]====\n", i);
        printf("  1 : %f;%f;%f\n",triangles[i].pt1.x,triangles[i].pt1.y,triangles[i].pt1.z);
        printf("  2 : %f;%f;%f\n",triangles[i].pt2.x,triangles[i].pt2.y,triangles[i].pt2.z);
        printf("  3 : %f;%f;%f\n\n",triangles[i].pt3.x,triangles[i].pt3.y,triangles[i].pt3.z);

    }

    object->triangleCount = trianglesC;
    object->triangle = triangles;

    printf("File loaded. %d points & %d triangles found.\n", pointsC, trianglesC);

    free(points);
    fclose(f);
    return 0;
}
