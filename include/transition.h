std::vector<Vecteur> emprise_shp(const char* nom_couche, const char* nom_layer);

void difference(OGRPolygon* poly_emprise, OGRMultiPolygon* poly, const char * nom_couche,const char* nom_layer);

void adapt(const char* nom_couche, const char* nom_layer, const char* couche_sortie, const char* layer_sortie);
