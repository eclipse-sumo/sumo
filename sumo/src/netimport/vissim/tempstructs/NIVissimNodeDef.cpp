//---------------------------------------------------------------------------//
//                        NIVissimNodeDef.cpp -  ccc
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
// Revision 1.9  2003/06/18 11:35:29  dkrajzew
// message subsystem changes applied and some further work done; seems to be stable but is not perfect, yet
//
// Revision 1.8  2003/06/05 11:46:57  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include <iostream> // !!! debug
#include <cassert>
#include "NIVissimNodeDef.h"
#include "NIVissimConnection.h"
#include "NIVissimDisturbance.h"
#include "NIVissimTL.h"

using namespace std;

NIVissimNodeDef::DictType NIVissimNodeDef::myDict;
int NIVissimNodeDef::myMaxID = 0;

NIVissimNodeDef::NIVissimNodeDef(int id, const std::string &name)
    : myID(id), myName(name)
{
}


NIVissimNodeDef::~NIVissimNodeDef()
{
}


bool
NIVissimNodeDef::dictionary(int id, NIVissimNodeDef *o)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
        myMaxID = myMaxID > id
            ? myMaxID
            : id;
//        o->computeBounding();
        return true;
    }
    return false;
}


NIVissimNodeDef *
NIVissimNodeDef::dictionary(int id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}

/*
void
NIVissimNodeDef::buildNodeClusters()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        int cluster = (*i).second->buildNodeCluster();
    }
}
*/


/*

IntVector
NIVissimNodeDef::getWithin(const AbstractPoly &p, double off)
{
    IntVector ret;
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimNodeDef *d = (*i).second;
        if(d->partialWithin(p, off)) {
            ret.push_back((*i).first);
        }
    }
    return ret;
}

bool
NIVissimNodeDef::partialWithin(const AbstractPoly &p, double off) const
{
    assert(myBoundery!=0&&myBoundery->xmax()>=myBoundery->xmin());
    return myBoundery->partialWithin(p, off);
}
*/

void
NIVissimNodeDef::dict_assignConnectionsToNodes()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        (*i).second->searchAndSetConnections();
    }
}


size_t
NIVissimNodeDef::dictSize()
{
    return myDict.size();
}



void
NIVissimNodeDef::clearDict()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}


int
NIVissimNodeDef::getMaxID()
{
    return myMaxID;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVissimNodeDef.icc"
//#endif

// Local Variables:
// mode:C++
// End:


