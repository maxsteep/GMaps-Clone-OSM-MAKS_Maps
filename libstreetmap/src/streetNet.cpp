#include "streetNet.h"
#include "graphics.h"
#include "mappinglib.h"
#include <math.h>
#include <queue>
#include <unordered_set>
#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/assert.hpp>
#include <algorithm>
#include <parallel/algorithm>   //new
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

//#define profile
//#define drawperf
//#define legality
//#define legalityInner

using namespace std;


streetNet* streetNet::thisObjPtr = NULL;

//Master class holding/loading all the data
streetNet::streetNet() {
    if (thisObjPtr != NULL) {
        delete thisObjPtr;
    }
    thisObjPtr = this;
}

//Master class holding/loading all the data
streetNet::~streetNet() {
    thisObjPtr = NULL;
}

streetNet* streetNet::getInstance() {
    if (thisObjPtr == NULL) {
        new streetNet();
    }
    return thisObjPtr;
}


//Populates the data structure with all the necessary data to avoid expensive level 2 API calls during user calls
void streetNet::load_data() {
    unsigned stSegNum = getNumberOfStreetSegments();    //speed
    unsigned stNum = getNumberOfStreets();  //speed
    unsigned intNum = getNumberOfIntersections();   //speed
    unsigned POINum = getNumberOfPointsOfInterest();    //speed
    unsigned FeatNum = getNumberOfFeatures();   //speed
    
    //ICON set-up START
    Surface poiicon("/homes/x/xuwenkai/ECE297/mapper/stop.png");
    POIIcon = poiicon;
    Surface subicon("/homes/x/xuwenkai/ECE297/mapper/train.png");
    SubIcon = subicon;
    Surface stcarticon("/homes/x/xuwenkai/ECE297/mapper/tram-front-view.png");
    StCartIcon = stcarticon;
    Surface busicon("/homes/x/xuwenkai/ECE297/mapper/busicon.png");
    BusIcon = busicon;
    Surface restauranticon("/homes/x/xuwenkai/ECE297/mapper/restaurant-cutlery-interface-symbol-in-a-circle.png");
    RestaurantIcon = restauranticon;
    Surface funicon("/homes/x/xuwenkai/ECE297/mapper/dancer-pose.png");
    FunIcon = funicon;
    Surface hospitalicon("/homes/x/xuwenkai/ECE297/mapper/hospital.png");
    HospitalIcon = hospitalicon;
    Surface nightlifeicon("/homes/x/xuwenkai/ECE297/mapper/wine-glass.png");
    NightLifeIcon = nightlifeicon;
    Surface cafeicon("/homes/x/xuwenkai/ECE297/mapper/hot-cup-of-coffee.png");
    CafeIcon = cafeicon;
    Surface bankicon("/homes/w/weiyuyao/mapper_repo/mapper/dollar-symbol.png");
    BankIcon = bankicon;
    Surface libraryicon("/homes/w/weiyuyao/mapper_repo/mapper/open-book.png");
    LibraryIcon = libraryicon;
    Surface policeicon("/homes/w/weiyuyao/mapper_repo/mapper/policeman.png");
    PoliceIcon = policeicon;
    Surface parkingicon("/homes/w/weiyuyao/mapper_repo/mapper/parking (1).png");
    ParkingIcon = parkingicon;
    Surface schoolicon("/homes/w/weiyuyao/mapper_repo/mapper/graduation-hat.png");
    SchoolIcon = schoolicon;
    Surface gasicon("/homes/w/weiyuyao/mapper_repo/mapper/gas-station.png");
    GasIcon = gasicon;
    Surface atmicon("/homes/w/weiyuyao/mapper_repo/mapper/cash-machine.png");
    AtmIcon = atmicon;
    Surface theatreicon("/homes/w/weiyuyao/mapper_repo/mapper/video-player-sign.png");
    TheatreIcon = theatreicon;
    Surface pharmacyicon("/homes/w/weiyuyao/mapper_repo/mapper/pills.png");
    PharmacyIcon = pharmacyicon;
    //ICON set-up END   
    
    //set up nodeMasterTable here **********************************************
    for (unsigned i = 0; i < getNumberOfNodes(); i ++) {
        nodeMasterTable[getNodeByIndex(i)->id()] = getNodeByIndex(i);
        
        //here we parse all OSMNode
        const OSMNode* newNode = getNodeByIndex(i);
        for (unsigned n = 0; n < getTagCount(newNode); ++ n) {
            std::pair<std::string, std::string> tag1 = getTagPair(newNode, n);
            if (tag1.first.compare("railway") == 0 && tag1.second.compare("station") == 0) {
                subwayStation newstation(i);
                subStList.emplace_back(newstation);
            }  
            if (tag1.first.compare("railway") == 0 && tag1.second.compare("tram_stop") == 0) {
                streetCartStop newstop(i);
                stCartStopList.emplace_back(newstop);
            }  
            if (tag1.first.compare("highway") == 0 && tag1.second.compare("bus_stop") == 0) {
                busStop newstop(i);
                busStopList.emplace_back(newstop);
            }  
        }
    }
    
    
    
    //set up intersecMasterList ************************************************
    //we have to set up this list first because other objects in this data base 
    //will need the InterAveLat to set up
    intersecMasterList.reserve(intNum);   //speed
    
    double maxInterLat = -180;   //these are used to calculate the average lat in the given map
    double minInterLat = 180;
    double maxInterLon = -360;
    double minInterLon = 360;
    
//    double sumLat = 0;
    for (unsigned l = 0; l < intNum; l++) {
        intersection newIntersec(l);
        intersecMasterList.emplace_back(newIntersec);
        sumIntLat += newIntersec.location.lat();
        double newLat = newIntersec.location.lat();
        double newLon = newIntersec.location.lon();
        if (newLat > maxInterLat) maxInterLat = newLat;
        if (newLat < minInterLat) minInterLat = newLat;
        if (newLon > maxInterLon) maxInterLon = newLon;
        if (newLon < minInterLon) minInterLon = newLon;
    }
//    InterAveLat = cos(((sumIntLat) / intNum) * DEG_TO_RAD);
    InterAveLat = cos(((maxInterLat + minInterLat)/2) * DEG_TO_RAD);
    
    //setting up proj lists for nanoflann
    
    //slower than that we had, insignificantly however
    for (unsigned i = 0; i < intNum; i++) {
        intersecMasterList[i].projPt.first = (getIntersectionPosition(i).lon() * DEG_TO_RAD) * InterAveLat;
        intersecMasterList[i].projPt.second = getIntersectionPosition(i).lat() * DEG_TO_RAD;
    }
    
    rangeLL = {minInterLat, minInterLon};
    rangeTR = {maxInterLat, maxInterLon};

    intersecMatrix.resize(stNum);   //speed
    for (unsigned q = 0; q < intNum; q ++) {
        std::vector<unsigned> stList = intersecMasterList[q].street_ID_list();
        for (unsigned r = 0; r < stList.size(); r ++) {
            intersecMatrix[stList[r]].emplace_back(q);
        }
    }
    
    
    //all the OSMWay gets parsed here into ferryList, subwayList, stCartList
    for (unsigned f = 0; f < getNumberOfWays(); f ++) {
        const OSMWay* newway = getWayByIndex(f);
        for (unsigned g = 0; g < getTagCount(newway); g ++) {
            if (getTagPair(newway, g).first.compare("route") == 0 
                    && getTagPair(newway, g).second.compare("ferry") == 0) {
                ferryWay newFerry(f);
                ferryList.emplace_back(newFerry);
            }
            if (getTagPair(newway, g).first.compare("railway") == 0 
                    && getTagPair(newway, g).second.compare("subway") == 0) {
                subway newSubway(f);
                subwayList.push_back(newSubway);
            }
            if (getTagPair(newway, g).first.compare("railway") == 0 
                    && getTagPair(newway, g).second.compare("tram") == 0) {
                streetCart newStCart(f);
                stCartList.push_back(newStCart);
            }
        }
    }

    
    //set up segMasterList *****************************************************
    //populates the map to avoid the outrageously costly string comparison
    std::map<std::string, roadTypes> roadTypesMap = boost::assign::map_list_of("motorway", motorway)("trunk", trunk)("primary",primary)("secondary", secondary)
                                                                                ("motorway_link", motorway_link)("trunk_link",trunk_link)
                                                                                ("primary_link",primary_link)("secondary_link",secondary_link)
                                                                                ("tertiary",tertiary)("tertiary_link",tertiary_link)
                                                                                ("residential", residential)("unclassified", unclassified)
                                                                                ("service", service)("path", path)("footway", footway)
                                                                                ("cycleway", cycleway)("construction", construction)("track",track)("steps", steps)
                                                                                ("bridleway", bridleway)("living_street", living_street)("pedestrian",pedestrian)("bus_guideway", bus_guideway)
                                                                                ("escape", escape)("raceway", raceway)("road",road);       
                                                                                
    //set up segInfoTable
    //this table are used for streetSegment constructing, so it need to be set
    //up before segMasterlist
    //segInfoTable.reserve(getNumberOfWays());
    for (unsigned v = 0; v < getNumberOfWays(); v ++) {
        const OSMWay* newWay = getWayByIndex(v);
        if (getTagCount(newWay) >= 2) {
            for (unsigned w = 0; w < getTagCount(newWay); w ++) {   //for (unsigned w = 0; w < getTagCount(newWay); w ++) { - swap with 2
                if (getTagPair(newWay, w).first.compare("highway") == 0) {
                    segInfoTable[newWay->id()] = roadTypesMap[getTagPair(newWay, w).second];   
                    //we only care about tag highway for now
                    //std::cout << "getTagPair(newWay, w).second = " << getTagPair(newWay, w).second << std::endl;
                    //std::cout << "segInfoTable[newWay->id()] = " << segInfoTable[newWay->id()] << std::endl;
                    //std::cout << "roadTypesMap[getTagPair(newWay, w).second] = " << roadTypesMap[getTagPair(newWay, w).second] << std::endl;
                }
            }
        }
    }

    
    //segMasterList
    segMasterList.reserve(stSegNum);    //speed
    for (unsigned i = 0; i < stSegNum; i ++) {
        streetSegment newSeg(i);
        segMasterList.emplace_back(newSeg);
        
        for (unsigned j = 0; j < segMasterList[i].projectedList.size(); j++) {
            coordSegMasterListSIZE++;
        };
        
        //set up segNet   
        segNet.resize(stSegNum);    //have to initialize before addressing per index
        //segNet[segMasterList[i].streetID].resize(stSegNum);
        segNet[segMasterList[i].streetID].emplace_back(i);
        
        //set up segTimeList
        segTimeList.reserve(stSegNum);    //speed
        double time = segMasterList[i].travelTime;
        segTimeList.emplace_back(time);
    }

    //can move into segMasterList setup, but cant reserve    
    segSegStNet.reserve(coordSegMasterListSIZE);
    for (size_t k = 0; k < segMasterList.size(); k++)
    {
        for(size_t j = 0; j < segMasterList[k].projectedList.size(); j++) {
            segSegStNet.emplace_back(std::make_pair(std::make_pair(segMasterList[k].projectedList[j].first, segMasterList[k].projectedList[j].second), segMasterList[k].segID));
        }
    }


    
    //I will return to this and re-implement it for prod
    /*
    segListLengths.resize(stSegNum);
    for (unsigned i = 0; i < stSegNum; i ++) {
        //std::vector<std::vector<std::pair<double,unsigned>>> segListLengths;
        if(segMasterList[i].streetID != 0){
            //std::vector<unsigned> segList = segNet[streetID];
            

            //unsigned segListSize = segList.size();  //speed
            //std::vector<std::pair<double,unsigned>> segListLengths;
            //segListLengths.reserve(segListSize);
            for (unsigned j = 0; j < segNet[segMasterList[j].streetID].size(); j++) {
                //std::vector<std::pair<double, unsigned>> tempVec;
                //tempVec.reserve(segNet[segMasterList[j].streetID].size());
                std::pair<double, unsigned> newpair;
                newpair.first = segMasterList[j].length;
                newpair.second = segMasterList[j].segID;
                //tempVec.emplace_back(newpair);
                
                segListLengths[segMasterList[j].streetID].emplace_back(newpair);
        }

        //std::sort(segListLengths.begin(), segListLengths.end(), [](streetSegment & one, streetSegment & two){return one.length < two.length;});
        std::sort(segListLengths[segMasterList[i].streetID].rbegin(), segListLengths[segMasterList[i].streetID].rend());
        }
    }*/

    
    //set up streetMasterList **************************************************
    streetMasterList.reserve(stNum);    //speed
    for (unsigned j = 0; j < stNum; j++) {
        street newStreet(j);
        streetMasterList.emplace_back(newStreet);
        
        //set up streetTableByName
        if (streetTableByName.find(streetMasterList[j].name) == streetTableByName.end()) {
            std::vector<unsigned> newList;
            newList.emplace_back(streetMasterList[j].streetID);
            streetTableByName[streetMasterList[j].name] = newList;
        }
        else {
            streetTableByName.find(streetMasterList[j].name)->second.emplace_back(streetMasterList[j].streetID);
        }
    }
    
    
    //set up the KD for NN, spatial hashing START *****************************************************
    //Necessary to use the find_closest_intersection functionality for a member field in the POI structure
    //set up the coordinate clouds for the intersection network
    loadPointCloud(cloudInter, intNum, 'i');
    //set up the kd-tree structures and indices for the nearest neighbour search
    //for the intersection network
    loadKDTreeIndex(KDTreeInter);
    //END set up the KD for NN, spatial hashing END *****************************************************
    
    
    //set up the POIMasterList *************************************************
    POIMasterList.reserve(POINum);  //speed
    double maxPOILat = -180;    //these are used to calculate the average lat in the given map
    double minPOILat = 180;
//    double sumPOIlat = 0;
    for (unsigned n = 0; n < POINum; n ++) {
        POI newPOI(n);
        POIMasterList.emplace_back(newPOI);
        sumPOILat += newPOI.location.lat();
        if (newPOI.location.lat() > maxPOILat) maxPOILat = newPOI.location.lat();
        if (newPOI.location.lat() < minPOILat) minPOILat = newPOI.location.lat();
    }
    //POIAveLat = cos(((sumPOILat) / POINum) * DEG_TO_RAD);
    POIAveLat = cos(((maxPOILat + minPOILat)/2) * DEG_TO_RAD);
    
    //POINameTable
    for (unsigned y = 0; y < POIMasterList.size(); ++ y) {
        std::string poiname = POIMasterList[y].name;
        if (POINameTable.find(poiname) == POINameTable.end()) {
            std::vector<unsigned> newpoivec;
            newpoivec.push_back(y);
            POINameTable[poiname] = newpoivec;
        }
        else {
            POINameTable[poiname].push_back(y);
        }
    }
    
    
    //set up the featureMasterList *********************************************
    featureMasterList.reserve(FeatNum);
    for (unsigned s = 0; s < FeatNum; s ++) {
        feature newFeature(s);
        featureMasterList.emplace_back(newFeature);
        for (unsigned i = 0; i < featureMasterList[s].projectedList.size(); i++) {
            coordSegFeatListSIZE++;
        }
    }
    
    //set up the hash table featureTableByName
    for (unsigned t = 0; t < featureMasterList.size(); t++) {
        std::string fname = featureMasterList[t].name;
        if (fname != "<noname>") {
            if (featureTableByName.find(fname) == featureTableByName.end()) {
                std::vector<unsigned> newVec;
                newVec.emplace_back(t);
                featureTableByName[fname] = newVec;
            }
            else {
                featureTableByName.find(fname)->second.emplace_back(t);
            }
        }
        //set up featureSizeTree and featureAreas
        std::pair<double, unsigned> newpair(featureMasterList[t].area, t);
        featureAreas.emplace_back(newpair);
    }

    std::sort(featureAreas.rbegin(), featureAreas.rend());
    
    segFeatNet.reserve(coordSegFeatListSIZE);
    for (size_t k = 0; k < featureAreas.size(); k++)
    {
        for(size_t j = 0; j < featureMasterList[featureAreas[k].second].projectedList.size(); j++) {
            segFeatNet.emplace_back(std::make_pair(
                                    std::make_pair(
                    featureMasterList[featureAreas[k].second].projectedList[j].first,
                    featureMasterList[featureAreas[k].second].projectedList[j].second),
                    featureMasterList[featureAreas[k].second].featureID));
        }
    }
    
    pseudoHash.resize(intNum);
    //counter.resize(intNum, 0);
    Pcounter.resize(intNum, 0);
    Dcounter.resize(intNum, 0);
    //std::cout << "counter[37] = " << counter[37] << std::endl;
    
    //set up the KD for NN, spatial hashing START *****************************************************
    
    //set up the coordinate clouds for the segment, POI and Feature networks respectively

    loadPointCloud(cloudPOI, POINum, 'p');
    loadPointCloud(cloudSegs, coordSegMasterListSIZE, 's');
    loadPointCloud(cloudFeat, coordSegFeatListSIZE, 'f');

    
    //set up the kd-tree structures and indices for the nearest neighbour search
    //for the segment, POI and Feature networks respectively

    loadKDTreeIndex(KDTreePOI);
    loadKDTreeIndex(KDTreeSegs);
    loadKDTreeIndex(KDTreeFeat);
    
    //END set up the KD for NN, spatial hashing END *****************************************************
    
    commonKeyWord.push_back("toronto");
    commonKeyWord.push_back("newyork");
    commonKeyWord.push_back("beijing");
    commonKeyWord.push_back("tokyo");
    commonKeyWord.push_back("tehran");
    commonKeyWord.push_back("sydney");
    commonKeyWord.push_back("singapore");
    commonKeyWord.push_back("saint helena");
    commonKeyWord.push_back("rio de janeiro");
    commonKeyWord.push_back("new delhi");
    commonKeyWord.push_back("moscow");
    commonKeyWord.push_back("london");
    commonKeyWord.push_back("hong kong");
    commonKeyWord.push_back("iceland");
    commonKeyWord.push_back("hamilton");
    commonKeyWord.push_back("golden-horseshoe");
    commonKeyWord.push_back("cairo");
    commonKeyWord.push_back("capetown");
    commonKeyWord.push_back("zoom in");
    commonKeyWord.push_back("zoom out");
    commonKeyWord.push_back("zoom fit");
    commonKeyWord.push_back("proceed");
    commonKeyWord.push_back("save");
    commonKeyWord.push_back("exit");
    commonKeyWord.push_back("quit"); 
    
    
}


//Returns the average latitude for all the intersections in the given data set 
double streetNet::getInterAveLat() {
    return InterAveLat;
}


//Returns the average latitude for all the POIs in the given data set
double streetNet::getPOIAveLat() {
    return POIAveLat;
}


std::pair<double, double> streetNet::getRange(bool LL, bool TR) {
    if (LL) return rangeLL;
    else if (TR) return rangeTR;
    else {
        std::pair<double, double> zero(0, 0);
        return zero;
    }
}


//Returns segment through ID using custom data structures
streetSegment streetNet::getSegByID(unsigned segID) {
    return segMasterList[segID];
}


//Returns street through ID using custom data structures
street streetNet::getStreetByID(unsigned stID) {
    return streetMasterList[stID];
}


//Returns list of street ID by its name
std::vector<unsigned> streetNet::getStreetIDByName(std::string name) {
    return streetTableByName[name];
}


intersection streetNet::getIntersecByID(unsigned interID) {
    return intersecMasterList[interID];
}


POI streetNet::getPOIByID(unsigned POIID) {
    return POIMasterList[POIID];
}


std::vector<unsigned> streetNet::findPOIbyName(std::string name) {
    return POINameTable[name];
}


unsigned streetNet::closestPOIbyName(std::string name, LatLon location) {
    std::vector<unsigned> poilist = POINameTable[name];
    double mindis = 999999;
    unsigned closestPOI = 0xffffffff;
    
    for (unsigned i = 0; i < poilist.size(); ++ i) {
        double currentDis = distance_between_points(location, POIMasterList[poilist[i]].location);
        if (currentDis < mindis) {
            mindis = currentDis;
            closestPOI = poilist[i];
        }
    }
    return closestPOI;
}


std::string streetNet::getStreetName(unsigned stID) {
    return streetMasterList[stID].name;
}


//Returns a list of segment ID of segments connected to the given intersection
std::vector<unsigned> streetNet::getSegByIntersec(unsigned intersecID) {
    return intersecMasterList[intersecID].segList;
}


