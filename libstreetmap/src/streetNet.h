#pragma once


#include <unordered_map>
#include <cstdlib>
#include <map>

#include "m1.h" //m1.h spec file
#include "m4.h" //m1.h spec file

#include "nnAdapter.h"  //Custom adapter/wrapper for the NN library

//ancillary class headers, necessary for the realization of the top level master data class
#include "streetSegment.h"
#include "street.h"
#include "POI.h"
#include "Intersection.h"
#include "feature.h"
#include "Point.h"
#include "graphics.h"
#include "OSMDatabaseAPI.h"
#include "m4.h"
#include <chrono>

static bool mapLoaded = false;

struct trunInfo;

/******************************Quick Start**************************************
 * StreetNet is a graph based data structure with 4 different kind of node:
 *       
 */


class streetNet
{
private:
    //this constant is used for distance calculation
    double InterAveLat; //average intersection latitude
    double POIAveLat;   //average POI latitude
    double sumIntLat = 0;
    double sumPOILat = 0;
    
    //this is apoximate range of the this city
    std::pair<double, double> rangeLL; //Lower Left 
    std::pair<double, double> rangeTR; //Top Right
    
    
    //OSMNode data structure ****************************************************
    std::unordered_map<OSMID, const OSMNode*> nodeMasterTable;
    
    //segment data structures***************************************************
    
    //this is a vector of all -segment objects-
    //this list is ordered by segment ID
    std::vector<streetSegment>  segMasterList;
    
//-------------------------NN---------------
    //need size of the entire structure to build the kdtree index
    unsigned coordSegMasterListSIZE = 0;
    
    //this is a vector of vectors network(2D matrix), it stores all segments
    //in the order of streets they belong to, ordered by street ID
    std::vector<std::vector<unsigned>> segNet;
    
    //vector of x,y projected coordinates for every curve point on every street segment, grouped per street segment id
    std::vector< std::pair< std::pair< double, double> , unsigned> > segSegStNet;

    //need size of the entire structure to build the kdtree index
    unsigned coordSegFeatListSIZE = 0;    
    
    //vector of x,y projected coordinates for every 'curve point' on every street feature, grouped per feature id
    std::vector< std::pair< std::pair< double, double> , unsigned> > segFeatNet;    
//-------------------------NN---------------    
    
    //I will return to this and re-implement it for prod
    //vector of st st seg lengths sorted per length
    //std::vector<std::vector<std::pair<double,unsigned>>> segListLengths;
    
    //this is a vector just for segment travel time
    //this is ordered by segID
    std::vector<double> segTimeList;
    
    //this is an Hash table of street type data 
    //key is its OSMID
    //value is its OSM tag[1] example: highway , motorway
    //std::unordered_map<OSMID, std::pair<std::string, std::string>> segInfoTable;
    std::unordered_map<OSMID, roadTypes> segInfoTable;
    
    //street data structure*****************************************************
    
    //this is the list of all street objects
    //this is ordered by street ID
    std::vector<street> streetMasterList;
    
    //this is a Hash Table of streets
    //it uses street name as key and store only a list of street IDs
    //note street name is not unique
    std::unordered_map<std::string, std::vector<unsigned>> streetTableByName;
    
    //this is a vector of all street name 
    //useful for auto correct
    std::vector<std::string> stNameList;
    
    
    //intersection data structure***********************************************
    
    //this is a list of all intersection objects
    //this is ordered by intersection ID
    std::vector<intersection> intersecMasterList;
    
    //this is a vector of vectors network(2D matrix) for intersections
    //this is ordered by streets
    std::vector<std::vector<unsigned>> intersecMatrix;
    
    
    //POI data structure********************************************************
    
    //this is the list of all POI in the map
    //this list is ordered by POI ID
    std::vector<POI> POIMasterList;
    
    //this is a hash table of POIs by name
    std::unordered_map<std::string, std::vector<unsigned>> POINameTable;
    
    //Feature data structures************************************************
    
    //this is the list of all features in the map
    //this list is ordered by featureID
    std::vector<feature> featureMasterList;
    
