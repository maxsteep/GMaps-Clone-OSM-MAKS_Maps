#include "streetSegment.h"
#include "streetNet.h"
#include "graphics.h"
#include "customHelperFunctions.h"


streetSegment::streetSegment() {
    ctType = streetSegment_;
    closeContour = false;
}

//Class holding information about street segments
streetSegment::streetSegment(unsigned segID_) :
    segID(segID_)
{
    //find OSMID
    contourOSMID = getStreetSegmentInfo(segID).wayOSMID;
    
    //find streetID
    streetID = getStreetSegmentInfo(segID).streetID;
    
    //find from & to
    from = getStreetSegmentInfo(segID).from;
    to = getStreetSegmentInfo(segID).to;
    
    //find oneWay
    oneWay = getStreetSegmentInfo(segID).oneWay;
    
    //find speedLimit
    speedLimit = getStreetSegmentInfo(segID).speedLimit;
    
    //find curvePointList
    for (unsigned i = 0; i < getStreetSegmentInfo(segID).curvePointCount; i ++) {
        curvePointList.emplace_back(getStreetSegmentCurvePoint(segID_, i));
    }

    //this following calculate length
    //std::vector<LatLon> allPtList; //this list have all the LatLon for all curve points
                              //and the end points
    //this add from
    pointList.push_back(getIntersectionPosition(getStreetSegmentInfo(segID_).from));
    //this add all curve points
    for (unsigned j = 0; j < getStreetSegmentInfo(segID_).curvePointCount; j ++) {
        pointList.push_back(getStreetSegmentCurvePoint(segID_, j));
    }
    //this add to
    pointList.push_back(getIntersectionPosition(getStreetSegmentInfo(segID_).to));
    //pointList = allPtList;  // - swap around, get rid of pointlist? 
    
    //calculate projectedList
    double cosAveLat = streetNet::getInstance()->getInterAveLat();
    for (unsigned i = 0; i < pointList.size(); i ++) {
        std::pair<double, double> newpair;
        newpair.first = pointList[i].lon() * DEG_TO_RAD * cosAveLat;
        newpair.second = pointList[i].lat() * DEG_TO_RAD;
        projectedList.emplace_back(newpair);
    }
    
    
    double dLength = 0;
    //this sums over all the distances between points
    lengthSegments.reserve(pointList.size()-2);
    //textAngles.reserve(pointList.size()-1);
    textAngles.reserve(pointList.size()-2);
    for (unsigned k = 0; k < (pointList.size() - 1); k ++) {
        double dist = distance_between_points(pointList[k], pointList[k+1]);
        dLength += dist;
        lengthSegments.emplace_back(dist);
        
        textAngles.emplace_back((atan((projectedList[k].second - projectedList[k+1].second)/(projectedList[k].first - projectedList[k+1].first)))/DEG_TO_RAD);
        
    }
    //textAngles.emplace_back((atan((projectedList[0].second - projectedList.back().second)/(projectedList[0].first - projectedList.back().first)))/DEG_TO_RAD);
    
    length = dLength;
    
    //this get the travel time of this segment
    travelTime = (dLength / (speedLimit / 3.6));
    
    ctType = streetSegment_;
    closeContour = false;
    
    //this following will look up its type info
    roadType = streetNet::getInstance() -> getSegInfo(contourOSMID);
//    std::cout<<streetNet::getInstance()->getSegInfo(contourOSMID).first<<std::endl;
    //exInfo0 = infoPair.first;
    //exInfo1 = infoPair.second;
    /*
        //this following will look up its type info
    std::pair<std::string, std::string> infoPair = streetNet::getInstance()->
                                                        getSegInfo(contourOSMID);
//    std::cout<<streetNet::getInstance()->getSegInfo(contourOSMID).first<<std::endl;
    exInfo0 = infoPair.first;
    exInfo1 = infoPair.second;
     */
    //std::cout << " roadType = " << roadType << std::endl;
}


std::string streetSegment::getStName() {
    return streetNet::getInstance()->getStreetName(streetID);
}


