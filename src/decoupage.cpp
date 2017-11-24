#include "Vecteur.h"
#include "ogrsf_frmts.h"
#include <iostream>
#include <cmath>
#include "obb.h"




int c = 0;

/**
 * \fn void ecrire(OGRPolygon* poly, OGRLayer *layer_w)
 *
 * \brief Write given poly in given layer
 *
 * \param OGRPolygon* polygon you want to write
 * \param OGRLayer *layer_w layer where you want to write polygons
 * \return void We don't return anything, just write in file
 */
void ecrire(OGRGeometry* poly_write, OGRLayer *layer_w){
/* On créé un premier objet de type polygon */



    if (poly_write->getGeometryType() == wkbPolygon){
            OGRPolygon* poly = (OGRPolygon*) poly_write;
            if(poly->get_Area() > 300 && poly->IsValid() && poly->get_Area()<100000){
                OGRFeature *feature_w;
                feature_w    = OGRFeature::CreateFeature(layer_w->GetLayerDefn());

                feature_w->SetGeometry(poly);
                feature_w->SetField( "numero", c );
                c=c+1;
                //std::cout << c << std::endl;
                if (layer_w->CreateFeature(feature_w) != OGRERR_NONE ){
                std::cout << "failed to create" << std::endl;
                OGRFeature::DestroyFeature(feature_w);
                }
            }
    }

    if (poly_write->getGeometryType() == wkbMultiPolygon){
        OGRMultiPolygon* l_poly = (OGRMultiPolygon*) poly_write;

        for (int i = 0; i < l_poly->getNumGeometries(); i++){
            OGRPolygon* poly = (OGRPolygon*) l_poly->getGeometryRef(i)->clone();
            if(poly->get_Area() > 300 && poly->IsValid() && poly->get_Area()<100000){
                OGRFeature *feature_w;
                feature_w    = OGRFeature::CreateFeature(layer_w->GetLayerDefn());

                feature_w->SetGeometry(poly);
                feature_w->SetField( "numero", c );
                c=c+1;
                //std::cout << c << std::endl;
                if (layer_w->CreateFeature(feature_w) != OGRERR_NONE ){
                std::cout << "failed to create" << std::endl;
                OGRFeature::DestroyFeature(feature_w);
                }
            }
        }
    }
}
/**
 * \fn  bool check_dimension(OGRPolygon* poly)
 *
 * \brief Check if the given polygon has a width 15x bigger than it height
 *
 * \param OGRPolygon* poly_before The polygon you want to check
 * \return bool true if the given polygon has a width 15x bigger than it height
 */
 bool check_dimension(OGRPolygon* poly){


    bool bill = false;
    OGRPolygon* convex_hull   = (OGRPolygon*) poly->ConvexHull();
    OGRLineString* ext_line   = prepare_obb(convex_hull);
    std::vector<Vecteur> test = CalcOmbb(ext_line);

    Vecteur obbUpperLeft   = test[0];
    Vecteur obbBottomLeft  = test[1];
    Vecteur obbBottomRight = test[2];
    Vecteur obbUpperRight  = test[3];

    long double distLeftRight  = obbUpperLeft.distance(obbUpperRight);
    long double distTopBottom  = obbUpperLeft.distance(obbBottomLeft);

    long double width  = 0;
    long double height = 0;

    if (distLeftRight > distTopBottom){
        width  = distLeftRight;
        height = distTopBottom;
    }
    else{
        width  = distTopBottom;
        height = distLeftRight;
    }

    long double rapport = width/height;

    if(rapport > 15){
        bill = true;
    }
    return bill;
}



/**
 * \fn std::vector<OGRPolygon*>  decoupage(OGRPolygon* poly_before)
 *
 * \brief Function that split one polygon into two equals parts along the Oriented bounding box
 *
 * \param OGRPolygon* poly_before The polygon you want to split
 * \return std::vector<OGRPolygon*>  Size-2 Vector that contained the two parts of the splitted polygon
 */
