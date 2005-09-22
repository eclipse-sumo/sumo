#ifndef MSPREDICATES_H
#define MSPREDICATES_H

/**
 * @file   MSPredicates.h
 * @author Christian Roessel
 * @date   Started Thu Sep 11 16:15:30 2003
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

#include <microsim/MSVehicle.h>
//#include "MSHaltingDetectorContainer.h"
#include <functional>

/* =========================================================================
 * namespace
 * ======================================================================= */
namespace Predicate
{
    template< typename ContainerItem >
    struct PosGreaterC :
        public std::binary_function< ContainerItem, SUMOReal, bool >
    {
        bool operator() ( const ContainerItem& item, SUMOReal pos ) const;
    };

    // specialization
    template<>
    struct PosGreaterC< MSVehicle* > :
        public std::binary_function< MSVehicle*, SUMOReal, bool >
    {
        bool operator() ( const MSVehicle* item, SUMOReal pos ) const {
            return item->pos() > pos;
        }
    };

    template< typename ContainerItem >
    struct VehEqualsC :
        public std::binary_function< ContainerItem, MSVehicle*, bool >
    {
        bool operator() ( const ContainerItem& item,
                          const MSVehicle* veh ) const;
    };

    // specialization
    template<>
    struct VehEqualsC< MSVehicle* > :
        public std::binary_function< MSVehicle*, MSVehicle*, bool >
    {
        bool operator() ( const MSVehicle* item,
                          const MSVehicle* veh ) const {
            return item == veh;
        }
    };
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/


#endif

// Local Variables:
// mode:C++
// End:
