/****************************************************************************/
/// @file    NIVissimConflictArea.cpp
/// @author  Lukas Grohmann
/// @date    Aug 2015
/// @version $Id$
///
// A temporary storage for conflict areas imported from Vissim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <iterator>
#include <map>
#include <string>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include "NIVissimConflictArea.h"
#include "NIVissimConnection.h"
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBNode.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
NIVissimConflictArea::DictType NIVissimConflictArea::myDict;


// ===========================================================================
// method definitions
// ===========================================================================
NIVissimConflictArea::NIVissimConflictArea(int id,
        const std::string& link1,
        const std::string& link2,
        const std::string& status)
    : myConflictID(id), myFirstLink(link1), mySecondLink(link2), myStatus(status) {
}


NIVissimConflictArea::~NIVissimConflictArea() {}




bool
NIVissimConflictArea::dictionary(int id, const std::string& link1,
                                 const std::string& link2,
                                 const std::string& status) {
    NIVissimConflictArea* ca = new NIVissimConflictArea(id, link1, link2, status);
    if (!dictionary(id, ca)) {
        delete ca;
        return false;
    }
    return true;
}



bool
NIVissimConflictArea::dictionary(int id, NIVissimConflictArea* ca) {
    DictType::iterator i = myDict.find(id);
    if (i == myDict.end()) {
        myDict[id] = ca;
        return true;
    }
    return false;
}



NIVissimConflictArea*
NIVissimConflictArea::dictionary(int id) {
    DictType::iterator i = myDict.find(id);
    if (i == myDict.end()) {
        return 0;
    }
    return (*i).second;
}



NIVissimConflictArea*
NIVissimConflictArea::dict_findByLinks(const std::string& link1,
                                       const std::string& link2) {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); i++) {
        if (((*i).second->myFirstLink == link1) &&
                ((*i).second->mySecondLink == link2)) {
            return (*i).second;
        }
    }
    return 0;
}


void
NIVissimConflictArea::clearDict() {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}


void
NIVissimConflictArea::setPriorityRegulation(NBEdgeCont& ec) {
    std::map<int, NIVissimConflictArea*>::iterator it;
    for (it = myDict.begin(); it != myDict.end(); it++) {
        NIVissimConflictArea* const conflictArea = it->second;
        NIVissimConnection* const firstLink = NIVissimConnection::dictionary(TplConvert::_str2int(conflictArea->getFirstLink()));
        NIVissimConnection* const secondLink = NIVissimConnection::dictionary(TplConvert::_str2int(conflictArea->getSecondLink()));
        if (firstLink == 0 || secondLink == 0) {
            continue;
        }
        // status == "TWOYIELDSONE"
        NIVissimConnection* priority_conn = firstLink;
        NIVissimConnection* subordinate_conn = secondLink;
        if (conflictArea->getStatus() == "ONEYIELDSTWO") {
            priority_conn = secondLink;
            subordinate_conn = firstLink;
        }
        const std::string mayDriveFrom_id = toString<int>(priority_conn->getFromEdgeID());
        const std::string mayDriveTo_id = toString<int>(priority_conn->getToEdgeID());
        const std::string mustStopFrom_id = toString<int>(subordinate_conn->getFromEdgeID());
        const std::string mustStopTo_id = toString<int>(subordinate_conn->getToEdgeID());

        NBEdge* const mayDriveFrom =  ec.retrievePossiblySplit(mayDriveFrom_id, true);
        NBEdge* const mayDriveTo =  ec.retrievePossiblySplit(mayDriveTo_id, false);
        NBEdge* const mustStopFrom =  ec.retrievePossiblySplit(mustStopFrom_id, true);
        NBEdge* const mustStopTo =  ec.retrievePossiblySplit(mustStopTo_id, false);

        if (mayDriveFrom != 0 && mayDriveTo != 0 && mustStopFrom != 0 && mustStopTo != 0) {
            NBNode* node = mayDriveFrom->getToNode();
            node->addSortedLinkFoes(
                NBConnection(mayDriveFrom, mayDriveTo),
                NBConnection(mustStopFrom, mustStopTo));
        }
    }
}


/****************************************************************************/
