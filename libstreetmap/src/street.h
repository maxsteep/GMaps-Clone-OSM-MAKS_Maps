#pragma once


#include "StreetsDatabaseAPI.h" //Layer 2 OSM API



//Stores information regarding street names through street IDs
class street
{
private:
    std::string name;
    unsigned streetID;
    
//protected:
//    unsigned longest;
    
    
public:
    street();
    
    
    street(unsigned streetID_);
    
    std::string getStName();
    
    
    //unsigned returnLongestSegment();
    
    
    //friends list = class XXX can freely access data
    friend class streetNet;
    //friend class streetSegment;
};
