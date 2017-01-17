/****************************************************************************/
/// @file    NIImporter_RobocupRescue.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in robocup rescue league format
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
 * @brief Importer for networks stored in robocup rescue league format
 *
 */
class NIImporter_RobocupRescue {
public:
    /** @brief Loads content of the optionally given RoboCup Rescue League files
     *
     * If the option "robocup-dir" is set, the files contained in the folder are read.
     *
     * If the option "robocup-dir" is not set, this method simply returns.
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void loadNetwork(const OptionsCont& oc, NBNetBuilder& nb);


protected:
    /** @brief Constructor
     *
     * @param[in] nc The node control to fill
     * @param[in] ec The edge control to fill
     */
    NIImporter_RobocupRescue(NBNodeCont& nc, NBEdgeCont& ec);


    /// @brief Destructor
    ~NIImporter_RobocupRescue();


    /** @brief Loads nodes from the given file
     * @param[in] file The file to read nodes from
     */
    void loadNodes(const std::string& file);


    /** @brief Loads edges from the given file
     * @param[in] file The file to read edges from
     */
    void loadEdges(const std::string& file);


protected:
    /// @brief The node container to fill
    NBNodeCont& myNodeCont;

    /// @brief The edge container to fill
    NBEdgeCont& myEdgeCont;

private:
    /// @brief Invalidated assignment operator
    NIImporter_RobocupRescue& operator=(const NIImporter_RobocupRescue& s);
};


#endif

/****************************************************************************/

