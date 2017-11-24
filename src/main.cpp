#include "test.h"
#include <iostream>
#include <vector>
#include "transition.h"

int main()
{

    const char* route_dissolve     = "data/input/buffer_dissolve_paris.shp";
    const char* dissolve_layer     = "buffer_dissolve_paris";

    const char* couche_adapt       = "data/output/test_adapt.shp";
    const char* layer_adapt        = "test_adapt";

    const char* shp_to_write       = "data/output/test_paris_recur.shp";


    std::cout << "Creating parcels network from street network ..." << std::endl;
    adapt(route_dissolve, dissolve_layer, couche_adapt, layer_adapt);

    std::cout << "Loading created data ..." << std::endl;
    std::vector<char*> tab_export_poly = lecture(couche_adapt, layer_adapt);

    std::cout << "Splitting parcels ..." << std::endl;
    ecriture(tab_export_poly,shp_to_write, 6000);
    return 0;
}


