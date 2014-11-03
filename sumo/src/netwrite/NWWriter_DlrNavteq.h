/****************************************************************************/
/// @file    NWWriter_DlrNavteq.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    26.10.2012
/// @version $Id$
///
// Exporter writing networks using DlrNavteq (Elmar) format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NWWriter_DlrNavteq_h
#define NWWriter_DlrNavteq_h


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
 * @class NWWriter_DlrNavteq
 * @brief Exporter writing networks using XML (native input) format
 *
 */
class NWWriter_DlrNavteq {
public:
    /** @brief Writes the network into XML-files (nodes, edges, connections,
     *   traffic lights)
     * @param[in] oc The options to use
     * @param[in] nb The network builder from which to read data
     */
    static void writeNetwork(const OptionsCont& oc, NBNetBuilder& nb);

    /// @brief get the navteq road class
    static int getRoadClass(NBEdge* edge);

private:
    /** @brief Writes the nodes_unsplitted file
     * @param[in] oc The options to use
     * @param[in] nc The node-container from which to read data
     * @param[in] ec The edge-container from which to read data
     */
    static void writeNodesUnsplitted(const OptionsCont& oc, NBNodeCont& nc, NBEdgeCont& ec);

    /** @brief Writes the links_unsplitted file
     * @param[in] oc The options to use
     * @param[in] nc The node-container from which to read data
     * @param[in] ec The edge-container from which to read data
     */
    static void writeLinksUnsplitted(const OptionsCont& oc, NBEdgeCont& ec);

    /** @brief Writes the traffic_signals file
     * @param[in] oc The options to use
     * @param[in] nc The node-container from which to read data
     */
    static void writeTrafficSignals(const OptionsCont& oc, NBNodeCont& nc);


    /// @brief write header comments (input paramters, date, etc...)
    static void writeHeader(OutputDevice& device, const OptionsCont& oc);

    /// @brief build the ascii-bit-vector for column vehicle_type
    static std::string getAllowedTypes(SVCPermissions permissions);

    /// @brief get the navteq speed class based on the speed in km/h
    static int getSpeedCategory(int kph);

    /// @brief get the SPEED_LIMIT as defined by elmar (upper bound of speed category)
    static int getSpeedCategoryUpperBound(int kph);

    /// @brief get the lane number encoding
    static unsigned int getNavteqLaneCode(const unsigned int numLanes);

    /// @brief get the length of the edge when measured up to the junction center
    static SUMOReal getGraphLength(NBEdge* edge);

    /// @brief magic value for undefined stuff
    static const std::string UNDEFINED;

    /// @brief get edge speed rounded to kmh
    static inline int speedInKph(SUMOReal metersPerSecond) {
        return (int)std::floor(metersPerSecond * 3.6 + 0.5);
    }
};


#endif

/****************************************************************************/

