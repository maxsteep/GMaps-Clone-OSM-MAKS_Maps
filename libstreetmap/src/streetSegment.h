#pragma once


#include "StreetsDatabaseAPI.h" //Layer 2 OSM API

#include "m1.h" //necessary for the find_distance_between_two_points in the scope of this file
#include "Contour.h"


//Defines the conversion: km per hour to meters per second
#define KMPH_TO_MPS 0.27777777778



//Class holding information about street segments
class streetSegment : public Contour
{
private:
    unsigned segID;
    //OSMID segOSMID;
    unsigned streetID;
    unsigned from, to;
    bool oneWay; //if true, street only go from -> to
    double speedLimit;
    std::vector<LatLon> curvePointList;
    //std::vector<LatLon> pointList;
    //std::vector<std::pair<double, double>> projectedList;
    //double length;
    std::vector<double> lengthSegments;
    std::vector<double> textAngles;
    double travelTime;
    //std::string exInfo0;  //this default store the primary type
    //std::string exInfo1;  //this default store the secondary type
    
    
public:
    streetSegment();
    
    
    //Real class constructor follow below
    //Expects street segment ID
    //Populates the class object with data
    //Includes the length calculation
    streetSegment(unsigned segID_);
    
    double getTravelTime();
    unsigned setSpeedLimit();
    
    std::string getStName();
    
    void drawStName();
    
    
    //friends list = class XXX can freely access data
    friend class intersection;
    friend class streetNet;
};

//this is the most accurate way to calculate distance on earth
//this will work any where on earth
double distance_on_Earth(LatLon point1, LatLon point2);