//Returns a list of street ID of streets connecting in the given intersection
std::vector<std::string> streetNet::getStreetByIntersection(unsigned intersecID) {
    std::vector<std::string> stList;
    unsigned listSize = intersecMasterList[intersecID].street_ID_list().size();
    
    for (unsigned i = 0; i < listSize; i ++) {
        stList.emplace_back(streetMasterList[intersecMasterList[intersecID].street_ID_list()[i]].name);
    }
    
    return stList;
}


//Checks whether the given intersections are directly connected
bool streetNet::intersecDirectlyConected(unsigned intersecID1, unsigned intersecID2) {
    unsigned segList1Size = intersecMasterList[intersecID1].segList.size();
    
    //check for corner case where ID1 and ID2 are the same
    if (intersecID1 == intersecID2)
        return true;
    
    //normal case
    for (unsigned j = 0; j < segList1Size; j ++) {
        if (segMasterList[intersecMasterList[intersecID1].segList[j]].to == intersecID2)
            return true;
        else if (segMasterList[intersecMasterList[intersecID1].segList[j]].from
                == intersecID2 
                && !segMasterList[intersecMasterList[intersecID1].segList[j]].oneWay)
                    return true;
    }
    return false;
}


//Returns a list of intersections that are directly connected
std::vector<unsigned> streetNet::listIntersecDirectlyConected(unsigned intersecID) {
    std::vector<unsigned> interList;
    unsigned segListSize = intersecMasterList[intersecID].segList.size();
    
    for (unsigned j = 0; j < segListSize; j ++) {
        if (segMasterList[intersecMasterList[intersecID].segList[j]].to  != intersecID)
            interList.emplace_back(segMasterList[intersecMasterList[intersecID].segList[j]].to);
        else if (segMasterList[intersecMasterList[intersecID].segList[j]].from
                != intersecID 
                && !segMasterList[intersecMasterList[intersecID].segList[j]].oneWay)
                    interList.emplace_back(segMasterList[intersecMasterList[intersecID].segList[j]].from);
    }
    
    //the following code re-orders the list into unique list
    std::sort(interList.begin(), interList.end());
    interList.erase(std::unique(interList.begin(), interList.end()), interList.end());
    
    return interList;
}


//Returns a list of segments making up the given street
std::vector<unsigned> streetNet::getSegInStreet(unsigned streetID) {
    return segNet[streetID];
}


//Returns a list of intersections for any given street 
std::vector<unsigned> streetNet::getIntersecInStreet(unsigned streetID) {
    std::vector<unsigned> interList = intersecMatrix[streetID];

    //the following code re-orders the list into unique list
    std::sort(interList.begin(), interList.end());
    interList.erase(std::unique(interList.begin(), interList.end()), interList.end());
    
    return interList;
}


//find one intersection by two street IDs
std::vector<unsigned> streetNet::getIntersecByStreets(unsigned stID1, unsigned stID2) {
    std::vector<unsigned> intersecList;
    std::vector<unsigned> st1InterList = this->getIntersecInStreet(stID1);
    std::vector<unsigned> st2InterList = this->getIntersecInStreet(stID2);
    
    unsigned i = 0;
    unsigned j = 0;
    
    unsigned stInter1Num = st1InterList.size(); //speed
    unsigned stInter2Num = st2InterList.size(); //speed
    
    while (i < stInter1Num && j < stInter2Num) {
        if (st1InterList[i] == st2InterList[j]) {
            intersecList.emplace_back(st1InterList[i]);
            i++;
            j++;
        }
        else {
            if (st1InterList[i] > st2InterList[j])
                j++;
            else 
                i++;
        }
    }

    return intersecList;
}


//find one or more intersections by two street names
std::vector<unsigned> streetNet::getIntersecByStName(std::string stName1, std::string stName2) {
    std::vector<unsigned> IDList;
    
    std::vector<unsigned> stList1 = streetTableByName[stName1];
    std::vector<unsigned> stList2 = streetTableByName[stName2];
    
    unsigned stList1Size = stList1.size();  //speed
    unsigned stList2Size = stList2.size();  //speed
    
    for (unsigned i = 0; i < stList1Size; i ++) {
        for (unsigned j = 0; j < stList2Size; j ++) {
            std::vector<unsigned> newID = this->getIntersecByStreets(stList1[i], stList2[j]);
            if (newID.size() != 0){
                for (unsigned k = 0; k < newID.size(); k ++) {
                    IDList.push_back(newID[k]);
                }
            }
        }
    }

    return IDList;
    
}


std::vector<unsigned> streetNet::getFeatureByName(std::string name_) {
    return featureTableByName[name_];
}


feature streetNet::getFeasture(unsigned id) {
    return featureMasterList[id];
}


//Returns length of a given segment
double streetNet::getSegLength(unsigned segID) {
    return segMasterList[segID].length;
}


//Returns length of a given street
double streetNet::getStreetLength(unsigned streetID_) {
    std::vector<unsigned> segList = segNet[streetID_];
    double length = 0;
    
    unsigned segListSize = segList.size();  //speed
    
    for (unsigned i = 0; i < segListSize; i ++) {
        length += segMasterList[segList[i]].length;
    }
    return length;
}


//Returns the travel time for the given segment
double streetNet::getTravelTime(unsigned segID) {
    return segTimeList[segID];
}

unsigned streetNet::findSegByInter(unsigned inter0, unsigned inter1) {
    double mintime = 999999;
    unsigned fastSeg = 0xffffffff;
    std::vector<unsigned> segList = intersecMasterList[inter0].segList;
    for (unsigned i = 0; i < segList.size(); ++ i) {
        if (!segMasterList[segList[i]].oneWay || segMasterList[segList[i]].from == inter0) {
            if (segMasterList[segList[i]].from == inter1
                    || segMasterList[segList[i]].to == inter1) {
                if (segMasterList[segList[i]].travelTime < mintime) {
                    mintime = segMasterList[segList[i]].travelTime;
                    fastSeg = segList[i];
                }
            }
        }
    }
    
    return fastSeg;
}


//I will return to this and re-implement it for prod
/*
std::vector<std::pair<double,unsigned>> streetNet::getLargestSegments(unsigned streetID) {
    
    if(streetID!=0){
        std::vector<unsigned> segList = segNet[streetID];

        unsigned segListSize = segList.size();  //speed
        std::vector<std::pair<double,unsigned>> segListLengths;
        //segListLengths.reserve(segListSize);
        for (unsigned i = 0; i < segListSize; i++) {
            std::pair<double, unsigned> newpair;
            newpair.first = segMasterList[segList[i]].length;
            newpair.second = segList[i];
            segListLengths.emplace_back(newpair);
        }

        //std::sort(segListLengths.begin(), segListLengths.end(), [](streetSegment & one, streetSegment & two){return one.length < two.length;});
        std::sort(segListLengths.rbegin(), segListLengths.rend());
    
        return segListLengths;
    }
    else {
        //std::pair<double, unsigned> newpair;
        //newpair = std::make_pair(0,0);
        //std::vector<std::pair<double,unsigned>> Empty;
        //Empty.emplace_back(newpair);
        return {};
    }
    
    return segListLengths[streetID];
}
*/

roadTypes streetNet::getSegInfo(OSMID segOSMID) {
    return segInfoTable[segOSMID];
}

//std::pair<std::string, std::string> streetNet::getSegInfo(OSMID segOSMID) {
//    return segInfoTable[segOSMID];
//}


const OSMNode* streetNet::getOSMNode(OSMID nodeOSMID) {
    return nodeMasterTable[nodeOSMID];
}


//NN start
//Populates the PointClouds with x,y coordinates derived from latlons
template <typename T>
void streetNet::loadPointCloud(PointCloud<T> &point, const size_t size, char type){
    
    //Initializes the vector so I can directly address it's contents. Equivalent to reserve and emplace_back.
    point.pts.resize(size);
    if (type == 'i') {
        for (size_t i=0; i < size; i++)
        {
            point.pts[i].x = intersecMasterList[i].projPt.first;
            point.pts[i].y = intersecMasterList[i].projPt.second;
            //point.pts[allelements].x = intersecMasterList[allelements].projPt.first; - is there a quicker way to do this?
        }
    }
    else if (type == 'p') {
        for (size_t i=0; i < size; i++)
        {
            point.pts[i].x = POIMasterList[i].projPt.first;
            point.pts[i].y = POIMasterList[i].projPt.second;
        }
    }
    else if (type == 'f') {
        for (size_t i=0; i < size; i++)
        {
            point.pts[i].x = segFeatNet[i].first.first;
            point.pts[i].y = segFeatNet[i].first.second;
        }
    }
    else if (type == 's') {
        for (size_t i = 0; i < size; i++) {
            point.pts[i].x = segSegStNet[i].first.first;
            point.pts[i].y = segSegStNet[i].first.second;
        }
    }
}

//Populates the PointClouds with x,y coordinates derived from latlons
template <typename T>
void streetNet::loadPointCloudS(PointCloud<T> &point, const size_t size, char type, const std::vector<DeliveryInfo>& deliveries, const std::vector<unsigned>& depots){
    
    //Initializes the vector so I can directly address it's contents. Equivalent to reserve and emplace_back.
    point.pts.resize(size*2);
    
    if (type == 's') {//dummy
        for (size_t i=0; i < size; i++)//dummy
        {//dummy
        }//dummy
    }
    else if (type == 'd') {//faster than with ifs?
        size_t j = 0;
        for (size_t i = 0; i < size; i++) {
            //0 and even are pickups ( 0 is considered even)
            point.pts[j].x = intersecMasterList[deliveries[i].pickUp].projPt.first;
            point.pts[j].y = intersecMasterList[deliveries[i].pickUp].projPt.second;
            //odd are dropoffs
            point.pts[j+1].x = intersecMasterList[deliveries[i].dropOff].projPt.first; //int division will give dropOff if odd
            point.pts[j+1].y = intersecMasterList[deliveries[i].dropOff].projPt.second;
            j+=2;
        }
    }
}


//Builds the kdtree index to iterate against
template <typename num_t>
void streetNet::loadKDTreeIndex(my_kd_tree_t<num_t> &index){
    index.buildIndex();
}


//Performs nearest neighbour search using kd-trees
unsigned streetNet::kdtree_NN(char objectType, double x, double y, LatLon my_position){
    double query_pt[2] = {x, y};
    //std::cout << "x = " << x << " y = " << y << std::endl;

    // ----------------------------------------------------------------
    // knnSearch():  Perform a search for the N closest points
    // Find the "num_closest" nearest neighbors to the query_point[0:dim-1]. Their indices are stored inside the result object.
    // ----------------------------------------------------------------
    
    

    if (objectType == 'i') {
        //N results code, N hardcoded to 2
        size_t num_results = 2;
        std::vector<size_t> ret_index(num_results);
        std::vector<double> out_dist_sqr(num_results);
        
        KDTreeInter.knnSearch(&query_pt[0], num_results, &ret_index[0], &out_dist_sqr[0]);
        
        //std::pair<double, unsigned> current = {DBL_MAX, 0};
        //for(unsigned i = 0; i < num_results; i++) {
        //    std::pair<double, unsigned> min = {find_distance_between_two_points(my_position, intersecMasterList[ret_index[i]].location), i};
        //    if (min.first < current.first) current = min;
            //std::cout << "min first = " << min.first << " min.second = " << min.second << std::endl;
            //std::cout << "current first = " << current.first << " current.second = " << current.second << std::endl;
        //}
        double a = find_distance_between_two_points(my_position, intersecMasterList[ret_index[0]].location);//distance_between_points   CHANGE
        double b = find_distance_between_two_points(my_position, intersecMasterList[ret_index[1]].location);//distance_between_points   CHANGE
        unsigned index = (a < b) ? 0 : 1;
        
        return ret_index[index];
        
        //return ret_index[0];
    }/*
    if (objectType == 's') {
        //N results code, N hardcoded to 20
        size_t num_results = 20;
        std::vector<size_t> ret_index(num_results);
        std::vector<double> out_dist_sqr(num_results);
        
        KDTreeD.knnSearch(&query_pt[0], num_results, &ret_index[0], &out_dist_sqr[0]);
        
        if ( ret_index[0] % 2 == 0) //even -> pickup
            unsigned a = ret_index[0];
        else    //->dropOff
            int a = ret_index[0]/2;
        if ( ret_index[1] % 2 == 0) //even
            unsigned b = ret_index[1];
        else    //->dropOff
            int b = ret_index[1]/2;
        
        unsigned index = (a < b) ? 0 : 1;
        
        return ret_index[index];
        
        //return ret_index[0];
    }*/
    else {
        //N results code, N hardcoded to 1
        size_t num_results = 1;
        std::vector<size_t> ret_index(num_results);
        std::vector<double> out_dist_sqr(num_results);
        
        KDTreePOI.knnSearch(&query_pt[0], num_results, &ret_index[0], &out_dist_sqr[0]);
        
        
        return ret_index[0];
    }
}

//Performs nearest neighbour search using kd-trees for radial closest neighbours
std::vector<unsigned> streetNet::radialNN(const char objectType, const double x, const double y, const double search_radius) {
    double query_pt[2] = {x, y};

    std::vector<std::pair<size_t, double> >   ret_matches; 
    
    //should reserve
    //ret_matches.reserve();
    
    nanoflann::SearchParams params;
    //If searchParams.sorted==true, the output list is sorted by ascending distances.
    params.sorted = false;
    
    //street segments
    if (objectType == 's') {
        const size_t nMatches = KDTreeSegs.radiusSearch(&query_pt[0],search_radius, ret_matches, params);
         
        std::vector<unsigned> segIDs;
        segIDs.reserve(nMatches);
        
        //auto const LegStart = std::chrono::high_resolution_clock::now();
        //#pragma omp parallel for
        for (size_t i = 0; i < nMatches; i++) {
            segIDs.emplace_back(segSegStNet[ret_matches[i].first].second);
        }
        
        //auto LegEnd = std::chrono::high_resolution_clock::now();
        //auto Leg =
        //    std::chrono::duration_cast<std::chrono::microseconds> (
        //    LegEnd - LegStart);
        //std::cout << "emplacing takes: " << Leg.count() << "us" << endl; 
        #ifdef drawperf
            auto const costStart = std::chrono::high_resolution_clock::now();
        #endif  
        unsigned segIDsSize = segIDs.size();
        std::unordered_set<unsigned> s;
        //#pragma omp parallel for //->throws error, use intel TBB?
        for (unsigned i = 0; i < segIDsSize; i++) {
            s.insert(segIDs[i]);
        }
        segIDs.assign( s.begin(), s.end() );
        //NOT NEEDED sort( segIDs.begin(), segIDs.end() ); //- not needed

        
        //__gnu_parallel::sort(segIDs.begin(), segIDs.end());
        //segIDs.erase(std::unique(segIDs.begin(), segIDs.end()), segIDs.end());
        #ifdef drawperf
            auto const costEnd = std::chrono::high_resolution_clock::now();
            auto const costTime = 
            std::chrono::duration_cast<std::chrono::milliseconds>(costEnd - costStart);
            std::cout << "uniquing: " << costTime.count() << "ms" << endl;
        #endif
        return segIDs;
    }
    
    //feature segments
    else if (objectType == 'f') {
        const size_t nMatches = KDTreeFeat.radiusSearch(&query_pt[0],search_radius, ret_matches, params);
         
        std::vector<unsigned> FeatIDs;
        FeatIDs.reserve(nMatches);
        __gnu_parallel::sort(ret_matches.begin(), ret_matches.end());//__gnu_parallel
        
        //#pragma omp parallel for
        for (size_t i = 0; i < nMatches; i++) {
            FeatIDs.emplace_back(segFeatNet[ret_matches[i].first].second);
        }
        #ifdef drawperf
            auto const costStart = std::chrono::high_resolution_clock::now();
        #endif    
        //SORT PRESERVING ORDER START
        //fast - anything but fast, 30 ms on 004 first toronto zoom level
        //no better solution exists to preserve order
        std::set<unsigned> valueExistsSet;

        auto newEnd = std::remove_if(FeatIDs.begin(), FeatIDs.end(), [&valueExistsSet](const unsigned& value)
        {
            //if (__gnu_parallel::find(valueExistsSet.begin(), valueExistsSet.end(), value) != std::end(valueExistsSet))
            //    return true;
            if (valueExistsSet.find(value) != std::end(valueExistsSet))
                return true;

            valueExistsSet.insert(value);
            return false;
        });

        FeatIDs.erase(newEnd, FeatIDs.end());
        #ifdef drawperf
            auto const costEnd = std::chrono::high_resolution_clock::now();
            auto const costTime = 
            std::chrono::duration_cast<std::chrono::milliseconds>(costEnd - costStart);
            std::cout << "uniquing feat: " << costTime.count() << "ms" << endl;
        #endif
        return FeatIDs;
    }
    
    //POIs
    else if (objectType == 'p') {
        const size_t nMatches = KDTreePOI.radiusSearch(&query_pt[0],search_radius, ret_matches, params);
         
        std::vector<unsigned> POI_IDs;
        POI_IDs.reserve(nMatches);
        for (size_t i = 0; i < nMatches; i++) {
            POI_IDs.emplace_back(POIMasterList[ret_matches[i].first].POIID);
        }
        //std::sort(POI_IDs.begin(), POI_IDs.end());
        //POI_IDs.erase(std::unique(POI_IDs.begin(), POI_IDs.end()), POI_IDs.end());
        
        return POI_IDs;
    } 
    
    //deliveries for salesman
    else if (objectType == 'd') {
        const size_t nMatches = KDTreeD.radiusSearch(&query_pt[0],search_radius, ret_matches, params);
         
        std::vector<unsigned> D_IDs;
        D_IDs.reserve(nMatches);
        for (size_t i = 0; i < nMatches; i++) {
            D_IDs.emplace_back(intersecMasterList[ret_matches[i].first].intersecID);
        }
        std::sort(D_IDs.begin(), D_IDs.end());
        D_IDs.erase(std::unique(D_IDs.begin(), D_IDs.end()), D_IDs.end());
        
        return D_IDs;
    } 
}

//NN end


void streetNet::drawAllSeg() {
    defaultWidth = round(((1/get_visible_world().get_height())/1800));
    HighwayWidth = round(((1/get_visible_world().get_height())/900)) + 2;
    HighwayLinkWidth = round(((1/get_visible_world().get_height())/900));
    primeryWidth = round(((1/get_visible_world().get_height())/900)) + 1;
    //std::cout<< "get_visible_world().get_height() = " << get_visible_world().get_height() << std::endl;
    
    //SWITCH TO CASE STATEMENTS
    if (get_visible_world().get_height() > 0.002){//0.007
        //auto const costStart = std::chrono::high_resolution_clock::now();
//#pragma omp parallel for
        for (unsigned i = 0; i < segMasterList.size(); i ++) {
            segMasterList[i].drawContour();
        } 
        //auto const costEnd = std::chrono::high_resolution_clock::now();
        //auto const costTime = 
        //std::chrono::duration_cast<std::chrono::milliseconds>(costEnd - costStart);
        //std::cout << "drawing all: " << costTime.count() << "ms" << endl;
    }/*
    else if (get_visible_world().get_height() > 0.004){//NEED TO OPTIMISE, doesn't make sense
        auto const costStart = std::chrono::high_resolution_clock::now();
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/450;
        resultsRadialNN = streetNet::radialNN('s', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        //std::cout << "seg count = " << segMasterList.size() << " radial count = " << resultsRadialNN.size() << std::endl;
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            segMasterList[resultsRadialNN[i]].drawContour();
        } 
        auto const costEnd = std::chrono::high_resolution_clock::now();
        auto const costTime = 
        std::chrono::duration_cast<std::chrono::milliseconds>(costEnd - costStart);
        std::cout << "drawing NN first zoom level: " << costTime.count() << "ms" << endl;
    }
    else if (get_visible_world().get_height() > 0.002){//NEED TO OPTIMISE
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/750;
        resultsRadialNN = streetNet::radialNN('s', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        std::cout << "seg count = " << segMasterList.size() << " radial count = " << resultsRadialNN.size() << std::endl;
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            segMasterList[resultsRadialNN[i]].drawContour();
        } 
    }*/
    else if (get_visible_world().get_height() > 0.001){
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/1250;
        resultsRadialNN = streetNet::radialNN('s', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            segMasterList[resultsRadialNN[i]].drawContour();
        } 
    }
    else if (get_visible_world().get_height() > 0.0009){
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/2000;
        resultsRadialNN = streetNet::radialNN('s', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            segMasterList[resultsRadialNN[i]].drawContour();
        } 
    }
    else if (get_visible_world().get_height() > 0.0005){
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/3500;
        resultsRadialNN = streetNet::radialNN('s', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            segMasterList[resultsRadialNN[i]].drawContour();
        } 
    }
    else if (get_visible_world().get_height() > 0.0003){
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/6000;
        resultsRadialNN = streetNet::radialNN('s', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            segMasterList[resultsRadialNN[i]].drawContour();
        } 
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {//to avoid overlap
            segMasterList[resultsRadialNN[i]].drawStName();
        }
    }
    else {
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/10000;
        //std::cout << "Radius = " << radius << std::endl;
        resultsRadialNN = streetNet::radialNN('s', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            segMasterList[resultsRadialNN[i]].drawContour();
        }
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {//to avoid overlap
            segMasterList[resultsRadialNN[i]].drawStName();
        }
    }  
    
    if (subwayON) {
        for (unsigned s = 0; s < subwayList.size(); s ++) {
            subwayList[s].drawContour();
        }
    }
    if (stCartON) {
        for (unsigned t = 0; t < stCartList.size(); t ++) {
            stCartList[t].drawContour();
        }
    }
    /*
    //draw street name
    if (get_visible_world().get_height() < 0.0005) {
//#pragma omp parallel for
        for (unsigned k = 0; k < resultsRadialNN.size(); ++k) {
            segMasterList[resultsRadialNN[k]].drawStName();
    //        std::thread t0(streetNet::segMasterList[k].drawStName, this);
    //        std::thread t1(streetNet::segMasterList[k+1].drawStName);

        }
    }*/
    
    /*
    //draw street name
    if (get_visible_world().get_height() < 0.0005) {
        for (unsigned k = 0; k < segMasterList.size(); ++k) {
            segMasterList[k].drawStName();
    //        std::thread t0(streetNet::segMasterList[k].drawStName, this);
    //        std::thread t1(streetNet::segMasterList[k+1].drawStName);

        }
    } 
    */
    
    if (get_visible_world().get_height() < 0.0008) {
        for (unsigned f = 0; f < ferryList.size(); f ++) {
            ferryList[f].drawContour();
            ferryList[f].drawFerryName();
        }
    }
}


