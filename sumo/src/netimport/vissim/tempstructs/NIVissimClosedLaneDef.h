#ifndef NIVissimClosedLaneDef_h
#define NIVissimClosedLaneDef_h

#include <utils/common/IntVector.h>

class NIVissimClosedLaneDef {
public:
    NIVissimClosedLaneDef(int lane, const IntVector &assignedVehicles);
    ~NIVissimClosedLaneDef();
private:
    int myLaneNo;
    IntVector myAssignedVehicles;
};

#endif

