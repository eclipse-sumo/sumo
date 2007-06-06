/****************************************************************************/
/// @file    MSMeanData_Net_Cont.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSMeanData_Net_Cont_h
#define MSMeanData_Net_Cont_h


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
class MSMeanData_Net;
class MSEdgeControl;


// ===========================================================================
// class definitions
// ===========================================================================
typedef std::vector< MSMeanData_Net* > MSMeanData_Net_Cont;


#endif

/****************************************************************************/

