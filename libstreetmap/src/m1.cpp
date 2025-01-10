#include "m1.h"     //m1.h spec file
#include "streetNet.h"  //global data class header
#include "nnAdapter.h"  //Custom adapter/wrapper for the NN library
#include "mappinglib.h"


//Global class that loads/stores all the data.

//Loads a map streets.bin file. Returns true if successful, false if some error
//occurs and the map can't be loaded.
bool load_map(std::string map_path) {
    close_map();
    
    //this real is just try to be compatable to the autotester
    if (map_path.compare("/cad2/ece297s/public/maps/toronto_canada.streets.bin") == 0) {
        pathOSM = "toronto_canada.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/london_england.streets.bin") == 0) {
        pathOSM = "london_england.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/saint-helena.streets.bin") == 0) {
        pathOSM = "saint-helena.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/new-york_usa.streets.bin") == 0) {
        pathOSM = "new-york_usa.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/tokyo_japan.streets.bin") == 0) {
        pathOSM = "tokyo_japan.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/beijing_china.streets.bin") == 0) {
        pathOSM = "beijing_china.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/cairo_egypt.streets.bin") == 0) {
        pathOSM = "cairo_egypt.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/cape-town_south-africa.streets.bin") == 0) {
        pathOSM = "cape-town_south-africa.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/golden-horseshoe_canada.streets.bin") == 0) {
        pathOSM = "golden-horseshoe_canada.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/hamilton_canada.streets.streets.bin") == 0) {
        pathOSM = "hamilton_canada.streets.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/hong-kong_china.streets.bin") == 0) {
        pathOSM = "hong-kong_china.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/iceland.streets.bin") == 0) {
        pathOSM = "iceland.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/moscow_russia.streets.bin") == 0) {
        pathOSM = "moscow_russia.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/new-delhi_india.streets.bin") == 0) {
        pathOSM = "new-delhi_india.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/rio-de-janeiro_brazil.streets.bin") == 0) {
        pathOSM = "rio-de-janeiro_brazil.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/saint-helena.streets.bin") == 0) {
        pathOSM = "saint-helena.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/singapore.streets.streets.bin") == 0) {
        pathOSM = "singapore.streets.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/sydney_australia.streets.bin") == 0) {
        pathOSM = "sydney_australia.osm.bin";
    }
    else if (map_path.compare("/cad2/ece297s/public/maps/tehran_iran.streets.bin") == 0) {
        pathOSM = "tehran_iran.osm.bin";
    }
    
    loadOSMDatabaseBIN("/cad2/ece297s/public/maps/" + pathOSM);
    //Load your map related data structures here
    bool success = loadStreetsDatabaseBIN(map_path);
    
    //if load failed return bool false
    if (!success) return success;
    //this will set up everything inside the streetNet::getInstance()
    //Initialize the global streetNet::getInstance()
    streetNet::getInstance()->load_data();
    mapLoaded = true;
    return success;
}


//Close the map (if loaded)
void close_map() {
    if (!mapLoaded)return;
    //Clean-up your map related data structures here
    if (streetNet::getInstance() != NULL){
        unsigned FeatNum = getNumberOfFeatures();   //speed
        for (unsigned i = 0 ; i < FeatNum; i++) {
            delete [] streetNet::getInstance()->getFeasture(i).pointArray;
        }
        delete streetNet::getInstance();
        closeStreetDatabase();
    }
    closeOSMDatabase();
    mapLoaded = false;
    
    
    //delete [] pointArray;
}


std::vector<unsigned> find_street_ids_from_name(std::string street_name) {
    
    return streetNet::getInstance()->getStreetIDByName(street_name);
}

std::vector<unsigned> find_intersection_street_segments(unsigned intersection_id) {
    
    return streetNet::getInstance()->getSegByIntersec(intersection_id);
}

std::vector<std::string> find_intersection_street_names(unsigned intersection_id) {
    
    return streetNet::getInstance()->getStreetByIntersection(intersection_id);
}


bool are_directly_connected(unsigned intersection_id1, unsigned intersection_id2) {
    
    return streetNet::getInstance()->intersecDirectlyConected(intersection_id1, intersection_id2);
}

std::vector<unsigned> find_adjacent_intersections(unsigned intersection_id){
    
    return streetNet::getInstance()->listIntersecDirectlyConected(intersection_id);
}


//Returns all street segments for the given street
std::vector<unsigned> find_street_street_segments(unsigned street_id){
    
    return streetNet::getInstance()->getSegInStreet(street_id);
}


//Returns all intersections along the a given street
std::vector<unsigned> find_all_street_intersections(unsigned street_id){
    
    return streetNet::getInstance()->getIntersecInStreet(street_id);
}


//Return all intersection ids for two intersecting streets
//This function will typically return one intersection id.
//However street names are not guaranteed to be unique, so more than 1 intersection id may exist
std::vector<unsigned> find_intersection_ids_from_street_names
            (std::string street_name1, std::string street_name2){ 
    
    return streetNet::getInstance()->getIntersecByStName(street_name1, street_name2);
}


double find_distance_between_two_points(LatLon point1, LatLon point2){   
    
    return distance_between_points(point1, point2);
}


double find_street_segment_length(unsigned street_segment_id){
    
    return streetNet::getInstance()->getSegLength(street_segment_id);
}


//Returns the length of the specified street in meters
double find_street_length(unsigned street_id){
    
    return streetNet::getInstance()->getStreetLength(street_id);
}


//Returns the travel time to drive a street segment in seconds (time = distance/speed_limit)
double find_street_segment_travel_time(unsigned street_segment_id){
    
    return streetNet::getInstance()->getTravelTime(street_segment_id);
}


//Returns the nearest point of interest to the given position
unsigned find_closest_point_of_interest(LatLon my_position){
    double x, y;
    x = (my_position.lon() * DEG_TO_RAD) * streetNet::getInstance()->getInterAveLat();//SHOULD BE POI AVE LAT LOOK INTO THIS
    y = my_position.lat() * DEG_TO_RAD;
    
    //Performs a kd-tree search on the pre-built data structure
    return streetNet::getInstance()->kdtree_NN('p', x, y, my_position);;//clPOIt;
}


//Returns the the nearest intersection to the given position
unsigned find_closest_intersection(LatLon my_position){
    double x, y;
    x = (my_position.lon() * DEG_TO_RAD) * streetNet::getInstance()->getInterAveLat();
    y = my_position.lat() * DEG_TO_RAD;
    
    //Performs a kd-tree search on the pre-built data structure
    return streetNet::getInstance()->kdtree_NN('i', x, y, my_position);
}
