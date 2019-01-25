/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NGFrame.h
/// @author  Daniel Krajzewicz
/// @date    06.05.2011
/// @version $Id$
///
// Sets and checks options for netgen
/****************************************************************************/
#ifndef NGFrame_h
#define NGFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NGFrame
 * @brief Sets and checks options for netgen
 */
class NGFrame {
public:
    /** @brief Inserts options used by the network generator
     */
    static void fillOptions();


    /** @brief Checks set options from the OptionsCont-singleton for being valid
     * @return Whether needed (netgen) options are set and have proper values
     */
    static bool checkOptions();


};


#endif

/****************************************************************************/

