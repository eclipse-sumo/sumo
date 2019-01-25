/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIImporter_RobocupRescue.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in robocup rescue league format
/****************************************************************************/
#ifndef NIImporter_RobocupRescue_h
#define NIImporter_RobocupRescue_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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

