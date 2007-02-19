/****************************************************************************/
/// @file    MSPredicates.h
/// @author  Christian Roessel
/// @date    Thu Sep 11 16:15:30 2003
/// @version $Id$
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
#ifndef MSPredicates_h
#define MSPredicates_h
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

#include <microsim/MSVehicle.h>
//#include "MSHaltingDetectorContainer.h"
#include <functional>

// ===========================================================================
// namespace
// ===========================================================================
namespace Predicate
{
template< typename ContainerItem >
struct PosGreaterC :
            public std::binary_function< ContainerItem, SUMOReal, bool >
{
    bool operator()(const ContainerItem& item, SUMOReal pos) const;
};

// specialization
template<>
struct PosGreaterC< MSVehicle* > :
            public std::binary_function< MSVehicle*, SUMOReal, bool >
{
    bool operator()(const MSVehicle* item, SUMOReal pos) const
    {
        return item->getPositionOnLane() > pos;
    }
};

template< typename ContainerItem >
struct VehEqualsC :
            public std::binary_function< ContainerItem, MSVehicle*, bool >
{
    bool operator()(const ContainerItem& item,
                    const MSVehicle* veh) const;
};

// specialization
template<>
struct VehEqualsC< MSVehicle* > :
            public std::binary_function< MSVehicle*, MSVehicle*, bool >
{
    bool operator()(const MSVehicle* item,
                    const MSVehicle* veh) const
    {
        return item == veh;
    }
};
}


#endif

/****************************************************************************/

