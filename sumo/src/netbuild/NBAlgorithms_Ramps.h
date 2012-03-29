/****************************************************************************/
/// @file    NBAlgorithms_Ramps.h
/// @author  Daniel Krajzewicz
/// @date    29. March 2012
/// @version $Id$
///
// Algorithms for highway on-/off-ramps computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBAlgorithms_Ramps_h
#define NBAlgorithms_Ramps_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class NBNetBuilder;
class OptionsCont;
class NBNode;
class NBEdgeCont;
class NBDistrictCont;


// ===========================================================================
// class definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// NBAlgorithms_Ramps
// ---------------------------------------------------------------------------
/* @class NBRampsComputer
 * @brief Computes highway on-/off-ramps (if wished)
 */
class NBRampsComputer {
public:
    /** @brief Computes highway on-/off-ramps (if wished)
     * @param[in, changed] nb The network builder which contains the current network representation
     * @param[in] oc The options container
     */
    static void computeRamps(NBNetBuilder &nb, OptionsCont& oc);

private:
    static bool mayNeedOnRamp(OptionsCont& oc, NBNode* cur);
    static bool mayNeedOffRamp(OptionsCont& oc, NBNode* cur);
    static bool buildOnRamp(OptionsCont& oc, NBNode* cur, NBNodeCont& nc, NBEdgeCont& ec, NBDistrictCont& dc, std::vector<NBEdge*>& incremented);
    static void buildOffRamp(OptionsCont& oc, NBNode* cur, NBNodeCont& nc, NBEdgeCont& ec, NBDistrictCont& dc, std::vector<NBEdge*>& incremented);
    static void checkHighwayRampOrder(NBEdge *&pot_highway, NBEdge *&pot_ramp);


};


#endif

/****************************************************************************/

