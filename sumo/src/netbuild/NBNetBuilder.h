/****************************************************************************/
/// @file    NBNetBuilder.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// Instance responsible for building networks
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBNetBuilder_h
#define NBNetBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include "NBEdgeCont.h"
#include "NBTypeCont.h"
#include "NBNodeCont.h"
#include "NBNode.h"
#include "NBTrafficLightLogicCont.h"
#include "NBDistrictCont.h"
#include "NBJoinedEdgesMap.h"
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class OutputDevice;
class GeoConvHelper;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBNetBuilder
 * @brief Instance responsible for building networks
 *
 * The class' - and the netbuild module's - functionality is embedded within the
 *  compute() method.
 *
 * @addtogroup netbuild
 * @{
 *
 * -# Removing self loops
 *  \n Removes edges which end at the node they start at using NBNodeCont::removeSelfLoops().
 * -# Joining double connections
 *  \n Joins edges between same nodes using NBNodeCont::recheckEdges().
 * -# Finding isolated roads (optional)
 * -# Removing empty nodes and geometry nodes (optional)
 *  \n Removed nodes with no incoming/outgoing edges and nodes which can be transformed into
 *   geometry point using NBNodeCont::removeUnwishedNodes().
 * -# Removing unwished edges (optional)
 *  \n If "keep-edges.postload" and "keep-edges.explicit" are set, the edges not within "keep-edges.explicit" are
 *   removed from the network using NBEdgeCont::removeUnwishedEdges().
 * -# Rechecking nodes after edge removal (optional)
 *  \n If any of the edge removing options was set ("keep-edges.explicit", "remove-edges.explicit", "keep-edges.postload",
 *   "keep-edges.by-vclass", "keep-edges.input-file"), the now orphaned nodes are removed using
 *   NBNodeCont::removeUnwishedNodes().
 * -# Splitting geometry edges (optional)
 *  \n If "geometry.split" is set, edge geometries are converted to nodes using
 *   NBEdgeCont::splitGeometry().
 * -# Normalising/transposing node positions
 *  \n If "offset.disable-normalization", "offset.x", and "offset.y" are not
 *   set, the road graph's instances are moved to the origin.
 * -# Guessing and setting on-/off-ramps
 * -# Guessing and setting TLs
 * -# Computing turning directions
 * -# Sorting nodes' edges
 * -# Guessing and setting roundabouts
 * -# Computing Approached Edges
 * -# Computing Approaching Lanes
 * -# Dividing of Lanes on Approached Lanes
 * -# Appending Turnarounds (optional)
 * -# Rechecking of lane endings
 * -# Computing node shapes
 * -# Computing edge shapes
 * -# Computing tls logics
 * -# Computing node logics
 * -# Computing traffic light logics
 *
 *  @todo Removing unwished edges: Recheck whether this can be done during loading - whether this option/step is really needed.
 *  @todo Finding isolated roads: Describe
 *  @bug Removing empty nodes and geometry nodes: Ok, empty nodes should be removed, uh? But this is only done if "geometry.remove" is set.
 * @}
 */
class NBNetBuilder {
    friend class GNENet; // for triggering intermediate build steps

public:
    /// @brief Constructor
    NBNetBuilder();

    /// @brief Destructor
    ~NBNetBuilder();


    /** @brief Initialises the storage by applying given options
     *
     * Options, mainly steering the acceptance of edges, are parsed
     *  and the according internal variables are set.
     *
     * @param[in] oc The options container to read options from
     * @exception ProcessError If something fails (message is included)
     */
    void applyOptions(OptionsCont& oc);


    /** @brief Performs the network building steps
     *
     * @param[in] oc Container that contains options for building
     * @param[in] explicitTurnarounds List of edge ids for which turn-arounds should be added (used by NETEDIT)
     * @param[in] removeUnwishedNodes Whether to remove orphan nodes (and optionally convert some nodes to geometry)
     * @exception ProcessError (recheck)
     */
    void compute(OptionsCont& oc,
                 const std::set<std::string>& explicitTurnarounds = std::set<std::string>(),
                 bool removeUnwishedNodes = true);



