#include "Vecteur.h"
#include "ogrsf_frmts.h"
#include <iostream>
#include <cmath>
#include "decoupage.h"
#include <algorithm>


/**
 * \fn OGRLineString* prepare_obb (OGRPolygon* poly_prep)
 *
 * \brief Transform the given OGRPolygon* into a OGRLineString* with points
 * in counter clockwise order
 *
 * \param OGRPolygon* polygon you want to transform
 * \param OGRLayer *layer_w layer where you want to write polygons
 *
 * \return OGRLineString* The LineString in counterclockwise order
 */
OGRLineString* prepare_obb (OGRPolygon* poly_prep){

    OGRLinearRing* ext_ring = poly_prep->getExteriorRing();
    //std::cout << "la ext_ring est valide ? " << ext_ring->IsValid() << std::endl;
    //std::cout << "nombre de points" << ext_ring->getNumPoints() << std::endl;
    if (ext_ring->isClockwise() == 1){
        ext_ring->reverseWindingOrder();
    }

    OGRLineString* ext_line = (OGRLineString*) ext_ring;
    return ext_line;
}


/**
 * \fn Vecteur IntersectLines(Vecteur start0, Vecteur dir0, Vecteur start1, Vecteur dir1)
 *
 * \brief Compute the intersection between one point and a direction (start0,dir0)
 * with one other point and one other direction (start1, dir1)
 *
 * \param Vecteur start0 The first point coordinate
 * \param Vecteur dir0 The first direction vector coordinate
 * \param Vecteur start1 The second point coordinate
 * \param Vecteur dir1 The second direction vector coordinate
 *
 * \return Vecteur Point Coordinate of computed intersection
 */
Vecteur IntersectLines(Vecteur start0, Vecteur dir0, Vecteur start1, Vecteur dir1)
{
    long double dd = dir0.x*dir1.y-dir0.y*dir1.x;
    // dd=0 => lines are parallel. we don't care as our lines are never parallel.
    long double dx = start1.x-start0.x;
    long double dy = start1.y-start0.y;
    long double t = (dx*dir1.y-dy*dir1.x)/dd;

    return  Vecteur(start0.x+t*dir0.x, start0.y+t*dir0.y);

}

/**
 * \fn bool sortx (Vecteur a,Vecteur b)
 *
 * \brief Ascending Sorting method for "Vecteur" type along the x attribute
 *
 * \param Vecteur a
 * \param Vecteur b
 * \return bool a boolean that is true if a.x<b.x, else otherwise
 */
bool sortx (Vecteur a,Vecteur b) { return (a.x<b.x); }


/**
 * \fn std::vector<Vecteur> order(std::vector<Vecteur> obb)
 *
 * \brief Order obb points in order to have always extrem points in following order:
 * obbUpperLeft, obbBottomLeft, obbBottomRight, obbUpperRight
 *
 * \param std::vector<Vecteur> the 4 points representation of oriented bounding box
 * \return std::vector<Vecteur> the 4 points sorted in order
 */
std::vector<Vecteur> order(std::vector<Vecteur> obb){

    std::vector<Vecteur> sorted;

    std::sort (obb.begin(), obb.end(), sortx);

    if (obb[0].y > obb[1].y){
        sorted.push_back(obb[0]);
        sorted.push_back(obb[1]);
    }
    else{
        sorted.push_back(obb[1]);
        sorted.push_back(obb[0]);
    }

    if(obb[2].y > obb[3].y){
        sorted.push_back(obb[3]);
        sorted.push_back(obb[2]);
    }
    else{
        sorted.push_back(obb[2]);
        sorted.push_back(obb[3]);
    }

    return sorted;
}

/**
 * \fn std::vector<Vecteur> CalcOmbb(OGRLineString* convexHull, OGRPolygon* poly,OGRLayer* layer_w)

 *
 * \brief computes the minimum area enclosing rectangle
 * (aka oriented minimum bounding box)
 *
 * \param OGRLineString* convexHull The computed convexHull of the polygon
 * in OGRLineString* format
 *
 * \return std::vector<Vecteur> 4-size Vector that contained the 4 points coordinates
 * of the minimum area enclosing rectangle
 */
