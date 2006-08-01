#ifndef MSCROSSSECTION_H
#define MSCROSSSECTION_H

/**
 * @file   MSCrossSection.h
 * @author Christian Roessel
 * @date   Started Tue Nov 25 15:23:28 2003
 * @version
 * @brief
 *
 *
 */

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <microsim/MSUnit.h>
#include <vector>

/* =========================================================================
 * class declarations
 * ======================================================================= */

class MSLane;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSCrossSection
{
public:
    MSCrossSection( MSLane* lane, SUMOReal pos )
        : laneM( lane )
        , posM( pos )
        {}

    MSLane* laneM;
    SUMOReal posM;
};


typedef std::vector< MSCrossSection > CrossSectionVector;
typedef CrossSectionVector::iterator CrossSectionVectorIt;
typedef CrossSectionVector::const_iterator CrossSectionVectorConstIt;

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
