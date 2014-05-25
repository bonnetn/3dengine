#include "triangulation.h"

#include "engine_objects.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void triangulate(TD_ObjectPoly* objATri, TD_Object* triangulatedObj);
void nullifyTri(TRI_ears ears, TRI_Triangle *tri, TRI_Triangle nul);
void createPoints(TD_Point *pN, TRI_Point *p, TRI_ears ears);
void createPoly(TRI_Point *point, TRI_Poly *poly);
int createTri(TRI_Poly *poly, TRI_Triangle *tri, TRI_ears ears, TRI_Point null1, TRI_Triangle nul);
TRI_Vector makeVec(TRI_Point a, TRI_Point b);
int conVorC(TRI_Point p1, TRI_Point p2, TRI_Point p3);
void findAngle(TRI_Poly *poly, TRI_ears ears, TRI_Point null1);
double angle(TRI_Vector a, TRI_Vector b);
float zCoord(TRI_Point p1, TRI_Point p2, TRI_Point p3);
int isInside(TRI_Point *p, TRI_Point a, TRI_Point b, TRI_Point c,TRI_ears ears);
int triangulation(TRI_Triangle *tri, TRI_Triangle *ear, int len);
TRI_ears cutEar(TRI_Triangle *earCut, TRI_Triangle *ear, TRI_Point *p, int lastTri, TRI_Triangle nul, TRI_Point null1, TRI_ears ears);
int comparePoint(TRI_Point a, TRI_Point b);
int compareTriangle(TRI_Triangle a, TRI_Triangle b);
int TRI_TD(TRI_Triangle *ear, TRI_ears ears, TD_Object* triangulatedObj, int arrayLen, TRI_Triangle nul);
void showData(TD_Object* triangulatedObj, TRI_ears ears);

//Triangulation d'une polygon
void triangulate(TD_ObjectPoly* objATri, TD_Object* triangulatedObj)
{
  int j = 0;
  int lenOfTriArray = 0;
  TRI_ears ears;
  int i;
  for(i = 0; i < objATri->polyCount; i++)
  {
//Variable init
        ears.verts = objATri->poly[i].pointsCount;
        ears.TotalVerts = objATri->poly[i].pointsCount;
        ears.numEarCut = 0;

    //Cree tout les tableau
    TRI_Triangle *tri = malloc( sizeof(TRI_Triangle) * ears.TotalVerts );
    if( tri == NULL )
        exit(EXIT_FAILURE);
    TRI_Triangle *ear= malloc( sizeof(TRI_Triangle) * ears.TotalVerts );
    if( ear == NULL )
        exit(EXIT_FAILURE);
    TRI_Triangle *earCut= malloc( sizeof(TRI_Triangle) * ears.TotalVerts );
    if( earCut == NULL )
        exit(EXIT_FAILURE);
    TRI_Point *p = malloc( sizeof(TRI_Point) * ears.TotalVerts );
    if( p == NULL )
        exit(EXIT_FAILURE);

    TRI_Point null1;                // = new Point(0,0,0,-1,-1,-1);
        null1.x = null1.y = null1.z = 0;
        null1.prev = null1.next = null1.current = null1.angle = -1;
    TRI_Triangle nul;           // = new Triangle(null1,null1,null1,-1,-1);
        nul.p1 = nul.pE = nul.p2 = null1;
        nul.prev = nul.next = 0;
    TRI_Poly poly;

    int amountOfTri = 0;
    int earLen = 0;


//Setup
    //Debut de la triangulation
    //Nullify les triangles
    nullifyTri(ears, tri, nul);
    nullifyTri(ears, ear, nul);
    nullifyTri(ears, earCut, nul);

    //Cree les Points
    createPoints(objATri->poly[i].points, p, ears);
    //Cree le Poly
    createPoly(p, &poly);
    //Trouver si les points sont convex ou concave
    findAngle(&poly, ears, null1);
    //Cree les Triangles
    amountOfTri = createTri(&poly, tri, ears, null1, nul);
    //Triangulate
    earLen = triangulation(tri, ear, amountOfTri);
    //Enlever un oreilles
    ears = cutEar(earCut,ear, p, 0, nul, null1, ears);
//Triangulate
    while(ears.numEarCut < ears.TotalVerts-2)
    {
        //Refaire le Poly
        createPoly(p, &poly);
        //Recalcul l'Angle
        findAngle(&poly, ears, null1);
        //Refaire les Triangles
        amountOfTri = createTri(&poly, tri, ears, null1, nul);
        //Triangulate
        earLen = triangulation(tri, ear, amountOfTri);
        //Enlever un oreille
        ears = cutEar(earCut,ear, p, 0, nul, null1, ears);
    }
    //Fin du triangulation
    //Enlever le dernier oreille
  ears = cutEar(earCut,ear, p, 0, nul, null1, ears);
  //Transfore les structures doubly linked en structures normale
  lenOfTriArray = TRI_TD(earCut, ears, triangulatedObj, lenOfTriArray, nul);
  //dealloué les tableau
  free(tri);
  free(ear);
  free(earCut);
  free(p);
  }
}

