/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NIFrame.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Sets and checks options for netimport
/****************************************************************************/
#ifndef NIFrame_h
#define NIFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class NIFrame
 * @brief Sets and checks options for netimport
 */
class NIFrame {
public:
    /** @brief Inserts options used by the network importer and network building modules
     *
     * Calls "NBNetBuilder::insertNetBuildOptions" for inserting network
     *  building options.
     */
    static void fillOptions(bool forNetedit = false);


    /** @brief Checks set options from the OptionsCont-singleton for being valid
     * @return Whether all needed options are set
     * @todo Unused currently; repair/fill
     */
    static bool checkOptions();


};


#endif

/****************************************************************************/

