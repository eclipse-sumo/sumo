/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBPTLineCont.h
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
/// @version $Id$
///
// Container for NBPTLine during netbuild
/****************************************************************************/

#ifndef SUMO_NBPTLINECONT_H
#define SUMO_NBPTLINECONT_H


#include <vector>
#include "utils/vehicle/SUMOAbstractRouter.h"
#include "NBPTLine.h"
#include "NBEdgeCont.h"

class NBPTLineCont {

public:
    /// @brief constructor
    NBPTLineCont();

    /// @brief destructor
    ~NBPTLineCont();

    /// @brief insert new line
    void insert(NBPTLine* pLine);

    /** @brief Returns the pointer to the begin of the stored pt lines
    * @return The iterator to the beginning of stored pt lines
    */
    std::vector<NBPTLine*>::const_iterator begin() const {
        return myPTLines.begin();
    }

    /** @brief Returns the pointer to the end of the stored pt lines
     * @return The iterator to the end of stored pt lines
     */
    std::vector<NBPTLine*>::const_iterator end() const {
        return myPTLines.end();
    }

    const std::vector<NBPTLine*>& getLines() const {
        return myPTLines;
    }

    void process(NBEdgeCont& cont);

    /// @brief add edges that must be kept
    void addEdges2Keep(const OptionsCont& oc, std::set<std::string>& into);

    /// @brief select the correct stop on superposed rail edges
    void fixBidiStops(const NBEdgeCont& ec);

    std::set<std::string>& getServedPTStops();
private:

    static const int FWD;
    static const int BWD;

    /// @brief The map of names to pt lines
    std::vector<NBPTLine*> myPTLines;

    long long int myIdCnt;
    void reviseStops(NBPTLine* myPTLine, NBEdgeCont& cont);
    void constructRoute(NBPTLine* myPTLine, NBEdgeCont& cont);

    std::set<std::string> myServedPTStops;

    static double getCost(const NBEdgeCont& ec, SUMOAbstractRouter<NBEdge, NBVehicle>& router,
                          const NBPTStop* from, const NBPTStop* to, const NBVehicle* veh);
};


#endif //SUMO_NBPTLINECONT_H
