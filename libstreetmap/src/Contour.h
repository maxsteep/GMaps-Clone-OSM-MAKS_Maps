#pragma once
#include "LatLon.h"
#include "OSMID.h"
#include <vector>


enum contourType{
    dontCare = 0,
    streetSegment_,
    subway_,
    feature_,
    ferryWay_,
    streetCart_
};

enum roadTypes {
    //highway = 0,//always
    unknown = 0,
    motorway,
    trunk,
    primary,
    secondary,
    motorway_link,
    trunk_link,
    primary_link,
    secondary_link,
    tertiary,
    tertiary_link,
    residential,
    unclassified,
    service,
    path,
    footway,
    cycleway,
    construction,
    track,
    steps,
    bridleway,
    living_street,
    pedestrian,
    bus_guideway,
    escape,
    raceway,
    road 
};

class Contour
{
private:
    unsigned contourID;
protected:
    contourType ctType;
    bool closeContour;
    std::vector<LatLon> pointList;
    std::vector<std::pair<double, double>> projectedList;
    OSMID contourOSMID;
    double length; //in meters
    //double area; //in case of open contour, area = 0
    
    std::string exInfo0;  //this default store the primary type
    std::string exInfo1;  //this default store the secondary type.
    
    roadTypes roadType;

public:
    Contour();
    Contour(unsigned contourID_);
    
    //this gives access to the pointList
    std::vector<LatLon> getPointList();
    
    
    //this will draw the contour by its respective type
    void drawContour(bool highlight = false);
    
    friend class streetNet;
};



class ferryWay : public Contour
{
private:
    unsigned ferryWayID;
    std::string name;
    //exInfo0 is bicyle info
    //exInfo1 is on foot info
public:
    ferryWay();
    ferryWay(unsigned contourID_);
    
    void drawFerryName();
};



class subway : public Contour
{
private:
    unsigned subwayID;
    std::string name;
public:
    subway();
    subway(unsigned contourID_);
    
    void drawSubwayName();
};



class streetCart : public Contour
{
private:
    unsigned streetCartID;
    std::string name; //only very little have 
public:
    streetCart();
    streetCart(unsigned contourID_);
};
