#include <utils/common/IntVector.h>
#include "NIVissimClosedLaneDef.h"


NIVissimClosedLaneDef::NIVissimClosedLaneDef(int lane,
                                             const IntVector &assignedVehicles)
    : myLaneNo(lane), myAssignedVehicles(assignedVehicles)
{
}


NIVissimClosedLaneDef::~NIVissimClosedLaneDef()
{
}


