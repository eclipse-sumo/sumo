//---------------------------------------------------------------------------//
//                        ROJPEdge.cpp -
//  An edge the router may route through
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 20 Jan 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.3  2004/02/02 16:21:06  dkrajzew
// new default behaviour on incomplete edge propabilities
//
// Revision 1.2  2004/01/26 09:57:14  dkrajzew
// setting of the default direction percentages corrected
//
// Revision 1.1  2004/01/26 06:09:11  dkrajzew
// initial commit for jp-classes
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <algorithm>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include "ROJPEdge.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
ROJPEdge::ROJPEdge(const std::string &id)
    : ROEdge(id)
{
}


ROJPEdge::~ROJPEdge()
{
    for(FollowerUsageCont::iterator i=myFollowingDefs.begin(); i!=myFollowingDefs.end(); i++) {
        delete (*i).second;
    }
}


void
ROJPEdge::addFollower(ROEdge *s)
{
    ROEdge::addFollower(s);
    myFollowingDefs[static_cast<ROJPEdge*>(s)] = new FloatValueTimeLine();
}


void
ROJPEdge::addFollowerPropability(ROJPEdge *follower, unsigned int begTime,
                                 unsigned int endTime, float percentage)
{
    FollowerUsageCont::iterator i = myFollowingDefs.find(follower);
    if(i==myFollowingDefs.end()) {
        MsgHandler::getErrorInstance()->inform(
            string("The edges '") + getID() + string("' and '")
            + follower->getID() + string("' are not connected."));
        return;
    }
    (*i).second->addValue(begTime, endTime, percentage);
}


ROJPEdge *
ROJPEdge::chooseNext(unsigned int time) const
{
    if(myFollowingEdges.size()==0) {
        return 0;
    }
    // check whether any definition exists
    FollowerUsageCont::const_iterator i;
    // check how many edges are defined for the given time step
    size_t noDescs = 0;
    bool hasDescription = true;
    for(i=myFollowingDefs.begin(); i!=myFollowingDefs.end()&&hasDescription; i++) {
        if((*i).second->describesTime(time)) {
            noDescs++;
        }
    }
    // get a random number between zero and one
    double chosen = (double)rand() /
        ( static_cast<double>(RAND_MAX) + 1);
    // if no description is given for the current time
    if(noDescs==0) {
        //  use the defaults
        std::vector<float>::const_iterator j;
        size_t pos = 0;
        for(j=myParsedTurnings.begin(); j!=myParsedTurnings.end(); j++) {
            chosen = chosen - (*j);
            if(chosen<=0) {
                return static_cast<ROJPEdge*>(myFollowingEdges[pos]);
            }
            pos++;
        }
        return static_cast<ROJPEdge*>(myFollowingEdges[0]);
    }
    // if the propabilities are given for all following edges
    if(noDescs==myFollowingEdges.size()) {
        // use the loaded definition
            // choose the appropriate one from the list
        for(i=myFollowingDefs.begin(); i!=myFollowingDefs.end(); i++) {
            chosen = chosen - (*i).second->getValue(time);
            if(chosen<=0) {
                return (*i).first;
            }
        }
        return (*myFollowingDefs.begin()).first;
    } else {
        // ok, at least one description is missing
        //  the missing descriptions will be treated as zero
        bool allZero = true; // we won't be as stupid as our users may be...
        while(allZero) { // ... we may loop several times, but not forever
            allZero = false;
            for(i=myFollowingDefs.begin(); i!=myFollowingDefs.end(); i++) {
                if((*i).second->describesTime(time)) {
                    chosen = chosen - (*i).second->getValue(time);
                    if(chosen<=0) {
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
ROJPEdge::setTurnDefaults(const std::vector<float> &defs)
{
    // I hope, we'll find a less ridiculous solution for this
    std::vector<float> tmp(defs.size()*myFollowingEdges.size(), 0);
        // store in less common multiple
    size_t i;
    for(i=0; i<defs.size(); i++) {
        for(size_t j=0; j<myFollowingEdges.size(); j++) {
            tmp[i*myFollowingEdges.size()+j] = defs[i]
                / 100.0 / (myFollowingEdges.size());
        }
    }
        // parse from less common multiple
    for(i=0; i<myFollowingEdges.size(); i++) {
        double value = 0;
        for(size_t j=0; j<defs.size(); j++) {
            value += tmp[i*defs.size()+j];
        }
        myParsedTurnings.push_back(value);
    }
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


