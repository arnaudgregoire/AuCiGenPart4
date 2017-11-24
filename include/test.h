#include "ogrsf_frmts.h"
#include <vector>
#include "Vecteur.h"

char* test_lecture(int num);
void test_OBB(char* export_polygon);
void test_ecriture(char* export_polygon, char* nom_couche, int seuil);
void ecriture(std::vector<char*> tab_export_poly,const char* nom_couche, int seuil);
std::vector<char*> lecture(const char* nom_couche,const char* nom_layer);
