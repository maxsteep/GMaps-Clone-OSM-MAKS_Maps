#include "mappinglib.h"
#include "debug.h"
#include "graphics.h"
#include "streetNet.h"
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include <X11/keysym.h>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <string>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <map>
#include <iostream>
#include <iterator>




using namespace std;

vector<unsigned> selectedInter;
//function definition:

unsigned selectSeg(unsigned inter0, unsigned inter1) {
    unsigned segment = streetNet::getInstance()->findSegByInter(inter0, inter1);
    if (segment != 99999999) {
        streetNet::getInstance()->hightLightSeg(segment);
    }
    return segment;
}


void highlightSegs() {
    for (unsigned i = 1; i < selectedInter.size(); ++ i) {
        unsigned valid = selectSeg(selectedInter[i-1], selectedInter[i]);
        if (valid == 99999999) return;
    }
}


void highlightInters(vector<unsigned> interList) {
    for (unsigned i = 0; i < interList.size(); ++ i) {
        streetNet::getInstance()->getIntersecByID(interList[i]).highlightPoint();
    }
}



namespace pathtest {
bool path_is_legal(const unsigned start_intersection, const unsigned end_intersection, const std::vector<unsigned>& path) {
    std::vector<unsigned> end_intersections = {end_intersection};

    return path_is_legal(start_intersection, end_intersections, path);
}

bool path_is_legal(const unsigned start_intersection, const std::vector<unsigned>& end_intersections, const std::vector<unsigned>& path) {
    //'start_intersection' is the intersection id of the starting intersection
    //'end_intersection' is the intersection id of the ending intersection
    //'path' is a vector street segment id's

    if (path.size() < 1) {
        cout << "Path is empty" << endl;
        return false; //If it is a valid path it must contain at-least one segment

    } else {
        //General case
        //To verify the path by walking along each segment checking:
        //  * That we are going a legal direction (i.e. not against one-ways)
        //  * That each segment is connected to the previous intersection
        //  * That the final segment is connected to a valid end_intersection
        //We start our intersection at the start_intersection
        assert(path.size() >= 1);

        unsigned curr_intersection = start_intersection;

        for (size_t i = 0; i < path.size(); i++) {
            StreetSegmentInfo seg_info = getStreetSegmentInfo(path[i]);

            //Are we moving forward or back along the segment?
            bool seg_traverse_forward;
            if (seg_info.from == curr_intersection) {
                //We take care to check 'from' first. This ensures
                //we get a reasonable traversal direction even in the 
                //case of a self-looping one-way segment

                //Moving forwards
                seg_traverse_forward = true;

            } else if (seg_info.to == curr_intersection) {
                //Moving backwards
                seg_traverse_forward = false;

            } else {
                assert(seg_info.from != curr_intersection && seg_info.to != curr_intersection);

                //This segment isn't connected to the current intersection
                cout << "segment: " << i << "isn't connected to intersection : " 
                        << curr_intersection << endl;
                return false;
            }

            //Are we going the wrong way along the segment?
            if (!seg_traverse_forward && seg_info.oneWay) {
                cout << "segment: " << i << "gose the wrong way" << endl;
                return false;
            }

            //Advance to the next intersection
            curr_intersection = (seg_traverse_forward) ? seg_info.to : seg_info.from;

        }

        //We should be at a valid end intersection
        auto iter = std::find(end_intersections.begin(), end_intersections.end(), curr_intersection);
        if (iter == end_intersections.end()) {
            //Not at a valid end intersection
            cout << "Path did not end at end point" << endl;
            return false;
        }

    }

    //Everything validated
    return true;
}

}