    //this is a hash table for a list of feature that have a valid name
    std::unordered_map<std::string, std::vector<unsigned>> featureTableByName;
    
    //this is a BST with all feature in order of area
    //std::multimap<double, unsigned> featureSizeTree;
    
    //Vector of feature areas, sorted in the decreasing order of area, contains the Layer 2 ID as .second
    std::vector<std::pair<double, unsigned>> featureAreas;
    
    //this is a list for subway station
    std::vector<subwayStation> subStList;
    
    //this is a list of street cart stop
    std::vector<streetCartStop> stCartStopList;
    
    //this is a list of all bus stop
    std::vector<busStop> busStopList;
    
    //this is a list of all ferry way
    std::vector<ferryWay> ferryList;
    
    //this is a list of all subway line
    std::vector<subway> subwayList;
    
    //this is a list of all streetCart line
    std::vector<streetCart> stCartList;
    
    //this is temperaly
    std::vector<std::string> commonKeyWord;
    
    //multi
    std::vector< std::vector< std::pair< unsigned, std::pair< bool, unsigned> > > > pseudoHash;
    //std::vector< std::pair< unsigned, bool> > pseudoHash;
    //std::vector< std::pair< unsigned, std::pair< bool, unsigned> > > pseudoHash;
    //std::vector<unsigned> counter;
    std::vector<unsigned> Pcounter;
    std::vector<unsigned> Dcounter;
    
    //WARNING!!!   
    //All private member beyond this point are Magic made with more Magic
    //I guess Max is a magician then ;)_________________________________________
    //==========================================================================    
    //Custom nanoflann wrapper data structures follow
    
    //NN data structure********************************************************
    
    //PointClouds are the x,y sets of double projected coordinates for the latlons for all the intersections/POIs/Features
    PointCloud<double> cloudInter;
    PointCloud<double> cloudPOI;
    PointCloud<double> cloudSegs;
    PointCloud<double> cloudFeat;
    
    PointCloud<double> cloudD;
    

    //KDTrees with indices against which the NN search iterates
    my_kd_tree_t<double> KDTreePOI = {2 /*dim*/, cloudPOI, nanoflann::KDTreeSingleIndexAdaptorParams(30 /* max leaf */) };
    my_kd_tree_t<double> KDTreeInter = {2 /*dim*/, cloudInter, nanoflann::KDTreeSingleIndexAdaptorParams(30 /* max leaf */) };
    my_kd_tree_t<double> KDTreeSegs = {2 /*dim*/, cloudSegs, nanoflann::KDTreeSingleIndexAdaptorParams(30 /* max leaf */) };
    my_kd_tree_t<double> KDTreeFeat = {2 /*dim*/, cloudFeat, nanoflann::KDTreeSingleIndexAdaptorParams(30 /* max leaf */) };
    
    my_kd_tree_t<double> KDTreeD = {2 /*dim*/, cloudD, nanoflann::KDTreeSingleIndexAdaptorParams(30 /* max leaf */) };
    
    std::vector<unsigned> resultsRadialNN;
    
    //NN data structure********************************************************

    /***************************************************************************
     * the following are extra data that queried from layer 1 API ||||||||||||||
     * =========================================================================
     */
    
    static streetNet* thisObjPtr;
    
public:
    
    //class level flags
    bool subwayON = false;
    bool stCartON = false;
    bool busON = false;
    
    bool restarantON = false;
    bool funPlaceON = false;
    bool icecreamON = false;
    bool doctorsON = false;
    bool cafeON = false;
    bool dentistON = false;
    bool hospitalON = false;
    bool condoON = false;
    bool clinicON = false;
    bool pharmacyON = false;
    bool parkingON = false;
    bool collegeON = false;
    bool bankON = false;
    bool childcareON = false;
    bool libraryON = false;
    bool pubON = false;
    bool barON = false;
    bool labON = false;
    bool schoolON = false;
    bool fuelON = false;
    bool policeON = false;
    bool atmON = false;
    bool theatreON = false;
    bool lawyerON = false;
    bool spaON = false;
    bool cinemaON = false;
    bool telephoneON = false;
    bool tutoringON = false;
    bool psychicON = false;
    bool veterinaryON = false;
    bool kindergartenON = false;
    
    
    //class level icon file
    Surface POIIcon;
    Surface SubIcon;
    Surface StCartIcon;
    Surface BusIcon;
    Surface RestaurantIcon;
    Surface FunIcon;
    Surface HospitalIcon;
    Surface NightLifeIcon;
    Surface CafeIcon;    
    Surface BankIcon;
    Surface LibraryIcon;
    Surface PoliceIcon;
    Surface ParkingIcon;
    Surface SchoolIcon;
    Surface GasIcon;
    Surface TheatreIcon;
    Surface AtmIcon;
    Surface PharmacyIcon;
    
