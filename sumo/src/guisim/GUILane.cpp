#include <string>
#include <utility>
#include <utils/qutils/NewQMutex.h>
#include <microsim/MSLane.h>
#include <utils/geom/Position2D.h>
#include "GUILane.h"

GUILane::GUILane( std::string id, double maxSpeed, double length, 
                 MSEdge* edge )
    : MSLane(id, maxSpeed, length, edge)
{
}


GUILane::~GUILane()
{
}


void 
GUILane::setPosition(double x1, double y1, double x2, double y2)
{
    _begin = Position2D(x1, y1);
    _end = Position2D(x2, y2);
    _direction = Position2D((x1-x2)/myLength, (y1-y2)/myLength);
    _rotation = acos((x1-x2)/myLength)*180/3.14159265;
    int tmpRot = _rotation;
    if(tmpRot==90||tmpRot==89) {
        if(y1<y2) {
            _rotation = -90;
        }
    }
}

/*
const std::pair<Position2D, Position2D> &
GUILane::getPos() const
{
    return _position;
}*/

const MSLane::VehCont &
GUILane::getVehiclesLocked() 
{
    _lock.lock();
    return myVehicles;
}

double
GUILane::getLength() const
{
    return myLength;
}

const Position2D &
GUILane::getBegin() const
{
    return _begin;
}


const Position2D &
GUILane::getEnd() const
{
    return _end;
}


const Position2D &
GUILane::getDirection() const
{
    return _direction;
}


double
GUILane::getRotation() const
{
    return _rotation;
}


void 
GUILane::unlockVehicles()
{
    _lock.unlock();
}


void 
GUILane::moveExceptFirst()
{
    _lock.lock();
    MSLane::moveExceptFirst();
    _lock.unlock();
}


void 
GUILane::moveExceptFirst(MSEdge::LaneCont::const_iterator firstNeighLane,
                         MSEdge::LaneCont::const_iterator lastNeighLane )
{
    _lock.lock();
    MSLane::moveExceptFirst(firstNeighLane, lastNeighLane);
    _lock.unlock();
}


bool 
GUILane::emit( MSVehicle& newVeh )
{
    _lock.lock();
    bool ret = MSLane::emit(newVeh);
    _lock.unlock();
    return ret;
}


bool 
GUILane::isEmissionSuccess( MSVehicle* aVehicle )
{
    _lock.lock();
    bool ret = MSLane::isEmissionSuccess(aVehicle);
    _lock.unlock();
    return ret;
}


void 
GUILane::moveFirst( bool respond )
{
    _lock.lock();
    MSLane::moveFirst(respond);
    _lock.unlock();
}

