/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
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
/// @file    NBAlgorithms_Railway.h
/// @author  Jakob Erdmann
/// @author  Melanie Weber
/// @date    29. March 2018
///
// Algorithms for railways
/****************************************************************************/
#pragma once
#include <config.h>

#include <set>
#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdge;
class NBEdgeCont;
class NBPTStopCont;
class NBPTLine;
class NBPTLineCont;
class OptionsCont;
class NBVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// NBAlgorithms_Railway
// ---------------------------------------------------------------------------
/* @class NBRailwayTopologyAnalyzer
 * @brief Computes and adapts the topology for the rail network, especially bidi
 */
class NBRailwayTopologyAnalyzer {
public:
    static void analyzeTopology(NBEdgeCont& ec);
    static int repairTopology(NBEdgeCont& ec, NBPTStopCont& sc, NBPTLineCont& lc);
    static int makeAllBidi(NBEdgeCont& ec);
    static void extendDirectionPriority(NBEdgeCont& ec, bool fromUniDir);

    /// routing edge
    class Track {
    public:
        Track(NBEdge* e, int i = -1, const std::string& _id = "") :
            edge(e),
            index(i < 0 ? edge->getNumericalID() : i),
            id(_id == "" ? edge->getID() : _id),
            minPermissions(edge->getPermissions()) {
        }

        void addSuccessor(Track* track);
        const std::vector<Track*>& getSuccessors(SUMOVehicleClass svc = SVC_IGNORING) const;
        const std::vector<std::pair<const Track*, const Track*> >& getViaSuccessors(SUMOVehicleClass svc = SVC_IGNORING) const;

        const std::string& getID() const {
            return id;
        }
        int getNumericalID() const {
            return index;
        }
        double getLength() const {
            return 0.;
        }
        const Track* getBidiEdge() const {
            return this;
        }
        bool isInternal() const {
            return false;
        }
        inline bool prohibits(const NBVehicle* const /*veh*/) const {
            return false;
        }
        inline bool restricts(const NBVehicle* const /*veh*/) const {
            return false;
        }

        NBEdge* edge;

    private:
        const int index;
        const std::string id;
        std::vector<Track*> successors;
        std::vector<std::pair<const Track*, const Track*> > viaSuccessors;
        SVCPermissions minPermissions;
        mutable std::map<SUMOVehicleClass, std::vector<Track*> > svcSuccessors;
        mutable std::map<SUMOVehicleClass, std::vector<std::pair<const Track*, const Track*> > > svcViaSuccessors;

        Track& operator=(const Track&) = delete;
    };
    static double getTravelTimeStatic(const Track* const track, const NBVehicle* const veh, double time);

private:
    static std::set<NBNode*> getRailNodes(NBEdgeCont& ec, bool verbose = false);
    static std::set<NBNode*> getBrokenRailNodes(NBEdgeCont& ec, bool verbose = false);

    /// @brief filter out rail edges among all edges of a the given node
    static void getRailEdges(const NBNode* node, EdgeVector& inEdges, EdgeVector& outEdges);

    /// @brief filter for rail edges but do not return (legacy) all purpose edges
    static bool hasRailway(SVCPermissions permissions) {
        return (permissions & SVC_RAIL_CLASSES) > 0 && permissions != SVCAll;
    }

    static bool isStraight(const NBNode* node, const NBEdge* e1, const NBEdge* e2);
    static bool hasStraightPair(const NBNode* node, const EdgeVector& edges, const EdgeVector& edges2);
    static bool allBroken(const NBNode* node, NBEdge* candOut, const EdgeVector& in, const EdgeVector& out);
    static bool allSharp(const NBNode* node, const EdgeVector& in, const EdgeVector& out, bool countBidiAsSharp = false);
    static bool allBidi(const EdgeVector& edges);
    static NBEdge* isBidiSwitch(const NBNode* n);

    /// @brief add bidi-edge for the given edge
    static NBEdge* addBidiEdge(NBEdgeCont& ec, NBEdge* edge, bool update = true);

    /// @brief add further bidi-edges near existing bidi-edges
    static int extendBidiEdges(NBEdgeCont& ec);
    static int extendBidiEdges(NBEdgeCont& ec, NBNode* node, NBEdge* bidiIn);

    /// @brief reverse edges sequences that are to broken nodes on both sides
    static int reverseEdges(NBEdgeCont& ec, NBPTStopCont& sc);

    /// @brief add bidi-edges to connect buffers stops in both directions
    static int addBidiEdgesForBufferStops(NBEdgeCont& ec);

    /// @brief add bidi-edges to connect switches that are approached in both directions
    static int addBidiEdgesBetweenSwitches(NBEdgeCont& ec);

    /// @brief add bidi-edges to connect successive public transport stops
    static int addBidiEdgesForStops(NBEdgeCont& ec, NBPTLineCont& lc, NBPTStopCont& sc);

    /// @brief add bidi-edges to connect straight tracks
    static int addBidiEdgesForStraightConnectivity(NBEdgeCont& ec, bool geometryLike);

    /// @brief recompute turning directions for both nodes of the given edge
    static void updateTurns(NBEdge* edge);

    /// @brief identify lines that are likely to require bidirectional tracks
    static std::set<NBPTLine*> findBidiCandidates(NBPTLineCont& lc);

};
