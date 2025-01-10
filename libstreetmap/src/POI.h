#pragma once

#include "Point.h"
#include "StreetsDatabaseAPI.h" //Layer 2 OSM API


//Holds information regarding points of interests (POIs))
//this is a derived class from Point
//Point will handle all the find closed 
class POI : public Point
{
private:
    unsigned POIID;
    std::string type;
    std::string name;
    unsigned closestIntersection;
    
public:
    POI();
    POI(unsigned POIID_);
    
    std::string get_name();
    std::string get_type();
    
    void drawPoint();
    void drawRestaurant();
    void drawFunPlace();
    void drawIceCream();
    void drawCafe();
    void drawDentist();
    void drawHospital();
    void drawDoctors();
    void drawCondo();
    void drawClinic();
    void drawPharmacy();
    void drawParking();
    void drawBank();
    void drawCollege();
    void drawChildcare();
    void drawLibrary();
    void drawPub();
    void drawBar();
    void drawLab();
    void drawSchool();
    void drawTheatre();
    void drawFuel();
    void drawAtm();
    void drawLawyer();
    void drawPolice();
    void drawKindergarten();
    void drawSpa();
    void drawTelephone();
    void drawCinema();
    void drawTutoring();
    void drawPsychic();
    void drawVeterinary();
    
    
    //friends list = class XXX can freely access data
    friend class streetNet;
};
