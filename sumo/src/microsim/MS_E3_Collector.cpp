/**
 * @file   MS_E3_Collector.cpp
 * @author Christian Roessel
 * @date   Started Tue Nov 25 12:40:18 2003
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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "MS_E3_Collector.h"

// static member
std::string MS_E3_Collector::xmlHeaderM(
"<?xml version=\"1.0\" standalone=\"yes\"?>\n\n"
"<!--\n"
"- nE3Vehicles [veh]\n"
"- meanTraveltime [s]\n"
"- meanNHaltsPerVehicle [n]\n"
"-->\n\n");

namespace E3
{
    DetType& operator++( DetType& det ) {
        return det = ( ALL == det ) ? MEAN_TRAVELTIME : DetType( det + 1 );
    }
    
    Containers& operator++( Containers& cont ) {
        return cont =
            ( TRAVELTIME == cont ) ? VEHICLES : Containers( cont + 1 );
    }
}
