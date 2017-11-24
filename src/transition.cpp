#include "ogrsf_frmts.h"
#include <vector>
#include "test.h"
#include "Vecteur.h"
#include <iostream>
#include "decoupage.h"


/**
 * \fn std::vector<Vecteur> emprise_shp(const char* nom_couche, const char* nom_layer)
 *
 * \brief Compute the bound of the shapefile
 *
 * \param const char* nom_couche name of the shapefile
 * \param const char* nom_layer name of the layer
 * \return std::vector<Vecteur> 2-Size vector containin two points (minx,miny) (maxx,maxy) int he Vecteur format
 */
std::vector<Vecteur> emprise_shp(const char* nom_couche, const char* nom_layer){

    std::vector<Vecteur> emprise;
    double maxX = std::numeric_limits<double>::min();
    double maxY = std::numeric_limits<double>::min();
    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();

    /* on initialise les drivers */
    GDALAllRegister();
    GDALDataset  *projet;
    projet = (GDALDataset*) GDALOpenEx(nom_couche, GDAL_OF_VECTOR, NULL, NULL, NULL );
    if( projet == NULL )
    {std::cout <<  "Open failed" << std::endl;}
    /* On ouvre le fichier ARRONDISSEMENT.SHP*/
    OGRLayer  *poLayer;
    poLayer = projet->GetLayerByName(nom_layer);
    /*on parcourt l'ensemble des objets contenus dans le fichier*/
    OGRFeature *poFeature;
    poLayer->ResetReading();

    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {
        /*On affiche la géométrie de l'objet*/
        OGRGeometry *poGeometry;
        poGeometry = poFeature->GetGeometryRef();

        //std::cout << poGeometry->getGeometryName() << std::endl;
        //std::cout <<  wkbFlatten(poGeometry->getGeometryType()) << std::endl;

        OGREnvelope envelope;
        OGRMultiPolygon * poPolygon = (OGRMultiPolygon *) poGeometry;
        poPolygon->getEnvelope(&envelope);

        if(envelope.MaxX > maxX){
            maxX = envelope.MaxX;
        }
        if(envelope.MaxY > maxY){
            maxY = envelope.MaxY;
        }
        if(envelope.MinX < minX){
            minX = envelope.MinX;
        }
        if(envelope.MinY < minY){
            minY = envelope.MinY;
        }
    /* On libère la mémoire*/
        OGRFeature::DestroyFeature( poFeature );

    }
    GDALClose( projet );
    //std::cout << minX << " "<< minY << " "<< maxX << " "<< maxY << std::endl;
    emprise.push_back(Vecteur(minX, minY));
    emprise.push_back(Vecteur(maxX, maxY));
    return emprise;
}

/**
 * \fn void difference(OGRPolygon* poly_emprise, OGRMultiPolygon* poly, const char * nom_couche,const char* nom_layer)
 *
 * \brief Compute the Symetric difference of two given polygon and write it in a shapefile
 * whith name given in parameters
 *
 * \param OGRPolygon* poly_emprise The first polygon
 * \param OGRMultiPolygon* poly The second polygon, which will be used with the first one
 * to compute the symmetri difference
 * \return void only writing in file
 */
void difference(OGRPolygon* poly_emprise, OGRMultiPolygon* poly, const char * nom_couche,const char* nom_layer){
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
    layer_w = projet_w->CreateLayer( nom_layer, &projection, wkbPolygon );

    OGRFieldDefn oField( "numero", OFTInteger );
    if( layer_w->CreateField( &oField ) != OGRERR_NONE )
    {std::cout << "Creating Name field failed." << std::endl;}

    OGRMultiPolygon* network_parcels = new OGRMultiPolygon();

    network_parcels = (OGRMultiPolygon*) poly_emprise->SymDifference(poly);

    //ecrire(poly_emprise, layer_w);
    //ecrire(poly        , layer_w);

    ecrire(network_parcels, layer_w);
    GDALClose( projet_w );

};

/**
 * \fn void adapt(const char* nom_couche, const char* nom_layer, const char* couche_sortie, const char* layer_sortie)
 *
 * \brief Transform a street network composed of one polygon into a network of parcels with an area
 * between 300 and 100 000 square meters
 *
 * \param const char* nom_couche The name of the shapefile containing the street network
 * \param const char* nom_layer The name of the layer containing the street network
 * \param const char* couche_sortie The name of the shapefile which will contain the parcel network
 * \param const char* layer_sortie The name of the layer which will contain the parcel network
 * \return void only writing in file
 */
void adapt(const char* nom_couche, const char* nom_layer, const char* couche_sortie, const char* layer_sortie){

    std::vector<Vecteur> emprise = emprise_shp(nom_couche,nom_layer);
    std::vector<char*> l_poly    = lecture(nom_couche,nom_layer);
    //std::cout << l_poly.size() << std::endl;
    char* poly_txt = l_poly.at(0);

    OGRMultiPolygon* poly = new OGRMultiPolygon();
    poly->importFromWkt(&poly_txt);
    //std::cout << poly->getNumGeometries() << std::endl;

    OGRPolygon* poly_emprise = new OGRPolygon();
    OGRLinearRing ring_emprise;

    ring_emprise.addPoint(emprise[0].x, emprise[0].y);
    ring_emprise.addPoint(emprise[1].x, emprise[0].y);
    ring_emprise.addPoint(emprise[1].x, emprise[1].y);
    ring_emprise.addPoint(emprise[0].x, emprise[1].y);
    ring_emprise.addPoint(emprise[0].x, emprise[0].y);
    poly_emprise->addRing(&ring_emprise);


    difference(poly_emprise,poly, couche_sortie, layer_sortie);

}