    //streetWidth
    int defaultWidth;
    int HighwayWidth;
    int HighwayLinkWidth;
    int primeryWidth;
    
    
    // singleton implementation
    static streetNet* getInstance();
    
    // singleton implementation
    streetNet();
    ~streetNet();
    
    //this function must be called to construct the streetNet data structure
    //this will load all data from what ever map is loaded by loadStreetsDatabaseBIN
    void load_data();
    
    
    
    //Functional functions here*************************************************
    //functions here basically acted as a API for our own data structure
    
    //Returns the average latitude for all the intersections in the given data set 
    double getInterAveLat();
    
    //Returns the average latitude for all the POIs in the given data set
    double getPOIAveLat();
    
    //Return map range, pass(1,0) to get Lower Left, (0,1) to get Top Right
    std::pair<double, double> getRange(bool LL, bool TR);
    
    //Returns segment through ID using custom data structures
    streetSegment getSegByID(unsigned segID);
    
    //Returns street through ID using custom data structures
    street getStreetByID(unsigned stID);
    
    //Returns list of street ID by its name
    std::vector<unsigned> getStreetIDByName(std::string name);
    
    //return a intersection object by ID
    intersection getIntersecByID(unsigned interID);
    
    //get a POI by its ID
    POI getPOIByID(unsigned POIID);
    
    //find a list of POI by name
    std::vector<unsigned> findPOIbyName(std::string name);
    
    //give a location, find the closed POI with a name
    //return 0xffffffff if can not find any
    unsigned closestPOIbyName(std::string name, LatLon location);
    
    //this return a string of name of the street
    std::string getStreetName(unsigned stID);
    
    //Returns a list of segment ID of segments connected to the given intersection
    std::vector<unsigned> getSegByIntersec(unsigned intersecID);
    
    //Returns a list of street ID of streets connecting in the given intersection
    std::vector<std::string> getStreetByIntersection(unsigned intersecID);
    
    //Checks whether the given intersections are directly connected
    bool intersecDirectlyConected(unsigned intersecID1, unsigned intersecID2);
    
    //Returns a list of intersections that are directly connected
    std::vector<unsigned> listIntersecDirectlyConected(unsigned intersecID);
    
    //Returns a list of segments making up the given street
    std::vector<unsigned> getSegInStreet(unsigned streetID);
    
    //Returns a list of intersections for any given street 
    std::vector<unsigned> getIntersecInStreet(unsigned streetID);
    
    //find one intersection by two street IDs
    std::vector<unsigned> getIntersecByStreets(unsigned stID1, unsigned stID2);
    
    //find one or more intersections by two street names
    std::vector<unsigned> getIntersecByStName(std::string stName1, std::string stName2);
    
    //this fin a feature by name
    std::vector<unsigned> getFeatureByName(std::string name_);
    
    //this give access to a feature
    feature getFeasture(unsigned id);
    
    //Returns length of a given segment
    double getSegLength(unsigned segID);
    
    //Returns length of a given street
    double getStreetLength(unsigned streetID);
    
    //Returns the travel time for the given segment
    double getTravelTime(unsigned segID);
    
    //this find a seg by two intersection, return 99999999 if DNE
    unsigned findSegByInter(unsigned inter0, unsigned inter1);
    
    //Returns n largest segments for a given street
    //std::vector<std::pair<double,unsigned>> getLargestSegments(unsigned streetID_);
    
