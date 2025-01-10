#include "m3.h"
#include "m1.h"
#include <vector>
#include <string>
#include "streetNet.h"

using namespace std;

double compute_path_travel_time(const std::vector<unsigned>& path, 
                                const double turn_penalty) {
    return streetNet::getInstance()->pathTravelTime(path, turn_penalty);
}


std::vector<unsigned> find_path_between_intersections(const unsigned intersect_id_start, 
                                                      const unsigned intersect_id_end,
                                                      const double turn_penalty) {
    return streetNet::getInstance()
            ->nodePath2edgePath(streetNet::getInstance()
            ->getPathFromIntersec(intersect_id_start, intersect_id_end, turn_penalty, false, true),
            turn_penalty);
}


std::vector<unsigned> find_path_to_point_of_interest(const unsigned intersect_id_start, 
                                               const std::string point_of_interest_name,
                                               const double turn_penalty) {
//    LatLon startLoc = streetNet::getInstance()
//            ->getIntersecByID(intersect_id_start).get_location();
//    unsigned endPOI = streetNet::getInstance()->closestPOIbyName(point_of_interest_name, startLoc);
//    
//    unsigned endInter = find_closest_intersection(streetNet::getInstance()
//            ->getPOIByID(endPOI).getLocation());
//    
//    return find_path_between_intersections(intersect_id_start, endInter, turn_penalty);
    return streetNet::getInstance()
                ->nodePath2edgePath(streetNet::getInstance()
                ->inter2POIpath(intersect_id_start, point_of_interest_name, turn_penalty),
            turn_penalty);
}