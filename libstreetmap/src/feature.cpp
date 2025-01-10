#include "feature.h"
#include "StreetsDatabaseAPI.h"
#include "graphics.h"
#include "streetNet.h"
#include <cmath>


using namespace std;


feature::feature() {}


feature::feature(unsigned featureID_) : featureID(featureID_) {
    name = getFeatureName(featureID_);
    contourOSMID = getFeatureOSMID(featureID_);
    type = getFeatureType(featureID_);
    
    double latSum = 0;
    double lonSum = 0;
    
    unsigned pointCount = getFeaturePointCount(featureID_);
    for (unsigned i = 0; i < pointCount; i ++) {
        LatLon newPt = getFeaturePoint(featureID, i);
        pointList.emplace_back(newPt);
        latSum += newPt.lat();
        lonSum += newPt.lon();
    }
    
    LatLon center(latSum/pointCount, lonSum/pointCount);
    centerLocation = center;
    
    
    //calculate projectedList
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    for (unsigned i = 0; i < pointList.size(); i ++) {
        std::pair<double, double> newpair;
        newpair.first = pointList[i].lon() * DEG_TO_RAD * cosAveLat;
        newpair.second = pointList[i].lat() * DEG_TO_RAD;
        projectedList.emplace_back(newpair);
    }
    
    if (type == Shoreline || type == Stream || type == Building) area = 0;
    else {  //general feature case, bar the ones listed right above
        double sum = 0;
        unsigned int pointNum = projectedList.size(), i = 0, j = pointNum - 1; 

        for (; i < pointNum; i++) {
          sum += (projectedList[j].first  + projectedList[i].first )
               * (projectedList[j].second - projectedList[i].second); 
          j = i; 
        }

        area = 0.5 * (sum < 0 ? -sum : sum);
        
    }
    
    pointArray = new t_point[projectedList.size()];
    
    for (unsigned i = 0; i < projectedList.size(); i ++) {
        pointArray[i].x = projectedList[i].first;
        pointArray[i].y = projectedList[i].second;
    }
}


//feature::~feature() {
//    delete [] pointArray;
//}


LatLon feature::getLocation() {
    return centerLocation;
}


FeatureType feature::getType() {
    return type;
}


void feature::drawContour() {
    t_color featureColor;
    
    t_color ParkColour(205, 231, 167);
    t_color BuildingColour(245, 236, 215);
    t_color waterColour(174, 210, 255);
    t_color islandColour(234, 234, 234);
    t_color beachColour(250, 243, 201);
    
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    
    //this draw all the water
    if (type == Lake || type == River || type == Shoreline) {
        featureColor = waterColour;
        setcolor(featureColor);
        fillpoly(pointArray, pointList.size());
    }
    
    if (type == Island) {
        featureColor = islandColour;
        setcolor(islandColour);
        fillpoly(pointArray, pointList.size());
    }
      
    if (type == Park || type == Greenspace || type == Golfcourse) {
        featureColor = ParkColour;
  
        setcolor(featureColor);
        fillpoly(pointArray, pointList.size());
    }
    
    
    
    if (type == Beach) {
        featureColor = beachColour;
        setcolor(featureColor);
        fillpoly(pointArray, pointList.size());
    }
    
    if ((get_visible_world().get_height() < 0.002)){
        if (type == Stream){
            featureColor = waterColour;
            for (unsigned j = 1; j < projectedList.size(); j ++) {
                set_draw_mode(DRAW_NORMAL); 
                setlinestyle(SOLID, ROUND);
                setlinewidth(1);
                setcolor(featureColor);  
                drawline(projectedList[j-1].first, projectedList[j-1].second, 
                        projectedList[j].first, projectedList[j].second);
            }
        }
        
        if (type == Building) {
            featureColor = BuildingColour;
            t_color buildingOutLIne(236, 215, 176);
            setcolor(featureColor);
            fillpoly(pointArray, pointList.size());

            //the following draw the outline of a building
            if (get_visible_world().get_height() < 0.0005) {
                //this add the fist point to the last again, so we have a closed contour
                projectedList.emplace_back(projectedList[0]);
                if (!(rect_off_screen(projectedList[0].first, projectedList[0].second, 
                        projectedList[0].first, projectedList[0].second))) {
                    for (unsigned j = 1; j < projectedList.size(); j ++) {
                        set_draw_mode(DRAW_NORMAL); 
                        setlinestyle(SOLID, ROUND);
                        setlinewidth(1);
                        setcolor(buildingOutLIne);  
                        drawline(projectedList[j-1].first, projectedList[j-1].second,
                                projectedList[j].first, projectedList[j].second);
                    }
                }
            }
        }
    }
    
}