    /// @name Retrieval of subcontainers
    /// @{

    /** @brief Returns the edge container
     * @return The edge container (reference)
     */
    NBEdgeCont& getEdgeCont() {
        return myEdgeCont;
    }


    /** @brief Returns the node container
     * @return The node container (reference)
     */
    NBNodeCont& getNodeCont() {
        return myNodeCont;
    }


    /** @brief Returns the type container
     * @return The type container (reference)
     */
    NBTypeCont& getTypeCont() {
        return myTypeCont;
    }


    /** @brief Returns the traffic light logics container
     * @return The traffic light logics container (reference)
     */
    NBTrafficLightLogicCont& getTLLogicCont() {
        return myTLLCont;
    }


    /** @brief Returns the districts container
     * @return The districts container (reference)
     */
    NBDistrictCont& getDistrictCont() {
        return myDistrictCont;
    }


    /** @brief Returns the determined roundabouts
     * @return The list of roundabout edges
     */
    const std::vector<EdgeVector>& getRoundabouts() const {
        return myRoundabouts;
    }


    /** @brief Returns the joined edges map
     * @return The map of joined edges
     */
    const NBJoinedEdgesMap& getJoinedEdgesMap() const {
        return myJoinedEdges;
    }
    /// @}


    /// @brief declare that roundabouts have been seen during loading
    void haveSeenRoundabouts() {
        myHaveSeenRoundabouts = true;
    }

    /**
     * @brief transforms loaded coordinates
     * handles projections, offsets (using GeoConvHelper) and import of height data (using Heightmapper if available)
     * @param[in,out] from The coordinate to be transformed
     * @param[in] includeInBoundary Whether to patch the convex boundary of the GeoConvHelper default instance
     * @param[in] from_srs The spatial reference system of the input coordinate
     * @notde These methods are located outside of GeoConvHelper to avoid linker-dependecies on INTERNAL_LIBS and GDAL for libgeom
     */
    static bool transformCoordinates(Position& from, bool includeInBoundary = true, GeoConvHelper* from_srs = 0);
    static bool transformCoordinates(PositionVector& from, bool includeInBoundary = true, GeoConvHelper* from_srs = 0);


protected:
    /**
     * @class by_id_sorter
     * @brief Sorts nodes by their ids
     */
    class by_id_sorter {
    public:
        /// @brief constructor
        explicit by_id_sorter() { }

        int operator()(const NBNode* n1, const NBNode* n2) const {
            return n1->getID() < n2->getID();
        }

    };

protected:
    /// @brief The used container for nodes
    NBNodeCont myNodeCont;

    /// @brief The used container for street types
    NBTypeCont myTypeCont;

    /// @brief The used container for edges
    NBEdgeCont myEdgeCont;

    /// @brief The used container for traffic light logics
    NBTrafficLightLogicCont myTLLCont;

    /// @brief The used container for districts
    NBDistrictCont myDistrictCont;

    /// @brief whether a sumo network with roundabout information was loaded
    bool myHaveSeenRoundabouts;

    /// @brief Edges marked as belonging to a roundabout (each EdgeVector is a roundabout)
    std::vector<EdgeVector> myRoundabouts;

    /// @brief Map of joined edges
    NBJoinedEdgesMap myJoinedEdges;

private:
    /// @brief shift network so its lower left corner is at 0,0
    void moveToOrigin(GeoConvHelper& geoConvHelper);

private:
    /// @brief invalidated copy constructor
    NBNetBuilder(const NBNetBuilder& s);

    /// @brief invalidated assignment operator
    NBNetBuilder& operator=(const NBNetBuilder& s);

};


#endif

/****************************************************************************/

