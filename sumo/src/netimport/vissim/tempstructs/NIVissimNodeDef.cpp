/****************************************************************************/
/// @file    NIVissimNodeDef.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
// static member variables
// ===========================================================================
NIVissimNodeDef::DictType NIVissimNodeDef::myDict;
int NIVissimNodeDef::myMaxID = 0;


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimNodeDef::NIVissimNodeDef(int id, const std::string& name)
    : myID(id), myName(name) {}


NIVissimNodeDef::~NIVissimNodeDef() {}


bool
NIVissimNodeDef::dictionary(int id, NIVissimNodeDef* o) {
    DictType::iterator i = myDict.find(id);
    if (i == myDict.end()) {
        myDict[id] = o;
        myMaxID = myMaxID > id
                  ? myMaxID
                  : id;
//        o->computeBounding();
        return true;
    }
    return false;
}


NIVissimNodeDef*
NIVissimNodeDef::dictionary(int id) {
    DictType::iterator i = myDict.find(id);
    if (i == myDict.end()) {
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

std::vector<int>
NIVissimNodeDef::getWithin(const AbstractPoly &p, SUMOReal off)
{
    std::vector<int> ret;
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


void
NIVissimNodeDef::dict_assignConnectionsToNodes() {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); i++) {
        (*i).second->searchAndSetConnections();
    }
}
*/


size_t
NIVissimNodeDef::dictSize() {
    return myDict.size();
}



void
NIVissimNodeDef::clearDict() {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}


int
NIVissimNodeDef::getMaxID() {
    return myMaxID;
}



/****************************************************************************/

