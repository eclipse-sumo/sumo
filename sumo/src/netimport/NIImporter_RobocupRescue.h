/****************************************************************************/
/// @file    NIImporter_RobocupRescue.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in SUMO format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIImporter_RobocupRescue_h
#define NIImporter_RobocupRescue_h


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


// ===========================================================================
// class declarations
// ===========================================================================
class NBNetBuilder;
class NBEdge;
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIImporter_RobocupRescue
 * @brief Importer for networks stored in SUMO format
 *
 */
class NIImporter_RobocupRescue
{
public:
    /** @brief Loads network definition from the assigned option and stores it in the given network builder
     *
     * If the option "robocup-net" is set, the file stored therein is read and
     *  the network definition stored therein is stored within the given network
     *  builder.
     *
     * If the option "robocup-net" is not set, this method simply returns.
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void loadNetwork(const OptionsCont &oc, NBNetBuilder &nb);


protected:
    /** @brief Constructor
     *
     * @param[in] nc The node control to fill
     * @param[in] ec The edge control to fill
     */
    NIImporter_RobocupRescue(NBNodeCont &nc, NBEdgeCont &ec);


    /// @brief Destructor
    ~NIImporter_RobocupRescue() throw();


    void loadNodes(const std::string &file);
    void loadEdges(const std::string &file);


protected:
    /// @brief The node container to fill
    NBNodeCont &myNodeCont;

    /// @brief The edge container to fill
    NBEdgeCont &myEdgeCont;

};


#endif

/****************************************************************************/

