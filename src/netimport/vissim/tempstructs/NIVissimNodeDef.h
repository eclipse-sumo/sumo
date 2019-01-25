/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIVissimNodeDef.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
#ifndef NIVissimNodeDef_h
#define NIVissimNodeDef_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <map>
#include <utils/geom/Boundary.h>
#include "NIVissimExtendedEdgePointVector.h"
#include "NIVissimNodeCluster.h"


class NIVissimNodeDef {
public:
    NIVissimNodeDef(int id, const std::string& name);
    virtual ~NIVissimNodeDef();
    int buildNodeCluster();
//    virtual void computeBounding() = 0;
//    bool partialWithin(const AbstractPoly &p, double off=0.0) const;
//    virtual void searchAndSetConnections() = 0;
    virtual double getEdgePosition(int edgeid) const = 0;

public:
    static bool dictionary(int id, NIVissimNodeDef* o);
    static NIVissimNodeDef* dictionary(int id);
//    static std::vector<int> getWithin(const AbstractPoly &p, double off=0.0);
//    static void buildNodeClusters();
//    static void dict_assignConnectionsToNodes();
    static int dictSize();
    static void clearDict();
    static int getMaxID();
protected:
    int myID;
    std::string myName;

private:
    typedef std::map<int, NIVissimNodeDef*> DictType;
    static DictType myDict;
    static int myMaxID;
};


#endif

/****************************************************************************/

