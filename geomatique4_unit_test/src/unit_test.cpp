#include "../../include/Vecteur.h"
#include "ogrsf_frmts.h"
#include <iostream>
#include <cmath>
#include "../../include/obb.h"
#include "catch.hpp"

OGRPolygon* test_convex_hull(){
    OGRLinearRing ring_test;
    ring_test.addPoint(0,0);
    ring_test.addPoint(0,3);
    ring_test.addPoint(3,3);
    ring_test.addPoint(3,0);
    ring_test.addPoint(0,0);

    OGRLineString line_test = (OGRLineString) ring_test;

    OGRPolygon* poly_test = new OGRPolygon();

    poly_test->addRing(&ring_test);

    OGRPolygon* convex_hull   = (OGRPolygon*) poly_test->ConvexHull();

    return convex_hull;
}

OGRLineString* test_prepare_obb(){

    OGRPolygon* convex_hull = test_convex_hull();

    OGRLineString* ext_line   = prepare_obb(convex_hull);

    return ext_line;
}


std::vector<Vecteur> test_obbh(){

    OGRLineString* ext_line = test_prepare_obb();

    std::vector<Vecteur> test = CalcOmbb(ext_line);

    return test;
}

TEST_CASE("The convex hull is correctly computed"){
    OGRPolygon* convex_hull = test_convex_hull();
    REQUIRE(convex_hull->IsValid() == 1);
    REQUIRE(convex_hull->get_Area() > 0);

}


TEST_CASE( "The obb exterior ring is correctly computed"){
    OGRLineString* ext_line = test_prepare_obb();
    REQUIRE(ext_line->IsValid() == 0);
    REQUIRE(ext_line->getGeometryType() == wkbLineString);

}

TEST_CASE( "OBB is correctly computed", "[obb]" ) {
    std::vector<Vecteur> test = test_obbh();
    REQUIRE( std::floor(test[0].x) == 0 );
    REQUIRE( std::floor(test[0].y) == 3 );
    REQUIRE( std::floor(test[1].x) == 0 );
    REQUIRE( std::floor(test[1].y) == 0 );
    REQUIRE( std::floor(test[2].x) == 3 );
    REQUIRE( std::floor(test[2].y) == 0 );
    REQUIRE( std::floor(test[3].x) == 3 );
    REQUIRE( std::floor(test[3].y) == 3 );
}


