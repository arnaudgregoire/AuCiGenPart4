#include <gdal_priv.h>
#include "ogrsf_frmts.h"
#include <iostream>
#include <vector>
#include "Vecteur.h"
#include "obb.h"
#include "decoupage.h"

/**
 * \fn std::vector<char*> lecture(const char* nom_couche,const char* nom_layer)
 *
 * \brief Read all features of the given layer in the given shapefile and stock them
 * into a vector of char* Well-Known-Text format
 *
 * \param const char* nom_couche The name of the Shapefile where the data will be read
 * \param const char* nom_layer The name of the layer where the data will be read
 *
 * \return std::vector<char*> a vector of char* containing the exported polygons in WKT format
 */
std::vector<char*> lecture(const char* nom_couche,const char* nom_layer)
{
    std::vector<char*> tab_export_poly;
    char* export_poly;


    /* on initialise les drivers */
    GDALAllRegister();

    GDALDataset  *projet;
    projet = (GDALDataset*) GDALOpenEx(nom_couche, GDAL_OF_VECTOR, NULL, NULL, NULL );

    if( projet == NULL )
    {
        std::cout <<  "Open failed" << std::endl;
    }
    /* On ouvre le fichier ARRONDISSEMENT.SHP*/
    OGRLayer  *poLayer;
    poLayer = projet->GetLayerByName(nom_layer);
    //char* (wktTargetSrs);

    /*On regarde le fichier de projection et on l'affiche */
    //OGRSpatialReference* sourceSrs = poLayer->GetSpatialRef();
    //sourceSrs->exportToWkt(&wktTargetSrs);
    //std::cout << wktTargetSrs << std::endl;


    /*on parcourt l'ensemble des objets contenus dans le fichier*/
    OGRFeature *poFeature;
    poLayer->ResetReading();
    int idx = 0;
    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {
        /*On affiche la géométrie de l'objet*/
        OGRGeometry *poGeometry;
        poGeometry = poFeature->GetGeometryRef();
        //std::cout << "type de géométrie:"<< poGeometry->getGeometryName() << std::endl;

        /*On affiche l'aire  et le nombre de sommets (en vérifiant que l'objet est un polygone) */
        if( poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) ==  wkbPolygon)
        {

            OGRPolygon *poPolygon = (OGRPolygon *) poGeometry;
            /*On exporte la géométrie dans une chaine de caractère */

            poPolygon->exportToWkt(&export_poly);
            tab_export_poly.push_back(export_poly);

        }

        if( poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) ==  wkbMultiPolygon)
        {

            OGRMultiPolygon *poPolygon = (OGRMultiPolygon *) poGeometry;
            /*On exporte la géométrie dans une chaine de caractère */

            poPolygon->exportToWkt(&export_poly);
            tab_export_poly.push_back(export_poly);

        }
        else
            {
            //std::cout <<  "Ce n'est pas un polygone" <<std::endl;
            }


    /* On libère la mémoire*/
        OGRFeature::DestroyFeature( poFeature );
        idx+=1;
    }
    GDALClose( projet );
    return tab_export_poly;
}

/**
 * \fn void ecriture(std::vector<char*> tab_export_poly,const char* nom_couche, int seuil)
 *
 * \brief Split all polygons with the OBB method if they have an area taller than the given threshold
 * and write them in the given shapefile name
 *
 * \param std::vector<char*> tab_export_poly a vector of polygon in the WKT format
 * \param const char* nom_couche The name of the shapefile where you want the data to be written
 *
 * \return int seuil the treshold area that determine if each polygon will be splitted
 */
void ecriture(std::vector<char*> tab_export_poly,const char* nom_couche, int seuil){
    /* On initialise notre projet pour la création en écriture d'un shp*/
    const char *pszDriverName = "ESRI Shapefile";
    GDALDriver *poDriver;
    GDALAllRegister();
    poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
    if( poDriver == NULL )
    {std::cout <<  "Driver non available" << std::endl;}


    /* On initialise le projet en écriture pour écrire le fichier test.shp*/
    GDALDataset *projet_w;
    std::string chemin;
    projet_w = poDriver->Create( nom_couche , 0, 0, 0, GDT_Unknown, NULL );
    if( projet_w == NULL )
    {std::cout <<   "Creation of output file failed" << std::endl;}


    /* On récupère les caractéristiques de la projection Lambert 93*/
    OGRSpatialReference projection;
    projection.importFromEPSG( 2154 );
    char *specif_projection;
    projection.exportToWkt(&specif_projection);
    //std::cout << specif_projection << std::endl;


    /* On créé la couche test dans notre shp test.shp*/
    OGRLayer *layer_w;
    layer_w = projet_w->CreateLayer( "test", &projection, wkbPolygon );

    OGRFieldDefn oField( "numero", OFTInteger );
    if( layer_w->CreateField( &oField ) != OGRERR_NONE )
    {std::cout << "Creating Name field failed." << std::endl;}

    /*On importe la géométrie précédemment sauvegardé*/
    OGRPolygon poly = OGRPolygon();
    char* poly_txt;
    bool check;

    for (unsigned int i=0U; i<tab_export_poly.size();i++){

        poly_txt = tab_export_poly[i];
        poly.importFromWkt(&poly_txt);
        check = check_dimension(&poly);

        if(poly.get_Area()>300 && !check){

            imperative(seuil, &poly, layer_w);
            //recur(seuil,&poly,layer_w);
        }
    }

    GDALClose( projet_w );

}

