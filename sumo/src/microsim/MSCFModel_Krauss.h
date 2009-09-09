/****************************************************************************/
/// @file    MSCFModel_Krauss.h
/// @author  Tobias Mayer
/// @date    Tue, 28 Jul 2009
/// @version $Id$
///
// The Krauss car-following model and parameter
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

#ifndef MSCFMODEL_KRAUSS_H
#define	MSCFMODEL_KRAUSS_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSCFModel.h"

class MSCFModel_Krauss : public MSCFModel {
public:     
    MSCFModel_Krauss(const MSVehicleType* vtype, SUMOReal dawdle, SUMOReal tau) throw();

    ~MSCFModel_Krauss() throw();

    SUMOReal ffeV(MSVehicle *veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed) const;

    SUMOReal ffeV(MSVehicle *veh, SUMOReal gap2pred, SUMOReal predSpeed) const;

    SUMOReal ffeV(MSVehicle *veh, const MSVehicle * const pred) const;

    SUMOReal ffeS(MSVehicle *veh, SUMOReal gap2pred) const;

    SUMOReal maxNextSpeed(SUMOReal speed) const;

    SUMOReal brakeGap(SUMOReal speed) const;

    SUMOReal approachingBrakeGap(SUMOReal speed) const;

    SUMOReal interactionGap(SUMOReal vF, SUMOReal laneMaxSpeed, SUMOReal vL) const;

    bool hasSafeGap(SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal laneMaxSpeed) const;

    SUMOReal safeEmitGap(SUMOReal speed) const;

    SUMOReal dawdle(SUMOReal speed) const;

    SUMOReal decelAbility() const;
private:
    SUMOReal _vsafe(SUMOReal gap2pred, SUMOReal predSpeed) const;


    /// @brief The vehicle's dawdle-parameter. 0 for no dawdling, 1 for max.
    SUMOReal myDawdle;

    /// @brief The driver's reaction time [s]
    SUMOReal myTau;

    /// The precomputed value for 1/(2*d)
    SUMOReal myInverseTwoDecel;

    /// The precomputed value for myDecel*myTau
    SUMOReal myTauDecel;
    /// @}
};

#endif	/* MSCFMODEL_KRAUSS_H */