void streetNet::drawFeature() {
    set_draw_mode(DRAW_NORMAL); // Should set this if your program does any XOR drawing in callbacks.
    clearscreen(); /* Should precede drawing for all drawscreens */
    
    /*
    //this draw everything by size except buildings
    for (unsigned i = 0; i < featureAreas.size(); ++ i) {
        featureMasterList[featureAreas[i].second].drawContour();
    }
    */
    
    //SWITCH TO CASE STATEMENTS
    if (get_visible_world().get_height() > 0.002){//> 0.006
        //this draw everything by size except buildings
        //auto const costStart = std::chrono::high_resolution_clock::now();
        for (unsigned i = 0; i < featureAreas.size(); ++ i) {
            featureMasterList[featureAreas[i].second].drawContour();
            //delay(5);
            //std::cout << "featureAreas[i].second = " << featureAreas[i].second << std::endl;
            //delay(25);
        }
        //auto const costEnd = std::chrono::high_resolution_clock::now();
        //auto const costTime = 
        //std::chrono::duration_cast<std::chrono::milliseconds>(costEnd - costStart);
        //std::cout << "drawing all feat: " << costTime.count() << "ms" << endl;
    }/*
    else if (get_visible_world().get_height() > 0.006){
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width());
        resultsRadialNN = streetNet::radialNN('f', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        std::cout << "6" << std::endl;
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            featureMasterList[resultsRadialNN[i]].drawContour();
        }
    }
    else if (get_visible_world().get_height() > 0.004){
        //auto const costStart = std::chrono::high_resolution_clock::now();
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/450;
        resultsRadialNN = streetNet::radialNN('f', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        //std::cout << "4" << std::endl;
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            featureMasterList[resultsRadialNN[i]].drawContour();
        }
        //auto const costEnd = std::chrono::high_resolution_clock::now();
        //auto const costTime = 
        //std::chrono::duration_cast<std::chrono::milliseconds>(costEnd - costStart);
        //std::cout << "drawing all feat NN: " << costTime.count() << "ms" << endl;
    }
    else if (get_visible_world().get_height() > 0.003){
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/300;
        resultsRadialNN = streetNet::radialNN('f', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            featureMasterList[resultsRadialNN[i]].drawContour();
        }
    }
    else if (get_visible_world().get_height() > 0.002){
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/500;
        resultsRadialNN = streetNet::radialNN('f', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            featureMasterList[resultsRadialNN[i]].drawContour();
        }
    }*/
    else if (get_visible_world().get_height() > 0.001){
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/1250;
        resultsRadialNN = streetNet::radialNN('f', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            featureMasterList[resultsRadialNN[i]].drawContour();
        }
    }
    else if (get_visible_world().get_height() > 0.0009){
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/2000;
        resultsRadialNN = streetNet::radialNN('f', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            featureMasterList[resultsRadialNN[i]].drawContour();
        }
    }
    else if (get_visible_world().get_height() > 0.0008){
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/1250;
        resultsRadialNN = streetNet::radialNN('f', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        //std::cout << "8" << std::endl;
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            featureMasterList[resultsRadialNN[i]].drawContour();
        }
    }
    else if (get_visible_world().get_height() > 0.0005){
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/3500;
        resultsRadialNN = streetNet::radialNN('f', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            featureMasterList[resultsRadialNN[i]].drawContour();
        }
    }
    else if (get_visible_world().get_height() > 0.0003){
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/4000;
        resultsRadialNN = streetNet::radialNN('f', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            featureMasterList[resultsRadialNN[i]].drawContour();
        }
    }
    else {
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/4000;
        //std::cout << "Radius = " << radius << std::endl;
        resultsRadialNN = streetNet::radialNN('f', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            featureMasterList[resultsRadialNN[i]].drawContour();
        }
    }
}


void streetNet::drawPOI() {
    
    if (get_visible_world().get_height() < 0.00003) {
        double radius = sqrt(get_visible_world().get_height()*get_visible_world().get_height() + get_visible_world().get_width()*get_visible_world().get_width())/10000;
        resultsRadialNN = streetNet::radialNN('p', get_visible_world().get_xcenter(), get_visible_world().get_ycenter(), radius);
        for (unsigned i = 0; i < resultsRadialNN.size(); i ++) {
            POIMasterList[resultsRadialNN[i]].drawPoint();
        }
    }
    
    /*
    if (get_visible_world().get_height() < 0.00003) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawPoint();
        }
    } 
    */
    
    if (restarantON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawRestaurant();
        }
    }
    
    if (funPlaceON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawFunPlace();
        }
    }
    
    if (icecreamON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawIceCream();
        }
    }
    
    if (cafeON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawCafe();
        }
    }
    
    if (dentistON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawDentist();
        }
    }
    
    if (hospitalON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawHospital();
        }
    }
    
    if (doctorsON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawDoctors();
        }
    }
    
    if (condoON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawCondo();
        }
    }
    
    if (clinicON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawClinic();
        }
    }
    
    if (pharmacyON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawPharmacy();
        }
    }
    
    if (parkingON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawParking();
        }
    }
    
    if (collegeON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawCollege();
        }
    }
    if (bankON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawBank();
        }
    }
    
    if (childcareON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawChildcare();
        }
    }
    
    if (libraryON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawLibrary();
        }
    }
    
    if (pubON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawPub();
        }
    }
    
    if (barON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawBar();
        }
    }
    
    if (labON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawLab();
        }
    }
    
    if (schoolON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawSchool();
        }
    }
    
    if (kindergartenON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawKindergarten();
        }
    }
    

    if (lawyerON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawLawyer();
        }
    }


    if (theatreON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawTheatre();
        }
    }


    if (atmON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawAtm();
        }
    }


    if (policeON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawPolice();
        }
    }


    if (fuelON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawFuel();
        }
    }
    
    if (veterinaryON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawVeterinary();
        }
    }
    
    if (cinemaON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawCinema();
        }
    }
    
    if (tutoringON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawTutoring();
        }
    }
    
    if (psychicON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawPsychic();
        }
    }
    
    if (telephoneON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawTelephone();
        }
    }
    
    if (spaON) {
        for (unsigned i = 0; i < POIMasterList.size(); i ++) {
            POIMasterList[i].drawSpa();
        }
    }
    
    //draw subway station
    if (get_visible_world().get_height() < 0.00003 || subwayON) {
        for (unsigned j = 0; j < subStList.size(); j ++) {
            subStList[j].drawSubStation();
        }
    }
    //draw streetCart stop
    if (get_visible_world().get_height() < 0.00003 || stCartON) {
        for (unsigned j = 0; j < stCartStopList.size(); j ++) {
            stCartStopList[j].drawStCartStop();
        }
    }
    //draw bus stop
    if (get_visible_world().get_height() < 0.00003 || busON) {
        for (unsigned j = 0; j < busStopList.size(); j ++) {
            busStopList[j].drawBusStop();
        }
    }
}


void streetNet::hightLightSeg(unsigned segID) {
    defaultWidth = round(((1/get_visible_world().get_height())/1800));
    HighwayWidth = round(((1/get_visible_world().get_height())/900)) + 2;
    HighwayLinkWidth = round(((1/get_visible_world().get_height())/900));
    primeryWidth = round(((1/get_visible_world().get_height())/900)) + 1;
    
    segMasterList[segID].drawContour(true);
}


void streetNet::PrintAreas() {
    for(unsigned int i = 0; i < 500; i++) {//featureAreas.size()
        std::cout << "area " << i << " = " << featureAreas[i].first << " id = " << featureAreas[i].second << std::endl;
    }
    
}


std::string streetNet::autoComplete(std::string input) {
    int len = input.length();
    
    for (unsigned i = 0; i < streetMasterList.size(); i ++) {
        if (streetMasterList[i].name.substr(0, len).compare(input) == 0)
            return streetMasterList[i].name;
    }
    
    for (unsigned i = 0; i < featureMasterList.size(); i ++) {
        if (featureMasterList[i].name.substr(0, len).compare(input) == 0)
            return featureMasterList[i].name;
    }
    
    for (unsigned i = 0; i < commonKeyWord.size(); ++ i) {
        if (commonKeyWord[i].substr(0, len).compare(input) == 0)
            return commonKeyWord[i];
    }
    
    return "";
}


double streetNet::pathTravelTime(const std::vector<unsigned>& segList, 
                                                const double trunCost) {
    double time = 0;
    if (segList.size() == 0) return time;
    streetSegment* prevSeg = &segMasterList[segList[0]];
    time += prevSeg->travelTime;
    streetSegment* currentSeg;
    for (unsigned i = 1; i < segList.size(); ++ i) {
        currentSeg = &segMasterList[segList[i]];
        if (prevSeg->streetID != currentSeg->streetID) { //turning 
            time += trunCost;
        }
        time += currentSeg->travelTime;
        prevSeg = currentSeg;
    }
    return time;
}


unsigned streetNet::pathLength(const std::vector<unsigned>& segList) {
    unsigned length = 0;
    for (unsigned i = 0; i < segList.size(); ++ i) {
        length += segMasterList[segList[i]].length;
    }
    return length;
}


double streetNet::heuristic(unsigned start, unsigned end) {
    std::pair<double, double> startXY 
        = {intersecMasterList[start].projPt.first,
            intersecMasterList[start].projPt.second};
    std::pair<double, double> endXY 
        = {intersecMasterList[end].projPt.first,
            intersecMasterList[end].projPt.second};
    
    return sqrt(pow((startXY.first - endXY.first), 2) + pow((startXY.second - endXY.second), 2));
}


unsigned streetNet::getNextIntersec(unsigned start, unsigned stSeg) {
    if (segMasterList[stSeg].from != start) return segMasterList[stSeg].from;
    else return segMasterList[stSeg].to;
}


double streetNet::adjIntersecTravelTime(unsigned start, unsigned end, 
                                    std::vector<unsigned>& currentSt,
                                    unsigned& newSt, double trunCost) {
    
    std::vector<unsigned> segList = intersecMasterList[start].segList;
    std::vector<unsigned> path;
    for (unsigned i = 0; i < segList.size(); ++ i) {
        if ((segMasterList[segList[i]].from == start && segMasterList[segList[i]].to == end)
                || (segMasterList[segList[i]].to == start && segMasterList[segList[i]].from == end)) {
            path.push_back(segList[i]);
        }
    }
    double minTime = 999999;
    unsigned street = 0;
    for (unsigned j = 0; j < path.size(); ++ j) {
        streetSegment &thisSeg = segMasterList[path[j]];
        double actualTime = thisSeg.travelTime;
        if (currentSt[start] != thisSeg.streetID && currentSt[start] != 0xfffffffe)
        {
            actualTime += trunCost;
        }
        if (actualTime < minTime) {
            minTime = actualTime;
            street = thisSeg.streetID;
        }
//        if (start == 59909 && end == 59910) {
//            std::cout << "Street " << thisSeg.streetID << " cost " << actualTime
//                    << " with segment cost " << thisSeg.travelTime << std::endl;
//            std::cout << "Street segment ID: " << thisSeg.segID << std::endl;
//        }
    }
    
//    if (start == 59909 && end == 59910) {
//        std::cout << "Selected street: " << street << " with cost " << minTime << std::endl;
//        
//    }
    newSt = street;
    
    return minTime;
}


//this function is a bit hard to under stand, please add good comments for
//everything you added or changed here
std::vector<unsigned> streetNet::getPathFromIntersec(unsigned start, unsigned end,
                                                double turnCost, bool demo, bool Astar) {
    bool pathFound = false;
    std::vector<unsigned> path;
    std::priority_queue<neighbor, std::vector<neighbor>, closerNeighbor> frontier;
    std::vector<double> timeSofar;
    timeSofar.resize(intersecMasterList.size(), 0);
    std::vector<unsigned> currentSt; //this keep track of the street are at for every frontiers
    currentSt.resize(intersecMasterList.size(), 0xffffffff);
    currentSt[start] = 0xfffffffe;
    std::vector<bool> visited;
    visited.resize(intersecMasterList.size(), false);

    //this is a double duty data vector, both served as a visited flag system also 
    //as a trail system for re constucte path in the end
    //comeFrom[inter] = 0xffffffff -> intersection are not visited
    //comeFrom[inter] = interid != 0xffffffff
    //                  -> intersection are visited & its visited from Inter# interid
    std::vector<unsigned> comeFrom;
    comeFrom.resize(intersecMasterList.size(), 0xffffffff);
    comeFrom[start] = start;//set starting point as visited
    
    if (start == end) return path;
    
    neighbor startNB(start, 0, 0);
    frontier.push(startNB);
    
    while(!frontier.empty()) { //search until all notes are reached 
        neighbor current = frontier.top(); //get the top of the Q
        frontier.pop();                    //remove the top right after accessing
        
//        if (current.time >= timeSofar[current.ID]) continue;
        
        if (current.ID == end) {
            pathFound = true;
//            std::cout << "================================" << std::endl;
//            std::cout << current.time << std::endl;
//            std::cout << "================================" << std::endl;
            break; //exit if find end
        }
        
        if (visited[current.ID]) continue;
        visited[current.ID] = true;
        
        std::vector<unsigned> neighbors = 
                                listIntersecDirectlyConected(current.ID);
        
        //add neighbors into frontier (expending)
        for (unsigned i = 0; i < neighbors.size(); ++ i) {
//            unsigned hackFix = currentSt[neighbors[i]];
            unsigned newSt = 0xffffffff;
            double newtime = current.time //timeSofar[current.ID]
                + adjIntersecTravelTime(current.ID, neighbors[i], 
                                    currentSt, newSt, turnCost);
            if (comeFrom[neighbors[i]] == 0xffffffff            //if not visited
                    || newtime < timeSofar[neighbors[i]]) { 
                timeSofar[neighbors[i]] = newtime;
                double heur = 0;
                if (Astar) {
                    heur = heuristic(neighbors[i], end) / 35; //35m/s is max city speed limit
                }
                neighbor newneignbor(neighbors[i], newtime, heur);
                frontier.push(newneignbor); //add the neighbors
                comeFrom[neighbors[i]] = current.ID; //set reached flag
                currentSt[neighbors[i]] = newSt;
            }
//            else {
//                currentSt[neighbors[i]] = hackFix;
//            }
        }
        
        if (demo) { 
            delay(5);
            intersecMasterList[current.ID].highlightPoint();
        }
    }   
    
    //now we have reached the end, let's re-construct the path we took
    //we will do this by going backwards
    if (pathFound) {
        path.push_back(end);
        unsigned current = end;
        while (current != start) {
            path.push_back(comeFrom[current]);
            current = comeFrom[current];
        }
        std::reverse(path.begin(), path.end()); //reverse the vector
    }
    return path;
}


std::vector<unsigned> streetNet::inter2POIpath(unsigned start, std::string endPOI, 
                                    double turnCost, bool demo, bool Astar) {
    bool pathFound = false;
    std::vector<unsigned> path;
    //this will creates a list of possible ends
    std::vector<unsigned> POIends = POINameTable[endPOI];
    std::vector<unsigned> ends;
    unsigned end = 0xffffffff;
    for (unsigned i = 0; i < POIends.size(); ++ i) {
        unsigned closestInter = POIMasterList[POIends[i]].closestIntersection;
        if (closestInter == start) return path;
        ends.push_back(closestInter);
    }
    if (ends.size() != 1) Astar = false; // for more than 1 ends, can't use A*
    
    std::priority_queue<neighbor, std::vector<neighbor>, closerNeighbor> frontier; 
    std::vector<double> timeSofar;
    timeSofar.resize(intersecMasterList.size(), 0);
    std::vector<unsigned> currentSt; //this keep track of the street are at for every frontiers
    currentSt.resize(intersecMasterList.size(), 0xffffffff);
    
    std::vector<bool> visited;
    visited.resize(intersecMasterList.size(), false);

    //this is a double duty data vector, both served as a visited flag system also 
    //as a trail system for re construct path in the end
    //comeFrom[inter] = 0xffffffff -> intersection are not visited
    //comeFrom[inter] = interid != 0xffffffff
    //                  -> intersection are visited & its visited from Inter# interid
    std::vector<unsigned> comeFrom;
    comeFrom.resize(intersecMasterList.size(), 0xffffffff);
    comeFrom[start] = 0;//set starting point as visited, 0 have no meaning in this case
    
    neighbor startNB(start, 0, 0);
    frontier.push(startNB);
    
    while(!frontier.empty()) { //search until all notes are reached 
        neighbor current = frontier.top(); //get the top of the Q
        frontier.pop();                    //remove the top right after accessing
        
        for (unsigned j = 0; j < ends.size(); ++ j) {
            if (current.ID == ends[j]) {
                end = ends[j];
                pathFound = true;
//                std::cout << + << std::endl;
//                std::cout << current.time << std::endl;
//                std::cout << "================================" << std::endl;
                break; //exit if find the first end
            }
        }
        if (pathFound) break;
        
        if (visited[current.ID]) continue;
        visited[current.ID] = true;
        
        std::vector<unsigned> neighbors = 
                                listIntersecDirectlyConected(current.ID);
        
        //add neighbors into frontier (expending)
        for (unsigned i = 0; i < neighbors.size(); ++ i) {
//            unsigned hackFix = currentSt[neighbors[i]];
            unsigned newSt = 0xffffffff;
            double newtime = current.time //timeSofar[current.ID]
                + adjIntersecTravelTime(current.ID, neighbors[i], 
                                    currentSt, newSt, turnCost);
            if (comeFrom[neighbors[i]] == 0xffffffff            //if not visited
                    || newtime < timeSofar[neighbors[i]]) { 
                timeSofar[neighbors[i]] = newtime;
                double heur = 0;
                if (Astar) {
                    heur = heuristic(neighbors[i], end) / 35; //35m/s is max city speed limit
                }
                neighbor newneignbor(neighbors[i], newtime, heur);
                frontier.push(newneignbor); //add the neighbors
                comeFrom[neighbors[i]] = current.ID; //set reached flag
                currentSt[neighbors[i]] = newSt;
            }
        }
        if (demo) { 
            delay(5);
            intersecMasterList[current.ID].highlightPoint();
        }
    }   
    
    //now we have reached the end, let's re-construct the path we took
    //we will do this by going backwards
    if (pathFound) {
        path.push_back(end);
        unsigned current = end;
        while (current != start) {
            path.push_back(comeFrom[current]);
            current = comeFrom[current];
        }
        std::reverse(path.begin(), path.end()); //reverse the vector
    }
    return path;
}


std::vector<unsigned> streetNet::nodePath2edgePath(const std::vector<unsigned>& nodePath,
                                                            double turncost) {
    std::vector<unsigned> edgePath;
    unsigned prevSt = 0xffffffff;
    
    for (unsigned i = 1; i < nodePath.size(); ++ i) {
        double mintime = 999999;
        unsigned fastSeg = 0xffffffff;
        std::vector<unsigned> segList = intersecMasterList[nodePath[i-1]].segList;
        for (unsigned j = 0; j < segList.size(); ++ j) {
//            if (nodePath[i] == 59910 && nodePath[i-1] == 59909) {
//                std::cout << "Considering segment " << segList[j] << std::endl;
//            }
            if (!segMasterList[segList[j]].oneWay 
                        || segMasterList[segList[j]].from == nodePath[i-1]) {
                if (segMasterList[segList[j]].from == nodePath[i]
                        || segMasterList[segList[j]].to == nodePath[i]) {
                    double curTime = segMasterList[segList[j]].travelTime;
                    if (segMasterList[segList[j]].streetID != prevSt 
                            && prevSt != 0xffffffff) {
                        curTime += turncost;
                    }
//                    if (nodePath[i] == 59910 && nodePath[i-1] == 59909) {
//                        std::cout << "Calculated cost " << curTime << std::endl;
//                    }
                    if (curTime < mintime) {
                        mintime = curTime;
                        fastSeg = segList[j];
                    }
                }
            }
        }
        if (fastSeg == 0xffffffff) {
            std::cout << "Error in node path: intersection #" << nodePath[i-1]
                    << " is not adjacent to intersection #" << nodePath[i] << std::endl;
        }
        else {
            edgePath.push_back(fastSeg);
            prevSt = segMasterList[fastSeg].streetID;
        }
    }
//    for (unsigned i = 1; i < nodePath.size(); ++ i) {
//        edgePath.push_back(findSegByInter(nodePath[i-1], nodePath[i]));
//    }
    return edgePath;
}


std::vector<trunInfo> streetNet::path2direction(const std::vector<unsigned>& edgePath) {
    std::vector<trunInfo> directions;
    if (edgePath.size() <= 1) return directions;
     double currentDis = 0;
    trunInfo startPt;//start point is spacial, we don't care about turn direction in this point
    directions.push_back(startPt);
    for (unsigned i = 1; i < edgePath.size(); ++ i) {
        currentDis += segMasterList[edgePath[i-1]].length;
      
        //a trun info is created when we truned
        if ((segMasterList[edgePath[i-1]].streetID != segMasterList[edgePath[i]].streetID)&& i!= (edgePath.size()-1)) {
            directions[directions.size()-1].nextTurn = currentDis;
            directions[directions.size()-1].stID = segMasterList[edgePath[i]].streetID;
            currentDis = 0;//reset currentDis
            
            //set the last trun (next trun & stID)
            //----------------------------------------------------
            //set the current trun (turn type)
            trunInfo currentTurn;
            
            
            streetSegment* prevSeg = &segMasterList[edgePath[i-1]];
            streetSegment* nextSeg = &segMasterList[edgePath[i]];
            
            LatLon prevInt;
            LatLon currentInt;
            LatLon nextInt;
            //only 2 direc
            if (prevSeg->from == nextSeg->from) {
                currentInt = intersecMasterList[prevSeg->from].location;
                prevInt = intersecMasterList[prevSeg->to].location;
                nextInt = intersecMasterList[nextSeg->to].location;
            }
            else if (prevSeg->from == nextSeg->to) {
                currentInt = intersecMasterList[prevSeg->from].location;
                prevInt = intersecMasterList[prevSeg->to].location;
                nextInt = intersecMasterList[nextSeg->from].location;
            }
            else if (prevSeg->to == nextSeg->from) {
                currentInt = intersecMasterList[prevSeg->to].location;
                prevInt = intersecMasterList[prevSeg->from].location;
                nextInt = intersecMasterList[nextSeg->to].location;
            }
            else if (prevSeg->to == nextSeg->to){
                currentInt = intersecMasterList[prevSeg->to].location;
                prevInt = intersecMasterList[prevSeg->from].location;
                nextInt = intersecMasterList[nextSeg->from].location;
            }
            
            double PCx = currentInt.lon() - prevInt.lon();
            double PCy = currentInt.lat() - prevInt.lat();
            double CNx = nextInt.lon() - currentInt.lon();
            double CNy = nextInt.lat() - currentInt.lat();
            
            double crossZ = PCx*CNy - PCy*CNx;
            double angle = crossZ/((pow(pow(PCy,2.0)+pow(PCx,2.0),0.5))*(pow(pow(CNx,2.0)+pow(CNy,2.0),0.5)));
            
            if (crossZ > 0 ) {
                if(fabs(angle) < 0.08){
                    directions[directions.size()-1].turnDirection = Stright;
                }
                else {
                    directions[directions.size()-1].turnDirection = Left;
                }
            }
            else if (crossZ == 0) directions[directions.size()-1].turnDirection = Stright;
            else if (crossZ < 0) {
                if(fabs(angle) < 0.08){
                    directions[directions.size()-1].turnDirection = Stright;
                }
                else {
                    directions[directions.size()-1].turnDirection = Right; 
            }
            }
            directions.push_back(currentTurn);
        }
        
        if (i == (edgePath.size()-1)) {
            directions[directions.size()-1].nextTurn = currentDis;
            directions[directions.size()-1].stID = 0;
            currentDis = 0;//reset currentDis
            
            //set the last trun (next trun & stID)
 
        }
    }
    
    //add the end node
    
    
    return directions;
}


std::vector<unsigned> streetNet::path2Nodes(unsigned start, 
                    std::vector<unsigned> ends, double turnCost, unsigned& endStop) {
    bool pathFound = false;
    std::vector<unsigned> path;
    unsigned end = 0xffffffff;
    bool Astar = true;  
    if (ends.size() != 1) Astar = false; // for more than 1 ends, can't use A*
    
    std::priority_queue<neighbor, std::vector<neighbor>, closerNeighbor> frontier; 
    std::vector<double> timeSofar;
    timeSofar.resize(intersecMasterList.size(), 0);
    std::vector<unsigned> currentSt; //this keep track of the street are at for every frontiers
    currentSt.resize(intersecMasterList.size(), 0xffffffff);
    std::vector<bool> visited;
    visited.resize(intersecMasterList.size(), false);

    //this is a double duty data vector, both served as a visited flag system also 
    //as a trail system for re construct path in the end
    //comeFrom[inter] = 0xffffffff -> intersection are not visited
    //comeFrom[inter] = interid != 0xffffffff
    //                  -> intersection are visited & its visited from Inter# interid
    std::vector<unsigned> comeFrom;
    comeFrom.resize(intersecMasterList.size(), 0xffffffff);
    comeFrom[start] = 0;//set starting point as visited, 0 have no meaning in this case
    
    neighbor startNB(start, 0, 0);
    frontier.push(startNB);
    
    while(!frontier.empty()) { //search until all notes are reached 
        neighbor current = frontier.top(); //get the top of the Q
        frontier.pop();                    //remove the top right after accessing
        
        for (unsigned j = 0; j < ends.size(); ++ j) {
            if (current.ID == ends[j]) {
                end = ends[j];
                pathFound = true;
                break; //exit if find the first end
            }
        }
        if (pathFound) break;
        
        if (visited[current.ID]) continue;
        visited[current.ID] = true;
        
        std::vector<unsigned> neighbors = 
                                listIntersecDirectlyConected(current.ID);
        
        //add neighbors into frontier (expending)
        for (unsigned i = 0; i < neighbors.size(); ++ i) {
//            unsigned hackFix = currentSt[neighbors[i]];
            unsigned newSt = 0xffffffff;
            double newtime = current.time //timeSofar[current.ID]
                + adjIntersecTravelTime(current.ID, neighbors[i], 
                                    currentSt, newSt, turnCost);
            if (comeFrom[neighbors[i]] == 0xffffffff            //if not visited
                    || newtime < timeSofar[neighbors[i]]) { 
                timeSofar[neighbors[i]] = newtime;
                double heur = 0;
                if (Astar) {
                    heur = heuristic(neighbors[i], ends[0]) / 35; //35m/s is max city speed limit
                }
                neighbor newneignbor(neighbors[i], newtime, heur);
                frontier.push(newneignbor); //add the neighbors
                comeFrom[neighbors[i]] = current.ID; //set reached flag
                currentSt[neighbors[i]] = newSt;
            }
        }
    }   
    
    //now we have reached the end, let's re-construct the path we took
    //we will do this by going backwards
    if (pathFound) {
        path.push_back(end);
        unsigned current = end;
        while (current != start) {
            path.push_back(comeFrom[current]);
            current = comeFrom[current];
        }
        std::reverse(path.begin(), path.end()); //reverse the vector
    }
    
    endStop = end;
    return path;
}


std::vector<double> streetNet::NodePathCosts(unsigned start, 
                                    std::vector<unsigned> ends,
                                    std::vector<std::vector<unsigned>>& allPath, double turnCost) {
    bool allPathFound = false;
//    unsigned end = 0xffffffff;
    bool Astar = true;  
    if (ends.size() != 1) Astar = false; // for more than 1 ends, can't use A*
    
    std::vector<unsigned> copyEnds = ends; //deep copy ??!
    
    std::vector<double> costMap;
    costMap.resize(getNumberOfIntersections());
    
    std::priority_queue<neighbor, std::vector<neighbor>, closerNeighbor> frontier; 
    std::vector<double> timeSofar;
    timeSofar.resize(intersecMasterList.size(), 0);
    std::vector<unsigned> currentSt; //this keep track of the street are at for every frontiers
    currentSt.resize(intersecMasterList.size(), 0xffffffff);
    std::vector<bool> visited;
    visited.resize(intersecMasterList.size(), false);

    //this is a double duty data vector, both served as a visited flag system also 
    //as a trail system for re construct path in the end
    //comeFrom[inter] = 0xffffffff -> intersection are not visited
    //comeFrom[inter] = interid != 0xffffffff
    //                  -> intersection are visited & its visited from Inter# interid
    std::vector<unsigned> comeFrom;
    comeFrom.resize(intersecMasterList.size(), 0xffffffff);
    comeFrom[start] = 0;//set starting point as visited, 0 have no meaning in this case
    
    neighbor startNB(start, 0, 0);
    frontier.push(startNB);
    
    while(!frontier.empty()) { //search until all notes are reached 
        neighbor current = frontier.top(); //get the top of the Q
        frontier.pop();                    //remove the top right after accessing
        

        for (unsigned j = 0; j < ends.size(); ++ j) {
            if (current.ID == ends[j]) {
//                std::cout << "Reach end: " << ends[j] << endl;
                ends.erase(std::remove(ends.begin(), ends.end(), current.ID), ends.end()); //once we reached this end, remove it from ends
                costMap[current.ID] = current.time;
                if (ends.empty()) {
                    allPathFound = true;
                }
                break; //exit if find the all ends 
            }
        }
        
        if (allPathFound) break;
        
        if (visited[current.ID]) continue;
        visited[current.ID] = true;
        
        std::vector<unsigned> neighbors = 
                                listIntersecDirectlyConected(current.ID);
        
        //add neighbors into frontier (expending)
        for (unsigned i = 0; i < neighbors.size(); ++ i) {
//            unsigned hackFix = currentSt[neighbors[i]];
            unsigned newSt = 0xffffffff;
            double newtime = current.time //timeSofar[current.ID]
                + adjIntersecTravelTime(current.ID, neighbors[i], 
                                    currentSt, newSt, turnCost);
            if (comeFrom[neighbors[i]] == 0xffffffff            //if not visited
                    || newtime < timeSofar[neighbors[i]]) { 
                timeSofar[neighbors[i]] = newtime;
                double heur = 0;
                if (Astar) {
                    heur = heuristic(neighbors[i], ends[0]) / 35; //35m/s is max city speed limit
                }
                neighbor newneignbor(neighbors[i], newtime, heur);
                frontier.push(newneignbor); //add the neighbors
                comeFrom[neighbors[i]] = current.ID; //set reached flag
                currentSt[neighbors[i]] = newSt;
            }
        }
    }   
    
    //now we have reached the end, let's re-construct the path we took
    //we will do this by going backwards
    std::vector<std::vector<unsigned>> newPathes;
    newPathes.resize(getNumberOfIntersections());
    
    if (allPathFound) {
        for (unsigned e = 0; e < copyEnds.size(); ++ e) {
//            std::vector<unsigned> newpath;
            if (!newPathes[copyEnds[e]].empty()) continue;
            newPathes[copyEnds[e]].push_back(copyEnds[e]);
            unsigned current = copyEnds[e];
            while (current != start) {
               newPathes[copyEnds[e]].push_back(comeFrom[current]);
                current = comeFrom[current];
            }
            std::reverse(newPathes[copyEnds[e]].begin(), newPathes[copyEnds[e]].end()); //reverse the vector
//            allPath[copyEnds[e]] = newpath;
        }
        
        #pragma omp critical
        {
            allPath = newPathes;
        }
        
    }
    
    return costMap;
}


bool streetNet::routeLookingGood(const std::vector<unsigned>& route,
                    std::vector<std::vector<unsigned>>& pickUpLookUp) {
    for (unsigned i = 0; i < route.size(); ++ i) {
        //if this stop is a drop off, check for pick up before
        if(!pickUpLookUp[route[i]].empty()) {
            if (i == 0) return false; //path can't start with a drop off 
            //we always look at the last one first, then pop it
            std::vector<unsigned> itsPickUp = pickUpLookUp[route[i]];
            for (unsigned k = 0; k < itsPickUp.size(); ++ k) {
                //look at all elements before
                for (unsigned j = 0; j < i; ++ j) {
                    if (itsPickUp[k] == route[j]) {
                        break;
                    }
                    
                    //at the end, there still no pick up, route is not good
                    if (j == (i-1)) return false;
                }
            }
        }
    }
    return true;
    
}


bool streetNet::routeLookingGood2ndTry(const std::vector<unsigned>& route,
                    std::vector<std::vector<unsigned>>& pickUpLookUp) {
    return false;
}


//hash in through uID
    //need to know whether pickup or dropoff
        //if pickup make the corresponding dropoff point true - gotta have a field for the corresponding value
        //if dropoff check whether accessible or not

//so i have multiple pickups and dropoffs mapping to the same unsigned id in the pseudoHash table
//def bad for pickups as the corresponding pair gets overwritten

bool streetNet::legalityCheck(const std::vector<unsigned>& route, std::vector< std::vector< std::pair< unsigned, std::pair< bool, unsigned> > > >& pseudoHash) {//std::vector< std::vector< std::pair< unsigned, bool> > >& pseudoHash multi
    //#pragma omp parallel for
    //unsigned wtf = 0;
    std::vector<unsigned> undoer;
    //std::vector<unsigned> counter;
    //counter.reserve(route.size());
    //unsigned pickupCount = 0;
    //std::cout<<"do we get here?"<<std::endl;
    for (unsigned i = 0; i < route.size(); i++) {
        //std::cout<<"route[i], i = "<<i<<" route[i]= "<<route[i]<<std::endl;
        for (unsigned k = 0; k < pseudoHash[route[i]].size(); k++) {
            //std::cout<<"do we get here? 2"<<std::endl;
            //pseudoHash[pickUp uID][size of vector for that uID].dropOff/pickUp
            //first = corresponding dropoff id; second.first = dropoff enabled bool; second.second = k for corresponding dropoff inner loop
            if (pseudoHash[route[i]][k].first != 0) { //means pickup
                //wtf++;
                //Hash[corresponding dropOff id][which same dropoff id for the pickup] 
                //pseudoHash[pseudoHash[route[i]][k].first][pseudoHash[pseudoHash[route[i]][k].first][k].second.first].second.first++;
                //hashing in to the corresponding dropOff, to the corresponding i-th place in the vector
                pseudoHash[pseudoHash[route[i]][k].first][pseudoHash[route[i]][k].second.second].second.first = true;   //pickup, make corresponding dropoff available
                undoer.emplace_back(pseudoHash[route[i]][k].first); //holds the outer loop dropoff ids, links to a vector when undoing changes
                //std::cout<<"do we get here? = "<<wtf<<std::endl;
                //counter[i]++;
            }
            else  {//==0 -> dropoff
                //std::cout<<"do we get here? else"<<std::endl;
                //hashing in through dropOff id, need to come up with a way to hash in to the correct inner vector
                if(pseudoHash[route[i]][pseudoHash[route[i]][k].second.second].second.first == false)  {
                    for (unsigned m = 0; m < undoer.size(); m++) {
                        for (unsigned n = 0; n < pseudoHash[undoer[m]].size(); n++)
                            pseudoHash[undoer[m]][n].second.first = false;
                    }
                    return false;   //dropoff, and corresponding pickup have not been taken yet = return false
                }
            }
        }
    }
    for (unsigned j = 0; j < undoer.size(); j++) {
        for (unsigned k = 0; k < pseudoHash[undoer[j]].size(); k++)
            pseudoHash[undoer[j]][k].second.first = false;
    }
    return true;
}

/*
bool streetNet::legalityCheck(const std::vector<unsigned>& route, std::vector< std::pair< unsigned, bool> >& pseudoHash) {//std::vector< std::pair< unsigned, bool> >& pseudoHash
    std::vector<unsigned> undoer;   //used to keep track of what fields have been modified in the numberOfIntersections resized pseudo hash table
    //pseudoHash and counter are both resized to number of intersections, counter is also initialized to zeros during load_map
    bool flag = false;  //not all dropoffs have been reached, break out, init to false
    
    for (unsigned i = 0; i < route.size() && !flag; i++) {   //route[i] is the uID for either pickUp or dropOff
        if (pseudoHash[route[i]].first != 0) { //means pickup
            pseudoHash[pseudoHash[route[i]].first].second = true;   //pickup, make corresponding dropoff available, pseudoHash[route[i]].first is the dropOff id (always)
            undoer.emplace_back(pseudoHash[route[i]].first);    //undoer holds dropOff ids cause we only change these in the pseudoHash
            //counter[pseudoHash[route[i]].first]+=1;//pseudoHash[route[i]].first is ALWAYS the dropOff id, increment to denote amount of dropoffs per id
            counter[route[i]]+=1;   //adding 1 to the corresponding entry in the pseudoPickupHash, corresponding to the pickUp id
        }
        else  {//==0 -> dropoff
            if(pseudoHash[route[i]].second == false)  {//means havent been picked up yet
                //for (unsigned j = 0; j < undoer.size(); j++) {
                //    pseudoHash[undoer[j]].second = false;   //nulling the reachable dropoffs
                    //counter[route[j]] = 0;  //nulling the counter
                //}
                flag = true;   //dropoff, and corresponding pickup have not been taken yet = return false
                //break;
            }
            //counter[route[i]]-=1;   //route[i] is the dropOff id
        }
    }
    
    if(!flag) { //check whether all pickUps have been made
        for (unsigned j = 0; j < route.size(); j++) {   //route[j] is the uID for either pickUp or dropOff
            if (pseudoHash[route[j]].first != 0) { //means pickup
                if (counter[route[j]] != Pcounter[route[j]]){    //checking if all dropoffs have been done (PICKUPS)
                    //counter[route[j]] = 0;  //nulling the counter
                //    flag = true;    //not all dropoffs have been reached, break out
                }
            }
        }
    }
    
    for (unsigned j = 0; j < undoer.size(); j++) {//if reached here undoer size is equal to route size
        pseudoHash[undoer[j]].second = false;   //nulling the reachable dropoffs
    }
    
    for (unsigned j = 0; j < route.size(); j++) {
        if (pseudoHash[route[j]].first != 0) //means pickup
            counter[route[j]] = 0;   //nulling the locally changed global counter
    }
    
    if (flag)
        return false;
    else
        return true;
}
*/

/*
 bool streetNet::legalityCheck(const std::vector<unsigned>& route, std::unordered_map< unsigned, std::pair< unsigned, bool> > hash) {
    //#pragma omp parallel for
    for (unsigned i = 0; i < route.size(); ++ i) {
        if (hash[route[i]].first != 0)  hash[hash[route[i]].first].second = true;   //pickup, make corresponding dropoff available
        else  //==0 -> dropoff
            if(hash[route[i]].second == false)  return false;   //dropff, and corresponding pickup have not been taken yet = return false
    }
    return true;
}
 */