/*
char* test_lecture(int num)
{
    char* export_poly;


    // on initialise les drivers
    GDALAllRegister();

    GDALDataset  *projet;
    projet = (GDALDataset*) GDALOpenEx(
    "data/BDTOPO-75/BDTOPO/1_DONNEES_LIVRAISON_2011-12-00477/BDT_2-1_SHP_LAMB93_D075-ED113/H_ADMINISTRATIF/ARRONDISSEMENT.SHP"
    //"data/test/test_moitie_19.shp"
    , GDAL_OF_VECTOR, NULL, NULL, NULL );

    if( projet == NULL )
    {
        std::cout <<  "Open failed" << std::endl;
    }
    // On ouvre le fichier ARRONDISSEMENT.SHP
    OGRLayer  *poLayer;
    poLayer = projet->GetLayerByName( "ARRONDISSEMENT" );
    char* (wktTargetSrs);

    //On regarde le fichier de projection et on l'affiche
    OGRSpatialReference* sourceSrs = poLayer->GetSpatialRef();
    sourceSrs->exportToWkt(&wktTargetSrs);
    //std::cout << wktTargetSrs << std::endl;


    //on parcourt l'ensemble des objets contenus dans le fichier
    OGRFeature *poFeature;
    poLayer->ResetReading();
    int idx = 0;
    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {

        //On affiche la géométrie de l'objet
        OGRGeometry *poGeometry;
        poGeometry = poFeature->GetGeometryRef();
        //std::cout << "type de géométrie:"<< poGeometry->getGeometryName() << std::endl;

        //On affiche l'aire  et le nombre de sommets (en vérifiant que l'objet est un polygone)
        if( poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) ==  wkbPolygon)
        {

            OGRPolygon *poPolygon = (OGRPolygon *) poGeometry;
            //On exporte la géométrie dans une chaine de caractère
            if(num == idx){
                poPolygon->exportToWkt(&export_poly);
            }
        }

        else
            {std::cout <<  "Ce n'est pas un polygone" <<std::endl;}


    // On libère la mémoire
        OGRFeature::DestroyFeature( poFeature );
        idx+=1;
    }
    GDALClose( projet );
    return export_poly;
}
*/

