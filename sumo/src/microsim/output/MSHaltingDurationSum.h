#ifndef MSHALTINGDURATIONSUM_H
#define MSHALTINGDURATIONSUM_H

///
/// @file    MSHaltingDurationSum.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Fri Nov 28 2003 19:21 CET
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
#include "MSDetectorHaltingContainerWrapper.h"
#include <microsim/MSUnit.h>
#include <string>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSHaltingDurationSum
    :
    public halt::HaltEndObserver
{
public:

protected:
    typedef double DetectorAggregate;
    typedef halt::HaltEndObserver::ParameterType ParameterType;
    typedef DetectorContainer::HaltingsList Container;

    MSHaltingDurationSum( Container& toObserve )
        :
        toObserveM( toObserve )
        {
            toObserveM.attach( this );
        }

    virtual ~MSHaltingDurationSum()
        {
            toObserveM.detach( this );
        }

    virtual void update( ParameterType aObserved ) = 0;

    DetectorAggregate getValue( ParameterType aObserved )
        {
            return MSUnit::getInstance()->getSeconds(
                aObserved.haltingDurationM );
        }

    static std::string getDetectorName( void )
        {
            return "haltDurationSum";
        }

private:

    halt::HaltEndSubject& toObserveM;
};


// Local Variables:
// mode:C++
// End:

#endif // MSHALTINGDURATIONSUM_H