    //this return the info pair for segment
    roadTypes getSegInfo(OSMID roadTypes);
    
    
    //this let access to a OSMNode by OSMID
    const OSMNode* getOSMNode(OSMID nodeOSMID);
    
    //this will give a list of PIO that is within a range
    //std::vector<unsigned> nearByPOI(LatLon myPosition);   - Temporary placeholder, may or may not be kept(implemented)
    //Currently realized through custom KDTree search
    
    //this find the closest POI from a given point in LatLon
    //unsigned closestPOI(LatLon myPosition);   - Temporary placeholder, may or may not be kept(implemented)
    //Currently realized through custom KDTree search
    
    //Populates the PointClouds with---------------------------------------
    template <typename T>
    void loadPointCloud(PointCloud<T> &point, const size_t size, char type);
    
    template <typename T>
    void loadPointCloudS(PointCloud<T> &point, const size_t size, char type, const std::vector<DeliveryInfo>& deliveries, const std::vector<unsigned>& depots);

    //Builds the kdtree index to iterate against
    template <typename num_t>
    void loadKDTreeIndex(my_kd_tree_t<num_t> &index);

    //Performs nearest neighbour search using kd-trees for N (N being 1 for semantic reasons) closest neighbours
    unsigned kdtree_NN(char objectType, double x, double y, LatLon my_position);
    
    //Performs nearest neighbour search using kd-trees for radial closest neighbours
    std::vector<unsigned> radialNN(const char objectType, const double x, const double y, const double search_radius);
    
    std::string autoComplete(std::string input);
    
    
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //the following functions are used for drawing 
    
    void drawAllSeg();
    
    void drawFeature();
    
    void drawPOI();
    
    void hightLightSeg(unsigned segID);
    
    void PrintAreas();
    
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //the following are used for path finding
    
    //this return travel time of a path
    //assume path is leagel
    double pathTravelTime(const std::vector<unsigned>& segList, 
                                                const double trunCost);
    
    unsigned pathLength(const std::vector<unsigned>& segList);
    
    //this function compute the distance betweent two point
    //result is in [m]
    double heuristic(unsigned start, unsigned end);
    

    unsigned getNextIntersec(unsigned start, unsigned stSeg);
    
    //this only work with two directed connected intersection
    //this return the min time in between 
    double adjIntersecTravelTime(unsigned start, unsigned end, 
            std::vector<unsigned>& currentSt, unsigned& newSt, double trunCost);
    
    //this find a path in the form of a list of street segments
    //this take two intersections one as starting one as end
    std::vector<unsigned> getPathFromIntersec(unsigned start, unsigned end, 
                                    double turnCost, bool demo=false, bool Astar=false);
    
    //this is very interesting
    //this will find a path to the closest POI given by name
    //the path lead to the closest intersection to this POI
    std::vector<unsigned> inter2POIpath(unsigned start, std::string endPOI, 
                                    double turnCost, bool demo=false, bool Astar=false);
    
    //this convert a path of intersection into a path of segments
    //this assume fastest convertion 
    std::vector<unsigned> nodePath2edgePath(const std::vector<unsigned>& nodePath, double turncost);
    
    
    //this function generate a dirction info list from a edge path;
    std::vector<trunInfo> path2direction(const std::vector<unsigned>& edgePath);
    
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //Traveling Courier functions start here:
    
    //this is a modified path finding function witch take one start and multiple
    //ends, it return a path to the closest end by travel time
    //it also assign endStop the end it reached
    std::vector<unsigned> path2Nodes(unsigned start, std::vector<unsigned> ends, 
                                    double turnCost, unsigned& endStop);
    
    //this function calculate the cost in travel time from start to every ends
    //it return a hash table with ends ID as key, cost as value
    std::vector<double> NodePathCosts(unsigned start, std::vector<unsigned> ends, 
                                    std::vector<std::vector<unsigned>>& allPath, double turnCost);
    
    bool routeLookingGood(const std::vector<unsigned>& route,
                std::vector<std::vector<unsigned>>& pickUpLookUp);
    
    bool routeLookingGood2ndTry(const std::vector<unsigned>& route,
                    std::vector<std::vector<unsigned>>& pickUpLookUp);
    
