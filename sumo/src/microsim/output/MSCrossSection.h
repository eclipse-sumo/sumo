/****************************************************************************/
/// @file    MSCrossSection.h
/// @author  Christian Roessel
/// @date    Tue Nov 25 15:23:28 2003
/// @version $Id: $
///
// * @author Christian Roessel
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
#ifndef MSCrossSection_h
#define MSCrossSection_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSUnit.h>
#include <vector>

// ===========================================================================
// class declarations
// ===========================================================================

class MSLane;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class MSCrossSection
{
public:
    MSCrossSection(MSLane* lane, SUMOReal pos)
            : laneM(lane)
            , posM(pos)
    {}

    MSLane* laneM;
    SUMOReal posM;
};


typedef std::vector< MSCrossSection > CrossSectionVector;
typedef CrossSectionVector::iterator CrossSectionVectorIt;
typedef CrossSectionVector::const_iterator CrossSectionVectorConstIt;


#endif

/****************************************************************************/

