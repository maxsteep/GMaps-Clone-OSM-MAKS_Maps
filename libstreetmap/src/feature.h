#pragma once

#include <string>
#include <vector>
#include <string>
#include "LatLon.h"
#include "streetSegment.h"
#include "Contour.h"
#include "graphics.h"


class feature : public Contour
{
private:
    unsigned featureID;
    std::string name; //note: most feature don't have names
    //TypedOSMID featureOSMID;
    FeatureType type;
    LatLon centerLocation;
    
    //std::vector<LatLon> pointList;
    //std::vector<std::pair<double, double>> projectedList;
    double area;
    
    
public:
    t_point* pointArray;
    
    feature();
    feature(unsigned featureID_);
    //~feature();
    
    FeatureType getType();
    
    LatLon getLocation();
    
    void drawContour();
    
    friend class streetNet;
};
