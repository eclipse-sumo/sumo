/****************************************************************************/
/// @file    NIVissimConflictArea.cpp
/// @author  Lukas Grohmann
/// @date    Aug 2015
/// @version $Id$
///
// A temporary storage for conflcit areas imported from Vissim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
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
    : myConflictID(id), myFirstLink(link1), mySecondLink(link2), myStatus(status)
{}


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
NIVissimConflictArea::setPriorityRegulation(NBEdgeCont& ec){
    std::map<int, NIVissimConflictArea*>::iterator it;
    for (it = myDict.begin(); it != myDict.end(); it++){
        NIVissimConflictArea* conflictArea = it->second;
        std::string status = conflictArea->getStatus();
        if ((NIVissimConnection::dictionary(TplConvert::_str2int(conflictArea->getFirstLink().c_str())) == 0) ||
            (NIVissimConnection::dictionary(TplConvert::_str2int(conflictArea->getSecondLink().c_str())) == 0)){
            continue;
        }
        NIVissimConnection* priority_conn;
        NIVissimConnection* subordinate_conn;
        if (status == "TWOYIELDSONE") {
            priority_conn = NIVissimConnection::dictionary(TplConvert::_str2int(conflictArea->getFirstLink()));
            subordinate_conn = NIVissimConnection::dictionary(TplConvert::_str2int(conflictArea->getSecondLink()));
        }
        else if (status == "ONEYIELDSTWO"){
            priority_conn = NIVissimConnection::dictionary(TplConvert::_str2int(conflictArea->getSecondLink()));
            subordinate_conn = NIVissimConnection::dictionary(TplConvert::_str2int(conflictArea->getFirstLink()));
        }
        std::string mayDriveFrom_id = toString<int>(priority_conn->getFromEdgeID());
        std::string mayDriveTo_id = toString<int>(priority_conn->getToEdgeID());
        std::string mustStopFrom_id = toString<int>(subordinate_conn->getFromEdgeID());
        std::string mustStopTo_id = toString<int>(subordinate_conn->getToEdgeID());

        NBEdge* mayDriveFrom =  ec.retrievePossiblySplit(mayDriveFrom_id, true);
        NBEdge* mayDriveTo =  ec.retrievePossiblySplit(mayDriveTo_id, false);
        NBEdge* mustStopFrom =  ec.retrievePossiblySplit(mustStopFrom_id, true);
        NBEdge* mustStopTo =  ec.retrievePossiblySplit(mustStopTo_id, false);

        if (mayDriveFrom != 0 && mayDriveTo != 0 && mustStopFrom != 0 && mustStopTo != 0){
            NBNode* node = mayDriveFrom->getToNode();
            node->addSortedLinkFoes(
                NBConnection(mayDriveFrom, mayDriveTo),
                NBConnection(mustStopFrom, mustStopTo));
        }
    }
}


/****************************************************************************/