std::vector<Vecteur> CalcOmbb(OGRLineString* convexHull)
{

    // initialize attributes
    long double BestObbArea = std::numeric_limits<long double>::max();
    std::vector<Vecteur> BestObb;

    // compute directions of convex hull edges
    std::vector<Vecteur> edgeDirs;

    int nbr_points = convexHull->getNumPoints();
    nbr_points     -= 1;

     for(int i=0; i<nbr_points; ++i)
    {
        OGRPoint* pt1 = new OGRPoint();
        OGRPoint* pt2 = new OGRPoint();

        convexHull->getPoint(i,pt1);
        //std::cout << pt1->getX() << ", " << pt1->getY() << std::endl;
        convexHull->getPoint(i+1,pt2);

        Vecteur vec1 = Vecteur(pt1->getX(), pt1->getY());
        Vecteur vec2 = Vecteur(pt2->getX(), pt2->getY());

        edgeDirs.push_back(vec2.diff(vec1));
        edgeDirs[i].normalize();
        //std::cout << edgeDirs[i].x << ", "<<edgeDirs[i].y << std::endl;
    }
    //std::cout << "------------------------------------------------"<< std::endl;
    //std::cout << "taille du vecteur edgeDirs : " << edgeDirs.size() << std::endl;


    // compute extreme points
    Vecteur minPt = Vecteur (std::numeric_limits<long double>::max(),std::numeric_limits<long double>::max());
    Vecteur maxPt = Vecteur (std::numeric_limits<long double>::min(),std::numeric_limits<long double>::min());

    int leftIdx;
    int rightIdx;
    int topIdx;
    int bottomIdx;

    for (int i=0; i<nbr_points; ++i)
    {
        OGRPoint* pt_test = new OGRPoint();
        convexHull->getPoint(i,pt_test);

        if (pt_test->getX() < minPt.x)
        {
            minPt.x = pt_test->getX();
            leftIdx = i;
        }

        if (pt_test->getX() > maxPt.x)
        {
            maxPt.x  = pt_test->getX();
            rightIdx = i;
        }

        if (pt_test->getY() < minPt.y)
        {
            minPt.y   = pt_test->getY();
            bottomIdx = i;
        }

        if (pt_test->getY() > maxPt.y)
        {
            maxPt.y = pt_test->getY();
            topIdx  = i;
        }
    }


    // initial caliper lines + directions
    //
    //        top
    //      <-------
    //      |      A
    //      |      | right
    // left |      |
    //      V      |
    //      ------->
    //       bottom

    Vecteur leftDir   = Vecteur(0, -1);
    Vecteur rightDir  = Vecteur(0  , 1);
    Vecteur topDir    = Vecteur(-1 , 0);
    Vecteur bottomDir = Vecteur(1  , 0);

    // execute rotating caliper algorithm
    for (int i=0; i<nbr_points; ++i)
    {
        std::vector<long double> phis;
        phis.push_back(std::acos(leftDir.dot(edgeDirs[leftIdx])));
        phis.push_back(std::acos(rightDir.dot(edgeDirs[rightIdx])));
        phis.push_back(std::acos(topDir.dot(edgeDirs[topIdx])));
        phis.push_back(std::acos(bottomDir.dot(edgeDirs[bottomIdx])));
        //std::cout << phis[0] << ", " << phis[1] << ", " << phis[2] << ", " << phis[3] << ", " << std::endl;
        int lineWithSmallestAngle = 0;
        long double minimum = phis[0];
        for (int j = 0; j < 4; j++)
        {
            if(phis[j] < minimum){
                minimum   = phis[j];
                lineWithSmallestAngle = j;
            }
        }
        //std::cout << lineWithSmallestAngle << std::endl;
        // 0=left, 1=right, 2=top, 3=bottom
        switch (lineWithSmallestAngle)
        {
        case 0: // left
            leftDir   = edgeDirs[leftIdx].clone();
            rightDir  = leftDir.clone();
            rightDir.negate();
            topDir    = leftDir.orthogonal();
            bottomDir = topDir.clone();
            bottomDir.negate();
            leftIdx   = (leftIdx+1)%nbr_points;
            break;
        case 1: // right
            rightDir  = edgeDirs[rightIdx].clone();
            leftDir   = rightDir.clone();
            leftDir.negate();
            topDir    = leftDir.orthogonal();
            bottomDir = topDir.clone();
            bottomDir.negate();
            rightIdx  = (rightIdx+1)%nbr_points;
            break;
        case 2: // top
            topDir    = edgeDirs[topIdx].clone();
            bottomDir = topDir.clone();
            bottomDir.negate();
            leftDir   = bottomDir.orthogonal();
            rightDir  = leftDir.clone();
            rightDir.negate();
            topIdx    = (topIdx+1)%nbr_points;
            break;
        case 3: // bottom
            bottomDir = edgeDirs[bottomIdx].clone();
            topDir    = bottomDir.clone();
            topDir.negate();
            leftDir   = bottomDir.orthogonal();
            rightDir  = leftDir.clone();
            rightDir.negate();
            bottomIdx = (bottomIdx+1)%nbr_points;
            break;
        }
        /*
        std::cout << "------------------------------------------------"<< std::endl;
        std::cout<< "itération numéro : "<< i << std::endl;
        std::cout << "rightDir : " << rightDir.x << ", " << rightDir.y << std::endl;
        std::cout << "leftDir : " << leftDir.x << ", " << leftDir.y << std::endl;
        std::cout << "topDir : " << topDir.x << ", " << topDir.y << std::endl;
        std::cout << "bottomDir : " << bottomDir.x << ", " << bottomDir.y << std::endl;
        std::cout << "index : " << leftIdx << ", "<< rightIdx<< ", " << topIdx<< ", " << bottomIdx << std::endl;

        std::cout << "------------------------------------------------"<< std::endl;
        */

        OGRPoint* pt_left = new OGRPoint();
        convexHull->getPoint(leftIdx,pt_left);
        Vecteur leftStart = Vecteur(pt_left->getX(), pt_left->getY());

        OGRPoint* pt_right = new OGRPoint();
        convexHull->getPoint(rightIdx,pt_right);
        Vecteur rightStart = Vecteur(pt_right->getX(), pt_right->getY());

        OGRPoint* pt_top = new OGRPoint();
        convexHull->getPoint(topIdx,pt_top);
        Vecteur topStart = Vecteur(pt_top->getX(), pt_top->getY());

        OGRPoint* pt_bottom = new OGRPoint();
        convexHull->getPoint(bottomIdx,pt_bottom);
        Vecteur bottomStart = Vecteur(pt_bottom->getX(), pt_bottom->getY());

        Vecteur obbUpperLeft   = IntersectLines(leftStart, leftDir, topStart, topDir);
        Vecteur obbUpperRight  = IntersectLines(rightStart, rightDir, topStart, topDir);
        Vecteur obbBottomLeft  = IntersectLines(bottomStart, bottomDir, leftStart, leftDir);
        Vecteur obbBottomRight = IntersectLines(bottomStart, bottomDir, rightStart, rightDir);
/*
        OGRLinearRing ring_test;
        ring_test.addPoint(obbUpperLeft.x,obbUpperLeft.y);
        ring_test.addPoint(obbBottomLeft.x,obbBottomLeft.y);
        ring_test.addPoint(obbBottomRight.x,obbBottomRight.y);
        ring_test.addPoint(obbUpperRight.x,obbUpperRight.y);
        ring_test.addPoint(obbUpperLeft.x,obbUpperLeft.y);
        OGRPolygon poly_test;
        poly_test.addRing(&ring_test);

*/
        //ecrire(&poly_test, layer_w);


        long double distLeftRight   = obbUpperLeft.distance(obbUpperRight);
        long double distTopBottom   = obbUpperLeft.distance(obbBottomLeft);
        long double obbArea         = distLeftRight*distTopBottom;

        //std::cout << "Contains : " << poly_test.OGRCurvePolygon::Contains(poly) << std::endl;

        if(BestObbArea>obbArea){
            BestObb.clear();
            BestObb.push_back(obbUpperLeft);
            BestObb.push_back(obbBottomLeft);
            BestObb.push_back(obbBottomRight);
            BestObb.push_back(obbUpperRight);
            BestObb = order(BestObb);
            BestObbArea = obbArea;
        }
    /*
        OGRLinearRing ring_test;
        ring_test.addPoint(obbUpperLeft.x,obbUpperLeft.y);
        ring_test.addPoint(obbBottomLeft.x,obbBottomLeft.y);
        ring_test.addPoint(obbBottomRight.x,obbBottomRight.y);
        ring_test.addPoint(obbUpperRight.x,obbUpperRight.y);
        ring_test.addPoint(obbUpperLeft.x,obbUpperLeft.y);
        OGRLineString line_test = (OGRLineString) ring_test;
        ecrire(&line_test, layer_w);
*/

    };

    return BestObb;
}
