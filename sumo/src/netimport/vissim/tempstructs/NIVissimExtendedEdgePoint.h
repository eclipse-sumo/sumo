#ifndef NIVissimExtendedEdgePoint_h
#define NIVissimExtendedEdgePoint_h

#include <utils/common/IntVector.h>
#include <utils/geom/Position2D.h>

class NIVissimExtendedEdgePoint {
public:
    NIVissimExtendedEdgePoint(int edgeid, IntVector &lanes,
        double position, IntVector &assignedVehicles);
    ~NIVissimExtendedEdgePoint();
    int getEdgeID() const;
    double getPosition() const;
    Position2D getGeomPosition() const;
    const IntVector &getLanes() const;
private:
    int myEdgeID;
    IntVector myLanes;
    double myPosition;
    IntVector myAssignedVehicles;
};

#endif
