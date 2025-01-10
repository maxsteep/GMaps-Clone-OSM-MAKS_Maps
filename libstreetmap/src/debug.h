#ifndef DEBUG_H
#define DEBUG_H

#include "graphics.h"
#include <vector>
#include <thread>

using namespace std;

extern vector<unsigned> selectedInter;

//this find the segment from two intersections
//and highlight it
unsigned selectSeg(unsigned inter0, unsigned inter1);

void highlightSegs();

void highlightInters(vector<unsigned> interList);

namespace pathtest {

    bool path_is_legal(const unsigned start_intersection, const unsigned end_intersection, const std::vector<unsigned>& path);

    bool path_is_legal(const unsigned start_intersection, const std::vector<unsigned>& end_intersections, const std::vector<unsigned>& path);
    
}

#endif /* DEBUG_H */

