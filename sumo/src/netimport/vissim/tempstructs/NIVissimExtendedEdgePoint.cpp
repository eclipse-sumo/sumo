#include <utils/common/IntVector.h>
#include "NIVissimExtendedEdgePoint.h"
#include "NIVissimEdge.h"

NIVissimExtendedEdgePoint::NIVissimExtendedEdgePoint(
        int edgeid, IntVector &lanes, double position,
        IntVector &assignedVehicles)
    : myEdgeID(edgeid), myLanes(lanes), myPosition(position),
    myAssignedVehicles(assignedVehicles)
{
}


NIVissimExtendedEdgePoint::~NIVissimExtendedEdgePoint()
{
}


int
NIVissimExtendedEdgePoint::getEdgeID() const
{
    return myEdgeID;
}


double
NIVissimExtendedEdgePoint::getPosition() const
{
    return myPosition;
}


Position2D
NIVissimExtendedEdgePoint::getGeomPosition() const
{
    return
        NIVissimAbstractEdge::dictionary(myEdgeID)->getGeomPosition(myPosition);
}


const IntVector &
NIVissimExtendedEdgePoint::getLanes() const
{
    return myLanes;
}
