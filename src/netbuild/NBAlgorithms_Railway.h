/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBAlgorithms_Railway.h
/// @author  Jakob Erdmann
/// @author  Melanie Weber
/// @date    29. March 2018
/// @version $Id$
///
// Algorithms for railways
/****************************************************************************/
#ifndef NBAlgorithms_Railway_h
#define NBAlgorithms_Railway_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class NBNetBuilder;
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// NBAlgorithms_Railway
// ---------------------------------------------------------------------------
/* @class NBRampsComputer
 * @brief Computes highway on-/off-ramps (if wished)
 */
class NBRailwayTopologyAnalyzer {
public:
    /** @brief Computes highway on-/off-ramps (if wished)
     * @param[in, changed] nb The network builder which contains the current network representation
     * @param[in] oc The options container
     */
    static void analyzeTopology(NBNetBuilder& nb, OptionsCont& oc);


private:
};


#endif

/****************************************************************************/

