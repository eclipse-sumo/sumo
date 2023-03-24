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
/// @file    NBPTLineCont.h
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
///
// Container for NBPTLine during netbuild
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include "utils/router/SUMOAbstractRouter.h"
#include "NBEdgeCont.h"
#include "NBPTLine.h"
#include "NBVehicle.h"


// ===========================================================================
// class definitions
// ===========================================================================
class NBPTLineCont {
public:
    /// @brief destructor
    ~NBPTLineCont();

    /// @brief insert new line
    bool insert(NBPTLine* ptLine);

    const std::map<std::string, NBPTLine*>& getLines() const {
        return myPTLines;
    }

    void process(NBEdgeCont& ec, NBPTStopCont& sc, bool routeOnly = false);

    /// @brief replace the edge with the given edge list in all lines
    void replaceEdge(const std::string& edgeID, const EdgeVector& replacement);

    /// @brief select the correct stop on superposed rail edges
    void fixBidiStops(const NBEdgeCont& ec);

    /// @brief filter out edges that were removed due to --geometry.remove
    void removeInvalidEdges(const NBEdgeCont& ec);

    /// @brief ensure that all turn lanes have sufficient permissions
    void fixPermissions();

    std::set<std::string>& getServedPTStops();
private:

    static const int FWD;
    static const int BWD;

    /// @brief The map of names to pt lines
    std::map<std::string, NBPTLine*> myPTLines;

    /// @brief find directional edge for all stops of the line
    void reviseStops(NBPTLine* line, const NBEdgeCont& ec, NBPTStopCont& sc);

    void reviseSingleWayStops(NBPTLine* line, const NBEdgeCont& ec, NBPTStopCont& sc);

    /* @brief find way element corresponding to the stop
     * @note: if the edge id is updated, the stop extent is recomputed */
    std::shared_ptr<NBPTStop> findWay(NBPTLine* line, std::shared_ptr<NBPTStop> stop, const NBEdgeCont& ec, NBPTStopCont& sc) const;

    void constructRoute(NBPTLine* myPTLine, const NBEdgeCont& cont);

    std::set<std::string> myServedPTStops;

    static double getCost(const NBEdgeCont& ec, SUMOAbstractRouter<NBRouterEdge, NBVehicle>& router,
                          const std::shared_ptr<NBPTStop> from, const std::shared_ptr<NBPTStop> to, const NBVehicle* veh);

    static std::string getWayID(const std::string& edgeID);

    /// @brief The map of edge ids to lines that use this edge in their route
    std::map<std::string, std::set<NBPTLine*> > myPTLineLookup;
};


