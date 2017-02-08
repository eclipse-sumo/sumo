/****************************************************************************/
/// @file    MSCFModel_Rail.cpp
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
#include <iostream>
#include "MSCFModel_Rail.h"

MSCFModel_Rail::MSCFModel_Rail(const MSVehicleType *vtype, SUMOReal accel, SUMOReal decel, SUMOReal headwayTime)
        : MSCFModel_Krauss(vtype, accel,
                    decel,0,
                    headwayTime) {

}

MSCFModel_Rail::~MSCFModel_Rail() {

}
