/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    RODFFrame.h
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Michael Behrisch
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// Sets and checks options for df-routing
/****************************************************************************/
#ifndef RODFFrame_h
#define RODFFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RODFFrame
 * @brief Sets and checks options for df-routing
 */
class RODFFrame {
public:
    /** @brief Inserts options used by dfrouter into the OptionsCont-singleton
     */
    static void fillOptions();


    /** @brief Checks set options from the OptionsCont-singleton for being valid for usage within dfrouter
     *
     * @return Whether all needed options are set
     * @todo Unused currently; repair/fill
     */
    static bool checkOptions();

};


#endif

/****************************************************************************/

