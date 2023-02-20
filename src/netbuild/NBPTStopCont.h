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
/// @file    NBPTStopCont.h
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
///
// Container for public transport stops during the net building process
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <map>


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdge;
class NBEdgeCont;
class NBPTPlatform;
class NBPTStop;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBPTStopCont
 * @brief Container for public transport stops during the net building process
 *
 */
class NBPTStopCont {
public:

    ~NBPTStopCont();

    /** @brief Inserts a node into the map
    * @param[in] stop The pt stop to insert
    * @param[in] floating whether the stop is not referenced by a way or relation
    * @return Whether the pt stop could be added
    */
    bool insert(std::shared_ptr<NBPTStop> ptStop, bool floating = false);

    /// @brief Retrieve a previously inserted pt stop
    std::shared_ptr<NBPTStop> get(std::string id) const;

    /** @brief Returns an unmodifiable reference to the stored pt stops
     * @return The stored pt stops
     */
    const std::map<std::string, std::shared_ptr<NBPTStop> >& getStops() const {
        return myPTStops;
    }

    /** @brief remove stops on non existing (removed) edges
     *
     * @param cont
     */
    int cleanupDeleted(NBEdgeCont& cont);

    void assignLanes(NBEdgeCont& cont);

    /// @brief duplicate stops for superposed rail edges and return the number of generated stops
    int generateBidiStops(NBEdgeCont& cont);

    void localizePTStops(NBEdgeCont& cont);

    void assignEdgeForFloatingStops(NBEdgeCont& cont, double maxRadius);

    void findAccessEdgesForRailStops(NBEdgeCont& cont, double maxRadius, int maxCount, double accessFactor);

    void postprocess(std::set<std::string>& usedStops);

    /// @brief add edges that must be kept
    void addEdges2Keep(const OptionsCont& oc, std::set<std::string>& into);

    /// @brief replace the edge with the closes edge on the given edge list in all stops
    void replaceEdge(const std::string& edgeID, const std::vector<NBEdge*>& replacement);


    std::shared_ptr<NBPTStop> findStop(const std::string& origEdgeID, Position pos, double threshold = 1) const;

    std::shared_ptr<NBPTStop> getReverseStop(std::shared_ptr<NBPTStop> pStop, const NBEdgeCont& ec);

private:
    /// @brief Definition of the map of names to pt stops
    typedef std::map<std::string, std::shared_ptr<NBPTStop> > PTStopsCont;

    /// @brief The map of names to pt stops
    PTStopsCont myPTStops;

    /// @brief The map of edge ids to stops
    std::map<std::string, std::vector<std::shared_ptr<NBPTStop> > > myPTStopLookup;

    std::vector<std::shared_ptr<NBPTStop> > myFloatingStops;


    void assignPTStopToEdgeOfClosestPlatform(std::shared_ptr<NBPTStop> pStop, NBEdgeCont& cont);
    const NBPTPlatform* getClosestPlatformToPTStopPosition(std::shared_ptr<NBPTStop> pStop);
    std::shared_ptr<NBPTStop> assignAndCreatNewPTStopAsNeeded(std::shared_ptr<NBPTStop> pStop, NBEdgeCont& cont);
    double computeCrossProductEdgePosition(const NBEdge* edge, const Position& closestPlatform) const;

    static std::string getReverseID(const std::string& id);

    static std::set<std::string> myIgnoredStops;


public:
    static NBEdge* getReverseEdge(NBEdge* edge);

    static void addIgnored(const std::string& stopID) {
        myIgnoredStops.insert(stopID);
    }

    static bool wasIgnored(const std::string& stopID) {
        return myIgnoredStops.count(stopID) > 0;
    }

    void alignIdSigns();
};

