//---------------------------------------------------------------------------//
//                        ROEdge.cpp -
//  An edge the router may route through
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.11  2004/03/03 15:33:53  roessel
// Tried to make postloadInit more readable.
// Added an assert to avoid division by zero.
// Revision 1.10 2004/01/26 08:01:10 dkrajzew
// loaders and route-def types are now renamed in an senseful way;
// further changes in order to make both new routers work;
// documentation added
//
// Revision 1.9  2003/11/11 08:04:45  dkrajzew
// avoiding emissions of vehicles on too short edges
//
// Revision 1.8  2003/09/17 10:14:27  dkrajzew
// handling of unset values patched
//
// Revision 1.7  2003/06/19 15:22:38  dkrajzew
// inifinite loop on lane searching patched
//
// Revision 1.6 2003/06/18 11:20:54 dkrajzew new message and error
// processing: output to user may be a message, warning or an error
// now; it is reported to a Singleton (MsgHandler); this handler puts
// it further to output instances. changes: no verbose-parameter
// needed; messages are exported to singleton
//
// Revision 1.5  2003/04/14 13:54:20  roessel
// Removed "EdgeType::" in method ROEdge::getNoFollowing().
//
// Revision 1.4  2003/04/09 15:39:10  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.3  2003/02/07 10:45:04  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <utils/common/MsgHandler.h>
#include <algorithm>
#include <cassert>
#include "ROLane.h"
#include "ROEdge.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
ROEdge::ROEdge(const std::string &id)
    : _id(id), _dist(0), _speed(-1), _usingTimeLine(false)
{
}


ROEdge::~ROEdge()
{
    for(LaneUsageCont::iterator i=_laneCont.begin(); i!=_laneCont.end(); i++) {
        delete (*i).first;
        delete (*i).second;
    }
}


void
ROEdge::postloadInit(size_t idx)
{
    // !!! only when not lanes but the edge shall be used for routing
    if(_usingTimeLine) {
//         FloatValueTimeLine *tmp = (*(_laneCont.begin())).second;
//         for(size_t i=0; i<tmp->noDefinitions(); i++) {
//             double currValue = 0;
//             FloatValueTimeLine::TimeRange range(tmp->getRangeAtPosition(i));
//             for(LaneUsageCont::iterator j=_laneCont.begin();
//                 j!=_laneCont.end(); j++) {
//                 double tmp = (*j).second->getValue(range.first);
//                 if(tmp<0) {
//                     tmp = _dist / _speed;
//                 }
//                 currValue += tmp;
//             }
            
//             currValue = currValue / _laneCont.size();
//             _ownValueLine.addValue(range, currValue);
//         }
        // get the number of the ValuedTimeRanges that are in the
        // container associated to the first lane. We assume, that all
        // lanes have the same number of ValuedTimeRanges and that the
        // ranges are identical.
        assert( _laneCont.size() > 0 );
        FloatValueTimeLine* firstLanesValueTimeLines =
            (*(_laneCont.begin())).second;
        unsigned nValuedTimeRanges =
            firstLanesValueTimeLines->noDefinitions();
        FloatValueTimeLine::TimeRange range;

        // Assign the mean-value of the lane's values of the current
        // range to the edge's value of the same range.
        for( unsigned index = 0; index < nValuedTimeRanges; ++index ) {
            range = firstLanesValueTimeLines->getRangeAtPosition( index );
            double valueSum = 0;
            for( LaneUsageCont::iterator lane = _laneCont.begin();
                 lane != _laneCont.end(); ++lane ) {
                double value = lane->second->getValue( range.first );
                if ( value < 0 ) {
                    value = _dist / _speed; // default traveltime
                }
                valueSum += value;
            }
            _ownValueLine.addValue( range, valueSum / _laneCont.size() );
        }
    }
    // save the id
    myIndex = idx;
}


size_t
ROEdge::getIndex() const
{
    return myIndex;
}


void
ROEdge::addLane(ROLane *lane)
{
    double length = lane->getLength();
    _dist = length > _dist ? length : _dist;
    double speed = lane->getSpeed();
    _speed = speed > _speed ? speed : _speed;
    _laneCont[lane] = new FloatValueTimeLine();
}


void
ROEdge::setLane(long timeBegin, long timeEnd,
                const std::string &id, float value)
{
    LaneUsageCont::iterator i = _laneCont.begin();
    while(i!=_laneCont.end()) {
        if((*i).first->getID()==id) {
            (*i).second->addValue(timeBegin, timeEnd, value);
            _usingTimeLine = true;
            return;
        }
        i++;
    }
    MsgHandler::getErrorInstance()->inform(
        string("Un unknown lane '") + id
        + string("' occured at loading weights."));
}


void
ROEdge::addFollower(ROEdge *s)
{
    myFollowingEdges.push_back(s);
}


float
ROEdge::getMyEffort(long time) const
{
    if(_usingTimeLine) {
        return _ownValueLine.getValue(time);
    } else {
        return _dist / _speed;
    }
}


size_t
ROEdge::getNoFollowing()
{
    if(getType()==ET_SINK) {
        return 0;
    }
    return myFollowingEdges.size();
}


ROEdge *
ROEdge::getFollower(size_t pos)
{
    return myFollowingEdges[pos];
}


bool
ROEdge::isConnectedTo(ROEdge *e)
{
    return find(myFollowingEdges.begin(), myFollowingEdges.end(), e)!=myFollowingEdges.end();
}


double
ROEdge::getCost(long time) const
{
    return getMyEffort(time);
}


double
ROEdge::getDuration(long time) const
{
    return getMyEffort(time);
}


std::string
ROEdge::getID() const
{
    return _id;
}


void
ROEdge::setType(ROEdge::EdgeType type)
{
    myType = type;
}


ROEdge::EdgeType
ROEdge::getType() const
{
    return myType;
}


double
ROEdge::getLength() const
{
    assert(_laneCont.size()!=0);
    return (*(_laneCont.begin())).first->getLength();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


