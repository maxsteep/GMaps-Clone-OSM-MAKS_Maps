#include <iostream>
#include <unittest++/UnitTest++.h>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <string>
#include "graphics.h"
#include "m1.h"
#include "streetNet.h"
#include "mappinglib.h"

#include "StreetsDatabaseAPI.h"


using namespace std;

int main() {
    //Run the unit tests
    int num_failures = UnitTest::RunAllTests();
    
    

    drawAll();
    
    
    
    //Return the number of failures encountered while testing our custom code
    return num_failures;
}

//==============================================================================
//Start helper functions 