double streetSegment::getTravelTime() {
    return travelTime;
}


unsigned streetSegment::setSpeedLimit() {
    return speedLimit;
}


//Please don't pay attention to the commented out code, work HEAVILY in progress. 
void streetSegment::drawStName() {
    //draw name on the center of a segment
    //streetSegments don't have latlons available readily, only .from and .to, one needs to derive latlons from .from and to. 
    //LatLon fromLoc = streetNet::getInstance()->getIntersecByID(from).location;
    //LatLon toLoc = streetNet::getInstance()->getIntersecByID(to).location;
    
    //convert to coords
    //XY fromC = latlonsToCoord(fromLoc);//projectedList[0]
    //XY toC   = latlonsToCoord(toLoc);//projectedList[1]
    //if (curvePointList == 0)
    //    XY aveCoords((projectedList[0].first + projectedList[1].first)/2, (projectedList[0].second + projectedList[1].second)/2);
    //else {
    //    for (unsigned i = 0; i < curvePointList.size(); i++) {
    //        
    //    }
    //}

    
    
    //calculate the proper angle of text
    //angle should only in between -90 to +90
    //double rawAngle = atan((projectedList[0].second - projectedList[1].second)/(projectedList[0].first - projectedList[1].first));
    
    //equivalent to converting rad -> deg
    //double realAngle = (rawAngle / DEG_TO_RAD);

    //Decides whether the street segment's name should be drawn
    
    //if (get_visible_world().get_height() > 0.00066){
        //std::cout << "first level = " << get_visible_world().get_height() << std::endl;
    //}
//    if (get_visible_world().get_height() > 0.00009) {
        //std::cout << "second level = " << get_visible_world().get_height() << std::endl;
        //auto whichIt = std::max_element(lengthSegments.begin(), lengthSegments.end());
        //unsigned whichPointer = std::distance(lengthSegments.begin(), whichIt );
        //std::vector<unsigned> stSegTemp = streetNet::getInstance()->getSegInStreet(streetID);
        //unsigned longestStreetSegmentID = streetNet::getInstance()->getStreetByID(streetID).longest;
        //std::cout << longestStreetSegmentID << std::endl;
        //if (segID == longestStreetSegmentID) {
            //std::cout << longestStreetSegmentID << std::endl;
            //longest segment in a street
            //std::vector<unsigned> stSegTemp = streetNet::getInstance()->getSegInStreet(streetID);
            //auto whichIt = std::max_element(stSegTemp.begin(), stSegTemp.end());
            //unsigned whichPointer = std::distance(stSegTemp.begin(), whichIt ); 

            //auto whichIt = std::max_element(lengthSegments.begin(), lengthSegments.end());
            //unsigned whichPointer = std::distance(lengthSegments.begin(), whichIt);
            //std::vector<unsigned> stSegTemp = streetNet::getInstance()->getSegInStreet(streetID);


            /*
            for (unsigned i = 0; i < pointList.size()-1; i++) {
                bool dontDraw = false;
                if (length < 50) {
                    dontDraw = true;
                }

                //Draws the street segment's name
                if (stName != "<unknown>" && !dontDraw) {
                    settextattrs(8, textAngles.back());
                    setcolor(100, 100, 100);
                    double xBound = 0.000006;
                    double yBound = 0.0006;
                    if (textAngles.back() <= 45 && textAngles.back() >= -45) {
                        xBound = 0.0006;
                        yBound = 0.000006;
                    }
                    drawtext(((projectedList[0].first + projectedList.back().first)/2), ((projectedList[0].second + projectedList.back().second)/2), stName, xBound, yBound);
                }  
            }// ******************************/ 

            //for (unsigned i = 0; i < pointList.size()-1; i++) {
        
        //if (exInfo1.compare("motorway") == 0 
        //            || exInfo1.compare("trunk") == 0
        //            || exInfo1.compare("primary") == 0
        //            || exInfo1.compare("secondary") == 0
        //            || exInfo1.compare("tertiary") == 0
        //    ) {
            
//            int windowHeight = -get_visible_screen().get_height();
//            int windowWidth = get_visible_screen().get_width();
            
//           double worldHeight = get_visible_world().get_height();
            
            //(worldHeight * EARTH_RADIUS_IN_METERS)/windowHeight)
            
//            bool dontDraw = false;
            //if (length < 30) {
            //    dontDraw = true;
            //}
            
//            int textLength = get_text_length(stName) + 25;
//            double conversionFactor = windowHeight/(worldHeight * EARTH_RADIUS_IN_METERS);
            //std::cout << "conversionFactor = " << conversionFactor << std::endl;
            
            //if((length * conversionFactor) > textLength) {
                
//                for (unsigned i = 0; i < pointList.size()-1; i++) {
                    

                    //Draws the street segment's name
//                   if (stName != "<unknown>" && !dontDraw) {
//                        settextattrs(8, textAngles[i]);
//                        setcolor(100, 100, 100);
                        /*
                        double xBound = 0.000003;
                        double yBound = 0.0001;
                        if (textAngles[i] <= 45 && textAngles[i] >= -45) {
                            xBound = 0.0001;
                            yBound = 0.000003;
                            //stName.append(" CAUGHTIT ");
                        }*/
 //                       if(((lengthSegments[i] * conversionFactor) > textLength)&&((lengthSegments[i] * conversionFactor) > 30)) {
                            //std::cout << "length in px = " << lengthSegments[i] * (windowHeight/(worldHeight * EARTH_RADIUS_IN_METERS)) << std::endl;
                            //std::cout << "text length in px = " << get_text_length(stName) << std::endl;
                            //if((lengthSegments[i] * conversionFactor) > 30) {
                                //drawtext(((projectedList[i].first + projectedList[i+1].first)/2), ((projectedList[i].second + projectedList[i+1].second)/2), stName);
                                //if(!oneWay)
//                                    drawtext(((projectedList[i].first + projectedList[i+1].first)/2), ((projectedList[i].second + projectedList[i+1].second)/2), stName);
                                /*
                                else {
                                    //intersection inter = streetNet::getInstance()->getIntersecByID(from);
                                    //inter.location;

                                    if(pointList[0].lon() < pointList[1].lon() && !flag) {
                                        stName.append(" ->");
                                        flag = true;
                                    }
                                    else if(!flag) {
                                        stName.append(" <-");
                                        flag = true;
                                    }
                                    drawtext(((projectedList[i].first + projectedList[i+1].first)/2), ((projectedList[i].second + projectedList[i+1].second)/2), stName);
                                }*/
                            //}
//                        }
//                    }
//                }
            //}
        //}
            
            
            
            
            /*
            std::vector<std::pair<double,unsigned>> largestSegments = streetNet::getInstance()->getLargestSegments(streetID);
            
            //prints up to 5 largest segments
            for (unsigned i = 0; (i < largestSegments.size()) && (i < 5) && (streetID!=0); i++){
                streetSegment thisSeg = streetNet::getInstance()->getSegByID(largestSegments[i].second);
                if (thisSeg.segID == this->segID) {
                              
                    std::cout << "i = " << i << std::endl;
                    std::cout << "largestSegment = " << thisSeg.segID << std::endl;
                    auto whichIt = std::max_element(thisSeg.lengthSegments.begin(), thisSeg.lengthSegments.end());
                    unsigned whichPointer = std::distance(thisSeg.lengthSegments.begin(), whichIt);
                    std::cout << "whichPointer = " << whichPointer << std::endl;
                    std::cout << "length = " << lengthSegments[whichPointer] << std::endl;
                    bool dontDraw = false;
                    if (length < 50 || get_text_length(stName) > 100) {
                        dontDraw = true;
                    }

                    //Draws the street segment's name
                    if (stName != "<unknown>" && !dontDraw) {
                        settextattrs(8, textAngles[whichPointer]);
                        setcolor(100, 100, 100);
                        double xBound = 0.00001;
                        double yBound = 0.0001;
                        if (textAngles[whichPointer] <= 45 && textAngles[whichPointer] >= -45) {
                            xBound = 0.0001;
                            yBound = 0.00001;
                        }
                        drawtext(((projectedList[whichPointer].first + projectedList[whichPointer+1].first)/2),
                                ((projectedList[whichPointer].second + projectedList[whichPointer+1].second)/2), stName, xBound, yBound);
                    }
                    //std::cout << "streetID= " << streetID << std::endl;
                    //std::cout << " i = " << i << " type = " << exInfo1 << std::endl;
                    //std::cout << "largest segments are = " << largestSegments[i].second << " with the length of " << largestSegments[i].first << std::endl;
                }
            }*/
        
            //}
        //}
//    }
    
    if (length < 30) {
        return;
    }
    
    std::string stName = streetNet::getInstance()->getStreetName(streetID);
    if (get_visible_world().get_height() > 0.0004 || stName == "<unknown>"){
        return;
    }
    else if (get_visible_world().get_height() > 0.00009) {
        
        int windowHeight = -get_visible_screen().get_height();
        double worldHeight = get_visible_world().get_height();
        double conversionFactor = windowHeight/(worldHeight * EARTH_RADIUS_IN_METERS);
        
        for (unsigned i = 0; i < pointList.size()-1; i++) {
                    
            //Draws the street segment's name
            if (!(lengthSegments[i] < 30)) {
                settextattrs(8, textAngles[i]);
                setcolor(100, 100, 100);
                
                if((lengthSegments[i] * conversionFactor) > get_text_length(stName)*3) {
                    //std::cout << "lengthSegments[i] * conversionFactor =" << lengthSegments[i] * conversionFactor << std::endl;
                    //std::cout << "conversionFactor = " << conversionFactor << std::endl;
                    drawtext(((projectedList[i].first + projectedList[i+1].first)/2), ((projectedList[i].second + projectedList[i+1].second)/2), stName);
                }
            }
        }
    }
    else {
        
        int windowHeight = -get_visible_screen().get_height();
        double worldHeight = get_visible_world().get_height();
        double conversionFactor = windowHeight/(worldHeight * EARTH_RADIUS_IN_METERS);
        
        for (unsigned i = 0; i < pointList.size()-1; i++) {

            //Draws the street segment's name
            if (!(lengthSegments[i] < 30)) {
                settextattrs(8, textAngles[i]);
                setcolor(100, 100, 100);
                
                bool flag = false;
                if(!oneWay)
                {
                    drawtext(((projectedList[i].first + projectedList[i+1].first)/2), ((projectedList[i].second + projectedList[i+1].second)/2), stName);
                }
                else {
                    if(pointList[0].lon() < pointList[1].lon() && !flag) {
                        stName.append(" ->");
                        flag = true;
                    }
                    else if(!flag) {
                        stName.append(" <-");
                        flag = true;
                    }
                    if(((lengthSegments[i] * conversionFactor) > get_text_length(stName)*1.75)) {
                        drawtext(((projectedList[i].first + projectedList[i+1].first)/2), ((projectedList[i].second + projectedList[i+1].second)/2), stName);
                    }
                }
            }  
        }
    }
}


//this is a helper function calculater distance betweent two point on Earth
//Unit: Meter
//the following code comes from the internet by the Haversine formula
//it is very very hard to undersdand
double distance_on_Earth(LatLon point1, LatLon point2) {
  double lat1r, lon1r, lat2r, lon2r, u, v;
  lat1r = point1.lat() * DEG_TO_RAD;
  lon1r = point1.lon() * DEG_TO_RAD;
  lat2r = point2.lat() * DEG_TO_RAD;
  lon2r = point2.lon() * DEG_TO_RAD;
  u = sin((lat2r - lat1r)/2);
  v = sin((lon2r - lon1r)/2);
  return 2.0 * EARTH_RADIUS_IN_METERS * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
}
