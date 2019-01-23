/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBPTStopCont.h
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
/// @version $Id$
///
// Container for pt stops during the netbuilding process
/****************************************************************************/

#ifndef SUMO_NBPTSTOPCONT_H
#define SUMO_NBPTSTOPCONT_H

#include <string>
#include <map>
#include "NBPTStop.h"

class NBEdge;
class NBEdgeCont;

class NBPTStopCont {

public:

    ~NBPTStopCont();

    /** @brief Inserts a node into the map
    * @param[in] stop The pt stop to insert
    * @return Whether the pt stop could be added
    */
    bool insert(NBPTStop* ptStop);

    /// @brief Retrieve a previously inserted pt stop
    NBPTStop* get(std::string id);

    /// @brief Returns the number of pt stops stored in this container
    int size() const {
        return (int) myPTStops.size();
    }

    /** @brief Returns the pointer to the begin of the stored pt stops
    * @return The iterator to the beginning of stored pt stops
    */
    std::map<std::string, NBPTStop*>::const_iterator begin() const {
        return myPTStops.begin();
    }

    /** @brief Returns the pointer to the end of the stored pt stops
     * @return The iterator to the end of stored pt stops
     */
    std::map<std::string, NBPTStop*>::const_iterator end() const {
        return myPTStops.end();
    }

    const std::map<std::string, NBPTStop*>& getStops() const {
        return myPTStops;
    }


    /** @brief remove stops on non existing (removed) edges
     *
     * @param cont
     */
    void cleanupDeleted(NBEdgeCont& cont);

    void assignLanes(NBEdgeCont& cont);

    /// @brief duplicate stops for superposed rail edges and return the number of generated stops
    int generateBidiStops(NBEdgeCont& cont);

    void localizePTStops(NBEdgeCont& cont);

    void findAccessEdgesForRailStops(NBEdgeCont& cont, double maxRadius, int maxCount, double accessFactor);

    void postprocess(std::set<std::string>& usedStops);

    /// @brief add edges that must be kept
    void addEdges2Keep(const OptionsCont& oc, std::set<std::string>& into);

private:
    /// @brief Definition of the map of names to pt stops
    typedef std::map<std::string, NBPTStop*> PTStopsCont;

    /// @brief The map of names to pt stops
    PTStopsCont myPTStops;

    NBPTStop* getReverseStop(NBPTStop* pStop, NBEdgeCont& cont);


    void assignPTStopToEdgeOfClosestPlatform(NBPTStop* pStop, NBEdgeCont& cont);
    const NBPTPlatform* getClosestPlatformToPTStopPosition(NBPTStop* pStop);
    NBPTStop* assignAndCreatNewPTStopAsNeeded(NBPTStop* pStop, NBEdgeCont& cont);
    double computeCrossProductEdgePosition(const NBEdge* edge, const Position& closestPlatform) const;

    static std::string getReverseID(const std::string& id);

public:
    static NBEdge* getReverseEdge(NBEdge* edge);


    void alignIdSigns();
};

#endif //SUMO_NBPTSTOPCONT_H
