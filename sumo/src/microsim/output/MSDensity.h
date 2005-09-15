#ifndef MSDENSITY_H
#define MSDENSITY_H

/**
 * @file   MSDensity.h
 * @author Christian Roessel
 * @date   Started Tue Sep  9 22:27:23 2003
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
// $Log$
// Revision 1.4  2005/09/15 11:09:33  dkrajzew
// LARGE CODE RECHECK
//
//

//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)

/* =========================================================================
 * included modules
 * ======================================================================= */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include "MSDetectorCounterContainerWrapper.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;
class MSLane;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSDensity
{
protected:
    typedef double DetectorAggregate;
    typedef DetectorContainer::Count Container;

    MSDensity( double lengthInMeters,
               const Container& counter );

    virtual ~MSDensity( void )
        {}

    DetectorAggregate getDetectorAggregate( void );

    static std::string getDetectorName( void )
        {
            return "density";
        }

private:
    double detectorLengthM; //  [km]
    const Container& counterM;

    MSDensity();
    MSDensity( const MSDensity& );
    MSDensity& operator=( const MSDensity& );
};
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/


#endif

// Local Variables:
// mode:C++
// End:
