#include "customHelperFunctions.h"


XY latlonsToCoord(LatLon latlon) {
    XY coords;
    coords.first = (latlon.lon() * DEG_TO_RAD) * streetNet::getInstance()->getInterAveLat();
    coords.second = latlon.lat() * DEG_TO_RAD;
    
    return coords;
}
