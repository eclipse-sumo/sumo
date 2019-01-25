/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ROFrame.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// Sets and checks options for routing
/****************************************************************************/
#ifndef ROFrame_h
#define ROFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROFrame
 * @brief Sets and checks options for routing
 *
 * Normally, these methods are called from another frame (ROJTRFrame, RODUAFrame)...
 */
class ROFrame {
public:
    /** @brief Inserts options used by routing applications into the OptionsCont-singleton
     * @param[in] oc The options container to fill
     */
    static void fillOptions(OptionsCont& oc);


    /** @brief Checks whether options are valid
     *
     * To be valid,
     * @arg an output file must be given
     * @arg max-alternatives must not be lower than 2
     *
     * @param[in] oc The options container to fill
     * @return Whether all needed options are set
     * @todo probably, more things should be checked...
     */
    static bool checkOptions(OptionsCont& oc);

};


#endif

/****************************************************************************/

