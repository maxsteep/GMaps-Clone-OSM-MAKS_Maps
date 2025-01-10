//Group 062, #Max#imum effort

//Max
//Alina :)
//Kevin

//Mapping project rev 1.0 

/*Our mapping software uses a pre-processed binary database via osm2bin from the OpenStreetMaps to query and provide 
 relevant geographical and commercial information to the user. Across the four milestones we shall implement a full-range
 of functionality a user is both familiar with and would expect from a state of the art GIS mapping solution.
 Full graphical interface along with detailed information for both route, destination and potential Points of Interest shall be
 provided in the final release. We shall be using both Layer 1 and 2 of the StreetsDatabaseAPI*/


#include <iostream>
#include <string>
#include "StreetsDatabaseAPI.h"
#include "m1.h"
#include "m2.h"
#include "streetNet.h"
#include "OSMDatabaseAPI.h"

#include "m1.h"
#include "customHelperFunctions.h"

using namespace std;

int main(int argc, char** argv) {
    draw_map();

    return 0;
}
