/** \brief Définit un point dans l'espace
 *         Doubly linked structure
 */

typedef struct TRI_Point
{

  float x;
  float z;
  float y;
  int next;
  int prev;
  int current;
  int angle;

} TRI_Point;

/** \brief Définit un triangle dans l'espace
 *         Doubly linked structure
 */

typedef struct TRI_Triangle
{
    TRI_Point p1;
    TRI_Point pE;
    TRI_Point p2;
    int next;
    int prev;

} TRI_Triangle;

/** \brief Définit un polygone
 */

typedef struct TRI_Poly
{
    TRI_Point *p;

} TRI_Poly;

/** \brief Définit un vecteur dans l'espace
 */
typedef struct TRI_Vector
{

    double x;
    double y;
    double z;

} TRI_Vector;

/** \brief Variables qui sont envoyer vers les fonction
 */
typedef struct TRI_ears
{
    int verts;
    int TotalVerts;
    int numEarCut;

} TRI_ears;
