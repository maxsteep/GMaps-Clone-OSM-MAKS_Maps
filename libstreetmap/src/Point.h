#pragma once
#include "LatLon.h"
#include "OSMID.h"
#include "graphics.h"

enum point_type {
    noIdea = 0,
    POI_,
    intersection_,
    subwayStation_,
    streetCartStop_,
    busStation_
};


class Point
{
protected:
    unsigned pointID;
    OSMID pointOSMID;
    LatLon location;
    std::pair<double, double> projPt;
    point_type ptType;
public:
    Point();
    Point(unsigned pointID_);
    
    LatLon getLocation();
    OSMID getOSMID();
    
    t_point getWorldLocation();
    
    void highlightPoint();
    
    
    friend class streetNet;
};



class subwayStation : public Point
{
private:
    std::string name;
//    std::vector<unsigned> entranceList;
//    std::vector<std::string> extraInfo;
public:
    subwayStation();
    subwayStation(unsigned pointID_);
    
    void drawSubStation();
    
    
    friend class streetNet;
};



class streetCartStop : public Point
{
private:
    std::string name;
public:
    streetCartStop(unsigned pointID_);
    
    void drawStCartStop();
    
    friend class streetNet;
};



class busStop : public Point
{
private:
    std::string name;
public:
    busStop(unsigned pointID_);
    
    void drawBusStop();
    
    friend class streetNet;
};
