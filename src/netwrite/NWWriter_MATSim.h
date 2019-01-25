/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NWWriter_MATSim.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 04.05.2011
/// @version $Id$
///
// Exporter writing networks using the MATSim format
/****************************************************************************/
#ifndef NWWriter_MATSim_h
#define NWWriter_MATSim_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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
 * @class NWWriter_MATSim
 * @brief Exporter writing networks using the MATSim format
 *
 */
class NWWriter_MATSim {
public:
    /** @brief Writes the network into a MATSim-file
     *
     * @param[in] oc The options to use
     * @param[in] nb The network builder to fill
     */
    static void writeNetwork(const OptionsCont& oc, NBNetBuilder& nb);

};


#endif

/****************************************************************************/

