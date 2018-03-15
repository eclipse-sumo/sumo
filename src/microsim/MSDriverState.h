/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDriverState.h
/// @author  Michael Behrisch
/// @date    Tue, 21 Apr 2015
/// @version $Id$
///
// A class representing a vehicle driver's current mental state
/****************************************************************************/
#ifndef MSDriverState_h
#define MSDriverState_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <memory>
#include <utils/common/SUMOTime.h>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class declarations
// ===========================================================================
class MSJunction;
class MSLink;
class MSVehicle;
class MSLane;
class MSPerson;


// ===========================================================================
// class definitions
// ===========================================================================

/** @class MSDriverState
 * @brief  An object representing a traffic item. Used for influencing
 *         the task demand of the TCI car-following model.
 * @see MSCFModel_TCI
 */
class MSDriverState {

protected:
    /// @brief base class for VehicleCharacteristics, TLSCharacteristics, PedestrianCharacteristics, SpeedLimitCharacteristics, Junction Characteristics...
    /// @see TrafficItemType, @see MSCFModel_TCI
    struct MSTrafficItemCharacteristics {
        inline virtual ~MSTrafficItemCharacteristics() {};
    };

    // @brief Types of traffic items, @see TrafficItem
    enum MSTrafficItemType {
        TRAFFIC_ITEM_VEHICLE,
        TRAFFIC_ITEM_PEDESTRIAN,
        TRAFFIC_ITEM_SPEED_LIMIT,
        TRAFFIC_ITEM_JUNCTION
    };

    /** @class MSTrafficItem
     * @brief  An object representing a traffic item. Used for influencing
     *         the task demand of the TCI car-following model.
     * @see MSCFModel_TCI
     */
    struct MSTrafficItem {
        MSTrafficItem(MSTrafficItemType type, const std::string& id, std::shared_ptr<MSTrafficItemCharacteristics> data);
        static std::hash<std::string> hash;
        MSTrafficItemType type;
        size_t id_hash;
        std::shared_ptr<MSTrafficItemCharacteristics> data;
        SUMOTime remainingIntegrationTime;
        double integrationDemand;
        double latentDemand;
    };

    struct JunctionCharacteristics : MSTrafficItemCharacteristics {
        JunctionCharacteristics(const MSJunction* junction, const MSLink* egoLink, double dist) :
            junction(junction), approachingLink(egoLink), dist(dist) {};
        const MSJunction* junction;
        const MSLink* approachingLink;
        double dist;
    };

    struct PedestrianCharacteristics : MSTrafficItemCharacteristics {
        PedestrianCharacteristics(const MSPerson* pedestrian, double dist) :
            pedestrian(pedestrian), dist(dist) {};
        const MSPerson* pedestrian;
        double dist;
    };

    struct SpeedLimitCharacteristics : MSTrafficItemCharacteristics {
        SpeedLimitCharacteristics(const MSLane* lane, double dist, double limit) :
            dist(dist), limit(limit), lane(lane) {};
        const MSLane* lane;
        double dist;
        double limit;
    };

    struct VehicleCharacteristics : MSTrafficItemCharacteristics {
        VehicleCharacteristics(const MSVehicle* foe, double longitudinalDist, double lateralDist, double relativeSpeed) :
            longitudinalDist(longitudinalDist), lateralDist(lateralDist), foe(foe), relativeSpeed(relativeSpeed) {};
        const MSVehicle* foe;
        double longitudinalDist;
        double lateralDist;
        double relativeSpeed;
    };


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


public:

    MSDriverState(MSVehicle* veh);
    virtual ~MSDriverState() {};

    /// @name Interfaces to inform Driver Model about traffic items, which potentially
    ///       influence the driving difficulty.
    /// @{
    /** @brief Informs about leader.
    */
    virtual void registerLeader(const MSVehicle* leader, double gap, double relativeSpeed, double latGap = -1.);

    /** @brief Informs about pedestrian.
    */
    virtual void registerPedestrian(const MSPerson* pedestrian, double gap);

    /** @brief Informs about upcoming speed limit reduction.
    */
    virtual void registerSpeedLimit(const MSLane* lane, double speedLimit, double dist);

