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
// Revision 1.20  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.19  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.18  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.17  2005/05/04 08:46:09  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.16  2004/11/23 10:25:51  dkrajzew
// debugging
//
// Revision 1.15  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived
//  for an online-routing
//
// Revision 1.14  2004/04/23 12:43:00  dkrajzew
// warnings and errors are now reported to MsgHandler, not cerr
//
// Revision 1.13  2004/04/14 13:53:50  roessel
// Changes and additions in order to implement supplementary-weights.
//
// Revision 1.12  2004/03/19 13:03:01  dkrajzew
// some style adaptions
//
// Revision 1.11  2004/03/03 15:33:53  roessel
// Tried to make postloadInit more readable.
// Added an assert to avoid division by zero.
//
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include "ROLane.h"
#include "ROEdge.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

ROEdge::DictType ROEdge::myDict;
bool myHaveWarned = false;

/* =========================================================================
 * method definitions
 * ======================================================================= */
ROEdge::ROEdge(const std::string &id, int index)
    : _id(id), _dist(0), _speed(-1),
      _supplementaryWeightAbsolut(0),
      _supplementaryWeightAdd(0),
      _supplementaryWeightMult(0),
      _usingTimeLine(false),
      _hasSupplementaryWeights(false),
      /*myHaveWarned(false),*/ myIndex(index), myLength(-1)
{
}


ROEdge::~ROEdge()
{
    /*
    for(LaneUsageCont::iterator i=_laneCont.begin(); i!=_laneCont.end(); i++) {
        delete (*i).first;
        delete (*i).second;
    }
    */
    delete _supplementaryWeightAbsolut;
    delete _supplementaryWeightAdd;
    delete _supplementaryWeightMult;
}

/*
void
ROEdge::postloadInit(size_t idx)
{
    // !!! only when not lanes but the edge shall be used for routing
    if(_usingTimeLine) {
        // get the number of the ValuedTimeRanges that are in the
        // container associated to the first lane. We assume, that all
        // lanes have the same number of ValuedTimeRanges and that the
        // ranges are identical.
        assert( _laneCont.size() > 0 );
        FloatValueTimeLine* firstLanesValueTimeLines =
            (*(_laneCont.begin())).second;
        unsigned nValuedTimeRanges =
            firstLanesValueTimeLines->noDefinitions();

        // Assign the mean-value of the lane's values of the current
        // range to the edge's value of the same range.
        for( unsigned index = 0; index < nValuedTimeRanges; ++index ) {
            range = firstLanesValueTimeLines->getAtPosition( index );
            SUMOReal valueSum = 0;
            for( LaneUsageCont::iterator lane = _laneCont.begin();
                 lane != _laneCont.end(); ++lane ) {
                SUMOReal value = lane->second->getValue( range.first );
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
*/

size_t
ROEdge::getIndex() const
{
    return myIndex;
}


void
ROEdge::addLane(ROLane *lane)
{
    SUMOReal length = lane->getLength();
    assert(myLength==-1||length==myLength);
    myLength = length;
    _dist = length > _dist ? length : _dist;
    SUMOReal speed = lane->getSpeed();
    _speed = speed > _speed ? speed : _speed;
//    _laneCont[lane] = new FloatValueTimeLine();
    myDictLane[lane->getID()] = lane;
    myLanes.push_back(lane);
}

/*
void
ROEdge::setLane(long timeBegin, long timeEnd,
                const std::string &id, SUMOReal value)
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
*/


void
ROEdge::addWeight(SUMOReal value, SUMOTime timeBegin, SUMOTime timeEnd)
{
    _ownValueLine.add(timeBegin, timeEnd, value);
    _usingTimeLine = true;
}


void
ROEdge::addFollower(ROEdge *s)
{
    myFollowingEdges.push_back(s);
}


SUMOReal
ROEdge::getEffort(SUMOTime time) const
{
    FloatValueTimeLine::SearchResult searchResult;
    FloatValueTimeLine::SearchResult supplementarySearchResult;
    // check whether to use an absolute value
    bool hasAbsolutValue = false;
    if ( _hasSupplementaryWeights == true ) {
        searchResult =
            _supplementaryWeightAbsolut->getSearchStateAndValue( time );
        if ( searchResult.first == true ) {
            return searchResult.second;
        }
    }

    // ok, no absolute value was found, use the normal value (without)
    //  weight as default
    SUMOReal value = (SUMOReal) (_dist / _speed);
    if(_usingTimeLine) {
        searchResult = _ownValueLine.getSearchStateAndValue( time );
        if ( searchResult.first == false ) {
            if(!myHaveWarned) {
                WRITE_WARNING(string("ROEdge::getMyEffort(id ") + _id+ string(" ):"));
                WRITE_WARNING(string(" No interval matches passed time ")+ toString<SUMOTime>(time)  + string("."));
                WRITE_WARNING("Using edge's length / edge's speed.");
                myHaveWarned = true;
            }
        } else {
            value = searchResult.second;
        }
    }

    // check for additional values
    if ( _hasSupplementaryWeights == true ) {
        // for factors
        supplementarySearchResult =
            _supplementaryWeightMult->getSearchStateAndValue( time );
        if ( supplementarySearchResult.first == true ) {
            value *= supplementarySearchResult.second;
        }
        // for a value to add
        supplementarySearchResult =
            _supplementaryWeightAdd->getSearchStateAndValue( time );
        if ( supplementarySearchResult.first == true ) {
            value += supplementarySearchResult.second;
        }
    }
    // return final value
    return value;
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


SUMOReal
ROEdge::getCost(SUMOTime time)
{
    return getEffort(time);
}


SUMOReal
ROEdge::getDuration(SUMOTime time)
{
    return getEffort(time);
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


SUMOReal
ROEdge::getLength() const
{
    /*
    assert(_laneCont.size()!=0);
    return (*(_laneCont.begin())).first->getLength();
    */
    return myLength;
}

void
ROEdge::setSupplementaryWeights( FloatValueTimeLine* absolut,
                                 FloatValueTimeLine* add,
                                 FloatValueTimeLine* mult )
{
    _supplementaryWeightAbsolut = absolut;
    _supplementaryWeightAdd     = add;
    _supplementaryWeightMult    = mult;
    assert( _supplementaryWeightAbsolut != 0 &&
            _supplementaryWeightAdd     != 0 &&
            _supplementaryWeightMult    != 0 );
    _hasSupplementaryWeights = true;
}


void
ROEdge::clear()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}

bool
ROEdge::dictionary(string id, ROEdge* ptr)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}

ROEdge*
ROEdge::dictionary2(string id)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}




ROLane *
ROEdge::getLane(std::string name)
{
    //(assert(laneNo<myLanes->size()); ??? was ist assert
    //return *((*myLanes)[laneNo]);
    std::map<std::string, ROLane*>::const_iterator i=
		ROEdge::myDictLane.find(name);
    //assert(i!=myEmitterDict.end());
    return (*i).second;
}


ROLane*
ROEdge::getLane(size_t index)
{
	return myLanes[index];
}


SUMOReal
ROEdge::getSpeed() const
{
    return _speed;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


