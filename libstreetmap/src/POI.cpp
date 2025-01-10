#include "POI.h"
#include "graphics.h"
#include "streetNet.h"


//Default unused constructor
POI::POI() {}


//Holds information regarding points of interests (POIs))
POI::POI(unsigned POIID_) : POIID(POIID_) {
    type = getPointOfInterestType(POIID);
    name = getPointOfInterestName(POIID);
    location = getPointOfInterestPosition(POIID);
    pointOSMID = getPointOfInterestOSMNodeID(POIID);
    ptType = POI_;
    double worldAveLat = streetNet::getInstance()->getInterAveLat();
    projPt.first = location.lon()* DEG_TO_RAD * worldAveLat;
    projPt.second = location.lat()* DEG_TO_RAD;
    closestIntersection = find_closest_intersection(location);
}


std::string POI::get_name() {
    return name;
}


std::string POI::get_type() {
    return type;
}


void POI::drawPoint() {
    if (rect_off_screen(projPt.first, projPt.second, projPt.first, projPt.second)) return;
    
    if (type.compare("restaurant") != 0 && type.compare("stripclub") != 0&&
        type.compare("ice_cream") != 0 &&type.compare("dentist") != 0&&
        type.compare("cafe") != 0 &&type.compare("doctors") != 0&&
        type.compare("hospital") != 0 &&type.compare("condo") != 0&&
        type.compare("clinic") != 0 &&type.compare("pharmacy") != 0&&
        type.compare("parking") != 0 &&type.compare("college") != 0&&
        type.compare("bank") != 0 &&type.compare("childcare") != 0&&
        type.compare("library") != 0 &&type.compare("pub") != 0&&
        type.compare("bar") != 0 &&type.compare("lab") != 0&&
        type.compare("school") != 0 &&type.compare("fuel") != 0&&
        type.compare("police") != 0 &&type.compare("atm") != 0&&
        type.compare("theatre") != 0 &&type.compare("lawyer") != 0&&
        type.compare("spa") != 0 &&type.compare("cinema") != 0&&
        type.compare("telephone") != 0 &&type.compare("tutoring") != 0&&
        type.compare("psychic") != 0 &&type.compare("veterinary") != 0&&
        type.compare("kindergarten") != 0) {
        
//        double worldAveLat = streetNet::getInstance()->getInterAveLat();
//        std::pair<double, double> projPt(location.lon()* DEG_TO_RAD* worldAveLat,
//                                        location.lat()* DEG_TO_RAD);
        
        double heightInXY = get_visible_world().get_height();
        double heightInPixel = - get_visible_screen().get_height();
        double pixel_to_XY = heightInXY / heightInPixel;
        double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case

        draw_surface(streetNet::getInstance()->POIIcon, projPt.first - offset, projPt.second + offset);

        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second, name, 99999, 99999);
//        std::cout << type << std::endl;
    }
    
    drawRestaurant();
    drawFunPlace();
    drawIceCream();
    drawCafe();
    drawDentist();
    drawHospital();
    drawDoctors();
    drawCondo();
    drawClinic();
    drawPharmacy();
    drawParking();
    drawBank();
    drawCollege();
    drawChildcare();
    drawLibrary();
    drawPub();
    drawBar();
    drawLab();
    drawSchool();
    drawTheatre();
    drawFuel();
    drawAtm();
    drawLawyer();
    drawPolice();
    drawKindergarten();
    drawSpa();
    drawTelephone();
    drawCinema();
    drawTutoring();
    drawPsychic();
    drawVeterinary();
}


void POI::drawRestaurant() {
    if (type.compare("restaurant") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->RestaurantIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.0001){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second, name, 99999, 999999);
        }
    }
}

void POI::drawFunPlace() {
    if (type.compare("stripclub") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->FunIcon, projPt.first - offset, projPt.second + offset);
    
    settextattrs(10, 0);
    setcolor(100, 100, 100);
    drawtext_left(projPt.first + offset, projPt.second, name, 99999, 99999);
    }
}


void POI::drawIceCream() {
    if (type.compare("ice_cream") == 0) {
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->POIIcon, projPt.first - offset, projPt.second + offset);
    
    settextattrs(10, 0);
    setcolor(100, 100, 100);
    drawtext_left(projPt.first + offset, projPt.second, name, 99999, 99999);
    }
}


void POI::drawCafe() {
    if (type.compare("cafe") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->CafeIcon, projPt.first - offset, projPt.second + offset);
    if (get_visible_world().get_height() <0.0001){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second , name, 99999, 99999);
        }
    }
}


void POI::drawDentist() {
    if (type.compare("dentist") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->HospitalIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.0001){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawHospital() {
    if (type.compare("hospital") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->HospitalIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.01){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawDoctors() {
    if (type.compare("doctors") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->HospitalIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.001){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}


void POI::drawKindergarten() {
    if (type.compare("kindergarten") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->SchoolIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.001){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawClinic() {
    if (type.compare("clinic") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->HospitalIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.01){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawPharmacy() {
    if (type.compare("pharmacy") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->PharmacyIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.0001){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawParking() {
    if (type.compare("parking") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->ParkingIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.001){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawBank() {
    if (type.compare("bank") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->BankIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.0001){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawCollege() {
    if (type.compare("college") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->SchoolIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.01){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawChildcare() {
    if (type.compare("childcare") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->POIIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.01){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawLibrary() {
    if (type.compare("library") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->LibraryIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.001){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawPub() {
    if (type.compare("pub") == 0) {
        
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->NightLifeIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.0001){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawBar() {
    if (type.compare("bar") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->NightLifeIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.001){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawLab() {
    if (type.compare("lab") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->POIIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.01){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawSchool() {
    if (type.compare("school") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->SchoolIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.0001){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}


void POI::drawFuel() {
    if (type.compare("fuel") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->GasIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.0001){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawTheatre() {
    if (type.compare("theatre") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->TheatreIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.001){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawAtm() {
    if (type.compare("atm") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->AtmIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.001){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawPolice() {
    if (type.compare("police") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->PoliceIcon, projPt.first - offset, projPt.second + offset);
    
    
    
    settextattrs(10, 0);
    setcolor(100, 100, 100);
    drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
    }
}

void POI::drawLawyer() {
    if (type.compare("lawyer") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->POIIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.01){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawSpa() {
    if (type.compare("spa") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->POIIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.01){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawCinema() {
    if (type.compare("cinema") == 0) {
        
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->TheatreIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.01){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawTutoring() {
    if (type.compare("tutoring") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->POIIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.01){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawPsychic() {
    if (type.compare("psychic") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->POIIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.01){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawTelephone() {
    if (type.compare("telephone") == 0) {
        
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->POIIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.01){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}

void POI::drawVeterinary() {
    if (type.compare("veterinary") == 0) {

    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->POIIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.01){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}


void POI::drawCondo() {
    if (type.compare("condo") == 0) {
    
    double heightInXY = get_visible_world().get_height();
    double heightInPixel = - get_visible_screen().get_height();
    double pixel_to_XY = heightInXY / heightInPixel;
    double offset = (16 * pixel_to_XY)/2; //PNG size is 16*16 in this case
    
    draw_surface(streetNet::getInstance()->POIIcon, projPt.first - offset, projPt.second + offset);
    
    if (get_visible_world().get_height() <0.01){
        settextattrs(10, 0);
        setcolor(100, 100, 100);
        drawtext_left(projPt.first + offset, projPt.second  , name, 99999, 99999);
        }
    }
}