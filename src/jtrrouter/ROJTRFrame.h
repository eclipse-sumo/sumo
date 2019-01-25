/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ROJTRFrame.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Sets and checks options for jtr-routing
/****************************************************************************/
#ifndef ROJTRFrame_h
#define ROJTRFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROJTRFrame
 * @brief Sets and checks options for jtr-routing
 */
class ROJTRFrame {
public:
    /** @brief Inserts options used by jtrrouter into the OptionsCont-singleton
     *
     * As jtrrouter shares several options with other routing appplications, the
     *  insertion of these is done via a call to ROFrame::fillOptions.
     *
     * jtrrouer-specific options are added afterwards.
     */
    static void fillOptions();


    /** @brief Checks set options from the OptionsCont-singleton for being valid for usage within jtrrouter
     * @return Whether all needed options are set
     * @todo probably, more things should be checked...
     */
    static bool checkOptions();

};


#endif

/****************************************************************************/

