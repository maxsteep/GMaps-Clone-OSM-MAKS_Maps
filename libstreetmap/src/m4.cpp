#include <vector>
#include "m4.h"
#include "streetNet.h"
#include "mappinglib.h"




std::vector<unsigned> traveling_courier(const std::vector<DeliveryInfo>& deliveries, 
                                        const std::vector<unsigned>& depots, 
                                        const float turn_penalty) {
    return streetNet::getInstance()
            ->nodePath2edgePath(streetNet::getInstance()
            ->findRouteO3(deliveries, depots, turn_penalty), turn_penalty);
    //findRoute //findRouteO3
}