//Mets les triangles au zero
void nullifyTri(TRI_ears ears, TRI_Triangle *tri, TRI_Triangle nul)
{
   int i;
   for(i = 0; i < ears.TotalVerts; i++){
        tri[i] = nul;
   }
}

//Construire les points
void createPoints(TD_Point *pN, TRI_Point *p, TRI_ears ears)
{
  int i =0;
    p[0].x = pN[0].x;
    p[0].y = pN[0].y;
    p[0].z = pN[0].z;
    p[0].prev = ears.TotalVerts-1;
    p[0].next = 1;
    p[0].current = 0;
    p[0].angle = 0;

    p[ears.TotalVerts-1].x = pN[ears.TotalVerts-1].x;
    p[ears.TotalVerts-1].y = pN[ears.TotalVerts-1].y;
    p[ears.TotalVerts-1].z = pN[ears.TotalVerts-1].z;
    p[ears.TotalVerts-1].prev = ears.TotalVerts-2;
    p[ears.TotalVerts-1].next = 0;
    p[ears.TotalVerts-1].current = ears.TotalVerts-1;
    p[ears.TotalVerts-1].angle = 0;

  for (i = 1; i < ears.TotalVerts-1; i++)
  {
       p[i].x = pN[i].x;
       p[i].y = pN[i].y;
       p[i].z = pN[i].z;
       p[i].prev = i-1;
       p[i].next = i+1;
       p[i].current = i;
       p[i].angle = 0;
  }

}

//Cree le poly
void createPoly(TRI_Point *point, TRI_Poly *poly)
{
    poly->p = point;
}
int createTri(TRI_Poly *poly, TRI_Triangle *tri, TRI_ears ears, TRI_Point null1, TRI_Triangle nul)
{
  //Trouver le premiere point null
  int j = 0;
  int nonNull = 0;
  int i;
  for(i = 0; i < ears.TotalVerts; i++){
   if(comparePoint(poly->p[nonNull], null1) == 1){
     nonNull++;
   }else{
      break;
   }
  }
  i = nonNull;
  int t = 0;
  while(t < ears.TotalVerts-nonNull){
        //Si point n'est pas nul, convex, pas d'autre points dans le triangle et dans le tableau
      if(comparePoint(poly->p[i], null1) == 0 && poly->p[i].angle == 1 && isInside(poly->p, poly->p[poly->p[i].prev], poly->p[i], poly->p[poly->p[i].next], ears) && j > -1 && j < ears.TotalVerts){
        tri[j].p1 = poly->p[poly->p[i].prev];
        tri[j].pE = poly->p[i];
        tri[j].p2 = poly->p[poly->p[i].next];
        tri[j].next = j+1;
        tri[j].prev = j-1;
        j++;
      }else{
        tri[j] = nul;
      }
   t++;
   i = poly->p[i].next;
  }
  //Derniere et premiere point
  tri[0].prev = j-1;
  tri[j-1].next = 0;

  return j;
}

//Cree un vecteur
TRI_Vector makeVec(TRI_Point a, TRI_Point b)
{
  TRI_Vector vec;
  vec.x = b.x-a.x;
  vec.y = b.y-a.y;
  vec.z = b.z-a.z;
  return vec;
}

//Trouver l'angle entre les deux vecteurs a et b
double getAngle(TRI_Vector a, TRI_Vector b)
{
    double angle = atan2(a.y, a.x) - atan2(b.y, b.x);
    return angle;

}

//Convex ou concave
int conVorC(TRI_Point p1, TRI_Point p2, TRI_Point p3)
{
 TRI_Vector a = makeVec(p2, p1);
 TRI_Vector b = makeVec(p2, p3);

 double angle = getAngle(a, b);
 if(angle < M_PI){
   return 1;
 }else{
   return 0;
 }
}

//Trouver l'angle
void findAngle(TRI_Poly *poly, TRI_ears ears, TRI_Point null1)
{
  int nonNull = 0;
  int i;
  for(i = 0; i < ears.TotalVerts; i++){
   if(comparePoint(poly->p[nonNull], null1) == 1){
     nonNull++;
   }else{
      break;
   }
  }
  i = nonNull;
  int t = 0;
  while(t < ears.TotalVerts-nonNull){

    if(comparePoint(poly->p[i], null1) == 0 && conVorC(poly->p[poly->p[i].prev], poly->p[i], poly->p[poly->p[i].next]) == 1){
      poly->p[i].angle = 1;
    }else{
      poly->p[i].angle = 0;
    }
    t++;
    i = poly->p[i].next;
  }
}