/*
std::vector<unsigned> streetNet::findRouteO3(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty) {
    
    auto startTime = std::chrono::high_resolution_clock::now();
    unsigned deliveriesSize = deliveries.size();
    unsigned allStopsSize = deliveriesSize*2;
    //std::vector<unsigned> dropOffs;
    //std::vector<unsigned> pickUps;
    std::vector<unsigned> allStops;
    //std::vector<std::vector<unsigned>> dropOff2pickUp;
    //dropOff2pickUp.resize(getNumberOfIntersections());
    //std::unordered_map< unsigned, std::pair< unsigned, bool> > hash;
    //hash.reserve(deliveriesSize);
    allStops.reserve(deliveriesSize);
    
#ifdef profile 
    auto const setupStart = std::chrono::high_resolution_clock::now();
    std::cout << "Profiling interated path =============================" << endl;
#endif
    for (unsigned i = 0; i < deliveriesSize; ++ i) {
        //pickUps.emplace_back(deliveries[i].pickUp);
        allStops.emplace_back(deliveries[i].pickUp);
        //dropOffs.push_back(deliveries[i].dropOff);
        allStops.emplace_back(deliveries[i].dropOff);
        //dropOff2pickUp[deliveries[i].dropOff].push_back(deliveries[i].pickUp);
        //hash.emplace(deliveries[i].pickUp, std::make_pair( deliveries[i].dropOff, true));
        //hash.emplace(deliveries[i].dropOff, std::make_pair( 0, false));
        
        pseudoHash[deliveries[i].pickUp] = std::make_pair(deliveries[i].dropOff, true);
        pseudoHash[deliveries[i].dropOff] = std::make_pair(0, false);
    }
#ifdef profile 
    auto const setupEnd = std::chrono::high_resolution_clock::now();
    auto const setupTime = 
    std::chrono::duration_cast<std::chrono::microseconds>(setupEnd - setupStart);
    std::cout << "Set up time is: " << setupTime.count() << "us" << endl;
#endif
    
#ifdef profile 
    auto const costStart = std::chrono::high_resolution_clock::now();
#endif
    //we first need to set up a costMatrix for fast cost look up
    //this holds travel time from every stop to every stop
    //look up cost as: costMatrix[start][end]
    std::vector<std::vector<double>> costMatrix;
    costMatrix.resize(getNumberOfIntersections());
    std::vector<std::vector<std::vector<unsigned>>> pathMatrix;
    pathMatrix.resize(getNumberOfIntersections());
    for (unsigned i = 0; i < allStops.size(); ++ i) {
        pathMatrix[allStops[i]].resize(getNumberOfIntersections());
    }
    #pragma omp parallel for
    for (unsigned i = 0; i < allStopsSize; ++ i) {
//        std::vector<std::vector<unsigned>> newPath;
//        pathMatrix[allStops[i]].resize(getNumberOfIntersections());
//        std::cout << "Start with: " << allStops[i] << endl;
        costMatrix[allStops[i]] = NodePathCosts(allStops[i], allStops,
                                        pathMatrix[allStops[i]], turn_penalty);
//        pathMatrix[allStops[i]] = newPath;
    }
#ifdef profile 
    auto const costEnd = std::chrono::high_resolution_clock::now();
    auto const costTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(costEnd - costStart);
    std::cout << "Set up cost matrix takes: " << costTime.count() << "ms" << endl;
#endif
    
#ifdef profile 
    auto const intPathStart = std::chrono::high_resolution_clock::now();
#endif
    allStops = findRouteGreedy(deliveries, depots, costMatrix, turn_penalty);
#ifdef profile 
    auto const intPathEnd = std::chrono::high_resolution_clock::now();
    auto const intPathTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(intPathEnd - intPathStart);
    std::cout << "Get initial route takes: " << intPathTime.count() << "ms" << endl;
#endif
    
#ifdef profile 
    auto const iterStart = std::chrono::high_resolution_clock::now();
#endif
    
    //First let's find out what is our base travel time;
    double bestCost = findRouteTime(allStops, costMatrix);
    
    
    bool timeOut = false;
    unsigned twoOptsCount = 0;
    unsigned iterationCount = 0;
    unsigned whileCount = 0;
    unsigned newRouteCount = 0;
    
    while (!timeOut) { //this runs for 28s exactly, leave 2 second for clean up witch should take 50ms
        whileCount ++;
        //now we try 2-Opt
        
//        //first we try to find the 2 longest Edges
//        //edge is store as a pair<start, end>
//        std::pair<unsigned, unsigned> longestEdge = {0, 0};
//        std::pair<unsigned, unsigned> secondLongestEdge = {0, 0};
//        double longestEdgeCost = 0;
//        double secondLongestEdgeCost = 0;
//        for (unsigned i = 1; i < allStops.size(); ++ i) {
//            if (costMatrix[allStops[i-1]][allStops[i]] > secondLongestEdgeCost) {
//                if (costMatrix[allStops[i-1]][allStops[i]] > longestEdgeCost) {
//                    secondLongestEdgeCost = longestEdgeCost;
//                    longestEdgeCost = costMatrix[allStops[i-1]][allStops[i]];
//                    secondLongestEdge = longestEdge;
//                    longestEdge = {i-1, i};
//                }
//                else {
//                    secondLongestEdgeCost = costMatrix[allStops[i-1]][allStops[i]];
//                    secondLongestEdge = {i-1, i};
//                }
//            }
//        }
        
        if (allStops.size() < 3) break; 
        //Fuck it, we will run 2-opt on 2 random edges for now
        for (unsigned firstCut = 1; firstCut < (allStopsSize-1); ++ firstCut) {
            bool firstCutBreakFlag = false;
            for (unsigned secondCut = firstCut + 1; secondCut < allStops.size(); ++ secondCut) {
                twoOptsCount ++;
                //We cut our current path into 3 (pretty randomly)
                std::vector<unsigned> subRoute0(allStops.begin(), allStops.begin() + firstCut);
                std::vector<unsigned> subRoute1(allStops.begin() + firstCut ,
                                            allStops.begin() + secondCut);
                std::vector<unsigned> subRoute2(allStops.begin() + secondCut,
                                                    allStops.end());

                //now let's play with it
                //for now, lets just reverse the first subRoute and see
                std::reverse(subRoute1.begin(), subRoute1.end());
                std::vector<unsigned> newRoute;
                newRoute.insert(newRoute.end(), subRoute0.begin(), subRoute0.end());
                newRoute.insert(newRoute.end(), subRoute1.begin(), subRoute1.end());
                newRoute.insert(newRoute.end(), subRoute2.begin(), subRoute2.end());
                //check for legality fist
                #ifdef profileLeg 
                    auto const LegStart = std::chrono::high_resolution_clock::now();
                #endif
                if (legalityCheck(newRoute, pseudoHash)) {//pseudoHash
                #ifdef profileLeg 
                    auto LegEnd = std::chrono::high_resolution_clock::now();
                    auto Leg =
                        std::chrono::duration_cast<std::chrono::microseconds> (
                        LegEnd - LegStart);
                std::cout << "legality check takes: " << Leg.count() << "us" << endl;   
                #endif
                    //check weather the new path is batter
                    newRouteCount ++;
                    double newCost = findRouteTime(newRoute, costMatrix);
                    if (newCost < bestCost ||  ((double)rand() / (RAND_MAX)) < 0) {
                        bestCost = newCost;
                        allStops = newRoute;
                        iterationCount ++;
                        firstCutBreakFlag = true;
                        break;
                    }
                }
                
                //try reverse middle subRoute
                twoOptsCount ++;
                std::vector<unsigned> subRoute3(allStops.begin(), allStops.begin() + firstCut);
                std::vector<unsigned> subRoute4(allStops.begin() + firstCut ,
                                            allStops.begin() + secondCut);
                std::vector<unsigned> subRoute5(allStops.begin() + secondCut,
                                                    allStops.end());              
                std::reverse(subRoute3.begin(), subRoute3.end());
                newRoute.clear();
                newRoute.insert(newRoute.end(), subRoute3.begin(), subRoute3.end());
                newRoute.insert(newRoute.end(), subRoute4.begin(), subRoute4.end());
                newRoute.insert(newRoute.end(), subRoute5.begin(), subRoute5.end());
                //check for legality fist
                if (legalityCheck(newRoute, pseudoHash)) {
                    //check weather the new path is batter
                    newRouteCount ++;
                    double newCost = findRouteTime(newRoute, costMatrix);
                    if (newCost < bestCost ||  ((double)rand() / (RAND_MAX)) < 0) {
                        bestCost = newCost;
                        allStops = newRoute;
                        iterationCount ++;
                        firstCutBreakFlag = true;
                        break;
                    }
                }
                
                //try reverse last subRoute
                twoOptsCount ++;
                std::vector<unsigned> subRoute6(allStops.begin(), allStops.begin() + firstCut);
                std::vector<unsigned> subRoute7(allStops.begin() + firstCut ,
                                            allStops.begin() + secondCut);
                std::vector<unsigned> subRoute8(allStops.begin() + secondCut,
                                                    allStops.end());       
                std::reverse(subRoute8.begin(), subRoute8.end());
                newRoute.clear();
                newRoute.insert(newRoute.end(), subRoute6.begin(), subRoute6.end());
                newRoute.insert(newRoute.end(), subRoute7.begin(), subRoute7.end());
                newRoute.insert(newRoute.end(), subRoute8.begin(), subRoute8.end());
                //check for legality fist
                if (legalityCheck(newRoute, pseudoHash)) {//hash  //pseudoHash
                    //check weather the new path is batter
                    newRouteCount ++;
                    double newCost = findRouteTime(newRoute, costMatrix);
                    if (newCost < bestCost ||  ((double)rand() / (RAND_MAX)) < 0) {
                        bestCost = newCost;
                        allStops = newRoute;
                        iterationCount ++;
                        firstCutBreakFlag = true;
                        break;
                    }
                }
                
               
            }
            if (firstCutBreakFlag) break;
        }
        
     //we handle Wall clack here
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto wallClock =
                    std::chrono::duration_cast<std::chrono::duration<double>> (
                    currentTime - startTime);
        if (wallClock.count() > 28) {
            timeOut = true;
            std::cout << "Ran 2-Opts " << twoOptsCount << " times, " 
                    << "Found " << newRouteCount << " new route(s), "
                    << "Repeating " << (whileCount - iterationCount) << " times, "
                    << "with " << iterationCount << " iteration(s)" << endl;
        }
    }
#ifdef profile 
    auto const iterEnd = std::chrono::high_resolution_clock::now();
    auto const iterTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(iterEnd - iterStart);
    std::cout << "Iteration takes: " << iterTime.count() << "ms" << endl;
#endif
    
#ifdef profile 
    auto const pathStart = std::chrono::high_resolution_clock::now();
#endif
    //now we build the path from allStops
    // first pick a start depot
    unsigned startDepot;
    unsigned endDepot;
    path2Nodes(allStops[0], depots, turn_penalty, startDepot);
    
//    std::vector<std::vector<unsigned>> pathSet;
//    pathSet.resize(getNumberOfIntersections());
//    #pragma omp parallel for
//    for (unsigned i = 1; i < allStops.size(); ++ i) {
//        pathSet[i-1] = getPathFromIntersec(allStops[i-1], allStops[i],
//                                                turn_penalty);
//    }
    
    std::vector<unsigned> finalRoute = getPathFromIntersec(startDepot, allStops[0],
                                                turn_penalty);
#ifdef profile 
    auto const traceStart = std::chrono::high_resolution_clock::now();
#endif
    for (unsigned i = 1; i < allStops.size(); ++ i) {

        std::vector<unsigned> newpath = pathMatrix[allStops[i-1]][allStops[i]];
        if (newpath.empty()) {
            std::cout << "No path between" << allStops[i-1] << " and " << allStops[i] << endl;
            continue;
        }
        finalRoute.pop_back();
        finalRoute.insert(finalRoute.end(), newpath.begin(), newpath.end());
    }
#ifdef profile 
    auto const traceEnd = std::chrono::high_resolution_clock::now();
    auto const traceTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(traceEnd - traceStart);
    std::cout << "  Trace back takes: " << traceTime.count() << "ms" << endl;
#endif
    std::vector<unsigned> goHome = path2Nodes(allStops[allStops.size()-1], depots,
                                                turn_penalty, endDepot);
    
    finalRoute.pop_back();
    finalRoute.insert(finalRoute.end(), goHome.begin(), goHome.end());
#ifdef profile 
    auto const pathEnd = std::chrono::high_resolution_clock::now();
    auto const pathTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(pathEnd - pathStart);
    std::cout << "Building final path takes: " << pathTime.count() << "ms" << endl;
#endif

    return finalRoute;
}*/

