#include "Contour.h"
#include "OSMDatabaseAPI.h"
#include <vector>
#include "graphics.h"
#include "mappinglib.h"
#include "streetNet.h"
#include "cmath"
#include "streetSegment.h"

using namespace std;

Contour::Contour() {}


Contour::Contour(unsigned contourID_) : contourID(contourID_) {
}



std::vector<LatLon> Contour::getPointList(){
    return pointList;
}


void Contour::drawContour(bool highlight) {
//    if (rect_off_screen(projectedList[0].first, projectedList[0].second, 
//            projectedList.end()->first, projectedList.end()->second)) return;
//    
//    double visibleBottom = get_visible_world().bottom();
//    double visiableTop = get_visible_world().top();
//    double visiableLeft = get_visible_world().left();
//    double visiableRight = get_visible_world().right();
    
//    if ((projectedList[0].first < visiableLeft || projectedList[0].first > visiableRight)
//            && (projectedList.end()->first < visiableLeft || projectedList.end()->first > visibleBottom)) return;
//    if ((projectedList[0].second > visiableTop || projectedList[0].second < visiableRight)
//            && (projectedList.end()->second > visiableTop || projectedList.end()->second < visibleBottom)) return;
    
    line_types ctLineType = SOLID;
    t_color ctColour(111, 48, 146);
    int width = streetNet::getInstance()->defaultWidth;/*round((1/get_visible_world().get_height()) / 1800);*/
    e_draw_mode ctDrawMode;
    if (ctType == streetSegment_) {
        ctLineType = SOLID;
        ctDrawMode = DRAW_NORMAL;  //following is a list of tag we classify as "Highway"
        //if (exInfo0.compare("highway") == 0 
        //std::cout << "roadType = " << roadType << std::endl;
        if ((roadType == motorway)||(roadType == trunk)) {
            t_color highwayColour(161, 113, 187);
            ctColour = highwayColour;
            width = streetNet::getInstance()->HighwayWidth;
        }
        else if ((roadType == motorway_link)||(roadType == trunk_link)) {
            t_color highwayColour(189, 156, 209);
            ctColour = highwayColour;
            width = streetNet::getInstance()->HighwayLinkWidth;
        }
        else if ((roadType == primary)
                    || (roadType == secondary)
                    || (roadType == tertiary)
                    || (roadType == motorway_link)
                    || (roadType == trunk_link)
                    || (roadType == primary_link)
                    || (roadType == secondary_link)
                    || (roadType == tertiary_link)) {
                        
            ctColour = WHITE;
            width = streetNet::getInstance()->primeryWidth; //prime street width
        }
        else {
            ctColour = WHITE;
            width = streetNet::getInstance()->defaultWidth;  //minor street width
        }
    }
    else if (ctType == ferryWay_) {
        ctLineType = DASHED;
        ctDrawMode = DRAW_NORMAL; 
        width = 1;
        t_color ferryColour(0, 200, 255);
        ctColour = ferryColour;
    }
    else if (ctType == subway_) {
        ctLineType = SOLID;
        ctDrawMode = DRAW_NORMAL; 
        width = 2;
        t_color subwayColour(128, 128, 128);
        ctColour = subwayColour;
    }
    else if (ctType == streetCart_) {
        ctLineType = DASHED;
        ctDrawMode = DRAW_NORMAL; 
        width = 1;
        t_color stCartColour(128, 128, 128);
        ctColour = stCartColour;
    }
    //////////////////////////////////////////////high lighting
    if (highlight) {
        t_color highLighting(255, 96, 0); //UofT Blue
        ctColour = highLighting;
    }
    /////////////////////////////////////////////
    if ((roadType == motorway)||(roadType == trunk)
                    || (roadType == primary)
                    || (roadType == secondary)
                    || (roadType == motorway_link)
                    || (roadType == trunk_link)
                    || (roadType == primary_link)
                    || (roadType == secondary_link)
                    || ((roadType == tertiary)&&(get_visible_world().get_height() < 0.001))
                    || ((roadType == tertiary_link)&&(get_visible_world().get_height() < 0.001)) ) {
            for (unsigned j = 1; j < projectedList.size(); j ++) {
                set_draw_mode(ctDrawMode); 
                setlinestyle(ctLineType, ROUND);
                setlinewidth(width);
                setcolor(ctColour);  
                drawline(projectedList[j-1].first, projectedList[j-1].second, 
                        projectedList[j].first, projectedList[j].second);
            } 
            
    }
    else if (ctType == subway_ || ctType == streetCart_) {
        for (unsigned j = 1; j < projectedList.size(); j ++) {
            set_draw_mode(ctDrawMode); 
            setlinestyle(ctLineType, ROUND);
            setlinewidth(width);
            setcolor(ctColour);  
            drawline(projectedList[j-1].first, projectedList[j-1].second, 
                    projectedList[j].first, projectedList[j].second);
        }
    }
    else {
        if ((get_visible_world().get_height() < 0.0008) || highlight) {
            for (unsigned j = 1; j < projectedList.size(); j ++) {
                set_draw_mode(ctDrawMode); 
                setlinestyle(ctLineType, ROUND);
                setlinewidth(width);
                setcolor(ctColour);  
                drawline(projectedList[j-1].first, projectedList[j-1].second, 
                        projectedList[j].first, projectedList[j].second);                                                                                                                                                                                                                                                                                   
            }
        }
    }
}



