/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBPTLine.h
/// @author  Gregor Laemmel
/// @author  Nikita Cherednychek
/// @date    Tue, 20 Mar 2017
/// @version $Id$
///
// The representation of one direction of a single pt line
/****************************************************************************/
#ifndef SUMO_NBPTLINE_H
#define SUMO_NBPTLINE_H


#include <string>
#include <vector>
#include <map>
#include "NBEdge.h" // Cherednychek

// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class NBPTStop;
class NBEdgeCont;

class NBPTLine {

public:
    explicit NBPTLine(const std::string& id, const std::string& name,
                      const std::string& type, const std::string& ref, int interval, const std::string& nightService);

    void addPTStop(NBPTStop* pStop);

    const std::string& getLineID() const {
        return myPTLineId;
    }

    const std::string& getName() const {
        return myName;
    }

    const std::string& getType() const {
        return myType;
    }

    std::vector<NBPTStop*> getStops();
    void write(OutputDevice& device, NBEdgeCont& ec);
    void addWayNode(long long int way, long long int node);

    void setMyNumOfStops(int numStops);

    /// @brief get line reference (not unique)
    const std::string& getRef() const {
        return myRef;
    }

    void replaceStops(std::vector<NBPTStop*> stops) {
        myPTStops = stops;
    }

private:
    std::string myName;
    std::string myType;
    std::vector<NBPTStop*> myPTStops;

private:
    std::map<std::string, std::vector<long long int> > myWaysNodes;
    std::vector<std::string> myWays;
public:
    const std::vector<std::string>& getMyWays() const;
    std::vector<long long int>* getWaysNodes(std::string wayId);
private:

    std::string myCurrentWay;
    std::string myPTLineId;
    std::string myRef;
    int myInterval;
    std::string myNightService;

public:
    void addEdgeVector(std::vector<NBEdge*>::iterator fr, std::vector<NBEdge*>::iterator to);
private:
    // route of ptline
    std::vector<NBEdge*> myRoute;
public:
    const std::vector<NBEdge*>& getRoute() const;
private:

    int myNumOfStops;
};


#endif //SUMO_NBPTLINE_H
