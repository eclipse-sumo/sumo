/****************************************************************************/
/// @file    NWWriter_XML.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 11.05.2011
/// @version $Id$
///
// Exporter writing networks using XML (native input) format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NWWriter_XML_h
#define NWWriter_XML_h


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
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdge;
class NBEdgeCont;
class NBNetBuilder;
class NBNode;
class NBNodeCont;
class NBTrafficLightLogicCont;
class NBTypeCont;
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NWWriter_XML
 * @brief Exporter writing networks using XML (native input) format
 *
 */
class NWWriter_XML {
public:
    /** @brief Writes the network into XML-files (nodes, edges, connections,
     *   traffic lights)
     * @param[in] oc The options to use
     * @param[in] nb The network builder from which to read data
     */
    static void writeNetwork(const OptionsCont& oc, NBNetBuilder& nb);

    /** @brief Writes the joined-juncionts to file
     * @param[in] oc The options to use
     * @param[in] nc The node-container from which to read data
     */
    static void writeJoinedJunctions(const OptionsCont& oc, NBNodeCont& nc);

    /** @brief Writes street signs as POIs to file
     * @param[in] oc The options to use
     * @param[in] ec The edge-container from which to read data
     */
    static void writeStreetSigns(const OptionsCont& oc, NBEdgeCont& ec);

private:
    /** @brief Writes the nodes file
     * @param[in] oc The options to use
     * @param[in] nc The node-container from which to read data
     */
    static void writeNodes(const OptionsCont& oc, NBNodeCont& nc);

    /** @brief Writes the types file
     * @param[in] oc The options to use
     * @param[in] nc The type-container from which to read data
     */
    static void writeTypes(const OptionsCont& oc, NBTypeCont& tc);

    /** @brief Writes the edges and connections files
     * @param[in] oc The options to use
     * @param[in] nb The network build from which to read data
     */
    static void writeEdgesAndConnections(const OptionsCont& oc, NBNodeCont& nc, NBEdgeCont& ec);


    /** @brief Writes the traffic lights file
     * @param[in] oc The options to use
     * @param[in] tc The tll-container from which to read data
     * @param[in] ec The edge-container from which to read data
     */
    static void writeTrafficLights(const OptionsCont& oc, NBTrafficLightLogicCont& tc, NBEdgeCont& ec);

};


#endif

/****************************************************************************/

