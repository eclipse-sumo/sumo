#ifndef MSDETECTORPREDICATES_H
#define MSDETECTORPREDICATES_H

/**
 * @file   MSDetectorPredicates.h
 * @author Christian Roessel
 * @date   Started Thu Sep 11 16:15:30 2003
 * @version $Id$
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

// $Id$

#include "MSVehicle.h"
#include <functional>


template< typename ContainerItem >
class MSDetectorPredicates
{
public:
    struct PosGreater :
        public std::binary_function< ContainerItem, double, bool >
    {
        bool operator() ( const ContainerItem& item, double pos ) const;
    };

    struct VehEquals :
        public std::binary_function< ContainerItem, MSVehicle*, bool >
    {
        bool operator() ( const ContainerItem& item,
                          const MSVehicle* veh ) const;
    };
};

// specialization
template<>
class MSDetectorPredicates< MSVehicle* >
{
    struct PosGreater :
        public std::binary_function< MSVehicle*, double, bool >
    {
        bool operator() ( const MSVehicle* item, double pos ) const {
            return item->pos() > pos;
        }
    };

    struct VehEquals :
        public std::binary_function< MSVehicle*, MSVehicle*, bool >
    {
        bool operator() ( const MSVehicle* item,
                          const MSVehicle* veh ) const {
            return item == veh;
        }
    };
};


#endif

// Local Variables:
// mode:C++
// End:
