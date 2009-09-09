/****************************************************************************/
/// @file    MSCFModel_IDM.h
/// @author  Tobias Mayer
/// @date    Thu, 03 Sep 2009
/// @version $Id$
///
// The IDM car-following model and parameter
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#ifndef MSCFMODEL_IDM_H
#define	MSCFMODEL_IDM_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSCFModel.h"

#define DELTA_IDM 4.0

class MSCFModel_IDM : public MSCFModel {
public:
    MSCFModel_IDM(const MSVehicleType* vtype, SUMOReal dawdle,
                  SUMOReal timeHeadWay, SUMOReal mingap) throw();

    ~MSCFModel_IDM() throw();

    SUMOReal ffeV(MSVehicle *veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed) const;

    SUMOReal ffeV(MSVehicle *veh, SUMOReal gap2pred, SUMOReal predSpeed) const;

    SUMOReal ffeV(MSVehicle *veh, MSVehicle *pred) const;

    SUMOReal ffeS(MSVehicle *veh, SUMOReal gap2pred) const;

    SUMOReal maxNextSpeed(SUMOReal speed) const;

    SUMOReal brakeGap(SUMOReal speed) const;

    SUMOReal approachingBrakeGap(SUMOReal speed) const;

    SUMOReal interactionGap(SUMOReal vF, SUMOReal laneMaxSpeed, SUMOReal vL) const;

    bool hasSafeGap(SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal laneMaxSpeed) const;

    SUMOReal safeEmitGap(SUMOReal speed) const;

    SUMOReal dawdle(SUMOReal speed) const;

    SUMOReal decelAbility() const;

    std::string getModelName() const throw() {
        return "idm";
    }

private:
    SUMOReal _updateSpeed(SUMOReal gap2pred, SUMOReal mySpeed, SUMOReal predSpeed, SUMOReal desSpeed) const;

    SUMOReal desiredSpeed(MSVehicle *veh) const;

    /// @todo needs to be removed
    // Dawdling is not part of IDM, but needs to exist here until
    // position updating is moved to MSCFModel.
    SUMOReal myDawdle;

    /// @brief The driver's reaction time [s]
    SUMOReal myTimeHeadWay;

    /// @brief The desired minimum Gap to the leading vehicle (no matter the speed)
    SUMOReal myMinSpace;

    /// The precomputed value for 1/(2*d)
    SUMOReal myInverseTwoDecel;

    /// @}
};

#endif	/* MSCFMODEL_IDM_H */
