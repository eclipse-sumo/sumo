#include <vector>
#include <cmath>
#include <string>
#include <microsim/MSEdge.h>
#include <microsim/MSJunction.h>
#include "GUILane.h"
#include "GUIEdge.h"

GUIEdge::GUIEdge(std::string id)
    : MSEdge(id)
{
}


GUIEdge::~GUIEdge()
{
}

void
GUIEdge::initJunctions(MSJunction *from, MSJunction *to,
                       EdgeBasicFunction function)
{
    // set the information about the nodes
    //  !!! not longer needed
    _from = from;
    _to = to;
    // set the function information
    _function = function;
    // set the geomertical information for every lane
    double x1 = fromXPos();
    double y1 = -fromYPos();
    double x2 = toXPos();
    double y2 = -toYPos();
    double length = sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
    std::pair<double, double> offsets = 
        getLaneOffsets(x1, y1, x2, y2, length, 3.5);
    double xoff = offsets.first;
    double yoff = offsets.second;
    for(LaneCont::iterator i=myLanes->begin(); i<myLanes->end(); i++) {
        static_cast<GUILane*>(*i)->setPosition(x1+xoff, y1+yoff, x2+xoff, y2+yoff);
        xoff += offsets.first;
        yoff += offsets.second;
    }
}

std::pair<double, double>
GUIEdge::getLaneOffsets(double x1, double y1, 
                        double x2, double y2, 
                        double prev, double wanted) const // !!! not really a part of the edge
{
    double dx = x1 - x2;
    double dy = y1 - y2;
    if(dx<0) {
        if(dy<0) {
            return std::pair<double, double>(-dy*wanted/prev, -dx*wanted/prev);
        } else {
            return std::pair<double, double>(-dy*wanted/prev, dx*wanted/prev);
        }
    } else {
        if(dy<0) {
            return std::pair<double, double>(-dy*wanted/prev, -dx*wanted/prev);
        } else {
            return std::pair<double, double>(-dy*wanted/prev, dx*wanted/prev);
        }
    }
}


/*
std::pair<Position2D, Position2D>
GUIEdge::getLanePos(size_t laneNo) const
{
    return static_cast<GUILane*>((*myLanes)[laneNo])->pos();
}


const MSLane::VehCont &
GUIEdge::getLaneVehicles(size_t laneNo) const
{
    return static_cast<GUILane*>((*myLanes)[laneNo])->getVehicles();
}*/


GUILane &
GUIEdge::getLane(size_t laneNo)
{
    return *(static_cast<GUILane*>((*myLanes)[laneNo]));
}



std::vector<std::string>
GUIEdge::getNames()
{
    std::vector<std::string> ret;
    ret.reserve(MSEdge::myDict.size());
    for(MSEdge::DictType::iterator i=MSEdge::myDict.begin();
        i!=MSEdge::myDict.end(); i++) {
        ret.push_back((*i).first);
    }
    return ret;
}


double
GUIEdge::toXPos() const
{
    return _to->getXCoordinate();
}


double
GUIEdge::fromXPos() const
{
    return _from->getXCoordinate();
}


double
GUIEdge::toYPos() const
{
    return _to->getYCoordinate();
}


double
GUIEdge::fromYPos() const
{
    return _from->getYCoordinate();
}


std::string 
GUIEdge::getID() const
{
    return myID;
}


GUIEdge::EdgeBasicFunction 
GUIEdge::getPurpose() const
{
    return _function;
}

/*
void
GUIEdge::moveExceptFirstSingle()
{
    GUILane *lane = (static_cast<GUILane*>(*(myLanes->begin())));
    (static_cast<GUILane*>(*(myLanes->begin())))->moveExceptFirst();
}
*/
