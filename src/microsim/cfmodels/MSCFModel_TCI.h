/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSCFModel_TCI.h
/// @author  Leonhard Luecken
/// @date    Tue, 5 Feb 2018
/// @version $Id$
///
// Task Capability Interface car-following model.
/****************************************************************************/
#ifndef MSCFModel_TCI_h
#define MSCFModel_TCI_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <memory>
#include <functional>
#include "MSCFModel_Krauss.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSTrafficItem;


// ===========================================================================
// class definitions
// ===========================================================================

/** @class MSCFModel_TCI
 * @brief Task Capability Interface car-following model.
 * @see MSCFModel
 */
class MSCFModel_TCI : public MSCFModel {
public:
    /** @brief Constructor
     * @param[in] accel The maximum acceleration
     * @param[in] decel The maximum deceleration
     * @param[in] emergencyDecel The maximum emergency deceleration
     * @param[in] apparentDecel The deceleration as expected by others
     * @param[in] dawdle The driver imperfection
     * @param[in] headwayTime The driver's desired headway
     */
    MSCFModel_TCI(const MSVehicleType* vtype, double accel, double decel,
                     double emergencyDecel, double apparentDecel, double headwayTime);


    /// @brief Destructor
    ~MSCFModel_TCI();

    /// @name Implementations of the MSCFModel interface
    /// @{
    /// @brief apply dawdling
    double patchSpeedBeforeLC(const MSVehicle* veh, double vMin, double vMax) const;

    /** @brief Computes the vehicle's safe speed for approaching a non-moving obstacle (no dawdling)
     * this uses the maximumSafeStopSpeed
     * @param[in] veh The vehicle (EGO)
     * @param[in] gap2pred The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle
     * @todo generic Interface, models can call for the values they need
     */
    double stopSpeed(const MSVehicle* const veh, const double speed, double gap2pred) const;


    /** @brief Computes the vehicle's safe speed (no dawdling)
     * this uses the maximumSafeFollowSpeed
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The speed of LEADER
     * @return EGO's safe speed
     */
    double followSpeed(const MSVehicle* const veh, double speed, double gap2pred, double predSpeed, double predMaxDecel) const;


    /** @brief Returns the model's name
     * @return The model's name
     * @see MSCFModel::getModelName
     */
    int getModelID() const {
        return SUMO_TAG_CF_TCI;
    }

    /// @}


    /** @brief Duplicates the car-following model
     * @param[in] vtype The vehicle type this model belongs to (1:1)
     * @return A duplicate of this car-following model
     */
    MSCFModel* duplicate(const MSVehicleType* vtype) const;


    /** @brief Called whenever the vehicle is notified about a traffic item encounter.
     */
    void registerTrafficItem(std::shared_ptr<MSTrafficItem> ti);

private:
    /// @class OUProcess
    /// @brief An Ornstein-Uhlenbeck stochastic process
    class OUProcess {
    public:
        /// @brief constructor
        OUProcess(double initialState, double timeScale, double noiseIntensity);
        /// @brief destructor
        ~OUProcess();

        /// @brief evolve for a time step of length dt.
        void step(double dt);

        /// @brief set the process' timescale to a new value
        void setTimeScale(double timeScale) {
            myTimeScale = timeScale;
        };

        /// @brief set the process' noise intensity to a new value
        void setNoiseIntensity(double noiseIntensity) {
            myNoiseIntensity = noiseIntensity;
        };


        /// @brief set the process' state to a new value
        void setState(double state) {
            myState = state;
        };

        /// @brief Obtain the current state of the process
        double getState() const;

    private:
        /** @brief The current state of the process
         */
        double myState;

        /** @brief The time scale of the process
         */
        double myTimeScale;

        /** @brief The noise intensity of the process
         */
        double myNoiseIntensity;

    };

private:

    /** @brief Updates the internal variables to track the time between
     *        two calls to the state update (i.e., two action points). Needed for a consistent
     *        evolution of the error processes.
     */
    void updateStepDuration();

    /** @brief Calculates a value for the task difficulty given the capability and the demand
     *         and stores the result in myCurrentDrivingDifficulty.
     *  @see difficultyFunction()
     */
    void calculateDrivingDifficulty(double capability, double demand);


    /** @brief Transformation of the quotient demand/capability to obtain the actual
     *         difficulty value used to control driving performance.
     *  @note  The current implementation is a continuous piecewise affine function.
     *         It has two regions with different slopes. A slight ascend, where the capability
     *         is larger than the demand and a region of steeper ascend, where the demand
     *         exceeds the capability.
     */
    double difficultyFunction(double demandCapabilityQuotient) const;


    /** @brief Updates the myTaskCapability in dependence of the myTaskDifficulty to model a reactive
     *         level of attention. The characteristics of the process are determined by myHomeostasisDifficulty
     *         and myCapabilityTimeScale.
     *  @todo Review the implementation as simple exponential process.
     */
    void adaptTaskCapability();


    /// @name Updater for error processes.
    /// @{
    void updateAccelerationError();
    void updateSpeedPerceptionError();
    void updateHeadwayPerceptionError();
    void updateActionStepLength();
    /// @}


    /// @brief Updates the given error process
    void updateErrorProcess(OUProcess& errorProcess, double timeScaleCoefficient, double noiseIntensityCoefficient) const;

    /// @brief Initialize newly appeared traffic item
    void calculateLatentDemand(std::shared_ptr<MSTrafficItem> ti);

    /// @brief Register known traffic item to persist
    void updateItemIntegration(std::shared_ptr<MSTrafficItem> ti);