    /** @brief Informs about upcoming junction.
    */
    virtual void registerJunction(MSLink* link, double dist);

    /** @brief Takes into account vehicle-specific factors for the driving demand
    *          For instance, whether vehicle drives on an opposite direction lane, absolute speed, etc.
    */
    virtual void registerEgoVehicleState();

    /** @brief Trigger updates for the state variables according to the traffic situation
     *         (present traffic items)
    */
    virtual void update();
    /// @}


    /// @name Methods to obtain the current error quantities to be used by the car-following model
    /// @see TCIModel
    /// @{
    /// @see myAccelerationError
    inline double getAppliedAcceleration(double desiredAccel) {
        return desiredAccel + myAccelerationError.getState();
    };
    /// @see mySpeedPerceptionError
    inline double getPerceivedSpeedDifference(double trueSpeedDifference) {
        return trueSpeedDifference + mySpeedPerceptionError.getState();
    };
    /// @see myHeadwayPerceptionError
    inline double getPerceivedHeadway(double trueGap) {
        return trueGap + myHeadwayPerceptionError.getState();
    };
    /// @see myActionStepLength
    inline double getActionStepLength(){
        return myActionStepLength;
    };
    /// @}


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
    void calculateDrivingDifficulty();


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
    void calculateLatentDemand(std::shared_ptr<MSTrafficItem> ti) const;

    /// @brief Calculate demand induced by the given junction
    double calculateLatentJunctionDemand(std::shared_ptr<JunctionCharacteristics> ch) const;
    /// @brief Calculate demand induced by the given pedestrian
    double calculateLatentPedestrianDemand(std::shared_ptr<PedestrianCharacteristics> ch) const;
    /// @brief Calculate demand induced by the given pedestrian
    double calculateLatentSpeedLimitDemand(std::shared_ptr<SpeedLimitCharacteristics> ch) const;
    /// @brief Calculate demand induced by the given vehicle
    double calculateLatentVehicleDemand(std::shared_ptr<VehicleCharacteristics> ch) const;

    /// @brief Calculate integration demand induced by the given junction
    double calculateJunctionIntegrationDemand(std::shared_ptr<JunctionCharacteristics> ch) const;
    /// @brief Calculate integration demand induced by the given pedestrian
    double calculatePedestrianIntegrationDemand(std::shared_ptr<PedestrianCharacteristics> ch) const;
    /// @brief Calculate integration demand induced by the given pedestrian
    double calculateSpeedLimitIntegrationDemand(std::shared_ptr<SpeedLimitCharacteristics> ch) const;
    /// @brief Calculate integration demand induced by the given vehicle
    double calculateVehicleIntegrationDemand(std::shared_ptr<VehicleCharacteristics> ch) const;

    /// @brief Register known traffic item to persist
    void updateItemIntegration(std::shared_ptr<MSTrafficItem> ti) const;

    /// @brief Determine the integration demand and duration for a newly encountered traffic item (updated in place)
    ///        The integration demand takes effect during a short period after the first appearance of the item.
    void calculateIntegrationDemandAndTime(std::shared_ptr<MSTrafficItem> ti) const;

    /// @brief Calculate the integration time for an item approached with the given speed at given dist
    double calculateIntegrationTime(double dist, double speed) const;

    /// @brief Incorporate the item's demand into the total task demand.
    void integrateDemand(std::shared_ptr<MSTrafficItem> ti);

    /// @brief Called whenever the vehicle is notified about a traffic item encounter.
    void registerTrafficItem(std::shared_ptr<MSTrafficItem> ti);

private:

    MSVehicle* myVehicle;

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

    /// @name Field that reflect the current driving situation
    /// @{
    /// @brief Whether vehicle is driving on an opposite direction lane
    bool myAmOpposite;
    double myCurrentSpeed;
    double myCurrentAcceleration;
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

    /** @brief Factor for the demand if driving on an opposite direction lane
     */
    double myOppositeDirectionDrivingDemandFactor;

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



/// @brief Default values for the TCI Driver Model parameters
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
    static double myOppositeDirectionDrivingFactor;
};



#endif

/****************************************************************************/