//Trouver le signe de la point p1 par rapport à la segment (P2P3)
float zCoord(TRI_Point p1, TRI_Point p2, TRI_Point p3)
{
    //x1 (y2 - y3) + x2 (y3 - y1) + x3 (y1 - y2)
    return p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y)
                    + p3.x * (p1.y - p2.y);
  }

int isInside(TRI_Point *p, TRI_Point a, TRI_Point b, TRI_Point c, TRI_ears ears)
{
    int i;
    for(i = 0; i < ears.TotalVerts-1; i++){
        float z1 = zCoord(a, b, p[i]);
        float z2 = zCoord(b, c, p[i]);
        float z3 = zCoord(c, a, p[i]);
        if(z1 > 0 && z2 > 0 && z3 > 0){            //Si le point est negative, donc condition est faux, il est à gauche de la ligne donc dehors de la triangle
            return 0;
        }
    }
    return 1;
}
//Triangulation
int triangulation(TRI_Triangle *tri, TRI_Triangle *ear, int len)
{
  int j = 0;
  int i;
  for(i = 0; i < len; i++){
      ear[i] = tri[i];
      j++;
    }
    return j;
}

//Enlever un oreille
TRI_ears cutEar(TRI_Triangle *earCut, TRI_Triangle *ear, TRI_Point *p, int triCut, TRI_Triangle nul, TRI_Point null1, TRI_ears ears)
{
  int nonNull = 0;
  int i;
  for(i = 0; i < ears.TotalVerts; i++){
   if(compareTriangle(ear[i], nul) == 1){
     nonNull++;
   }else{
      break;
   }
  }
  triCut = nonNull;
    earCut[ears.numEarCut] = ear[triCut];
    if(ears.numEarCut == 0)
    {
        earCut[ears.numEarCut].prev = ears.TotalVerts-2;

    }else
    {
        earCut[ears.numEarCut].prev = ears.numEarCut-1;

    }
    if(ears.numEarCut == ears.TotalVerts-2)
    {
        earCut[ears.numEarCut].next = 0;

    }else
    {
        earCut[ears.numEarCut].next = ears.numEarCut+1;

    }
    ear[ear[triCut].prev].next = ear[triCut].next;
    ear[ear[triCut].next].prev = ear[triCut].prev;
    p[ear[triCut].pE.prev].next = p[ear[triCut].pE.current].next;
    p[ear[triCut].pE.next].prev = p[ear[triCut].pE.current].prev;
    p[ear[triCut].pE.current] = null1;
    ears.verts--;
    ear[triCut] = nul;
    ears.numEarCut++;
    return ears;
}

//Comparer deux structures de point
int comparePoint(TRI_Point a, TRI_Point b)
{
    if(a.x == b.x && a.y == b.y && a.z == b.z && a.next == b.next && a.prev == b.prev && a.current == b.current && a.angle == b.angle)
    {
        return 1;
    }
    return 0;
}

//Comparer deux structures de Triangles
int compareTriangle(TRI_Triangle a, TRI_Triangle b)
{
    if(comparePoint(a.p1, b.p1) == 1 && comparePoint(a.pE, b.pE) == 1 && comparePoint(a.p2, b.p2) == 1 && a.next == b.next && a.prev == b.prev)
    {
        return 1;
    }
    return 0;
}

//Convertir de TRI à TD
int TRI_TD(TRI_Triangle *TRI, TRI_ears ears, TD_Object* triangulatedObj, int arrayLen, TRI_Triangle nul)
{
    int t;
    int i = 0;
    for(t = arrayLen; t < ears.numEarCut-1 + arrayLen; t++)
    {
        if(compareTriangle(TRI[i], nul) == 0)
            {
        //Premiere point de la triangle
        triangulatedObj->triangle[t].pt1.x = TRI[i].p1.x;
        triangulatedObj->triangle[t].pt1.y = TRI[i].p1.y;
        triangulatedObj->triangle[t].pt1.z = TRI[i].p1.z;
        //Hauteur de la triangle
        triangulatedObj->triangle[t].pt2.x = TRI[i].pE.x;
        triangulatedObj->triangle[t].pt2.y = TRI[i].pE.y;
        triangulatedObj->triangle[t].pt2.z = TRI[i].pE.z;
        //derniere Point de la triangle
        triangulatedObj->triangle[t].pt3.x = TRI[i].p2.x;
        triangulatedObj->triangle[t].pt3.y = TRI[i].p2.y;
        triangulatedObj->triangle[t].pt3.z = TRI[i].p2.z;
            }
        i = TRI[i].next;
    }
    return t;
}
