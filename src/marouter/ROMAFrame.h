/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ROMAFrame.h
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Sets and checks options for ma-routing
/****************************************************************************/
#ifndef ROMAFrame_h
#define ROMAFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROMAFrame
 * @brief Sets and checks options for dua-routing
 */
class ROMAFrame {
public:
    /** @brief Inserts options used by duarouter into the OptionsCont-singleton
     *
     * As duarouter shares several options with other routing appplications, the
     *  insertion of these is done via a call to ROFrame::fillOptions.
     *
     * duarouter-specific options are added afterwards via calls to
     *  "addImportOptions" and "addDUAOptions".
     */
    static void fillOptions();


    /** @brief Checks set options from the OptionsCont-singleton for being valid for usage within duarouter
     *
     * Currently, this is done via a call to "ROFrame::checkOptions".
     *
     * @return Whether all needed options are set
     * @todo probably, more things should be checked...
     */
    static bool checkOptions();


protected:
    /** @brief Inserts import options used by duarouter into the OptionsCont-singleton
     */
    static void addImportOptions();


    /** @brief Inserts dua options used by duarouter into the OptionsCont-singleton
     */
    static void addAssignmentOptions();


};


#endif

/****************************************************************************/