std::vector<unsigned> streetNet::findRouteO3(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty) {
    
    auto startTime = std::chrono::high_resolution_clock::now();
    unsigned timeLimit = deliveries.size();
//    std::cout << "Problem size: " << timeLimit << endl;
    if (timeLimit > 28) timeLimit = 28;
    
    
    std::vector<unsigned> dropOffs;
    std::vector<unsigned> pickUps;
    std::vector<unsigned> allStops;
    //std::vector<std::vector<unsigned>> dropOff2pickUp;
    //dropOff2pickUp.resize(getNumberOfIntersections());
    
#ifdef profile 
    auto const setupStart = std::chrono::high_resolution_clock::now();
    std::cout << "Profiling integrated path =============================" << endl;
#endif
    
    //std::cout<<"Wtf"<<std::endl;
    for (unsigned i = 0; i < deliveries.size(); i++) {
        //std::cout<<"first"<<std::endl;
        //std::cout<<"pseudoHash[deliveries[i].dropOff].size() = "<<pseudoHash[deliveries[i].dropOff].size()<<std::endl;
        //std::cout<<"deliveries.size() = "<<deliveries.size()<<" i = "<<i<<std::endl;
        //std::cout<<"deliveries[i].dropOff " << deliveries[i].dropOff << std::endl;
        //pickUps.push_back(deliveries[i].pickUp);
        allStops.emplace_back(deliveries[i].pickUp);
        Pcounter[deliveries[i].pickUp]++;
        //dropOffs.push_back(deliveries[i].dropOff);
        allStops.emplace_back(deliveries[i].dropOff);
        Dcounter[deliveries[i].dropOff]++;
        //dropOff2pickUp[deliveries[i].dropOff].push_back(deliveries[i].pickUp);
        //if( pseudoHash[deliveries[i].pickUp].size()==0)
        //if(pseudoHash[deliveries[i].dropOff].size()==1)                                           //use second.second as direct [i][j] j value
        pseudoHash[deliveries[i].pickUp].emplace_back(deliveries[i].dropOff, std::make_pair(true, Dcounter[deliveries[i].dropOff]));//true is just a placeholder, unused
        
#ifdef legalityInner    
            for(unsigned j=0;j<pseudoHash[deliveries[i].pickUp].size();j++){
                std::cout<<"pseudoHash[deliveries["<<i<<"].pickUp] first = "<<pseudoHash[deliveries[i].pickUp][j].first<<std::endl;
                std::cout<<"pseudoHash[deliveries["<<i<<"].pickUp] second.first = "<<pseudoHash[deliveries[i].pickUp][j].second.first<<std::endl;
                std::cout<<"pseudoHash[deliveries["<<i<<"].pickUp] second.second = "<<pseudoHash[deliveries[i].pickUp][j].second.second<<std::endl;
            }
#endif
        //else
        //    pseudoHash[deliveries[i].pickUp].emplace_back(deliveries[i].dropOff, std::make_pair(true, pseudoHash[deliveries[i].dropOff].size()+1));//true is just a placeholder, unused
        //else {
        //    pseudoHash[deliveries[i].pickUp].emplace_back(std::make_pair(deliveries[i].dropOff, pseudoHash[deliveries[i].pickUp].size()));
        //}
        pseudoHash[deliveries[i].dropOff].emplace_back(0, std::make_pair(false, Pcounter[deliveries[i].pickUp]-1));
        
#ifdef legalityInner          
            for(unsigned j=0;j<pseudoHash[deliveries[i].dropOff].size();j++){
                std::cout<<"pseudoHash[deliveries["<<i<<"].dropOff] first = "<<pseudoHash[deliveries[i].dropOff][j].first<<std::endl;
                std::cout<<"pseudoHash[deliveries["<<i<<"].dropOff] second.first = "<<pseudoHash[deliveries[i].dropOff][j].second.first<<std::endl;
                std::cout<<"pseudoHash[deliveries["<<i<<"].dropOff] second.second = "<<pseudoHash[deliveries[i].dropOff][j].second.second<<std::endl;
            }
#endif
        //pseudoHash[deliveries[i].dropOff][pseudoHash[deliveries[i].pickUp].size()-1] = std::make_pair(0, false);
        //std::cout<<"second"<<std::endl;
        //std::cout<<"pseudoHash[deliveries[i].dropOff].size() = "<<pseudoHash[deliveries[i].dropOff].size()<<std::endl;
        //std::cout<<"deliveries.size() = "<<deliveries.size()<<" i = "<<i<<std::endl;
        //std::cout<<"deliveries[i].dropOff " << deliveries[i].dropOff << std::endl;
    }
#ifdef legality
    for (unsigned i = 0; i < allStops.size(); ++ i) {
        //std::cout<<"pseudoHash[deliveries[i].dropOff].size() = "<<pseudoHash[deliveries[i].dropOff].size()<<std::endl;
        //std::cout<<"deliveries[i].dropOff " << deliveries[i].dropOff << std::endl;
        for(unsigned j=0;j<pseudoHash[allStops[i]].size();j++){
            //deliveries = {DeliveryInfo(73593, 104453), DeliveryInfo(1981, 47060), DeliveryInfo(58204, 31403), DeliveryInfo(102129, 63975), DeliveryInfo(41153, 63608)};
            std::cout<<"allStops[i]= "<<allStops[i]<<std::endl;
            std::cout<<"pseudoHash[allStops[i]].size()= "<<pseudoHash[allStops[i]].size()<<std::endl;
            std::cout<<"corresponding dropoff id/first= "<<pseudoHash[allStops[i]][j].first<<std::endl;
            std::cout<<"dropoff enabled bool/second.first= "<<pseudoHash[allStops[i]][j].second.first<<std::endl;
            std::cout<<"k for corresponding dropoff inner loop/second.second= "<<pseudoHash[allStops[i]][j].second.second<<std::endl;
        }
    }
#endif
    //std::cout<<"Wtf2"<<std::endl;
    /*
    for (unsigned i = 0; i < allStops.size(); i++) {
        for (unsigned k = 0; k < pseudoHash[allStops[i]].size(); k++) {
            //std::cout << "allstops [] " <<i<<" allstops i = "<<allStops[i]<<std::endl;
            std::cout << "pseudoHash[" << i << "][" << k << "] first = " << pseudoHash[allStops[i]][k].first << " second = " << pseudoHash[allStops[i]][k].second.first
                    << " third = " << pseudoHash[allStops[i]][k].second.second << std::endl;
        }
    }*/
    //std::cout<<"Wtf3"<<std::endl;
    
#ifdef profile 
    auto const setupEnd = std::chrono::high_resolution_clock::now();
    auto const setupTime = 
    std::chrono::duration_cast<std::chrono::microseconds>(setupEnd - setupStart);
    std::cout << "Set up time is: " << setupTime.count() << "us" << endl;
#endif
    
#ifdef profile 
    auto const costStart = std::chrono::high_resolution_clock::now();
#endif
    //we first need to set up a costMatrix for fast cost look up
    //this holds travel time from every stop to every stop
    //look up cost as: costMatrix[start][end]
    std::vector<std::vector<double>> costMatrix;
    costMatrix.resize(getNumberOfIntersections());
    std::vector<std::vector<std::vector<unsigned>>> pathMatrix;
    pathMatrix.resize(getNumberOfIntersections());
    for (unsigned i = 0; i < allStops.size(); ++ i) {
        pathMatrix[allStops[i]].resize(getNumberOfIntersections());
    }
    #pragma omp parallel for
    for (unsigned i = 0; i < allStops.size(); ++ i) {
//        std::vector<std::vector<unsigned>> newPath;
//        pathMatrix[allStops[i]].resize(getNumberOfIntersections());
//        std::cout << "Start with: " << allStops[i] << endl;
        costMatrix[allStops[i]] = NodePathCosts(allStops[i], allStops,
                                        pathMatrix[allStops[i]], turn_penalty);
//        pathMatrix[allStops[i]] = newPath;
    }
#ifdef profile 
    auto const costEnd = std::chrono::high_resolution_clock::now();
    auto const costTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(costEnd - costStart);
    std::cout << "Set up cost matrix takes: " << costTime.count() << "ms" << endl;
#endif
    
#ifdef profile 
    auto const intPathStart = std::chrono::high_resolution_clock::now();
#endif
    allStops = findRouteGreedy(deliveries, depots, costMatrix, turn_penalty);
#ifdef profile 
    auto const intPathEnd = std::chrono::high_resolution_clock::now();
    auto const intPathTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(intPathEnd - intPathStart);
    std::cout << "Get initial route takes: " << intPathTime.count() << "ms" << endl;
#endif
    
#ifdef profile 
    auto const iterStart = std::chrono::high_resolution_clock::now();
#endif
    //findRouteO3
    //First let's find out what is our base travel time;
    double bestCost = findRouteTime(allStops, costMatrix);
    
    
    bool timeOut = false;
    unsigned twoOptsCount = 0;
    unsigned iterationCount = 0;
    unsigned whileCount = 0;
    unsigned newRouteCount = 0;
    int reverse = 3;
    int order = 0;
    
    while (!timeOut) { //this runs for 28s exactly, leave 2 second for clean up witch should take 50ms
        whileCount ++;
        //now we try 2-Opt
        
//        //first we try to find the 2 longest Edges
//        //edge is store as a pair<start, end>
//        std::pair<unsigned, unsigned> longestEdge = {0, 0};
//        std::pair<unsigned, unsigned> secondLongestEdge = {0, 0};
//        double longestEdgeCost = 0;
//        double secondLongestEdgeCost = 0;
//        for (unsigned i = 1; i < allStops.size(); ++ i) {
//            if (costMatrix[allStops[i-1]][allStops[i]] > secondLongestEdgeCost) {
//                if (costMatrix[allStops[i-1]][allStops[i]] > longestEdgeCost) {
//                    secondLongestEdgeCost = longestEdgeCost;
//                    longestEdgeCost = costMatrix[allStops[i-1]][allStops[i]];
//                    secondLongestEdge = longestEdge;
//                    longestEdge = {i-1, i};
//                }
//                else {
//                    secondLongestEdgeCost = costMatrix[allStops[i-1]][allStops[i]];
//                    secondLongestEdge = {i-1, i};
//                }
//            }
//        }
        //findRouteO3
        //if (allStops.size() < 3) break; 
//        //Fuck it, we will run 2-opt on 2 random edges for now
//        for (unsigned firstCut = 1; firstCut < (allStops.size()-1); ++ firstCut) {
//            bool firstCutBreakFlag = false;
//            for (unsigned secondCut = firstCut + 1; secondCut < allStops.size(); ++ secondCut) {
//                twoOptsCount ++;
//                //We cut our current path into 3 (pretty randomly)
//                std::vector<unsigned> subRoute0(allStops.begin(), allStops.begin() + firstCut);
//                std::vector<unsigned> subRoute1(allStops.begin() + firstCut ,
//                                            allStops.begin() + secondCut);
//                std::vector<unsigned> subRoute2(allStops.begin() + secondCut,
//                                                    allStops.end());
//
//                //now let's play with it
//                //for now, lets just reverse the first subRoute and see
//                std::reverse(subRoute1.begin(), subRoute1.end());
//                std::vector<unsigned> newRoute;
//                newRoute.insert(newRoute.end(), subRoute0.begin(), subRoute0.end());
//                newRoute.insert(newRoute.end(), subRoute1.begin(), subRoute1.end());
//                newRoute.insert(newRoute.end(), subRoute2.begin(), subRoute2.end());
//                
//                
//                //check for legality fist
////                #ifdef profile 
////                    auto const LegStart = std::chrono::high_resolution_clock::now();
////                #endif
//                if (routeLookingGood(newRoute, dropOff2pickUp)) {
////                #ifdef profile 
////                    auto LegEnd = std::chrono::high_resolution_clock::now();
////                    auto Leg =
////                        std::chrono::duration_cast<std::chrono::microseconds> (
////                        LegEnd - LegStart);
////                std::cout << "legality check takes: " << Leg.count() << "us" << endl;   
////                #endif
//                    //check weather the new path is batter
//                    newRouteCount ++;
//                    double newCost = findRouteTime(newRoute, costMatrix);
//                    if (newCost < bestCost ||  ((double)rand() / (RAND_MAX)) < 0) {
//                        bestCost = newCost;
//                        allStops = newRoute;
//                        iterationCount ++;
//                        firstCutBreakFlag = true;
//                        break;
//                    }
//                }
//                
//                //try reverse middle subRoute
//                twoOptsCount ++;
//                std::vector<unsigned> subRoute3(allStops.begin(), allStops.begin() + firstCut);
//                std::vector<unsigned> subRoute4(allStops.begin() + firstCut ,
//                                            allStops.begin() + secondCut);
//                std::vector<unsigned> subRoute5(allStops.begin() + secondCut,
//                                                    allStops.end());              
//                std::reverse(subRoute3.begin(), subRoute3.end());
//                newRoute.clear();
//                newRoute.insert(newRoute.end(), subRoute3.begin(), subRoute3.end());
//                newRoute.insert(newRoute.end(), subRoute4.begin(), subRoute4.end());
//                newRoute.insert(newRoute.end(), subRoute5.begin(), subRoute5.end());
//                //check for legality fist
//                if (routeLookingGood(newRoute, dropOff2pickUp)) {
//                    //check weather the new path is batter
//                    newRouteCount ++;
//                    double newCost = findRouteTime(newRoute, costMatrix);
//                    if (newCost < bestCost ||  ((double)rand() / (RAND_MAX)) < 0) {
//                        bestCost = newCost;
//                        allStops = newRoute;
//                        iterationCount ++;
//                        firstCutBreakFlag = true;
//                        break;
//                    }
//                }
//                
//                //try reverse last subRoute
//                twoOptsCount ++;
//                std::vector<unsigned> subRoute6(allStops.begin(), allStops.begin() + firstCut);
//                std::vector<unsigned> subRoute7(allStops.begin() + firstCut ,
//                                            allStops.begin() + secondCut);
//                std::vector<unsigned> subRoute8(allStops.begin() + secondCut,
//                                                    allStops.end());       
//                std::reverse(subRoute8.begin(), subRoute8.end());
//                newRoute.clear();
//                newRoute.insert(newRoute.end(), subRoute6.begin(), subRoute6.end());
//                newRoute.insert(newRoute.end(), subRoute7.begin(), subRoute7.end());
//                newRoute.insert(newRoute.end(), subRoute8.begin(), subRoute8.end());
//                //check for legality fist
//                if (routeLookingGood(newRoute, dropOff2pickUp)) {
//                    //check weather the new path is batter
//                    newRouteCount ++;
//                    double newCost = findRouteTime(newRoute, costMatrix);
//                    if (newCost < bestCost ||  ((double)rand() / (RAND_MAX)) < 0) {
//                        bestCost = newCost;
//                        allStops = newRoute;
//                        iterationCount ++;
//                        firstCutBreakFlag = true;
//                        break;
//                    }
//                }
//                
//               
//            }
//            if (firstCutBreakFlag) break;
//        }
        //findRouteO3
        if (getBetter(allStops, pseudoHash, costMatrix, 
                twoOptsCount, newRouteCount, bestCost, reverse, order, 
                startTime, deliveries.size())) {
            iterationCount ++;
        }
        else { //what do we do if no better route found
            reverse += 3;
        }
        if (reverse > 6) {
            reverse = 3;
            order ++;
        }
        
     //we handle Wall clock here
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto wallClock =
                    std::chrono::duration_cast<std::chrono::duration<double>> (
                    currentTime - startTime);
        if (wallClock.count() > timeLimit) {
            timeOut = true;
            std::cout << "Ran 2-Opts " << twoOptsCount << " times, " 
                    << "Found " << newRouteCount << " new route(s), "
                    << "Repeating " << (whileCount - iterationCount) << " times, "
                    << "with " << iterationCount << " iteration(s), " 
                    << "at level: " << reverse << " order type: " << order << endl;
        }
    }//findRouteO3
#ifdef profile 
    auto const iterEnd = std::chrono::high_resolution_clock::now();
    auto const iterTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(iterEnd - iterStart);
    std::cout << "Iteration takes: " << iterTime.count() << "ms" << endl;
#endif
    
#ifdef profile 
    auto const pathStart = std::chrono::high_resolution_clock::now();
#endif
    //now we build the path from allStops
    // first pick a start depot
    unsigned startDepot;
    unsigned endDepot;
    path2Nodes(allStops[0], depots, turn_penalty, startDepot);
    
//    std::vector<std::vector<unsigned>> pathSet;
//    pathSet.resize(getNumberOfIntersections());
//    #pragma omp parallel for
//    for (unsigned i = 1; i < allStops.size(); ++ i) {
//        pathSet[i-1] = getPathFromIntersec(allStops[i-1], allStops[i],
//                                                turn_penalty);
//    }
    
    std::vector<unsigned> finalRoute = getPathFromIntersec(startDepot, allStops[0],
                                                turn_penalty);
#ifdef profile 
    auto const traceStart = std::chrono::high_resolution_clock::now();
#endif
    for (unsigned i = 1; i < allStops.size(); ++ i) {

        std::vector<unsigned> newpath = pathMatrix[allStops[i-1]][allStops[i]];
        if (newpath.empty()) {
            std::cout << "No path between" << allStops[i-1] << " and " << allStops[i] << endl;
            continue;
        }
        finalRoute.pop_back();
        finalRoute.insert(finalRoute.end(), newpath.begin(), newpath.end());
    }//findRouteO3
#ifdef profile 
    auto const traceEnd = std::chrono::high_resolution_clock::now();
    auto const traceTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(traceEnd - traceStart);
    std::cout << "  Trace back takes: " << traceTime.count() << "ms" << endl;
#endif
    std::vector<unsigned> goHome = path2Nodes(allStops[allStops.size()-1], depots,
                                                turn_penalty, endDepot);
    
    finalRoute.pop_back();
    finalRoute.insert(finalRoute.end(), goHome.begin(), goHome.end());
#ifdef profile 
    auto const pathEnd = std::chrono::high_resolution_clock::now();
    auto const pathTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(pathEnd - pathStart);
    std::cout << "Building final path takes: " << pathTime.count() << "ms" << endl;
#endif
    
    for (unsigned i = 0; i < deliveries.size(); ++ i) {
        pseudoHash[deliveries[i].pickUp].clear();
        pseudoHash[deliveries[i].dropOff].clear();
        Pcounter[deliveries[i].pickUp]=0;
        Dcounter[deliveries[i].dropOff]=0;
    }
    //for (unsigned i = 0; i < deliveries.size(); ++ i) {
    //    std::cout<<"pseudoHash[deliveries[i].dropOff].size() = "<<pseudoHash[deliveries[i].dropOff].size()<<std::endl;
    //}

    return finalRoute;
}
//end findRouteO3

double streetNet::findRouteTime(std::vector<unsigned>& route,
             std::vector<std::vector<double>>& costMatrix) {
    double travelTime = 0;
    for (unsigned i = 1; i < route.size(); ++ i) {
        travelTime += costMatrix[route[i-1]][route[i]];
    }
    return travelTime;
}

std::vector<unsigned> streetNet::findRouteGreedy(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        std::vector<std::vector<double>>& costMatrix, 
                                        const float turn_penalty) {
    std::vector<unsigned> bestRoute;
    double bestTime = 99999999;
    
    unsigned srang = 1;
    if (deliveries.size() <= 8) srang = deliveries.size();
    else srang = 8;
    
    #pragma omp parallel for 
    for (unsigned s = 0; s < srang; ++ s) {
        //a pool of posiable next stop
        std::vector<unsigned> nextStopPool;
        std::vector<unsigned> routeSoFar;

        unsigned currentStop;
        std::vector<unsigned> numPickUp;
        numPickUp.resize(getNumberOfIntersections(), 0);
        std::unordered_map<unsigned, std::vector<unsigned>> pickUp2dropOff;

        //now let set up the begining:
        //first, we selected a depot to start with
    //    unsigned startDepot;;
    //        path2Nodes(deliveries[0].pickUp, depots, turn_penalty, startDepot);
    //        if (startDepot == 0xffffffff) return routeSoFar;
        currentStop = deliveries[s].pickUp;
        //now that we have nothing in our pocket, all the pick-ups are possiable
        //next stop
        for (unsigned i = 0; i < deliveries.size(); ++ i) {
            nextStopPool.push_back(deliveries[i].pickUp);
            //let's set up StopPickUp in the same time
    //        StopPickUp[deliveries[i].dropOff] = false;
            numPickUp[deliveries[i].pickUp] += 1;  
            pickUp2dropOff[deliveries[i].pickUp].push_back(deliveries[i].dropOff);
        }

        //OK, now that we are done with start, let's start finding route
        while (!nextStopPool.empty()) {
            unsigned newStop;
            std::vector<unsigned> newPath = path2Nodes(currentStop, nextStopPool, 
                    turn_penalty, newStop);

            std::vector<unsigned>::iterator newStopPt 
                    = std::find(nextStopPool.begin(), nextStopPool.end(), newStop);
            if (newStopPt != nextStopPool.end())
                nextStopPool.erase(newStopPt);
            if (newPath.empty()) {
                routeSoFar = newPath;
                break;
            }

            routeSoFar.push_back(newStop);
            //if a pick-up is reached, its drop-off become a possiable next stop
            if (numPickUp[newStop] > 0) {
                nextStopPool.push_back(pickUp2dropOff[newStop][pickUp2dropOff[newStop].size()-1]);
                pickUp2dropOff[newStop].pop_back();
                numPickUp[newStop] -= 1;
            }
            currentStop = newStop;
        }
        
        double newRouteCost = findRouteTime(routeSoFar, costMatrix);
        if (newRouteCost < bestTime) {
            bestTime = newRouteCost;
            bestRoute = routeSoFar;
        }
    }
    
    if (bestRoute.empty()) return bestRoute;

    //Sweet, we finished our delivery!!!!
    //now we just need to find the closest depot to go back to
//    unsigned endDepot;
//    std::vector<unsigned> FinalPath = path2Nodes(currentStop, depots, 
//                                            turn_penalty, endDepot);
//    routeSoFar.push_back(endDepot);
//    routeSoFar.push_back(endDepot);


    return bestRoute;
}

std::vector<unsigned> streetNet::findRoute(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty) {
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<unsigned> dropOffs;
    std::vector<unsigned> pickUps;
    std::vector<unsigned> allStops;
    std::vector<std::vector<unsigned>> dropOff2pickUp;
    dropOff2pickUp.resize(getNumberOfIntersections());
    
#ifdef profile 
    auto const setupStart = std::chrono::high_resolution_clock::now();
    std::cout << "Profiling integrated path =============================" << endl;
#endif
    for (unsigned i = 0; i < deliveries.size(); ++ i) {
        pickUps.push_back(deliveries[i].pickUp);
        allStops.push_back(deliveries[i].pickUp);
        dropOffs.push_back(deliveries[i].dropOff);
        allStops.push_back(deliveries[i].dropOff);
        dropOff2pickUp[deliveries[i].dropOff].push_back(deliveries[i].pickUp);
    }
#ifdef profile 
    auto const setupEnd = std::chrono::high_resolution_clock::now();
    auto const setupTime = 
    std::chrono::duration_cast<std::chrono::microseconds>(setupEnd - setupStart);
    std::cout << "Set up time is: " << setupTime.count() << "us" << endl;
#endif
    
#ifdef profile 
    auto const costStart = std::chrono::high_resolution_clock::now();
#endif
    //we first need to set up a costMatrix for fast cost look up
    //this holds travel time from every stop to every stop
    //look up cost as: costMatrix[start][end]
    std::vector<std::vector<double>> costMatrix;
    costMatrix.resize(getNumberOfIntersections());
    std::vector<std::vector<std::vector<unsigned>>> pathMatrix;
    pathMatrix.resize(getNumberOfIntersections());
    for (unsigned i = 0; i < allStops.size(); ++ i) {
        pathMatrix[allStops[i]].resize(getNumberOfIntersections());
    }
    #pragma omp parallel for
    for (unsigned i = 0; i < allStops.size(); ++ i) {
//        std::vector<std::vector<unsigned>> newPath;
//        pathMatrix[allStops[i]].resize(getNumberOfIntersections());
//        std::cout << "Start with: " << allStops[i] << endl;
        costMatrix[allStops[i]] = NodePathCosts(allStops[i], allStops,
                                        pathMatrix[allStops[i]], turn_penalty);
//        pathMatrix[allStops[i]] = newPath;
    }
#ifdef profile 
    auto const costEnd = std::chrono::high_resolution_clock::now();
    auto const costTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(costEnd - costStart);
    std::cout << "Set up cost matrix takes: " << costTime.count() << "ms" << endl;
#endif
    
#ifdef profile 
    auto const intPathStart = std::chrono::high_resolution_clock::now();
#endif
    allStops = findRouteGreedy(deliveries, depots, costMatrix, turn_penalty);
#ifdef profile 
    auto const intPathEnd = std::chrono::high_resolution_clock::now();
    auto const intPathTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(intPathEnd - intPathStart);
    std::cout << "Get initial route takes: " << intPathTime.count() << "ms" << endl;
#endif
    
#ifdef profile 
    auto const iterStart = std::chrono::high_resolution_clock::now();
#endif
    
    //First let's find out what is our base travel time;
    double bestCost = findRouteTime(allStops, costMatrix);
    
    
    bool timeOut = false;
    unsigned twoOptsCount = 0;
    unsigned iterationCount = 0;
    unsigned whileCount = 0;
    unsigned newRouteCount = 0;
    int reverse = 3;
    int order = 0;
    
    while (!timeOut) { //this runs for 28s exactly, leave 2 second for clean up witch should take 50ms
        whileCount ++;
        //now we try 2-Opt
        
//        //first we try to find the 2 longest Edges
//        //edge is store as a pair<start, end>
//        std::pair<unsigned, unsigned> longestEdge = {0, 0};
//        std::pair<unsigned, unsigned> secondLongestEdge = {0, 0};
//        double longestEdgeCost = 0;
//        double secondLongestEdgeCost = 0;
//        for (unsigned i = 1; i < allStops.size(); ++ i) {
//            if (costMatrix[allStops[i-1]][allStops[i]] > secondLongestEdgeCost) {
//                if (costMatrix[allStops[i-1]][allStops[i]] > longestEdgeCost) {
//                    secondLongestEdgeCost = longestEdgeCost;
//                    longestEdgeCost = costMatrix[allStops[i-1]][allStops[i]];
//                    secondLongestEdge = longestEdge;
//                    longestEdge = {i-1, i};
//                }
//                else {
//                    secondLongestEdgeCost = costMatrix[allStops[i-1]][allStops[i]];
//                    secondLongestEdge = {i-1, i};
//                }
//            }
//        }
        
        if (allStops.size() < 3) break; 
//        //Fuck it, we will run 2-opt on 2 random edges for now
//        for (unsigned firstCut = 1; firstCut < (allStops.size()-1); ++ firstCut) {
//            bool firstCutBreakFlag = false;
//            for (unsigned secondCut = firstCut + 1; secondCut < allStops.size(); ++ secondCut) {
//                twoOptsCount ++;
//                //We cut our current path into 3 (pretty randomly)
//                std::vector<unsigned> subRoute0(allStops.begin(), allStops.begin() + firstCut);
//                std::vector<unsigned> subRoute1(allStops.begin() + firstCut ,
//                                            allStops.begin() + secondCut);
//                std::vector<unsigned> subRoute2(allStops.begin() + secondCut,
//                                                    allStops.end());
//
//                //now let's play with it
//                //for now, lets just reverse the first subRoute and see
//                std::reverse(subRoute1.begin(), subRoute1.end());
//                std::vector<unsigned> newRoute;
//                newRoute.insert(newRoute.end(), subRoute0.begin(), subRoute0.end());
//                newRoute.insert(newRoute.end(), subRoute1.begin(), subRoute1.end());
//                newRoute.insert(newRoute.end(), subRoute2.begin(), subRoute2.end());
//                
//                
//                //check for legality fist
////                #ifdef profile 
////                    auto const LegStart = std::chrono::high_resolution_clock::now();
////                #endif
//                if (routeLookingGood(newRoute, dropOff2pickUp)) {
////                #ifdef profile 
////                    auto LegEnd = std::chrono::high_resolution_clock::now();
////                    auto Leg =
////                        std::chrono::duration_cast<std::chrono::microseconds> (
////                        LegEnd - LegStart);
////                std::cout << "legality check takes: " << Leg.count() << "us" << endl;   
////                #endif
//                    //check weather the new path is batter
//                    newRouteCount ++;
//                    double newCost = findRouteTime(newRoute, costMatrix);
//                    if (newCost < bestCost ||  ((double)rand() / (RAND_MAX)) < 0) {
//                        bestCost = newCost;
//                        allStops = newRoute;
//                        iterationCount ++;
//                        firstCutBreakFlag = true;
//                        break;
//                    }
//                }
//                
//                //try reverse middle subRoute
//                twoOptsCount ++;
//                std::vector<unsigned> subRoute3(allStops.begin(), allStops.begin() + firstCut);
//                std::vector<unsigned> subRoute4(allStops.begin() + firstCut ,
//                                            allStops.begin() + secondCut);
//                std::vector<unsigned> subRoute5(allStops.begin() + secondCut,
//                                                    allStops.end());              
//                std::reverse(subRoute3.begin(), subRoute3.end());
//                newRoute.clear();
//                newRoute.insert(newRoute.end(), subRoute3.begin(), subRoute3.end());
//                newRoute.insert(newRoute.end(), subRoute4.begin(), subRoute4.end());
//                newRoute.insert(newRoute.end(), subRoute5.begin(), subRoute5.end());
//                //check for legality fist
//                if (routeLookingGood(newRoute, dropOff2pickUp)) {
//                    //check weather the new path is batter
//                    newRouteCount ++;
//                    double newCost = findRouteTime(newRoute, costMatrix);
//                    if (newCost < bestCost ||  ((double)rand() / (RAND_MAX)) < 0) {
//                        bestCost = newCost;
//                        allStops = newRoute;
//                        iterationCount ++;
//                        firstCutBreakFlag = true;
//                        break;
//                    }
//                }
//                
//                //try reverse last subRoute
//                twoOptsCount ++;
//                std::vector<unsigned> subRoute6(allStops.begin(), allStops.begin() + firstCut);
//                std::vector<unsigned> subRoute7(allStops.begin() + firstCut ,
//                                            allStops.begin() + secondCut);
//                std::vector<unsigned> subRoute8(allStops.begin() + secondCut,
//                                                    allStops.end());       
//                std::reverse(subRoute8.begin(), subRoute8.end());
//                newRoute.clear();
//                newRoute.insert(newRoute.end(), subRoute6.begin(), subRoute6.end());
//                newRoute.insert(newRoute.end(), subRoute7.begin(), subRoute7.end());
//                newRoute.insert(newRoute.end(), subRoute8.begin(), subRoute8.end());
//                //check for legality fist
//                if (routeLookingGood(newRoute, dropOff2pickUp)) {
//                    //check weather the new path is batter
//                    newRouteCount ++;
//                    double newCost = findRouteTime(newRoute, costMatrix);
//                    if (newCost < bestCost ||  ((double)rand() / (RAND_MAX)) < 0) {
//                        bestCost = newCost;
//                        allStops = newRoute;
//                        iterationCount ++;
//                        firstCutBreakFlag = true;
//                        break;
//                    }
//                }
//                
//               
//            }
//            if (firstCutBreakFlag) break;
//        }
        
//        std::thread t(getBetter, ...);
//        
//        
//        
//        t.join();
        
        if (getBetter(allStops, pseudoHash, costMatrix, 
                twoOptsCount, newRouteCount, bestCost, reverse, order, 
                startTime, deliveries.size())) {
            iterationCount ++;
        }
        else { //what do we do if no better route found
            reverse += 3;
        }
        if (reverse > 6) {
            reverse = 3;
            order ++;
        }
        
     //we handle Wall clack here
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto wallClock =
                    std::chrono::duration_cast<std::chrono::duration<double>> (
                    currentTime - startTime);
        if (wallClock.count() > 26) {
            timeOut = true;
            std::cout << "Ran 2-Opts " << twoOptsCount << " times, " 
                    << "Found " << newRouteCount << " new route(s), "
                    << "Repeating " << (whileCount - iterationCount) << " times, "
                    << "with " << iterationCount << " iteration(s)" 
                    << "at level: " << reverse << " order type: " << order << endl;
        }
    }
#ifdef profile 
    auto const iterEnd = std::chrono::high_resolution_clock::now();
    auto const iterTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(iterEnd - iterStart);
    std::cout << "Iteration takes: " << iterTime.count() << "ms" << endl;
#endif
    
#ifdef profile 
    auto const pathStart = std::chrono::high_resolution_clock::now();
#endif
    //now we build the path from allStops
    // first pick a start depot
    unsigned startDepot;
    unsigned endDepot;
    path2Nodes(allStops[0], depots, turn_penalty, startDepot);
    
//    std::vector<std::vector<unsigned>> pathSet;
//    pathSet.resize(getNumberOfIntersections());
//    #pragma omp parallel for
//    for (unsigned i = 1; i < allStops.size(); ++ i) {
//        pathSet[i-1] = getPathFromIntersec(allStops[i-1], allStops[i],
//                                                turn_penalty);
//    }
    
    std::vector<unsigned> finalRoute = getPathFromIntersec(startDepot, allStops[0],
                                                turn_penalty);
#ifdef profile 
    auto const traceStart = std::chrono::high_resolution_clock::now();
#endif
    for (unsigned i = 1; i < allStops.size(); ++ i) {

        std::vector<unsigned> newpath = pathMatrix[allStops[i-1]][allStops[i]];
        if (newpath.empty()) {
            std::cout << "No path between" << allStops[i-1] << " and " << allStops[i] << endl;
            continue;
        }
        finalRoute.pop_back();
        finalRoute.insert(finalRoute.end(), newpath.begin(), newpath.end());
    }
#ifdef profile 
    auto const traceEnd = std::chrono::high_resolution_clock::now();
    auto const traceTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(traceEnd - traceStart);
    std::cout << "  Trace back takes: " << traceTime.count() << "ms" << endl;
#endif
    std::vector<unsigned> goHome = path2Nodes(allStops[allStops.size()-1], depots,
                                                turn_penalty, endDepot);
    
    finalRoute.pop_back();
    finalRoute.insert(finalRoute.end(), goHome.begin(), goHome.end());
#ifdef profile 
    auto const pathEnd = std::chrono::high_resolution_clock::now();
    auto const pathTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(pathEnd - pathStart);
    std::cout << "Building final path takes: " << pathTime.count() << "ms" << endl;
#endif

    return finalRoute;
}


