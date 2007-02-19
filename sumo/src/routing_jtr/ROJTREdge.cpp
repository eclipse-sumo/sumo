/****************************************************************************/
/// @file    ROJTREdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Jan 2004
/// @version $Id: $
///
// An edge the router may route through
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <algorithm>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include <utils/common/RandHelper.h>
#include "ROJTREdge.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
ROJTREdge::ROJTREdge(const std::string &id, int index)
        : ROEdge(id, index, false)
{}


ROJTREdge::~ROJTREdge()
{
    for (FollowerUsageCont::iterator i=myFollowingDefs.begin(); i!=myFollowingDefs.end(); i++) {
        delete(*i).second;
    }
}


void
ROJTREdge::addFollower(ROEdge *s)
{
    ROEdge::addFollower(s);
    myFollowingDefs[static_cast<ROJTREdge*>(s)] =
        new FloatValueTimeLine();
}


void
ROJTREdge::addFollowerProbability(ROJTREdge *follower, SUMOTime begTime,
                                  SUMOTime endTime, SUMOReal probability)
{
    FollowerUsageCont::iterator i = myFollowingDefs.find(follower);
    if (i==myFollowingDefs.end()) {
        MsgHandler::getErrorInstance()->inform("The edges '" + getID() + "' and '" + follower->getID() + "' are not connected.");
        return;
    }
    (*i).second->add(begTime, endTime, probability);
}


ROJTREdge *
ROJTREdge::chooseNext(SUMOTime time) const
{
    if (myFollowingEdges.size()==0) {
        return 0;
    }
    // check whether any definition exists
    FollowerUsageCont::const_iterator i;
    // check how many edges are defined for the given time step
    size_t noDescs = 0;
    bool hasDescription = true;
    for (i=myFollowingDefs.begin(); i!=myFollowingDefs.end()&&hasDescription; i++) {
        if ((*i).second->describesTime(time)) {
            noDescs++;
        }
    }
    // get a random number between zero and one
    SUMOReal chosen = randSUMO();
    // if no description is given for the current time
    if (noDescs==0) {
        //  use the defaults
        std::vector<SUMOReal>::const_iterator j;
        size_t pos = 0;
        for (j=myParsedTurnings.begin(); j!=myParsedTurnings.end(); j++) {
            chosen = chosen - (*j);
            if (chosen<0) {
                return static_cast<ROJTREdge*>(myFollowingEdges[pos]);
            }
            pos++;
        }
        return static_cast<ROJTREdge*>(myFollowingEdges[0]);
    }
    // if the probabilities are given for all following edges
    if (noDescs==myFollowingEdges.size()) {
        // use the loaded definition
        // choose the appropriate one from the list
        for (i=myFollowingDefs.begin(); i!=myFollowingDefs.end(); i++) {
            chosen = chosen - (*i).second->getValue(time);
            if (chosen<=0) {
                return (*i).first;
            }
        }
        return (*myFollowingDefs.begin()).first;
    } else {
        // ok, at least one description is missing
        //  the missing descriptions will be treated as zero
        bool allZero = true; // we won't be as stupid as our users may be...
        while (allZero) { // ... we may loop several times, but not forever
            allZero = false;
            for (i=myFollowingDefs.begin(); i!=myFollowingDefs.end(); i++) {
                if ((*i).second->describesTime(time)) {
                    chosen = chosen - (*i).second->getValue(time);
                    if (chosen<=0) {
                        return (*i).first;
                    }
                    allZero = allZero |
                              ((*i).second->getValue(time)!=0);
                }
            }
        }
        return (*myFollowingDefs.begin()).first;
    }
}


void
ROJTREdge::setTurnDefaults(const std::vector<SUMOReal> &defs)
{
    // I hope, we'll find a less ridiculous solution for this
    std::vector<SUMOReal> tmp(defs.size()*myFollowingEdges.size(), 0);
    // store in less common multiple
    size_t i;
    for (i=0; i<defs.size(); i++) {
        for (size_t j=0; j<myFollowingEdges.size(); j++) {
            tmp[i*myFollowingEdges.size()+j] = (SUMOReal)
                                               (defs[i] / 100.0 / (myFollowingEdges.size()));
        }
    }
    // parse from less common multiple
    for (i=0; i<myFollowingEdges.size(); i++) {
        SUMOReal value = 0;
        for (size_t j=0; j<defs.size(); j++) {
            value += tmp[i*defs.size()+j];
        }
        myParsedTurnings.push_back((SUMOReal) value);
    }
}



/****************************************************************************/

