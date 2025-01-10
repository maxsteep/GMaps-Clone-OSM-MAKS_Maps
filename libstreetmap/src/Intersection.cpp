#include "Intersection.h"
#include "streetNet.h"


//Default unused constructor
intersection::intersection() {}


//Holds intersection level data
intersection::intersection(unsigned intersecID_) :
    intersecID(intersecID_)
{
    //find name
    name = getIntersectionName(intersecID_);
    
    //find location
    location = getIntersectionPosition(intersecID_);
    
    //find OSMID
    pointOSMID = getIntersectionOSMNodeID(intersecID_);
    
    //following set up segList
    unsigned numSeg = getIntersectionStreetSegmentCount(intersecID_);
    
    //populate the list of all segments connected to this intersection
    segList.reserve(numSeg);    //speed
    for (unsigned i = 0; i < numSeg; i ++) {
        segList.emplace_back(getIntersectionStreetSegment(intersecID_, i));
    }
    //segList.shrink_to_fit();
    visited = false;
    
    //projPt.first = (getIntersectionPosition(intersecID_).lon() * DEG_TO_RAD) * streetNet::getInstance()->getInterAveLat(); //InterAveLat isnt available yet
    //projPt.second = getIntersectionPosition(intersecID_).lat() * DEG_TO_RAD;
}
  

//return name
std::string intersection::get_name() {
    return name;
}


//return intersecID
unsigned intersection::get_ID() {
    return intersecID;
}


//return location
LatLon intersection::get_location() {
    return location;
}


//return OSMID

OSMID intersection::get_OSMID() {
    return pointOSMID;
}


//this returns a list of street IDs meeting at the given intersection
std::vector<unsigned> intersection::street_ID_list() {
    std::vector<unsigned> IDList;
    
    unsigned segNum = segList.size();   //speed
    IDList.reserve(segNum);             //speed
    for (unsigned i = 0; i < segNum; i ++) {
        IDList.emplace_back(getStreetSegmentInfo(segList[i]).streetID);
    }
    
    return IDList;
}