std::vector<OGRPolygon*>  decoupage(OGRPolygon* poly_before){

    std::vector<OGRPolygon*> tab;

    /* On créé un objet polygone*/
    OGRPolygon poly_sub1;
    OGRPolygon poly_sub2;
    OGRPolygon* poly_feat1;
    OGRPolygon* poly_feat2;
    OGREnvelope envelope;
    OGRLinearRing ring_sub1;
    OGRLinearRing ring_sub2;


    OGRPolygon* convex_hull = (OGRPolygon*) poly_before->ConvexHull();
    //ecrire(convex_hull, layer_w);
    //std::cout << "aire de la convex_hul : *" << convex_hull->get_Area() << std::endl;
    OGRLineString* ext_line = prepare_obb(convex_hull);
    //std::cout << "la convex_hull valide ? : " << convex_hull->IsValid() << std::endl;
    //std::cout << "la OGRLineString valide ? : " << ext_line->IsValid() << std::endl;

    std::vector<Vecteur> test;

    test = CalcOmbb(ext_line);

    Vecteur obbUpperLeft   = test[0];
    Vecteur obbBottomLeft  = test[1];
    Vecteur obbBottomRight = test[2];
    Vecteur obbUpperRight  = test[3];

    /*
    for(int i=0; i<test.size(); i++){
    std::cout << test[i].x << ", " << test[i].y << std::endl;
    }
    std::cout << "-----------------" << std::endl;

    OGRLinearRing ring_test2;
    ring_test2.addPoint(test[0].x, test[0].y);
    ring_test2.addPoint(test[1].x, test[1].y);
    ring_test2.addPoint(test[2].x, test[2].y);
    ring_test2.addPoint(test[3].x, test[3].y);
    ring_test2.addPoint(test[0].x, test[0].y);
    OGRPolygon poly_test2 = OGRPolygon();
    poly_test2.addRing(&ring_test2);
    ecrire(&poly_test2, layer_w);


    //std::cout << "aire de la grosse bounding box : " << poly_test2.get_Area() << std::endl;



    for(int i=0; i<test.size(); i++){
    std::cout << test[i].x << ", " << test[i].y << std::endl;

    }
    */




/*
    std::cout << "obbUpperLeft : " << obbUpperLeft.x << ", " << obbUpperLeft.y << std::endl;
    std::cout << "obbUpperRight : " << obbUpperRight.x << ", " << obbUpperRight.y << std::endl;
    std::cout << "obbBottomLeft : " << obbBottomLeft.x << ", " << obbBottomLeft.y << std::endl;
    std::cout << "obbBottomRight : " << obbBottomRight.x << ", " << obbBottomRight.y << std::endl;
    std::cout << obbUpperRight.x - obbUpperLeft.x << std::endl;
    std::cout << obbBottomRight.x - obbBottomLeft.x << std::endl;
    std::cout << obbUpperRight.y - obbBottomRight.y << std::endl;
    std::cout << obbUpperLeft.y - obbBottomLeft.y << std::endl;
*/

    long double distLeftRight   = obbUpperLeft.distance(obbUpperRight);
    long double distTopBottom  = obbUpperLeft.distance(obbBottomLeft);
    //std::cout << "distLeftRight : " << distLeftRight << std::endl;
    //std::cout << "distTopBottom : " << distTopBottom << std::endl;

    /* on teste quel coté est la longeur du polygone */

    if(distLeftRight > distTopBottom){
        //std::cout << "decoupage vertical" << std::endl;
        /* découpage vertical */
        long double milieu_x_bottom = obbBottomLeft.x + (obbBottomRight.x - obbBottomLeft.x)/2;
        long double milieu_y_bottom = obbBottomLeft.y + (obbBottomRight.y - obbBottomLeft.y)/2;

        long double milieu_x_top    = obbUpperLeft.x  + (obbUpperRight.x - obbUpperLeft.x)/2;
        long double milieu_y_top    = obbUpperLeft.y  + (obbUpperRight.y - obbUpperLeft.y)/2;

        ring_sub1.addPoint(obbBottomLeft.x,obbBottomLeft.y);
        ring_sub1.addPoint(milieu_x_bottom, milieu_y_bottom);
        ring_sub1.addPoint(milieu_x_top,milieu_y_top);
        ring_sub1.addPoint(obbUpperLeft.x,obbUpperLeft.y);
        ring_sub1.addPoint(obbBottomLeft.x,obbBottomLeft.y);
        /*
        std::cout << obbBottomLeft.x << "," << obbBottomLeft.y << std::endl;
        std::cout << milieu_x_bottom << "," << milieu_y_bottom << std::endl;
        std::cout << milieu_x_top << "," << milieu_y_top << std::endl;
        std::cout << obbUpperLeft.x << "," << obbUpperLeft.y << std::endl;
        */
        poly_sub1.addRing(&ring_sub1);
        ring_sub2.addPoint(milieu_x_bottom,milieu_y_bottom);
        ring_sub2.addPoint(obbBottomRight.x,obbBottomRight.y);
        ring_sub2.addPoint(obbUpperRight.x,obbUpperRight.y);
        ring_sub2.addPoint(milieu_x_top,milieu_y_top);
        ring_sub2.addPoint(milieu_x_bottom,milieu_y_bottom);
        poly_sub2.addRing(&ring_sub2);
    }

    else{
        //std::cout << "decoupage horizontal" << std::endl;
        /* découpage horizontal */
        long double milieu_x_left = obbBottomLeft.x + (obbUpperLeft.x - obbBottomLeft.x)/2;
        long double milieu_y_left = obbBottomLeft.y + (obbUpperLeft.y - obbBottomLeft.y)/2;

        long double milieu_x_right = obbBottomRight.x + (obbUpperRight.x - obbBottomRight.x)/2;
        long double milieu_y_right = obbBottomRight.y + (obbUpperRight.y - obbBottomRight.y)/2;

        ring_sub1.addPoint(obbBottomLeft.x  , obbBottomLeft.y);
        ring_sub1.addPoint(obbBottomRight.x , obbBottomRight.y);
        ring_sub1.addPoint(milieu_x_right   , milieu_y_right);
        ring_sub1.addPoint(milieu_x_left    , milieu_y_left);
        ring_sub1.addPoint(obbBottomLeft.x  , obbBottomLeft.y);
        poly_sub1.addRing(&ring_sub1);
        ring_sub2.addPoint(milieu_x_left    , milieu_y_left);
        ring_sub2.addPoint(milieu_x_right   , milieu_y_right);
        ring_sub2.addPoint(obbUpperRight.x  , obbUpperRight.y);
        ring_sub2.addPoint(obbUpperLeft.x   , obbUpperLeft.y);
        ring_sub2.addPoint(milieu_x_left    , milieu_y_left);
        poly_sub2.addRing(&ring_sub2);

    }
/*
    std::cout << "sous obb 1 valide : "<< poly_sub1.IsValid()<< std::endl;
    std::cout << "aire obb 1 : " << poly_sub1.get_Area() << std::endl;

    std::cout << "sous obb 2 valide : "<< poly_sub2.IsValid()<< std::endl;
    std::cout << "aire obb 2 : " << poly_sub2.get_Area() << std::endl;

    std::cout << "polygone valide : "<< poly_before->IsValid()<< std::endl;
    std::cout << "aire polygone : " << poly_before->get_Area() << std::endl;

    std::cout << "sous obb 1 intersecte polygone : "<< poly_sub1.Intersects(poly_before)<< std::endl;
    std::cout << "sous obb 2 intersecte polygone : "<< poly_sub2.Intersects(poly_before)<< std::endl;

*/
    //ecrire(&poly_sub1, layer_w);
    //ecrire(&poly_sub2, layer_w);


    poly_feat1 = (OGRPolygon*) poly_sub1.Intersection(poly_before);
    poly_feat2 = (OGRPolygon*) poly_sub2.Intersection(poly_before);

    //ecrire(poly_feat1, layer_w);
    //ecrire(poly_feat2, layer_w);

    tab.push_back(poly_feat1);
    tab.push_back(poly_feat2);

    return tab;
}