/*
void test_ecriture(char* export_polygon, char* nom_couche, int seuil){
    // On initialise notre projet pour la création en écriture d'un shp
    const char *pszDriverName = "ESRI Shapefile";
    GDALDriver *poDriver;
    GDALAllRegister();
    poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
    if( poDriver == NULL )
    {std::cout <<  "Driver non available" << std::endl;}


    // On initialise le projet en écriture pour écrire le fichier test.shp
    GDALDataset *projet_w;
    std::string chemin;
    projet_w = poDriver->Create( nom_couche , 0, 0, 0, GDT_Unknown, NULL );
    if( projet_w == NULL )
    {std::cout <<   "Creation of output file failed" << std::endl;}


    // On récupère les caractéristiques de la projection Lambert 93
    OGRSpatialReference projection;
    projection.importFromEPSG( 2154 );
    char *specif_projection;
    projection.exportToWkt(&specif_projection);
    //std::cout << specif_projection << std::endl;


    // On créé la couche test dans notre shp test.shp
    OGRLayer *layer_w;
    layer_w = projet_w->CreateLayer( "test", &projection, wkbPolygon );

    OGRFieldDefn oField( "numero", OFTInteger );
    if( layer_w->CreateField( &oField ) != OGRERR_NONE )
    {std::cout << "Creating Name field failed." << std::endl;}

    //On importe la géométrie précédemment sauvegardé
    OGRPolygon poly = OGRPolygon();

    poly.importFromWkt(&export_polygon );
    //recur(&poly,layer_w);
    imperative(seuil,&poly,layer_w);

    GDALClose( projet_w );

}



void test_OBB(char* export_polygon){
    // On initialise notre projet pour la création en écriture d'un shp
    const char *pszDriverName = "ESRI Shapefile";
    GDALDriver *poDriver;
    GDALAllRegister();
    poDriver = GetGDALDriverManager()->GetDriverByName(pszDriverName );
    if( poDriver == NULL )
    {std::cout <<  "Driver non available" << std::endl;}


    // On initialise le projet en écriture pour écrire le fichier test.shp
    GDALDataset *projet_w;
    projet_w = poDriver->Create( "data/test/test_OBB.shp", 0, 0, 0, GDT_Unknown, NULL );
    if( projet_w == NULL )
    {std::cout <<   "Creation of output file failed" << std::endl;}


    //On récupère les caractéristiques de la projection Lambert 93
    OGRSpatialReference projection;
    projection.importFromEPSG( 2154 );
    char *specif_projection;
    projection.exportToWkt(&specif_projection);

    //std::cout << specif_projection << std::endl;

    // On créé la couche test dans notre shp test.shp
    OGRLayer *layer_w;
    layer_w = projet_w->CreateLayer( "test", &projection , wkbLineString );

    OGRFieldDefn oField( "numero", OFTInteger );
    if( layer_w->CreateField( &oField ) != OGRERR_NONE )
    {std::cout << "Creating Name field failed." << std::endl;}

    //On importe la géométrie précédemment sauvegardé
    OGRPolygon poly = OGRPolygon();
    poly.importFromWkt(&export_polygon);


    OGRPolygon* convex_hull = (OGRPolygon*) poly.ConvexHull();
    OGRLineString* ext_line = prepare_obb(convex_hull);

    std::vector<Vecteur> test;
    test = CalcOmbb(ext_line);
    for(int i=0; i<4; ++i)
    {
        std::cout << i << " : "<< test[i].x << ", " <<test[i].y << std::endl;
    }


    OGRLinearRing ring_test2;
    ring_test2.addPoint(test[0].x, test[0].y);
    ring_test2.addPoint(test[1].x, test[1].y);
    ring_test2.addPoint(test[2].x, test[2].y);
    ring_test2.addPoint(test[3].x, test[3].y);
    ring_test2.addPoint(test[0].x, test[0].y);
    OGRLineString ext_line2 = (OGRLineString) ring_test2;
    ecrire(&ext_line2,layer_w);
    ecrire(ext_line, layer_w);
    //test = CalcOmbb(ext_line, layer_w);


    OGRLinearRing ring_test;
    ring_test.addPoint(235, 774);
    ring_test.addPoint(245, 740);
    ring_test.addPoint(230, 710);
    ring_test.addPoint(240, 703);
    ring_test.addPoint(274, 733);
    ring_test.addPoint(306, 710);
    ring_test.addPoint(272, 690);
    ring_test.addPoint(277, 639);
    ring_test.addPoint(305, 645);
    ring_test.addPoint(347, 611);
    ring_test.addPoint(340, 639);
    ring_test.addPoint(298, 674);
    ring_test.addPoint(325, 702);
    ring_test.addPoint(335, 663);
    ring_test.addPoint(355, 645);
    ring_test.addPoint(350, 686);
    ring_test.addPoint(400, 710);
    ring_test.addPoint(360, 725);
    ring_test.addPoint(357, 755);
    ring_test.addPoint(328, 723);
    ring_test.addPoint(291, 741);
    ring_test.addPoint(289, 754);
    ring_test.addPoint(266, 757);
    ring_test.addPoint(235, 774);

    OGRLineString line_test = (OGRLineString) ring_test;
    OGRPolygon* poly_test = new OGRPolygon();
    poly_test->addRing(&ring_test);
    OGRPolygon* convex_hull = (OGRPolygon*) poly_test->ConvexHull();
    OGRLinearRing* ext_ring = convex_hull->getExteriorRing();
    std::cout << "isclockwise ? " << ext_ring->isClockwise() << std::endl;
    ext_ring->reverseWindingOrder();
    std::cout << "isclockwise ? " << ext_ring->isClockwise() << std::endl;
    OGRLineString* ext_line = (OGRLineString*) ext_ring;

    std::vector<Vecteur> test;
    test = CalcOmbb(ext_line, layer_w);


    //ecrire(ext_ring, layer_w);
    GDALClose( projet_w );
}
*/
