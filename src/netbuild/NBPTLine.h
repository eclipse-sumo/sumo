/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    NBPTLine.h
/// @author  Gregor Laemmel
/// @author  Nikita Cherednychek
/// @date    Tue, 20 Mar 2017
///
// The representation of one direction of a single pt line
/****************************************************************************/
#pragma once
#include <config.h>

#include <memory>
#include <map>
#include <string>
#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class NBEdge;
class NBEdgeCont;
class NBPTStop;
class NBPTStopCont;


// ===========================================================================
// class definitions
// ===========================================================================
class NBPTLine {
public:
    NBPTLine(const std::string& id, const std::string& name,
             const std::string& type, const std::string& ref, int interval, const std::string& nightService,
             SUMOVehicleClass vClass, RGBColor color);

    void addPTStop(std::shared_ptr<NBPTStop> pStop);

    const std::string& getLineID() const {
        return myPTLineId;
    }

    const std::string& getName() const {
        return myName;
    }

    const std::string& getType() const {
        return myType;
    }

    const std::vector<std::shared_ptr<NBPTStop> >& getStops();
    void write(OutputDevice& device);
    void addWayNode(long long int way, long long int node);

    void setMyNumOfStops(int numStops);

    /// @brief get line reference (not unique)
    const std::string& getRef() const {
        return myRef;
    }

    void replaceStops(std::vector<std::shared_ptr<NBPTStop> > stops) {
        myPTStops = stops;
    }
    /// @brief get stop edges and stop ids
    std::vector<std::pair<NBEdge*, std::string> > getStopEdges(const NBEdgeCont& ec) const;

    /// @brief return first valid edge of myRoute (if it doest not lie after the first stop)
    NBEdge* getRouteStart(const NBEdgeCont& ec) const;

    /// @brief return last valid edge of myRoute (if it doest not lie before the last stop)
    NBEdge* getRouteEnd(const NBEdgeCont& ec) const;

    /// @brief return whether the mentioned edges appear in that order in the route
    bool isConsistent(const std::vector<NBEdge*>& stops) const;

    SUMOVehicleClass getVClass() const {
        return myVClass;
    }

    /// @brief replace the given stop
    void replaceStop(std::shared_ptr<NBPTStop> oldStop, std::shared_ptr<NBPTStop> newStop);

    /// @brief replace the edge with the given edge list
    void replaceEdge(const std::string& edgeID, const std::vector<NBEdge*>& replacement);

    /// @brief remove invalid stops from the line
    void deleteInvalidStops(const NBEdgeCont& ec, const NBPTStopCont& sc);
    void deleteDuplicateStops();

    /// @brief remove invalid edges from the line
    void removeInvalidEdges(const NBEdgeCont& ec);

    void setName(const std::string& name) {
        myName = name;
    }

    inline const std::vector<std::string>& getWays() const {
        return myWays;
    }

    const std::vector<long long int>* getWayNodes(std::string wayId);

private:
    std::string myName;
    std::string myType;
    std::vector<std::shared_ptr<NBPTStop> > myPTStops;
    std::map<std::string, std::vector<long long int> > myWayNodes;
    std::vector<std::string> myWays;
    std::string myCurrentWay;
    std::string myPTLineId;
    std::string myRef;
    // official line color
    RGBColor myColor;

    // @brief the service interval in minutes
    int myInterval;

    std::string myNightService;
    SUMOVehicleClass myVClass;

public:
    void setEdges(const std::vector<NBEdge*>& edges);
private:
    // route of ptline
    std::vector<NBEdge*> myRoute;
public:
    const std::vector<NBEdge*>& getRoute() const;
private:

    int myNumOfStops;
};