    bool legalityCheck(const std::vector<unsigned>& route, std::vector< std::vector< std::pair< unsigned, std::pair< bool, unsigned> > > >& pseudoHash);//std::vector< std::pair< unsigned, bool> >& pseudoHash
    //bool legalityCheck(const std::vector<unsigned>& route, std::vector< std::vector< std::pair< unsigned, bool> > >& pseudoHash); //multi
    
    
    double findRouteTime(std::vector<unsigned>& route,
             std::vector<std::vector<double>>& costMatrix);
    
    std::vector<unsigned> findRouteGreedy(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        std::vector<std::vector<double>>& costMatrix, 
                                        const float turn_penalty);
    
    std::vector<unsigned> findRouteO3(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty);
    
    std::vector<unsigned> findRoute(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty);
    
    bool radiationBreeding(const std::vector<unsigned>& orignal, 
                std::vector<unsigned>& mutated,
                std::vector< std::vector< std::pair< unsigned, std::pair< bool, unsigned> > > >& pseudoHash,//std::vector< std::vector< std::pair< unsigned, bool> > >& pseudoHash,    multi
                unsigned firstCut, unsigned secondCut, int type, int order);
    //std::vector< std::pair< unsigned, bool> >& pseudoHash,
    
    bool getBetter(std::vector<unsigned>& orignal, 
                std::vector< std::vector< std::pair< unsigned, std::pair< bool, unsigned> > > >& pseudoHash,//std::vector< std::vector< std::pair< unsigned, bool> > >& pseudoHash,    multi
                std::vector<std::vector<double>>& costMatrix,
                unsigned& breedCount, unsigned& routeCount, double& bestCost,int reverse,
                int order, auto startTime, unsigned size);
    //std::vector< std::pair< unsigned, bool> >& pseudoHash,
    
    
    //std::vector<unsigned> findRouteAlina(const std::vector<DeliveryInfo>& deliveries, 
    //                                    const std::vector<unsigned>& depots, 
    //                                    const float turn_penalty);
    
    //std::vector<unsigned> findRouteMax(const std::vector<DeliveryInfo>& deliveries, 
    //                                    const std::vector<unsigned>& depots, 
    //                                    const float turn_penalty);
    
    //std::vector<unsigned> findRouteMax2(const std::vector<DeliveryInfo>& deliveries, 
    //                                    const std::vector<unsigned>& depots, 
    //                                    const float turn_penalty);
    
    //this function find a route 
    //std::vector<unsigned> findRouteKevin(const std::vector<DeliveryInfo>& deliveries, 
    //                                    const std::vector<unsigned>& depots, 
    //                                    const float turn_penalty);
    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    double distance_between_points_local(LatLon point1, LatLon point2);
};

//******************************************************************************
//this following structs are in help with priority queue
//this works, don't ask how!! **************************************************
struct neighbor
{
    unsigned ID;
    double time = 0; //time it take to reach this segment include this segment
    double localHeuristic = 0;
    
    neighbor(unsigned ID_, double time_, double localHeuristic_) : ID(ID_), time(time_)
                                        , localHeuristic(localHeuristic_) {}
};

struct closerNeighbor
{
    bool operator()(const neighbor& lhs, const neighbor& rhs) const
  {
    return (lhs.time + lhs.localHeuristic) > (rhs.time + rhs.localHeuristic);
  }
};
//******************************************************************************

enum trunType {
    Left,
    Right,
    Stright
};

struct trunInfo
{
    trunType turnDirection;
    unsigned stID;
    double nextTurn;

    
    trunInfo() {}
    trunInfo(trunType turnDirection_, unsigned stID_, double nextTurn_) 
        : turnDirection(turnDirection_), stID(stID_), nextTurn(nextTurn_) {}
};


double distance_between_points(LatLon point1, LatLon point2);


//v1 = {x1, y1, z1}
//v2 = {x2, y2, z2}
// this return va = {xa, ya, za} = v1 x v2; (3D vector cross product)
std::vector<double> crossProduct(std::vector<double> v1, std::vector<double> v2);