/**
 * \fn std::vector<OGRPolygon*>  multi_to_single(std::vector<OGRPolygon*> l_poly_multi)
 *
 * \brief Transform multipolygons to polygons and append them into a vector of Polygons
 *
 * \param std::vector<OGRPolygon*> The list of suspicious polygons that could be multipolygons
 * \return std::vector<OGRPolygon*>  The same vector but with multipolygons splitted into singles parts
 */
std::vector<OGRPolygon*>  multi_to_single(std::vector<OGRPolygon*> l_poly_multi){

    std::vector<OGRPolygon*> single_parts;

    for(unsigned int i=0U; i<l_poly_multi.size(); i++){

        if( l_poly_multi.at(i) != NULL && wkbFlatten(l_poly_multi.at(i)->getGeometryType()) ==  wkbMultiPolygon){
            //std::cout << "multipolygone détecté" << std::endl;
            OGRMultiPolygon* poly_multi = (OGRMultiPolygon*) l_poly_multi.at(i);
            int nb_morceaux =  poly_multi->getNumGeometries();
            //std::cout << c << std::endl;

            for( int j=0; j<nb_morceaux; j++){
                OGRPolygon* part = (OGRPolygon*) poly_multi->getGeometryRef(j)->clone();
                if(part->IsValid()){
                    single_parts.push_back(part);
                }
            }
        }

        if( l_poly_multi.at(i) != NULL && wkbFlatten(l_poly_multi.at(i)->getGeometryType()) ==  wkbPolygon && l_poly_multi.at(i)->IsValid()){
            single_parts.push_back(l_poly_multi.at(i));
        }
    }

    return single_parts;
}




