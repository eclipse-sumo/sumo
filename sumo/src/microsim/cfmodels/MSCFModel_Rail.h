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

class MSCFModel_Rail  : public MSCFModel {

public:
    virtual SUMOReal followSpeed(const MSVehicle *const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed,
                                 SUMOReal predMaxDecel) const;

    virtual SUMOReal stopSpeed(const MSVehicle *const veh, const SUMOReal speed, SUMOReal gap) const;

    virtual int getModelID() const;

    virtual MSCFModel *duplicate(const MSVehicleType *vtype) const;

public:
    /** @brief Constructor
     * @param[in] trainType The train type
     */
    MSCFModel_Rail(const MSVehicleType *vtype, std::string trainType);


    virtual ~MSCFModel_Rail();

    virtual SUMOReal maxNextSpeed(SUMOReal speed, const MSVehicle *const veh) const override;

    virtual SUMOReal minNextSpeed(SUMOReal speed, const MSVehicle *const veh) const override;
};


#endif //SUMO_MSCFMODEL_RAIL_H
