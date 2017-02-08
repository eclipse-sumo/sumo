/****************************************************************************/
/// @file    MSCFModel_Rail.h
/// @author  Gregor L\"ammel
/// @date    Tue, 08 Feb 2017
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SUMO_MSCFMODEL_RAIL_H
#define SUMO_MSCFMODEL_RAIL_H


#include "MSCFModel_Krauss.h"

class MSCFModel_Rail  : public MSCFModel_Krauss {

public:
    /** @brief Constructor
     * @param[in] accel The maximum acceleration
     * @param[in] decel The maximum deceleration
     * @param[in] headwayTime The haedwayTime
     */
    MSCFModel_Rail(const MSVehicleType *vtype, SUMOReal accel, SUMOReal decel, SUMOReal headwayTime);


    virtual ~MSCFModel_Rail();
};


#endif //SUMO_MSCFMODEL_RAIL_H
