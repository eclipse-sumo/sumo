#ifndef MSNSTARTEDHALTS_H
#define MSNSTARTEDHALTS_H

///
/// @file    MSNStartedHalts.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Fri Nov 28 2003 17:27 CET
/// @version
///
/// @brief
///
///

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
#include <config.h>
#endif // HAVE_CONFIG_H

#include "MSDetectorHaltingContainerWrapper.h"
#include <string>
/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
// E2 ED Detector
class MSNStartedHalts
    :
    public halt::HaltBeginObserver
{
public:

protected:
    typedef SUMOReal DetectorAggregate;
    typedef halt::HaltEndObserver::ParameterType ParameterType;
    typedef DetectorContainer::HaltingsList Container;

    MSNStartedHalts( Container& toObserve )
        :
        toObserveM( toObserve )
        {
            toObserveM.attach( this );
        }

    virtual ~MSNStartedHalts()
        {
            toObserveM.detach( this );
        }

    virtual void update( ParameterType aObserved ) = 0;

    DetectorAggregate getValue( ParameterType )
        {
            return 1; // just count the number of halts.
        }

    static std::string getDetectorName( void )
        {
            return "nStartedHalts";
        }

private:

    halt::HaltBeginSubject& toObserveM;

};


// Local Variables:
// mode:C++
// End:

#endif // MSNSTARTEDHALTS_H