/**
 * \fn int recur(OGRPolygon* poly,OGRLayer *layer_w)
 *
 * \brief Recursive function that implement algorithm 2 of scientific given publication
 * Split along the oriented bounding box
 *
 * \param OGRPolygon* poly Polygon to split
 * \param OGRLayer *layer_w The layer where the polygon will be write
 * \return 0, when the algorithm end
 */

int recur(int seuil, OGRPolygon* poly,OGRLayer *layer_w){

    if (poly->get_Area()>seuil){

        std::vector<OGRPolygon*> tab = multi_to_single(decoupage(poly));

        for(unsigned int k=0U; k<tab.size(); k++){
            if(tab.at(k)->get_Area() < seuil){
                ecrire(tab.at(k),layer_w);
            }
            else{
                recur(seuil, tab.at(k), layer_w);
            }
        }

    }
    else{
        ecrire(poly, layer_w);
    }

}

/**
 * \fn bool test_aire(int seuil, std::vector<OGRPolygon*> tab)
 *
 * \brief Function that test if a polygon in the given vector has an area above
 * the given threshold. Returns true if there is at least one above.
 *
 * \param int seuil Threshold
 * \param std::vector<OGRPolygon*> tab Vector of given Polygons
 * \return bool True if there is at least one polgon's area above the threshold
 */

bool test_aire(int seuil, std::vector<OGRPolygon*> tab){
    bool boule = false;
    for(unsigned int i=0U; i< tab.size(); i++){
        if(tab.at(i)->get_Area()> seuil){
            boule = true;
        }
    }
    return boule;
}

/**
 * \fn int imperative(int seuil, OGRPolygon* poly,OGRLayer *layer_w)
 *
 * \brief Imperative function that implement algorithm 2 of scientific given publication
 * Split along the oriented bounding box
 *
 * \param OGRPolygon* poly Polygon to split
 * \param OGRLayer *layer_w The layer where the polygon will be write
 * \return 0, when the algorithm end
 */

int imperative(int seuil, OGRPolygon* poly,OGRLayer *layer_w){
    int compteur = 0;

    if (poly->get_Area()<seuil){
        ecrire(poly, layer_w);
    }
    else{

        std::vector<OGRPolygon*> l_poly;
        l_poly.push_back(poly);
        std::vector<OGRPolygon*> l_decoup;

        while(test_aire(seuil, l_poly)){
                compteur += 1;

                for(unsigned int i=0U; i<l_poly.size();i++){

                    if(l_poly.at(i)->get_Area()>seuil){

                        //std::cout << "Aire : "<<l_poly.at(i)->get_Area() << std::endl;
                        l_decoup.clear();
                        l_decoup = multi_to_single(decoupage(l_poly.at(i)));
                        l_poly.erase(l_poly.begin() + i);
                         i--;
                        for(unsigned int j=0U; j<l_decoup.size();j++){
                            l_poly.push_back(l_decoup.at(j));
                        }

                        //std::cout << "i = " << i << std::endl;
                        //std::cout << "size = " << l_poly.size() << std::endl;
                    }
                }
        }
        for(unsigned int i=0U; i<l_poly.size(); i++){
            ecrire(l_poly.at(i),layer_w);

        }
    }
    return 0;
}



