/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2026 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSCFModel_NaSch.h
/// @author  Jerry Lin
/// @date    Thu, 13 Aug 2026
///
// The Nagel-Schreckenberg (1992) cellular automaton car-following model
/****************************************************************************/
#pragma once
#include <config.h>

#include "MSCFModel.h"
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/** @class MSCFModel_NaSch
 * @brief The Nagel-Schreckenberg (1992) cellular automaton car-following model
 *
 * A discretized car-following model in the spirit of the original NaSch
 * cellular automaton. Unlike SUMO's continuous models it does not derive a
 * safe speed from the leader's speed; instead the gap to the leader alone
 * bounds the next speed. Speeds are always rounded down to a multiple of one
 * "cell" per simulation step, where the cell length is implied by the
 * vehicle's **accel** attribute (a cell is covered by accelerating for
 * exactly one step, see the constructor). Randomized deceleration ("dawdling")
 * is applied with probability **sigma**, reusing SUMO's generic imperfection
 * parameter for the model's classic per-step slowdown rule.
 *
 * @see MSCFModel
 * @see https://github.com/eclipse-sumo/sumo/issues/12182
 */
class MSCFModel_NaSch : public MSCFModel {
public:
    /** @brief Constructor
     *  @param[in] vtype the type for which this model is built and also the parameter object to configure this model
     *  @param[in] dawdle the probability of randomized deceleration by one cell per step ("sigma")
     */
    MSCFModel_NaSch(const MSVehicleType* vtype, double dawdle);


    /// @brief Destructor
    ~MSCFModel_NaSch();


    /// @name Implementations of the MSCFModel interface
    /// @{

    /** @brief Computes the vehicle's safe speed (no dawdling)
     *
     * NaSch rule 2 ("slowing down"): the leader's own speed is irrelevant,
     * only the (net) gap to the leader bounds the next speed.
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap2pred The (net) distance to the LEADER
     * @param[in] predSpeed The speed of LEADER (ignored, see above)
     * @return EGO's safe speed
     */
    double followSpeed(const MSVehicle* const veh, double speed, double gap2pred,
                       double predSpeed, double predMaxDecel, const MSVehicle* const pred = 0, const CalcReason usage = CalcReason::CURRENT) const;


    /** @brief Computes the vehicle's safe speed for approaching a non-moving obstacle (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap2pred The (net) distance to the obstacle
     * @param[in] decel unused, NaSch does not bound the deceleration used for stopping (see minNextSpeedEmergency)
     * @return EGO's safe speed for approaching a non-moving obstacle
     */
    double stopSpeed(const MSVehicle* const veh, const double speed, double gap2pred, double decel, const CalcReason usage = CalcReason::CURRENT) const;


    /// @brief NaSch rule 3 ("randomization"): dawdle by exactly one cell with probability sigma
    double patchSpeedBeforeLC(const MSVehicle* veh, double vMin, double vMax) const;


    /** @brief NaSch does not impose an extra bound on emergency braking: a vehicle
     * may always come to a full (Euler) resp. arbitrarily quick (ballistic) stop
     * within a single step if the gap requires it.
     */
    double minNextSpeedEmergency(double speed, const MSVehicle* const veh = 0) const;


    /** @brief Returns the model's name
     * @return The model's name
     */
    int getModelID() const {
        return SUMO_TAG_CF_NASCH;
    }


    /** @brief Get the driver's imperfection
     * @return The dawdle probability of drivers of this class
     */
    double getImperfection() const {
        return myDawdle;
    }
    /// @}


    /** @brief Sets a new value for driver imperfection
     * @param[in] imperfection The new dawdle probability
     */
    void setImperfection(double imperfection) {
        myDawdle = imperfection;
    }


    /** @brief Duplicates the car-following model
     * @param[in] vtype The vehicle type this model belongs to (1:1)
     * @return A duplicate of this car-following model
     */
    MSCFModel* duplicate(const MSVehicleType* vtype) const;

protected:
    /** @brief Rounds a speed down to the nearest multiple of the cell speed
     * (the speed gained by accelerating for exactly one simulation step).
     * NaSch speeds always correspond to a whole number of cells per step.
     * @param[in] speed the continuous speed to discretize
     * @return the largest multiple of the cell speed that does not exceed speed (0 if speed <= 0)
     */
    double roundToCell(double speed) const;

    /// @brief the safe speed considering only the gap to the leader/obstacle (NaSch rule 1+2)
    double vsafe(const MSVehicle* const veh, double speed, double gap) const;

protected:
    /// @brief The probability of randomized deceleration by one cell per step ("sigma")
    double myDawdle;

    /// @brief The speed gained per step by accelerating at myAccel, i.e. one cell's worth of speed
    double myCellSpeed;
};
