#include "street.h"
#include "streetNet.h"

street::street() {}

//Stores information regarding street names through street IDs
street::street(unsigned streetID_) : streetID(streetID_) {
    //find name
    name = getStreetName(streetID_);
    //find longest segment
    //std::vector<unsigned> stSegTemp = streetNet::getInstance()->getSegInStreet(streetID_);
    //std::cout << "-----------------size------------" << stSegTemp.size() << "------------end size----------" << std::endl;
    //for (unsigned i = 0; i < stSegTemp.size(); i++) {
    //    //std::cout << i << "-----------------size------------" << stSegTemp.size() << "------------end size----------" << std::endl;
    //    //std::cout << i << " num segment" << stSegTemp[i] << std::endl;
    //}
    //auto whichIt = std::max_element(streetNet::getInstance()->getSegInStreet(streetID).begin(), streetNet::getInstance()->getSegInStreet(streetID).end());
    //auto whichIt = std::max_element(stSegTemp.begin(), stSegTemp.end());
    //longest = std::distance(stSegTemp.begin(), whichIt);
    //std::cout << longest << std::endl;
}

//unsigned street::returnLongestSegment(){
//    return longest;
//}
std::string street::getStName() {
    return name;
}