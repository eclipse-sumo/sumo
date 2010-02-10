/****************************************************************************/
/// @file    MSCrossSection.h
/// @author  Christian Roessel
/// @date    Tue Nov 25 15:23:28 2003
/// @version $Id$
///
// A simple description of a position on a lane (crossing of a lane)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSCrossSection
 * @brief A simple description of a position on a lane (crossing of a lane)
 */
class MSCrossSection {
public:
    /** @brief Constructor
     *
     * @param[in] lane The lane to cross
     * @param[in] pos The position at the lane
     */
    MSCrossSection(MSLane* lane, SUMOReal pos) throw() : myLane(lane) , myPosition(pos) {}


public:
    /// @brief The lane to cross
    MSLane* myLane;

    /// @brief The position at the lane
    SUMOReal myPosition;

};


typedef std::vector< MSCrossSection > CrossSectionVector;
typedef CrossSectionVector::iterator CrossSectionVectorIt;
typedef CrossSectionVector::const_iterator CrossSectionVectorConstIt;


#endif

/****************************************************************************/