bool streetNet::radiationBreeding(const std::vector<unsigned>& orignal, 
                    std::vector<unsigned>& mutated, 
                    std::vector< std::vector< std::pair< unsigned, std::pair< bool, unsigned> > > >& pseudoHash,//std::vector< std::vector< std::pair< unsigned, bool> > >& pseudoHash,    multi
                    unsigned firstCut, unsigned secondCut, int type, int order) {
//std::vector< std::pair< unsigned, bool> >& pseudoHash

    std::vector<unsigned> newRoute;

    std::vector<unsigned> subRoute0(orignal.begin(), orignal.begin() + firstCut);
    std::vector<unsigned> subRoute1(orignal.begin() + firstCut,
                                orignal.begin() + secondCut);
    std::vector<unsigned> subRoute2(orignal.begin() + secondCut,
                                        orignal.end());

    switch (type) {
        case 0: std::reverse(subRoute0.begin(), subRoute0.end());
                break;
        case 1: std::reverse(subRoute1.begin(), subRoute1.end());
                break;
        case 2: std::reverse(subRoute2.begin(), subRoute2.end());
                break;
        case 3: std::reverse(subRoute0.begin(), subRoute0.end());
                std::reverse(subRoute1.begin(), subRoute1.end());
                break;
        case 4: std::reverse(subRoute0.begin(), subRoute0.end());
                std::reverse(subRoute2.begin(), subRoute2.end());
                break;
        case 5: std::reverse(subRoute1.begin(), subRoute1.end());
                std::reverse(subRoute2.begin(), subRoute2.end());
                break;
        case 6: std::reverse(subRoute0.begin(), subRoute0.end());
                std::reverse(subRoute1.begin(), subRoute1.end());
                std::reverse(subRoute2.begin(), subRoute2.end());
                break;
        default: return false;
    }

    switch (order) {
        case 0: newRoute.insert(newRoute.end(), subRoute0.begin(), subRoute0.end());
                newRoute.insert(newRoute.end(), subRoute1.begin(), subRoute1.end());
                newRoute.insert(newRoute.end(), subRoute2.begin(), subRoute2.end());
                break;
        case 1: newRoute.insert(newRoute.end(), subRoute0.begin(), subRoute0.end());
                newRoute.insert(newRoute.end(), subRoute2.begin(), subRoute2.end());
                newRoute.insert(newRoute.end(), subRoute1.begin(), subRoute1.end());
                break;
        case 2: newRoute.insert(newRoute.end(), subRoute1.begin(), subRoute1.end());
                newRoute.insert(newRoute.end(), subRoute0.begin(), subRoute0.end());
                newRoute.insert(newRoute.end(), subRoute2.begin(), subRoute2.end());
                break;
        case 3: newRoute.insert(newRoute.end(), subRoute1.begin(), subRoute1.end());
                newRoute.insert(newRoute.end(), subRoute2.begin(), subRoute2.end());
                newRoute.insert(newRoute.end(), subRoute0.begin(), subRoute0.end());
                break;
        case 4: newRoute.insert(newRoute.end(), subRoute2.begin(), subRoute2.end());
                newRoute.insert(newRoute.end(), subRoute0.begin(), subRoute0.end());
                newRoute.insert(newRoute.end(), subRoute1.begin(), subRoute1.end());
                break;
        case 5: newRoute.insert(newRoute.end(), subRoute2.begin(), subRoute2.end());
                newRoute.insert(newRoute.end(), subRoute1.begin(), subRoute1.end());
                newRoute.insert(newRoute.end(), subRoute0.begin(), subRoute0.end());
                break;
        default: return false;
    }
    //for(unsigned i=0; i<newRoute.size();i++){
    //    std::cout<<"newRoute[i], i = "<<i<<" newRoute[i]= "<<newRoute[i]<<std::endl;
    //}

    if (legalityCheck(newRoute, pseudoHash)) {
        mutated = newRoute;
        return true;
    }
    else {
        return false;
    }
}


bool streetNet::getBetter(std::vector<unsigned>& orignal, 
                    std::vector< std::vector< std::pair< unsigned, std::pair< bool, unsigned> > > >& pseudoHash,//std::vector< std::vector< std::pair< unsigned, bool> > >& pseudoHash,    multi
                    std::vector<std::vector<double>>& costMatrix,
                    unsigned& breedCount, unsigned& routeCount, double& bestCost, 
                    int reverse, int order, auto startTime, unsigned size) {
    
    std::vector<unsigned> savedRoute;
    int hillClimbing = 0;
    std::vector<unsigned> newPath;
    //std::vector< std::pair< unsigned, bool> >& pseudoHash

    for (unsigned firstCut = 1; firstCut < (orignal.size()-1); ++ firstCut) {
        for (unsigned secondCut = firstCut + 1; secondCut < orignal.size() 
                ; ++ secondCut) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto wallClock =
                    std::chrono::duration_cast<std::chrono::duration<double>> (
                    currentTime - startTime);
            if (wallClock.count() > 28) {
                if (findRouteTime(orignal, costMatrix) > bestCost) {
                    orignal = savedRoute;
                }
                return false;
            }
            for (unsigned orderType = 0; orderType <= order; ++ orderType) {
                for (unsigned reverseType = 0; reverseType < reverse; ++ reverseType) {
                    breedCount ++;
                    double temperture;  //small num will trun off SA
                    
                    //don't ask where this come from
                    if (size >= 150) temperture = 32;
                    else if (size > 100 && size < 150) temperture = 24;
                    else if (size > 30 && size < 100) temperture = 1;
                    else temperture = 24;
                    
                    if (radiationBreeding(orignal, newPath, 
                            pseudoHash, firstCut, secondCut, reverseType, orderType)) {
                        routeCount ++;
                        double newCost = findRouteTime(newPath, costMatrix);
                        double Dcost = newCost - bestCost;
                        if (newCost < bestCost) {
                            bestCost = newCost;
                            orignal = newPath;
                            return true;
                        }
                        //for some chance a worth path will try a worth path
                        else if (((double)rand() / (RAND_MAX)) < (exp(-Dcost/temperture))) {
                            savedRoute = orignal;
                            hillClimbing = 1;
                            orignal = newPath;
                        }
                        else if (hillClimbing > 5) {
                            orignal = savedRoute;
                            hillClimbing = 0;
                        }
                        else if (hillClimbing != 0) hillClimbing ++;
//                        
//                        std::cout << exp(-Dcost/temperture) << " | ";
                        temperture = temperture / 1.1;
                    }
                }
            }
        }
    }
    return false;
}


double streetNet::distance_between_points_local(LatLon point1, LatLon point2) {
    double lat1r, lon1r, lat2r, lon2r;
    lat1r = point1.lat() * DEG_TO_RAD;
    lon1r = point1.lon() * DEG_TO_RAD;
    lat2r = point2.lat() * DEG_TO_RAD;
    lon2r = point2.lon() * DEG_TO_RAD;
    double midLat = (sumIntLat + sumPOILat) / (getNumberOfIntersections() + getNumberOfPointsOfInterest());
    double x1 = lon1r * midLat;
    double y1 = lat1r;
    double x2 = lon2r * midLat;
    double y2 = lat2r;
    return EARTH_RADIUS_IN_METERS * sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}


double distance_between_points(LatLon point1, LatLon point2){   
    double lat1r, lon1r, lat2r, lon2r;
    lat1r = point1.lat() * DEG_TO_RAD;
    lon1r = point1.lon() * DEG_TO_RAD;
    lat2r = point2.lat() * DEG_TO_RAD;
    lon2r = point2.lon() * DEG_TO_RAD;
    double midLat = cos((lat1r + lat2r) / 2);
    double x1 = lon1r * midLat;
    double y1 = lat1r;
    double x2 = lon2r * midLat;
    double y2 = lat2r;
    return EARTH_RADIUS_IN_METERS * sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}


//std::vector<double> crossProduct(std::vector<double> v1, std::vector<double> v2) {
//    
//    const int LOW = 0;
//    const int HIGH = 10;
//    
//    
//    
//}



/* scroll over please, cemetery for a greedy greedy function call, supplanted by a non-greedy greedy and 2opts
std::vector<unsigned> streetNet::findRouteMax(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty) {

    unsigned deliveriesSize = deliveries.size();
    unsigned depotsSize = depots.size();
    //double min = DBL_MAX;
    
    //std::vector< std::pair <unsigned, unsigned> > bestPair = {0,0};
    std::vector<unsigned> pickups;
    std::vector<unsigned> dropoffs;
    //hash table has vector inside because a single pickup intersection spot can and will link to multiple dropoff desintation intersections
    std::unordered_map<unsigned, std::vector<unsigned>> pickDrop;
    //std::vector< std::vector< unsigned> > depotToFirst;
    std::vector< std::pair< double, std::vector<unsigned> > > depotToFirst;
    //std::vector<double> depotToFirstTimes;
    std::vector<unsigned> tempPath;
    std::vector<unsigned> path;
    
    unsigned whichPickup;
    //unsigned emptyCount = 0;
    //unsigned depotToFirstBest;
    //double pathTakes = 0;
    //double pathTakesTemp = 0;
    
    //reserving vectors for performance
    pickups.reserve(deliveriesSize);
    dropoffs.reserve(deliveriesSize);
            //resizing to address by iterator directly
    depotToFirst.reserve(depotsSize);
    //depotToFirstTimes.resize(depotsSize);
    
    //two vectors below need a better way to define size
    //tempPath.reserve(deliveriesSize*2 + 2);
    //path.reserve(deliveriesSize*2 + 2);
    
    //approximation based on geometric distance is a poor idea on second thought
    //for ( unsigned i = 0; i < depotsSize; i++ ) {
    //    for ( unsigned j = 0 ; j < deliveriesSize; j++) {
    //        if (min < find_distance_between_two_points(intersecMasterList[depots[i]].location, intersecMasterList[deliveries[j].pickUp].location))
    //            bestPair.emplace(i,j);
    //    }
    //}
    
    //populating data structures
    for ( unsigned i = 0 ; i < deliveriesSize; i++) {
        pickups.emplace_back(deliveries[i].pickUp);
        dropoffs.emplace_back(deliveries[i].dropOff);
        pickDrop[deliveries[i].pickUp].emplace_back(deliveries[i].dropOff);
    }

    //finding a closest pickup location for every depot
    for ( unsigned i = 0; i < depotsSize; i++ ) {
        std::vector<unsigned> pathToTime = path2Nodes(depots[i], pickups, turn_penalty, whichPickup);
        
        //vector of best paths for every depot - time, path pairs
        //only emplace if path is valid and is non-empty
        if(!pathToTime.empty())
            depotToFirst.emplace_back(pathTravelTime(pathToTime, turn_penalty), pathToTime);
        
        //checks whether the path is empty and increases the empty path count by one, used as an exit condition for the loop
        //if (pathToTime.empty()) emptyCount++;
        //if (path.empty()) continue;
        
        //depotToFirstTimes[i].emplace_back(pathTravelTime(pathToTime, turn_penalty));
        
        //determining which route is the best
        
        //pathTakesTemp = pathTravelTime(pathToTime, turn_penalty);
        //if (pathTakes == 0) pathTakes = pathTakesTemp;
        
        //if (pathTakes < pathTakesTemp) {
        //    pathTakes = pathTakesTemp;
        //    depotToFirstBest = i;
        //}
    }
    
    //could not find any route from any depot to any pickup
    if (depotToFirst.empty()) return path;
    
    //sorted by travel time now - we cannot evaluate every path from every depot to every delivery due to CPU time constraints
    //decided to take 10 paths with closes depot-pickUp pairs
    std::sort(depotToFirst.begin(), depotToFirst.end());
    //depotToFirst.erase(std::remove(depotToFirst.begin(), depotToFirst.end(), 0), depotToFirst.end());
    
    //check if no route could have been found from any depot to any pickUp.
    //works because time to travel sorted NOPE
    //if (depotToFirst[0].first == 0) return path;
    //assigning the best path - could use more than one and use that as an iterator
    
    //checking the 10 closest pairs of depot-first pickup
    for ( unsigned i = 0; i < depotToFirst.size() && i < 10; i++) {
        //tempPath = depotToFirst[i].second;
        
        //while there are still pickUps to be made
        while(!pickups.empty()) {
            //holds the current end stop of the route
            unsigned endRoute;
            //depotToFirst[i].second[depotToFirst[i].second.back()] is the current stop, whereas depotToFirst[i].second[0] is the starting depot
            //this function call finds the nearest available pickup to the current intersection that we are on - the endRoute unsigned denoting an intersection.
            std::vector<unsigned> newPath = path2Nodes(depotToFirst[i].second[depotToFirst[i].second.back()], 
                    pickups, turn_penalty, endRoute);
            
            
            //need to find the endRoute that we found and remove it from the list of pickups to be made
            //std::vector<unsigned>::iterator endRoutePointer = std::find(pickups.begin(), pickups.end(), endRoute);
            
            //erasing the endRoute stop we already made
            //if (endRoutePointer != pickups.end()) pickups.erase(endRoutePointer);
            
            
            //need to find the endRoute that we found and remove it from the list of pickups to be made
            pickups.erase(std::remove(pickups.begin(), pickups.end(), endRoute), pickups.end());
            
            //checks if current path is non-valid and, as such, empty
            //breaks out to avoid running code against an invalid route
            if (newPath.empty()) {
                tempPath = newPath;
                break;
            }
        }
    }
    
    //unsigned end = depotToFirst[1].second.back();
    path = tempPath;

    return path;
}
*/

