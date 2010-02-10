/****************************************************************************/
/// @file    NIVissimNodeDef.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


#include <iostream> // !!! debug
#include <cassert>
#include "NIVissimNodeDef.h"
#include "NIVissimConnection.h"
#include "NIVissimDisturbance.h"
#include "NIVissimTL.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS
// ===========================================================================
// used namespaces
// ===========================================================================

using namespace std;

NIVissimNodeDef::DictType NIVissimNodeDef::myDict;
int NIVissimNodeDef::myMaxID = 0;

NIVissimNodeDef::NIVissimNodeDef(int id, const std::string &name)
        : myID(id), myName(name) {}


NIVissimNodeDef::~NIVissimNodeDef() {}


bool
NIVissimNodeDef::dictionary(int id, NIVissimNodeDef *o) {
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
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
NIVissimNodeDef::dictionary(int id) {
    DictType::iterator i=myDict.find(id);
    if (i==myDict.end()) {
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
NIVissimNodeDef::getWithin(const AbstractPoly &p, SUMOReal off)
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
NIVissimNodeDef::partialWithin(const AbstractPoly &p, SUMOReal off) const
{
    assert(myBoundary!=0&&myBoundary->xmax()>=myBoundary->xmin());
    return myBoundary->partialWithin(p, off);
}
*/

void
NIVissimNodeDef::dict_assignConnectionsToNodes() {
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        (*i).second->searchAndSetConnections();
    }
}


size_t
NIVissimNodeDef::dictSize() {
    return myDict.size();
}



void
NIVissimNodeDef::clearDict() {
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}


int
NIVissimNodeDef::getMaxID() {
    return myMaxID;
}



/****************************************************************************/

