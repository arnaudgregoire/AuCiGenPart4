std::vector<OGRPolygon*>  multi_to_single(std::vector<OGRPolygon*> l_poly_multi);

std::vector<OGRPolygon*> decoupage(OGRPolygon* poly_before);

void ecrire(OGRGeometry* poly, OGRLayer *layer_w);

int recur(int seuil, OGRPolygon*, OGRLayer *layer_w);

bool sortx (Vecteur a,Vecteur b);

int imperative(int seuil, OGRPolygon* poly,OGRLayer *layer_w);

bool test_aire(int seuil, std::vector<OGRPolygon*> tab);

bool check_dimension(OGRPolygon* poly);

