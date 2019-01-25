/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBFrame.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    09.05.2011
/// @version $Id$
///
// Sets and checks options for netbuild
/****************************************************************************/
#ifndef NBFrame_h
#define NBFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <iostream>
#include <vector>
#include <set>
#include "NBEdgeCont.h"
#include "NBTypeCont.h"
#include "NBNodeCont.h"
#include "NBNode.h"
#include "NBTrafficLightLogicCont.h"
#include "NBDistrictCont.h"
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBFrame
 * @brief Sets and checks options for netbuild
 */
class NBFrame {
public:
    /** @brief Inserts options used by the network converter
     * @param[in] forNetgen Whether netconvert options shall be omitted
     */
    static void fillOptions(bool forNetgen);


    /** @brief Checks set options from the OptionsCont-singleton for being valid
     * @return Whether needed (netbuild) options are set and have proper values
     */
    static bool checkOptions();


};


#endif

/****************************************************************************/

