/****************************************************************************/
/// @file    MSHaltingDurationSum.h
/// @author  Christian Roessel
/// @date    Fri Nov 28 2003 19:21 CET
/// @version $Id: $
///
// / @author  Christian Roessel <christian.roessel@dlr.de>
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
#ifndef MSHaltingDurationSum_h
#define MSHaltingDurationSum_h
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

#include "MSDetectorHaltingContainerWrapper.h"
#include <microsim/MSUnit.h>
#include <string>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class MSHaltingDurationSum
            :
            public halt::HaltEndObserver
{
public:

protected:
    typedef SUMOReal DetectorAggregate;
    typedef halt::HaltEndObserver::ParameterType ParameterType;
    typedef DetectorContainer::HaltingsList Container;

    MSHaltingDurationSum(Container& toObserve)
            :
            toObserveM(toObserve)
    {
        toObserveM.attach(this);
    }

    virtual ~MSHaltingDurationSum()
    {
        toObserveM.detach(this);
    }

    virtual void update(ParameterType aObserved) = 0;

    DetectorAggregate getValue(ParameterType aObserved)
    {
        return MSUnit::getInstance()->getSeconds(
                   aObserved.haltingDurationM);
    }

    static std::string getDetectorName(void)
    {
        return "haltDurationSum";
    }

private:

    halt::HaltEndSubject& toObserveM;
};


#endif

/****************************************************************************/

