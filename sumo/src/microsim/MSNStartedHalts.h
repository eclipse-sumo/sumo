#ifndef MSNSTARTEDHALTS_H
#define MSNSTARTEDHALTS_H

///
/// @file    MSNStartedHalts.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Fri Nov 28 2003 17:27 CET
/// @version $Id$
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

#include "MSDetectorHaltingContainerWrapper.h"
#include <string>

// E2 ED Detector
class MSNStartedHalts
    :
    public halt::HaltBeginObserver
{
public:

protected:
    typedef double DetectorAggregate;
    typedef halt::HaltEndObserver::Observed Observed;
    typedef DetectorContainer::HaltingsList Container;
//     typedef Container::HaltingsConstIt HaltingsConstIt;
//     typedef Container::InnerContainer Haltings;

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

    virtual void update( Observed& aObserved ) = 0;
    
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
