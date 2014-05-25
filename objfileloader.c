#include "objfileloader.h"
#include "triangulation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static int countLineType( FILE * f, char * prefix )
{
    char * buffer;
    int prefixLen, bufLen, count;

    count = 0;
    prefixLen = strlen(prefix);
    buffer = malloc( sizeof(char) * (prefixLen+1) );
    buffer[ prefixLen ] = '\0';

    if( buffer == NULL )
        exit(EXIT_FAILURE);

    rewind(f);

    while( !feof(f) )
    {

        fgets( buffer, prefixLen + 1, f );

        bufLen = strlen(buffer);
        if( bufLen > 0 && buffer[bufLen - 1] != '\n' )
            while( getc(f) != '\n' && !feof(f));

        if( !feof(f) && strcmp( buffer, prefix ) == 0 )
            ++count;

    }


    free(buffer);
    rewind(f);

    return count;

}

static void fillPoly( FILE * f, TD_ObjectPoly * obj, TD_Point * vertexes)
{

    char buffer[1024];
    char *buf;
    int ptCnt;
    int pt, poly, i;
    int bufferPt[32];

    poly = 0;

    obj->polyCount = countLineType( f, "f ");
    obj->poly = malloc( obj->polyCount * sizeof(TD_ObjectPoly) );
    if(obj->poly == NULL)
        exit(EXIT_FAILURE);

    rewind(f);

    while( !feof(f) && poly < 1024 )
    {
        fgets( buffer, 1024, f);
        if( memcmp( buffer, "f ", 2) == 0 )
        {
            ptCnt = 0;
            buf = buffer + 3;

            buf = strtok( buf, " " );

            while( buf != NULL )
            {
                if( sscanf( buf, "%d%*s", bufferPt + ptCnt) == 1)
                    ++ptCnt;
                buf = strtok( NULL, " " );

            }

            obj->poly[poly].pointsCount = ptCnt;

            obj->poly[poly].points = malloc( sizeof(TD_Point) * ptCnt );

            //printf("%d => %p\n", poly, obj->poly[poly].points);
            if(obj->poly[poly].points == NULL)
                exit(EXIT_FAILURE);

            for(i = 0; i < ptCnt; i++)
            {
                obj->poly[poly].points[i] = vertexes[bufferPt[i]-1];
            }

            ++poly;

        }
    }

    //printf("PolyCount: %d\n",obj->polyCount);

}

static void fillVertexesArray( FILE * f, TD_Point * vertexes )
{
    char buffer[3];
    int vertexIndex = 0;

    rewind(f);
    while( !feof(f) )
    {

        fgets( buffer, 3, f);
        if( strcmp( buffer, "v " ) == 0 )
        {
            fscanf(f, "%f %f %f", &(vertexes[vertexIndex].x), &(vertexes[vertexIndex].y), &(vertexes[vertexIndex].z) ); //
            vertexIndex++;
        }

        if( buffer[0] != '\0' && buffer[strlen(buffer)-1] != '\n')
            while(!feof(f) && fgetc(f) != '\n');

    }

}

static int loadObjPolyFile( char * fileName, TD_SpacePoly *space )
{

    FILE * f;
    TD_Point *vertexes;

    f = fopen( fileName, "r");
    if( f == NULL )
        return -1;

    vertexes = malloc( sizeof(TD_Point) * countLineType( f, "v ") );
    if( vertexes == NULL )
        exit(EXIT_FAILURE);

    fillVertexesArray( f, vertexes );


    space->objCount = 1;
    space->obj = malloc( sizeof(TD_ObjectPoly) );
    if( space->obj == NULL )
        exit(EXIT_FAILURE);

    fillPoly(f, space->obj, vertexes);


    //space->obj[0].polyCount = countLineType(f, "f ");
    //space->obj[0].poly = malloc( sizeof(TD_Poly) * space->obj[0].polyCount );

    free( vertexes );

    fclose(f);

    return 0;
}


static void freeObjPolyFile( TD_SpacePoly * space )
{

    int i,j;

    //puts("1");
    for(i = 0; i < space->objCount; i++)
    {
        //printf("=====%d====\n",i);
        for(j=0; j < space->obj[i].polyCount; j++)
        {
            //printf("i=%d;j=%d;PtCnt:%p\n", i,j,space->obj[i].poly[j].points);
            free(space->obj[i].poly[j].points);
        }
        if( space->obj[i].polyCount > 0)
            free(space->obj[i].poly);
    }
    //puts("op");
    if( space->objCount > 0 )
        free(space->obj);

    space->obj = NULL;
    space->objCount = 0;

}

static void triangulatePoly( TD_ObjectPoly *obj, TD_Object *triangulatedObj )
{
    /*int i;

    triangulatedObj->triangleCount = obj->polyCount;
    triangulatedObj->triangle = malloc( sizeof(TD_Triangle) * obj->polyCount );
    if( triangulatedObj->triangle == NULL )
        exit(EXIT_FAILURE);

    for(i = 0; i < obj->polyCount; i++)
    {

        triangulatedObj->triangle[i].pt1 = obj->poly[i].points[0];
        triangulatedObj->triangle[i].pt2 = obj->poly[i].points[1];
        triangulatedObj->triangle[i].pt3 = obj->poly[i].points[2];

    }*/
    triangulatedObj->triangleCount = 0;
    unsigned int i;
    for(i = 0; i < obj->polyCount; i++)
    {
        triangulatedObj->triangleCount += obj->poly[i].pointsCount - 2;
    }
        printf("poly count %d", obj->poly[i].pointsCount);
        printf("Tri count %d ",triangulatedObj->triangleCount);
    triangulatedObj->triangle = malloc( sizeof(TD_Triangle) * obj->polyCount );
    if( triangulatedObj->triangle == NULL )
        exit(EXIT_FAILURE);
    //printf("\n       x=%d,y=%d,z=%d",obj->poly[0].points[0].x,obj->poly[0].points[0].y,obj->poly[0].points[0].z);
    triangulate(obj, triangulatedObj);
    //printf("afterwards");
}


int loadObjFile( char * fileName, TD_Space *space )
{

    TD_SpacePoly spacePoly;
    int i;


    if( loadObjPolyFile( fileName, &spacePoly ) < 0)
        return -1;


    space->objCount = spacePoly.objCount;
    space->obj = malloc( sizeof(TD_Object) * spacePoly.objCount );
    if( space->obj == NULL )
        exit(EXIT_FAILURE);



    for(i=0; i < spacePoly.objCount; i++)
    {
        triangulatePoly(spacePoly.obj + i, space->obj + i);
    }

    freeObjPolyFile( &spacePoly);

    return 0;
}


void freeObjFile( TD_Space * space )
{

    int i;

    for(i = 0; i < space->objCount; i++)
    {
        if( space->obj[i].triangleCount > 0 )
            free(space->obj[i].triangle);
    }

    if( space->objCount > 0 )
        free(space->obj);

    space->obj = NULL;
    space->objCount = 0;

}

