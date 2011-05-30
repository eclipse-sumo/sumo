/****************************************************************************/
/// @file    NWWriter_SUMO.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 04.05.2011
/// @version $Id$
///
// Exporter writing networks using the SUMO format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class NBNetBuilder;
class NBNode;
class NBDistrict;
class OptionsCont;


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
    /** @brief Writes the network into a SUMO-file
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void writeNetwork(const OptionsCont &oc, NBNetBuilder &nb);


protected:
    /// @name Methods for writing network parts
    /// @{

    /** @brief Writes internal edges (<edge ... with id[0]==':') of the given node
     * @param[in] into The device to write the edges into
     * @param[in] n The node to write the edges of
     * @return Whether an internal edge was written
     */
    static bool writeInternalEdges(OutputDevice &into, const NBNode &n);


    /** @brief Writes an edge (<edge ...)
     * @param[in] into The device to write the edge into
     * @param[in] e The edge to write
     * @see writeLane()
     */
    static void writeEdge(OutputDevice &into, const NBEdge &e);


    /** @brief Writes a lane (<lane ...) of an edge
     * @param[in] into The device to write the edge into
     * @param[in] e The edge to write
     * @param[in] lID The ID of the lane
     * @param[in] eID The ID of the edge
     * @param[in] lane Lane definition
     * @param[in] length Lane's length
     * @param[in] index The index of the lane within the edge
     */
    static void writeLane(OutputDevice &into, const std::string &lID, const std::string &eID, 
        const NBEdge::Lane &lane, SUMOReal length, unsigned int index);


    /** @brief Writes a junction (<junction ...)
     * @param[in] into The device to write the edge into
     * @param[in] n The junction/node to write
     */
    static void writeJunction(OutputDevice &into, const NBNode &n);


    /** @brief Writes internal junctions (<junction with id[0]==':' ...) of the given node
     * @param[in] into The device to write the edge into
     * @param[in] n The junction/node to write internal nodes for
     */
    static bool writeInternalNodes(OutputDevice &into, const NBNode &n);


    /** @brief Writes connections outgoing from the given lane
     * @param[in] into The device to write the edge into
     * @param[in] e The edge to write links for
     * @param[in] lane The edge's lane to write links for
     * @param[in] includeInternal Whether information about inner-lanes used to cross the intersection shall be written
     */
    static void writeSucceeding(OutputDevice &into, const NBEdge &e, unsigned int lane, bool includeInternal);


    /** @brief Writes inner connections within the node
     * @param[in] into The device to write the edge into
     * @param[in] n The node to write inner links for
     */
    static bool writeInternalSucceeding(OutputDevice &into, const NBNode &n);


    /** @brief Writes a roundabout
     * @param[in] into The device to write the edge into
     * @param[in] r The roundabout to write
     */
    static void writeRoundabout(OutputDevice &into, const std::set<NBEdge*> &r);


    /** @brief Writes a district
     * @param[in] into The device to write the edge into
     * @param[in] d The district
     */
    static void writeDistrict(OutputDevice &into, const NBDistrict &d);


private:
    /** @brief Writes a single internal edge 
     * @param[in] into The device to write the edges into
     */
    static void writeInternalEdge(OutputDevice &into, 
            const std::string &id, SUMOReal vmax, SUMOReal length, const PositionVector &shape);

};


#endif

/****************************************************************************/