    /// @brief Determine the integration demand and duration for a newly encountered traffic item (updated in place)
    ///        The integration demand takes effect during a short period after the first appearance of the item.
    void calculateIntegrationDemandAndTime(std::shared_ptr<MSTrafficItem> ti);

    /// @brief Incorporate the item's demand into the total task demand.
    void integrateDemand(std::shared_ptr<MSTrafficItem> ti);


private:
    /// @name Variables for tracking update instants
    /// @see updateStepDuration()
    /// @{

    /// @brief Elapsed time since the last state update
    double myStepDuration;
    /// @brief Time point of the last state update
    double myLastUpdateTime;

    /// @}


    /// @name Dynamical quantities for the driving performance
    /// @{

    /** @brief Task capability (combines static and dynamic factors specific to the driver and the situation,
     *         total capability, attention, etc.). Follows myTaskDemand with some inertia (task-difficulty-homeostasis).
     */
    double myTaskCapability;
    double myMinTaskCapability, myMaxTaskCapability;

    /** @brief Task Demand (dynamic variable representing the total demand imposed on the driver by the driving situation and environment.
     *         For instance, number, novelty and type of traffic participants in neighborhood, speed differences, road curvature,
     *         headway to leader, number of lanes, traffic density, street signs, traffic lights)
     */
    double myTaskDemand;
    double myMaxTaskDemand;

    /** @brief Cached current value of the difficulty resulting from the combination of task capability and demand.
     *  @see calculateDrivingDifficulty()
     */
    double myCurrentDrivingDifficulty;
    /// @brief Upper bound for myCurrentDrivingDifficulty
    double myMaxDifficulty;
    /** @brief Slopes for the dependence of the difficulty on the quotient of demand and capability.
     *  @see   difficultyFunction();
     */
    double mySubCriticalDifficultyCoefficient, mySuperCriticalDifficultyCoefficient;

    /// @}


    /// @name Parameters for the dynamic adaptation of capability (attention) and demand
    /// @{

    /** @brief The desired value of the quotient myTaskDemand/myTaskCapability. Influences the fixed point of the
     *         process myTaskCapability -> myTaskDemand, @see adaptTaskCapability()
     */
    double myHomeostasisDifficulty;

    /** @brief Determines the time scale for the adaptation process of task capability towards the
     *         task difficulty.
     */
    double myCapabilityTimeScale;

    /// @}



    /** @brief Traffic items in the current neighborhood of the vehicle.
     */
    std::map<size_t, std::shared_ptr<MSTrafficItem> > myTrafficItems;
    std::map<size_t, std::shared_ptr<MSTrafficItem> > myNewTrafficItems;

    /// @name Actuation errors
    /// @{

    /** @brief Acceleration error. Modelled as an Ornstein-Uhlenbeck process.
     *  @see updateAccelerationError()
     */
    OUProcess myAccelerationError;
    /// @brief Coefficient controlling the impact of driving difficulty on the time scale of the acceleration error process
    double myAccelerationErrorTimeScaleCoefficient;
    /// @brief Coefficient controlling the impact of driving difficulty on the noise intensity of the acceleration error process
    double myAccelerationErrorNoiseIntensityCoefficient;

    /// @brief Action step length (increases with task difficulty, is similar to reaction time)
    double myActionStepLength;
    /// @brief Proportionality factor of myActionStepLength and driving difficulty
    double myActionStepLengthCoefficient;
    /// @brief Bounds for the action step length
    double myMinActionStepLength, myMaxActionStepLength;

    /// @}


    /// @name Perception errors
    /// @{

    /** @brief Error of estimation of the relative speeds of neighboring vehicles
     */
    OUProcess mySpeedPerceptionError;
    /// @brief Coefficient controlling the impact of driving difficulty on the time scale of the relative speed error process
    double mySpeedPerceptionErrorTimeScaleCoefficient;
    /// @brief Coefficient controlling the impact of driving difficulty on the noise intensity of the relative speed error process
    double mySpeedPerceptionErrorNoiseIntensityCoefficient;

    /** @brief Error of estimation of the distance/headways of neighboring vehicles
     */
    OUProcess myHeadwayPerceptionError;
    /// @brief Coefficient controlling the impact of driving difficulty on the time scale of the headway error process
    double myHeadwayPerceptionErrorTimeScaleCoefficient;
    /// @brief Coefficient controlling the impact of driving difficulty on the noise intensity of the headway error process
    double myHeadwayPerceptionErrorNoiseIntensityCoefficient;

    /// @}



};


/// @brief Default values for the MSCFModel_TCI parameters
struct TCIDefaults {
    static double myMinTaskCapability;
    static double myMaxTaskCapability;
    static double myMaxTaskDemand;
    static double myMaxDifficulty;
    static double mySubCriticalDifficultyCoefficient;
    static double mySuperCriticalDifficultyCoefficient;
    static double myHomeostasisDifficulty;
    static double myCapabilityTimeScale;
    static double myAccelerationErrorTimeScaleCoefficient;
    static double myAccelerationErrorNoiseIntensityCoefficient;
    static double myActionStepLengthCoefficient;
    static double myMinActionStepLength;
    static double myMaxActionStepLength;
    static double mySpeedPerceptionErrorTimeScaleCoefficient;
    static double mySpeedPerceptionErrorNoiseIntensityCoefficient;
    static double myHeadwayPerceptionErrorTimeScaleCoefficient;
    static double myHeadwayPerceptionErrorNoiseIntensityCoefficient;
};


#endif /* MSCFModel_TCI_H */