//Ferry ************************************************************************
ferryWay::ferryWay() {}


ferryWay::ferryWay(unsigned contourID_) : Contour(contourID_) {
    ctType = ferryWay_;
    closeContour = false;
    for (unsigned i = 0; i < getWayByIndex(contourID_)->ndrefs().size(); i ++) {
        OSMID curvePt = getWayByIndex(contourID_)->ndrefs()[i];
        pointList.emplace_back(streetNet::getInstance()->getOSMNode(curvePt)->coords());
    }
    
    //calculate projectedList
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    for (unsigned i = 0; i < pointList.size(); i ++) {
        std::pair<double, double> newpair;
        newpair.first = pointList[i].lon() * DEG_TO_RAD * cosAveLat;
        newpair.second = pointList[i].lat() * DEG_TO_RAD;
        projectedList.emplace_back(newpair);
    }
    
    contourOSMID = getWayByIndex(contourID_)->id();
    //area = 0;
    for (unsigned i = 0; i < getTagCount(getWayByIndex(contourID_)); i ++) {
        std::pair<std::string, std:: string> curTag = getTagPair(getWayByIndex(contourID_), i);
        if (curTag.first.compare("name") == 0) {
            name = curTag.second;
        }
        else if (curTag.first.compare("bicycle") == 0) {
            exInfo0 = curTag.second;
        }
        else if (curTag.first.compare("foot") == 0) {
            exInfo1 = curTag.second;
        }
    }
}


void ferryWay::drawFerryName() {
    if (get_visible_world().get_height() < 0.0005 && projectedList.size() > 2) {
        unsigned midPt = projectedList.size() / 2;
        
        double dx = projectedList[midPt - 1].first - projectedList[midPt + 1].first;
        double dy = projectedList[midPt - 1].second - projectedList[midPt + 1].second;
        
        double rotation = atan(dy/dx) / DEG_TO_RAD;
        
        setcolor(150, 150, 150);
        settextattrs(8, rotation);
        drawtext(projectedList[midPt].first, projectedList[midPt].second, name);
    }
}



//subway ***********************************************************************
subway::subway() {}

subway::subway(unsigned contourID_) : Contour(contourID_) {
    ctType = subway_;
    closeContour = false;
    for (unsigned i = 0; i < getWayByIndex(contourID_)->ndrefs().size(); i ++) {
        OSMID curvePt = getWayByIndex(contourID_)->ndrefs()[i];
        pointList.emplace_back(streetNet::getInstance()->getOSMNode(curvePt)->coords());
    }
    
    //calculate projectedList
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    for (unsigned i = 0; i < pointList.size(); i ++) {
        std::pair<double, double> newpair;
        newpair.first = pointList[i].lon() * DEG_TO_RAD * cosAveLat;
        newpair.second = pointList[i].lat() * DEG_TO_RAD;
        projectedList.emplace_back(newpair);
    }
    
    contourOSMID = getWayByIndex(contourID_)->id();
//    area = 0;
//    for (unsigned i = 0; i < getTagCount(getWayByIndex(contourID_)); i ++) {
//        std::pair<std::string, std:: string> curTag = getTagPair(getWayByIndex(contourID_), i);
//        if (curTag.first.compare("name") == 0) {
//            name = curTag.second;
//        }
//        else if (curTag.first.compare("bicycle") == 0) {
//            exInfo0 = curTag.second;
//        }
//        else if (curTag.first.compare("foot") == 0) {
//            exInfo1 = curTag.second;
//        }
//   }
}



//streetCart *******************************************************************
streetCart::streetCart() {}


streetCart::streetCart(unsigned contourID_) : Contour(contourID_) {
    ctType = streetCart_;
    closeContour = false;
    for (unsigned i = 0; i < getWayByIndex(contourID_)->ndrefs().size(); i ++) {
        OSMID curvePt = getWayByIndex(contourID_)->ndrefs()[i];
        pointList.emplace_back(streetNet::getInstance()->getOSMNode(curvePt)->coords());
    }
    
    //calculate projectedList
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    for (unsigned i = 0; i < pointList.size(); i ++) {
        std::pair<double, double> newpair;
        newpair.first = pointList[i].lon() * DEG_TO_RAD * cosAveLat;
        newpair.second = pointList[i].lat() * DEG_TO_RAD;
        projectedList.emplace_back(newpair);
    }
    
    contourOSMID = getWayByIndex(contourID_)->id();
//    area = 0;
//    for (unsigned i = 0; i < getTagCount(getWayByIndex(contourID_)); i ++) {
//        std::pair<std::string, std:: string> curTag = getTagPair(getWayByIndex(contourID_), i);
//        if (curTag.first.compare("name") == 0) {
//            name = curTag.second;
//        }
//        else if (curTag.first.compare("bicycle") == 0) {
//            exInfo0 = curTag.second;
//        }
//        else if (curTag.first.compare("foot") == 0) {
//            exInfo1 = curTag.second;
//        }
//   }
}
