#include "Point.h"
#include "OSMDatabaseAPI.h"
#include "graphics.h"
#include "streetNet.h"


using namespace std;


Point::Point() {}

Point::Point(unsigned pointID_) : pointID(pointID_) {}

LatLon Point::getLocation() {
    return location;
}


OSMID Point::getOSMID() {
    return pointOSMID;
}


t_point Point::getWorldLocation() {
    t_point thispoint(projPt.first, projPt.second);
    return thispoint;
}


void Point::highlightPoint() {
    set_coordinate_system(GL_WORLD); 
    set_drawing_buffer(ON_SCREEN); 
    
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    double x = location.lon() * DEG_TO_RAD * cosAveLat;
    double y = location.lat() * DEG_TO_RAD;
    
    setcolor(0, 42, 92); //UofT Blue
    fillrect(x-0.000001, y-0.000001, x + 0.000001, y + 0.000001);
    flushinput();
    set_drawing_buffer(OFF_SCREEN); 
}

//******************************************************************************
// class subwayStation

subwayStation::subwayStation() {}


subwayStation::subwayStation(unsigned pointID_) : Point(pointID_) {
    const OSMNode* station = getNodeByIndex(pointID_);
    pair<string, string> tag0 = getTagPair(station, 0);
    name = tag0.second;
    
    pointOSMID = station->id();
    location = station->coords();
    ptType = subwayStation_;
}


void subwayStation::drawSubStation() {
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    double x = location.lon() * DEG_TO_RAD * cosAveLat;
    double y = location.lat() * DEG_TO_RAD;
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (24 * pixel_to_XY)/2; //PNG size is 24*24 in this case
    
    draw_surface(streetNet::getInstance()->SubIcon, x - offset, y + offset);
    
    settextattrs(10, 0);
    setcolor(100, 100, 100);
    drawtext_left(x + offset, y, name, 99999, 99999);
}



// *****************************************************************************
streetCartStop::streetCartStop(unsigned pointID_) : Point(pointID_) {

    const OSMNode* STstop = getNodeByIndex(pointID_);
    for (unsigned i = 0; i < getTagCount(STstop); ++ i) {
        pair<string, string> tag = getTagPair(STstop, i);
        if (tag.first.compare("name") == 0) {
            name = tag.second;
        }
    }

    pointOSMID = STstop->id();
    location = STstop->coords();
    ptType = streetCartStop_;
}

void streetCartStop::drawStCartStop() {
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    double x = location.lon() * DEG_TO_RAD * cosAveLat;
    double y = location.lat() * DEG_TO_RAD;

    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 24*24 in this case

    draw_surface(streetNet::getInstance()->StCartIcon, x - offset, y + offset);

    settextattrs(10, 0);
    setcolor(100, 100, 100);
    drawtext_left(x + offset, y, name, 99999, 99999);
}



//******************************************************************************
busStop::busStop(unsigned pointID_) : Point(pointID_) {

    const OSMNode* busstop = getNodeByIndex(pointID_);
    for (unsigned i = 0; i < getTagCount(busstop); ++ i) {
        pair<string, string> tag = getTagPair(busstop, i);
        if (tag.first.compare("name") == 0) {
            name = tag.second;
        }
    }

    pointOSMID = busstop->id();
    location = busstop->coords();
    ptType = busStation_;
}

void busStop::drawBusStop() {
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    double x = location.lon() * DEG_TO_RAD * cosAveLat;
    double y = location.lat() * DEG_TO_RAD;

    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 24*24 in this case

    draw_surface(streetNet::getInstance()->BusIcon, x - offset, y + offset);

    if (get_visible_world().get_height() <= 0.00005) { 
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(x + offset, y, name, 99999, 99999);
    }
}