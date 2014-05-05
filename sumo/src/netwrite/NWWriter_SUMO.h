/****************************************************************************/
/// @file    NWWriter_SUMO.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 04.05.2011
/// @version $Id$
///
// Exporter writing networks using the SUMO format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NWWriter_SUMO_h
#define NWWriter_SUMO_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <netbuild/NBEdge.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>
#include <netbuild/NBConnectionDefs.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class OptionsCont;
class NBNetBuilder;
class NBTrafficLightLogicCont;
class NBNode;
class NBDistrict;
class NBEdgeControl;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NWWriter_SUMO
 * @brief Exporter writing networks using the SUMO format
 *
 */
class NWWriter_SUMO {
public:

    enum ConnectionStyle {
        SUMONET, // all connection information
        PLAIN,   // only edges and link indices
        TLL      // like plain but include tl information
    };

    /** @brief Writes the network into a SUMO-file
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void writeNetwork(const OptionsCont& oc, NBNetBuilder& nb);


    /** @brief Writes connections outgoing from the given edge (also used in NWWriter_XML)
     * @param[in] into The device to write the edge into
     * @param[in] from The edge to write connections for
     * @param[in] c The connection to write
     * @param[in] includeInternal Whether information about inner-lanes used to cross the intersection shall be written
     * @param[in] plain Whether only plain-xml output should be written (omit some attributes)
     */
    static void writeConnection(OutputDevice& into, const NBEdge& from, const NBEdge::Connection& c,
                                bool includeInternal, ConnectionStyle style = SUMONET);

    /// @brief writes the given prohibitions
    static void writeProhibitions(OutputDevice& into, const NBConnectionProhibits& prohibitions);

    /// @brief writes the traffic light logics to the given device
    static void writeTrafficLights(OutputDevice& into, const NBTrafficLightLogicCont& tllCont);

    /// @brief writes the location element
    static void writeLocation(OutputDevice& into);

    /// @brief writes allowed disallowed attributes if needed;
    static void writePermissions(OutputDevice& into, SVCPermissions permissions);

    /// @brief writes allowed disallowed attributes if needed;
    static void writePreferences(OutputDevice& into, SVCPermissions preferred);

protected:
    /// @name Methods for writing network parts
    /// @{

    /** @brief Writes internal edges (<edge ... with id[0]==':') of the given node
     * @param[in] into The device to write the edges into
     * @param[in] n The node to write the edges of
     * @param[in] origNames Whether original names shall be written as parameter
     * @return Whether an internal edge was written
     */
    static bool writeInternalEdges(OutputDevice& into, const NBNode& n, bool origNames);


    /** @brief Writes an edge (<edge ...)
     * @param[in] into The device to write the edge into
     * @param[in] e The edge to write
     * @param[in] noNames Whether names shall be ignored
     * @param[in] origNames Whether original names shall be written as parameter
     * @see writeLane()
     */
    static void writeEdge(OutputDevice& into, const NBEdge& e, bool noNames, bool origNames);


    /** @brief Writes a lane (<lane ...) of an edge
     * @param[in] into The device to write the edge into
     * @param[in] lID The ID of the lane
     * @param[in] eID The ID of the edge
     * @param[in] length Lane's length
     * @param[in] index The index of the lane within the edge
     * @param[in] origNames Whether original names shall be written as parameter
     */
    static void writeLane(OutputDevice& into, const std::string& eID, const std::string& lID,
                          SUMOReal speed, SVCPermissions permissions, SVCPermissions preferred,
                          SUMOReal endOffset, SUMOReal width, const PositionVector& shape,
                          const std::string& origID, SUMOReal length, unsigned int index, bool origNames);


    /** @brief Writes a junction (<junction ...)
     * @param[in] into The device to write the edge into
     * @param[in] n The junction/node to write
     * @param[in] checkLaneFoes Whether laneConflicts shall be checked at this junction
     */
    static void writeJunction(OutputDevice& into, const NBNode& n, const bool checkLaneFoes);


    /** @brief Writes internal junctions (<junction with id[0]==':' ...) of the given node
     * @param[in] into The device to write the edge into
     * @param[in] n The junction/node to write internal nodes for
     */
    static bool writeInternalNodes(OutputDevice& into, const NBNode& n);


    /** @brief Writes inner connections within the node
     * @param[in] into The device to write the edge into
     * @param[in] n The node to write inner links for
     */
    static bool writeInternalConnections(OutputDevice& into, const NBNode& n);


    /** @brief Writes a roundabout
     * @param[in] into The device to write the edge into
     * @param[in] r The roundabout to write
     * @param[in] ec The edge control to retrieve named edges from
     */
    static void writeRoundabout(OutputDevice& into, const std::vector<std::string>& r,
                                const NBEdgeCont& ec);


    /** @brief Writes a district
     * @param[in] into The device to write the edge into
     * @param[in] d The district
     */
    static void writeDistrict(OutputDevice& into, const NBDistrict& d);


private:
    /** @brief Writes a single internal connection
     * @param[in] from The id of the from-edge
     * @param[in] to The id of the to-edge
     * @param[in] toLane The indexd of the to-lane
     * @param[in] via The (optional) via edge
     */
    static void writeInternalConnection(OutputDevice& into,
                                        const std::string& from, const std::string& to,
                                        int fromLane, int toLane, const std::string& via);

    /// @brief writes a SUMOTime as int if possible, otherwise as a float
    static std::string writeSUMOTime(SUMOTime time);


    /// @brief the attribute value for a prohibition
    static std::string prohibitionConnection(const NBConnection& c);
};


#endif

/****************************************************************************/

