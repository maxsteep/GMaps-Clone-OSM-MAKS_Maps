#pragma once


#include "StreetsDatabaseAPI.h" //Layer 2 OSM API
#include "Point.h"


//Holds intersection level data
class intersection : public Point
{
private:
    std::string name;
//    LatLon location;
    unsigned intersecID;
    bool visited; //this is usefull for path finding
//    OSMID intersecOSMID;
    std::vector<unsigned> segList; //list of all segments connected to this intersection
    //this is a list of intersection that can be reached from this intercection
    std::vector<unsigned> connectedList;

    
public:
    intersection();
    
    
    //this is the proper constructor to use
    //all it need to know is the intersection ID
    //and it will find out the rest
    intersection(unsigned intersecID_);
    
    
    std::string get_name(); //return name
    unsigned get_ID();      //return intersecID
    LatLon get_location();  //return location
    OSMID get_OSMID();   //return OSMID
    
    
    //this returns a list of street IDs meeting at the given intersection
    std::vector<unsigned> street_ID_list();
    
    
    //friends list = class XXX can freely access data
    friend class streetSegment;
    friend class streetNet;
};
