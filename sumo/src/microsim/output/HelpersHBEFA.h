/****************************************************************************/
/// @file    HelpersHBEFA.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Helper methods for HBEFA-based emission computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef HelpersHBEFA_h
#define HelpersHBEFA_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <cassert>
#include <microsim/output/MSDetectorFileOutput.h>
#include <microsim/MSMoveReminder.h>
#include <limits>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class MSEdgeControl;
class MSEdge;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class HelpersHBEFA
 * @brief Helper methods for HBEFA-based emission computation
 */
class HelpersHBEFA
{
public:
    static SUMOReal computeCO(SUMOEmissionClass c, double v, double a) throw();
    static SUMOReal computeCO2(SUMOEmissionClass c, double v, double a) throw();
    static SUMOReal computeNOx(SUMOEmissionClass c, double v, double a) throw();
    static SUMOReal computePMx(SUMOEmissionClass c, double v, double a) throw();
    static SUMOReal computeFuel(SUMOEmissionClass c, double v, double a) throw();
};


#endif

/****************************************************************************/

