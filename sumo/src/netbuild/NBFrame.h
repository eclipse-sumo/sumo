/****************************************************************************/
/// @file    NBFrame.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    09.05.2011
/// @version $Id$
///
// Sets and checks options for netbuild
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBFrame_h
#define NBFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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