/*
std::vector<unsigned> streetNet::findRouteMax2(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty) {

    unsigned deliveriesSize = deliveries.size();
    unsigned depotsSize = depots.size();
    
    std::vector<unsigned> pickups;
    std::vector<unsigned> dropoffs;
    //hash table has vector inside because a single pickup intersection spot can and will link to multiple dropoff desintation intersections
    std::unordered_map<unsigned, std::vector<unsigned>> pickDrop;

    std::vector<unsigned> tempPath;
    std::vector<unsigned> path;
    
    unsigned whichPickup;

    //reserving vectors for performance
    pickups.reserve(deliveriesSize);
    dropoffs.reserve(deliveriesSize);
    
    //populating data structures
    for ( unsigned i = 0 ; i < deliveriesSize; i++) {
        pickups.emplace_back(deliveries[i].pickUp);
        dropoffs.emplace_back(deliveries[i].dropOff);
        pickDrop[deliveries[i].pickUp].emplace_back(deliveries[i].dropOff);
    }
    
    
    loadPointCloudS(cloudD, deliveriesSize, 'd');
    loadKDTreeIndex(KDTreeD);

    double radius = 100.0;//all 200 results
    std::vector< std::vector<unsigned> > results;
    results.reserve(depotsSize);
    //finding a closest pickup location for every depot
    for ( unsigned i = 0; i < depotsSize; i++ ) {
        results[i].emplace_back(radialNN('s', intersecMasterList[depots[i]].projPt.first, intersecMasterList[depots[i]].projPt.second, radius)); 
    }
    
    //could not find any route from any depot to any pickup
    if (depotToFirst.empty()) return path;
    
    //sorted by travel time now - we cannot evaluate every path from every depot to every delivery due to CPU time constraints
    //decided to take 10 paths with closes depot-pickUp pairs
    std::sort(depotToFirst.begin(), depotToFirst.end());
    //depotToFirst.erase(std::remove(depotToFirst.begin(), depotToFirst.end(), 0), depotToFirst.end());
    
    //check if no route could have been found from any depot to any pickUp.
    //works because time to travel sorted NOPE
    //if (depotToFirst[0].first == 0) return path;
    //assigning the best path - could use more than one and use that as an iterator
    
    //checking the 10 closest pairs of depot-first pickup
    for ( unsigned i = 0; i < depotToFirst.size() && i < 10; i++) {
        //tempPath = depotToFirst[i].second;
        
        //while there are still pickUps to be made
        while(!pickups.empty()) {
            //holds the current end stop of the route
            unsigned endRoute;
            //depotToFirst[i].second[depotToFirst[i].second.back()] is the current stop, whereas depotToFirst[i].second[0] is the starting depot
            //this function call finds the nearest available pickup to the current intersection that we are on - the endRoute unsigned denoting an intersection.
            std::vector<unsigned> newPath = path2Nodes(depotToFirst[i].second[depotToFirst[i].second.back()], 
                    pickups, turn_penalty, endRoute);
            
            
            //need to find the endRoute that we found and remove it from the list of pickups to be made
            //std::vector<unsigned>::iterator endRoutePointer = std::find(pickups.begin(), pickups.end(), endRoute);
            
            //erasing the endRoute stop we already made
            //if (endRoutePointer != pickups.end()) pickups.erase(endRoutePointer);
            
            
            //need to find the endRoute that we found and remove it from the list of pickups to be made
            pickups.erase(std::remove(pickups.begin(), pickups.end(), endRoute), pickups.end());
            
            //checks if current path is non-valid and, as such, empty
            //breaks out to avoid running code against an invalid route
            if (newPath.empty()) {
                tempPath = newPath;
                break;
            }
        }
    }
    
    //unsigned end = depotToFirst[1].second.back();
    path = tempPath;

    return path;
}
}*/

/*
std::vector<unsigned> streetNet::findRouteO3(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty) {
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    unsigned deliveriesSize = deliveries.size();
    //std::vector<unsigned> dropOffs;
    //std::vector<unsigned> pickUps;
    std::vector<unsigned> allStops;
    allStops.reserve(deliveriesSize);
    
    //first -> ==!0 => pickup, ==0 => dropoff ; second = dropoff available or not
    //std::unordered_map< unsigned, std::pair< unsigned, bool> > hash;
    //hash.reserve(deliveriesSize);
    
#ifdef profile 
    auto const setupStart = std::chrono::high_resolution_clock::now();
    std::cout << "Profiling integrated path =============================" << endl;
#endif
    for (unsigned i = 0; i < deliveriesSize; ++ i) {
        //pickUps.emplace_back(deliveries[i].pickUp);
        allStops.emplace_back(deliveries[i].pickUp);
        //dropOffs.emplace_back(deliveries[i].dropOff);
        allStops.emplace_back(deliveries[i].dropOff);
        
        pseudoHash[deliveries[i].pickUp] = std::make_pair(deliveries[i].dropOff, true);
        pseudoHash[deliveries[i].dropOff] = std::make_pair(0, false);
    }
#ifdef profile 
    auto const setupEnd = std::chrono::high_resolution_clock::now();
    auto const setupTime = 
    std::chrono::duration_cast<std::chrono::microseconds>(setupEnd - setupStart);
    std::cout << "Set up time is: " << setupTime.count() << "us" << endl;
#endif
    
#ifdef profile 
    auto const costStart = std::chrono::high_resolution_clock::now();
#endif
    //we first need to set up a costMatrix for fast cost look up
    //this holds travel time from every stop to every stop
    //look up cost as: costMatrix[start][end]
    std::vector<std::vector<double>> costMatrix;
    costMatrix.resize(getNumberOfIntersections());
    #pragma omp parallel for
    for (unsigned i = 0; i < allStops.size(); ++ i) {
        costMatrix[allStops[i]] = NodePathCosts(allStops[i], allStops, turn_penalty);
    }
#ifdef profile 
    auto const costEnd = std::chrono::high_resolution_clock::now();
    auto const costTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(costEnd - costStart);
    std::cout << "Set up cost matrix takes: " << costTime.count() << "ms" << endl;
#endif
    
#ifdef profile 
    auto const iterStart = std::chrono::high_resolution_clock::now();
#endif
    
    //First let's find out what is our base travel time;
    double bestCost = findRouteTime(allStops, costMatrix);
    
    
    bool timeOut = false;
    unsigned twoOptsCount = 0;
    unsigned iterationCount = 0;
    unsigned whileCount = 0;
    unsigned newRouteCount = 0;
    while (!timeOut) { //this runs for 20s exactly
        whileCount ++;
        //now we try 2-Opt
        
//        //first we try to find the 2 longest Edges
//        //edge is store as a pair<start, end>
//        std::pair<unsigned, unsigned> longestEdge = {0, 0};
//        std::pair<unsigned, unsigned> secondLongestEdge = {0, 0};
//        double longestEdgeCost = 0;
//        double secondLongestEdgeCost = 0;
//        for (unsigned i = 1; i < allStops.size(); ++ i) {
//            if (costMatrix[allStops[i-1]][allStops[i]] > secondLongestEdgeCost) {
//                if (costMatrix[allStops[i-1]][allStops[i]] > longestEdgeCost) {
//                    secondLongestEdgeCost = longestEdgeCost;
//                    longestEdgeCost = costMatrix[allStops[i-1]][allStops[i]];
//                    secondLongestEdge = longestEdge;
//                    longestEdge = {i-1, i};
//                }
//                else {
//                    secondLongestEdgeCost = costMatrix[allStops[i-1]][allStops[i]];
//                    secondLongestEdge = {i-1, i};
//                }
//            }
//        }
        
        if (allStops.size() < 3) break; 
        //Fuck it, we will run 2-opt on 2 random edges for now
        for (unsigned firstCut = 1; firstCut < (allStops.size()-1); ++ firstCut) {
            bool firstCutBreakFlag = false;
            for (unsigned secondCut = firstCut + 1; secondCut < allStops.size(); ++ secondCut) {
                twoOptsCount ++;
                //We cut our current path into 3 (pretty randomly)
                std::vector<unsigned> subRoute0(allStops.begin(), allStops.begin() + firstCut);
                std::vector<unsigned> subRoute1(allStops.begin() + firstCut ,
                                            allStops.begin() + secondCut);
                std::vector<unsigned> subRoute2(allStops.begin() + secondCut,
                                                    allStops.end());

                //now let's play with it
                //for now, lets just reverse the first subRoute and see
                std::reverse(subRoute1.begin(), subRoute1.end());
                std::vector<unsigned> newRoute;
                newRoute.insert(newRoute.end(), subRoute0.begin(), subRoute0.end());
                newRoute.insert(newRoute.end(), subRoute1.begin(), subRoute1.end());
                newRoute.insert(newRoute.end(), subRoute2.begin(), subRoute2.end());
                //check for legality fist
                #ifdef profile 
                    auto const LegStart = std::chrono::high_resolution_clock::now();
                #endif
                if (legalityCheck(newRoute, pseudoHash)) {
                #ifdef profile 
                    auto LegEnd = std::chrono::high_resolution_clock::now();
                    auto Leg =
                        std::chrono::duration_cast<std::chrono::microseconds> (
                        LegEnd - LegStart);
                    std::cout << "legality check takes: " << Leg.count() << "us" << endl;    
                #endif
                    //check weather the new path is batter
                    newRouteCount ++;
                    double newCost = findRouteTime(newRoute, costMatrix);
                    if (newCost < bestCost) {
                        bestCost = newCost;
                        allStops = newRoute;
                        iterationCount ++;
                        firstCutBreakFlag = true;
                        break;
                    }
                }
                
//                //try reverse middle subRoute
//                std::vector<unsigned> subRoute3(allStops.begin(), allStops.begin() + firstCut);
//                std::vector<unsigned> subRoute4(allStops.begin() + firstCut ,
//                                            allStops.begin() + secondCut);
//                std::vector<unsigned> subRoute5(allStops.begin() + secondCut,
//                                                    allStops.end());
//
//                
//                std::reverse(subRoute4.begin(), subRoute4.end());
//                newRoute.clear();
//                newRoute.insert(newRoute.end(), subRoute3.begin(), subRoute3.end());
//                newRoute.insert(newRoute.end(), subRoute4.begin(), subRoute4.end());
//                newRoute.insert(newRoute.end(), subRoute5.begin(), subRoute5.end());
//                //check for legality fist
//                if (routeLookingGood(newRoute, dropOff2pickUp)) {
//                    //check weather the new path is batter
//                    newRouteCount ++;
//                    double newCost = findRouteTime(newRoute, costMatrix);
//                    if (newCost < bestCost) {
//                        bestCost = newCost;
//                        allStops = newRoute;
//                        iterationCount ++;
//                        firstCutBreakFlag = true;
//                        break;
//                    }
//                }
//                
//                //try reverse last subRoute
//                std::vector<unsigned> subRoute6(allStops.begin(), allStops.begin() + firstCut);
//                std::vector<unsigned> subRoute7(allStops.begin() + firstCut ,
//                                            allStops.begin() + secondCut);
//                std::vector<unsigned> subRoute8(allStops.begin() + secondCut,
//                                                    allStops.end());
//
//                
//                std::reverse(subRoute8.begin(), subRoute8.end());
//                newRoute.clear();
//                newRoute.insert(newRoute.end(), subRoute6.begin(), subRoute6.end());
//                newRoute.insert(newRoute.end(), subRoute7.begin(), subRoute7.end());
//                newRoute.insert(newRoute.end(), subRoute8.begin(), subRoute8.end());
//                //check for legality fist
//                if (routeLookingGood(newRoute, dropOff2pickUp)) {
//                    //check weather the new path is batter
//                    newRouteCount ++;
//                    double newCost = findRouteTime(newRoute, costMatrix);
//                    if (newCost < bestCost) {
//                        bestCost = newCost;
//                        allStops = newRoute;
//                        iterationCount ++;
//                        firstCutBreakFlag = true;
//                        break;
//                    }
//                }
            }
            if (firstCutBreakFlag) break;
            
        }
        
     //Wall clock time is handled here
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto wallClock =
                    std::chrono::duration_cast<std::chrono::duration<double>> (
                    currentTime - startTime);
        if (wallClock.count() > 20) {
            timeOut = true;
            std::cout << "Ran 2-Opts " << twoOptsCount << " times, " 
                    << "Found " << newRouteCount << " new route(s), "
                    << "Repeating " << whileCount - iterationCount << " times, "
                    << "with " << iterationCount << " iteration(s)" << endl;
        }
    }
#ifdef profile 
    auto const iterEnd = std::chrono::high_resolution_clock::now();
    auto const iterTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(iterEnd - iterStart);
    std::cout << "Iteration takes: " << iterTime.count() << "ms" << endl;
#endif
    
#ifdef profile 
    auto const pathStart = std::chrono::high_resolution_clock::now();
#endif
    //now we build the path from allStops
    // first pick a start depot
    unsigned startDepot;
    unsigned endDepot;
    path2Nodes(allStops[0], depots, turn_penalty, startDepot);
    
    std::vector<std::vector<unsigned>> pathSet;
    pathSet.resize(getNumberOfIntersections());
    #pragma omp parallel for
    for (unsigned i = 1; i < allStops.size(); ++ i) {
        pathSet[i-1] = getPathFromIntersec(allStops[i-1], allStops[i],
                                                turn_penalty);
    }
    
    std::vector<unsigned> finalRoute = getPathFromIntersec(startDepot, allStops[0],
                                                turn_penalty);
#ifdef profile 
    auto const traceStart = std::chrono::high_resolution_clock::now();
#endif
    for (unsigned i = 1; i < allStops.size(); ++ i) {
#ifdef profile 
    auto const DKStart = std::chrono::high_resolution_clock::now();
#endif
        std::vector<unsigned> newpath = pathSet[i-1];
        if (newpath.empty()) continue;
#ifdef profile 
    auto const DKEnd = std::chrono::high_resolution_clock::now();
    auto const DKTime = 
    std::chrono::duration_cast<std::chrono::nanoseconds>(DKEnd - DKStart);
    std::cout << "      " << i << " ." << endl;
    std::cout << "      DK back takes: " << DKTime.count() << "ns" << endl;
#endif
#ifdef profile 
    auto const PopStart = std::chrono::high_resolution_clock::now();
#endif
        finalRoute.pop_back();
#ifdef profile 
    auto const PopEnd = std::chrono::high_resolution_clock::now();
    auto const PopTime = 
    std::chrono::duration_cast<std::chrono::nanoseconds>(PopEnd - PopStart);
    std::cout << "      Pop back takes: " << PopTime.count() << "ns" << endl;
#endif
#ifdef profile 
    auto const insStart = std::chrono::high_resolution_clock::now();
#endif
        finalRoute.insert(finalRoute.end(), newpath.begin(), newpath.end());
#ifdef profile 
    auto const insEnd = std::chrono::high_resolution_clock::now();
    auto const insTime = 
    std::chrono::duration_cast<std::chrono::nanoseconds>(insEnd - insStart);
    std::cout << "      Inserting takes: " << insTime.count() << "ns" << endl;
#endif
    }
#ifdef profile 
    auto const traceEnd = std::chrono::high_resolution_clock::now();
    auto const traceTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(traceEnd - traceStart);
    std::cout << "  Trace back takes: " << traceTime.count() << "ms" << endl;
#endif
    std::vector<unsigned> goHome = path2Nodes(allStops[allStops.size()-1], depots,
                                                turn_penalty, endDepot);
    
    finalRoute.pop_back();
    finalRoute.insert(finalRoute.end(), goHome.begin(), goHome.end());
#ifdef profile 
    auto const pathEnd = std::chrono::high_resolution_clock::now();
    auto const pathTime = 
    std::chrono::duration_cast<std::chrono::milliseconds>(pathEnd - pathStart);
    std::cout << "Building final path takes: " << pathTime.count() << "ms" << endl;
#endif

    return finalRoute;
}
*/

/*
std::vector<unsigned> streetNet::findRouteAlina(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty) {
    std::vector<unsigned> nextStopPool;
    std::vector<unsigned> index;
    std::vector<unsigned> routeSoFar;//full routine
    std::vector<unsigned> PickUp; // need to pick up
    std::vector<unsigned> DropOff;//allowed to drop of
    std::vector<unsigned> InittPath;//from start depot to first pick up
    double time = 0;
    unsigned currentDepot;
    unsigned currentposi;
    unsigned temp;
    for(unsigned i = 0 ; i < deliveries.size(); ++ i){
        PickUp.push_back(deliveries[i].pickUp);
        nextStopPool.push_back(deliveries[i].pickUp);
        index[deliveries[i].pickUp]= i;
        DropOff[deliveries[i].pickUp]= deliveries[i].dropOff;
        std::vector<unsigned> StartPath = path2Nodes(deliveries[i].pickUp, depots, 
                                            turn_penalty, currentDepot);
        if(time!=0){
            if (time> streetNet::getInstance()->pathTravelTime(StartPath, turn_penalty)){
                InittPath = StartPath;
                currentposi = deliveries[i].pickUp;
            }
        }
        else time = streetNet::getInstance()->pathTravelTime(StartPath, turn_penalty);       
    }
       
   
    //initialize the start depot    
    routeSoFar.insert(routeSoFar.end(),InittPath.begin(),InittPath.end());
    temp = nextStopPool[nextStopPool.size()-1];
    nextStopPool[nextStopPool.size()-1]=nextStopPool[index[currentposi]];
    nextStopPool[index[currentposi]] = temp;
    nextStopPool.pop_back();
    nextStopPool.push_back(DropOff[currentposi]);
    
    //start finding path
    while(nextStopPool.size()!=0){
        std::vector<unsigned> tempPath = path2Nodes(currentposi, nextStopPool, 
                                            turn_penalty, currentDepot);
        routeSoFar.pop_back();
        routeSoFar.insert(routeSoFar.end(), tempPath.begin(), tempPath.end());
        
        unsigned j = nextStopPool.size() ;
        unsigned ind = 0;
        while(j = nextStopPool.size()){
            if(nextStopPool[ind] == currentDepot){
                temp = nextStopPool[nextStopPool.size()-1];
                nextStopPool[nextStopPool.size()-1]=nextStopPool[j];
                nextStopPool[j] = temp;
                nextStopPool.pop_back();                
            }
            ind ++;
       }
        if(std::find(PickUp.begin(), PickUp.end(), 
                        currentDepot) != PickUp.end()){
                    nextStopPool.push_back(DropOff[currentDepot]);
                }
        currentposi = currentDepot;
    }
    
    // find the closest depot as ending
    unsigned endDepot;
    std::vector<unsigned> FinalPath = path2Nodes(currentposi, depots, 
                                            turn_penalty, endDepot);
    routeSoFar.insert(routeSoFar.end(), FinalPath.begin(), FinalPath.end());
    routeSoFar.push_back(endDepot);
    
    return routeSoFar;
}

std::vector<unsigned> streetNet::findRouteKevin(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty) {
    std::vector<unsigned> bestRoute;
    double bestTime = 99999999;
    #pragma omp parallel for 
    for (unsigned s = 0; s < depots.size() ; ++s) {//&& s < 18
        //a pool of posiable next stop
        std::vector<unsigned> nextStopPool;
        std::vector<unsigned> routeSoFar;

        unsigned currentStop;
        std::vector<unsigned> numPickUp;
        numPickUp.resize(getNumberOfIntersections(), 0);
        std::unordered_map<unsigned, std::vector<unsigned>> pickUp2dropOff;

        //now let set up the begining:
        //first, we selected a depot to start with
        unsigned startDepot = depots[s];
//        path2Nodes(deliveries[0].pickUp, depots, turn_penalty, startDepot);
//        if (startDepot == 0xffffffff) return routeSoFar;
        currentStop = startDepot;
        routeSoFar.push_back(currentStop);
        //now that we have nothing in our pocket, all the pick-ups are possiable
        //next stop
        for (unsigned i = 0; i < deliveries.size(); ++ i) {
            nextStopPool.push_back(deliveries[i].pickUp);
            //let's set up StopPickUp in the same time
    //        StopPickUp[deliveries[i].dropOff] = false;
            numPickUp[deliveries[i].pickUp] += 1;  
            pickUp2dropOff[deliveries[i].pickUp].push_back(deliveries[i].dropOff);
        }

        //OK, now that we are done with start, let's start finding route
        while (!nextStopPool.empty()) {
            unsigned newStop;
            std::vector<unsigned> newPath = path2Nodes(currentStop, nextStopPool, 
                    turn_penalty, newStop);

            std::vector<unsigned>::iterator newStopPt 
                    = std::find(nextStopPool.begin(), nextStopPool.end(), newStop);
            if (newStopPt != nextStopPool.end())
                nextStopPool.erase(newStopPt);
            if (newPath.empty()) {
                routeSoFar = newPath;
                break;
            }

            routeSoFar.pop_back();
            routeSoFar.insert(routeSoFar.end(), newPath.begin(), newPath.end());
            //if a pick-up is reached, its drop-off become a possiable next stop
            if (numPickUp[newStop] > 0) {
                nextStopPool.push_back(pickUp2dropOff[newStop][pickUp2dropOff[newStop].size()-1]);
                pickUp2dropOff[newStop].pop_back();
                numPickUp[newStop] -= 1;
            }
            currentStop = newStop;
        }
        if (routeSoFar.empty()) continue;

        //Sweet, we finished our delivery!!!!
        //now we just need to find the closest depot to go back to
        unsigned endDepot;
        std::vector<unsigned> FinalPath = path2Nodes(currentStop, depots, 
                                                turn_penalty, endDepot);
        routeSoFar.pop_back();
        routeSoFar.insert(routeSoFar.end(), FinalPath.begin(), FinalPath.end());
    //    routeSoFar.push_back(endDepot);
        
        double currentTime = pathTravelTime(routeSoFar, turn_penalty);
        if (currentTime < bestTime) {
            bestTime = currentTime;
            bestRoute = routeSoFar;
        }
    }

    return bestRoute;
}
*/
