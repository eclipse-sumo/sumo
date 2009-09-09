/****************************************************************************/
/// @file    MSCFModel.h
/// @author  Tobias Mayer
/// @date    Mon, 27 Jul 2009
/// @version $Id$
///
// The car-following model and parameter
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

#ifndef MSCFModel_h
#define	MSCFModel_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <string>
//#include "MSVehicle.h"
//#include "MSVehicleType.h"
#include <utils/common/FileHelpers.h>

// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicleType;
class MSVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSCFModel
 * @brief The car-following model abstraction
 *
 * MSCFModel is an interface for different car following Models to implement.
 *
 * It provides methods to compute a vehicles velocity for a simulation step.
 */
class MSCFModel {
public:
    /** @brief constructor
     *
     *  @param[in] rvtype a reference to the corresponding vtype
     */
    MSCFModel(const MSVehicleType* vtype) throw();

    /// @brief Destructor
    virtual ~MSCFModel() throw();

    /** general cf-model interface methods */

    // used by MSLane, can hopefully be removed eventually
    virtual SUMOReal ffeV(MSVehicle *veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed) const {
        return 0;
    }

    // used by MSLCM_DK2004, allows hypothetic values of gap2pred and predSpeed
    virtual SUMOReal ffeV(MSVehicle *veh, SUMOReal gap2pred, SUMOReal predSpeed) const {
        return 0;
    }

    // generic Interface, models can call for the values they need
    virtual SUMOReal ffeV(MSVehicle *veh, const MSVehicle * const pred) const {
        return 0;
    }

    virtual SUMOReal ffeS(MSVehicle *veh, SUMOReal gap2pred) const {
        return 0;
    }

    virtual SUMOReal maxNextSpeed(SUMOReal speed) const{
        return 0;
    }

    virtual SUMOReal brakeGap(SUMOReal speed) const {
        return 0;
    }
    
    virtual SUMOReal approachingBrakeGap(SUMOReal speed) const {
        return 0;
    }

    /// @todo evaluate signature
    virtual SUMOReal interactionGap(SUMOReal vF, SUMOReal laneMaxSpeed, SUMOReal vL) const {
        return 0;
    }

    /// @todo evaluate signature
    virtual bool hasSafeGap(SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal laneMaxSpeed) const {
        return false;
    }

    virtual SUMOReal safeEmitGap(SUMOReal speed) const {
        return 0;
    }

    // must exist until MSVehicle::move() is updated
    virtual SUMOReal dawdle(SUMOReal speed) const {
        return 0;
    }

    virtual SUMOReal decelAbility() const {
        return 0;
    }


        /** */
    SUMOReal timeHeadWayGap(SUMOReal speed) const {
        assert(speed >= 0);
        return SPEED2DIST(speed);
    }


    /// Saves the states of a vehicle
    virtual void saveState(std::ostream &os);


    virtual std::string getModelName() const throw() = 0;

 protected:
    const MSVehicleType* myType;

};


#endif	/* MSCFModel_h */

