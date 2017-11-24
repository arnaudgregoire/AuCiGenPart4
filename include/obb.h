#include "Vecteur.h"
#include <vector>

Vecteur IntersectLines(Vecteur start0, Vecteur dir0, Vecteur start1, Vecteur dir1);

std::vector<Vecteur> CalcOmbb(OGRLineString* convexHull);

OGRLineString* prepare_obb (OGRPolygon* poly_prep);

std::vector<Vecteur> order(std::vector<Vecteur> obb);
