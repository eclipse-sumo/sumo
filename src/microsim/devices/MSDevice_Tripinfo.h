/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2009-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSDevice_Tripinfo.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Fri, 30.01.2009
/// @version $Id$
///
// A device which collects info on the vehicle trip
/****************************************************************************/
#ifndef MSDevice_Tripinfo_h
#define MSDevice_Tripinfo_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "MSVehicleDevice.h"
#include <utils/common/SUMOTime.h>

// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVehicle;
class SUMOTrafficObject;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_Tripinfo
 * @brief A device which collects info on the vehicle trip (mainly on departure and arrival)
 *
 * Each device collects departure time, lane and speed and the same for arrival.
 *
 * @see MSDevice
 */
class MSDevice_Tripinfo : public MSVehicleDevice {
public:
    /** @brief Build devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether a tripinfo-device shall be built
     *  for the given vehicle.
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[filled] into The vector to store the built device in
     */
    static void buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into);

    /// @brief generate output for vehicles which are still in the network
    static void generateOutputForUnfinished();

    /// @brief record tripinfo data for pedestrians
    static void addPedestrianData(double walkLength, SUMOTime walkDuration, SUMOTime walkTimeLoss);

    /// @brief record tripinfo data for rides
    static void addRideData(double rideLength, SUMOTime rideDuration, SUMOVehicleClass vClass, const std::string& line, SUMOTime waitingTime);

    /// @brief get statistics for printing to stdout
    static std::string printStatistics();

    /// @brief accessors for GUINet-Parameters
    static double getAvgRouteLength();
    static double getAvgDuration();
    static double getAvgWaitingTime();
    static double getAvgTimeLoss();
    static double getAvgDepartDelay();

    static double getAvgWalkRouteLength();
    static double getAvgWalkDuration();
    static double getAvgWalkTimeLoss();

    static double getAvgRideRouteLength();
    static double getAvgRideWaitingTime();
    static double getAvgRideDuration();

public:
    /// @brief Destructor.
    ~MSDevice_Tripinfo();


    /// @brief resets counters
    static void cleanup();


    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{

    /** @brief Checks for waiting steps when the vehicle moves
     *
     * @param[in] veh Vehicle that asks this reminder.
     * @param[in] oldPos Position before move.
     * @param[in] newPos Position after move with newSpeed.
     * @param[in] newSpeed Moving speed.
     * @return True (always).
     */
    bool notifyMove(SUMOTrafficObject& veh, double oldPos, double newPos, double newSpeed);


    /** @brief Saves departure info on insertion
     *
     * @param[in] veh The entering vehicle.
     * @param[in] reason how the vehicle enters the lane
     * @return Always true
     * @see MSMoveReminder::notifyEnter
     * @see MSMoveReminder::Notification
     */
    bool notifyEnter(SUMOTrafficObject& veh, MSMoveReminder::Notification reason, const MSLane* enteredLane = 0);


    /** @brief Saves arrival info
     *
     * @param[in] veh The leaving vehicle.
     * @param[in] lastPos Position on the lane when leaving.
     * @param[in] isArrival whether the vehicle arrived at its destination
     * @param[in] isLaneChange whether the vehicle changed from the lane
     * @return True if it did not leave the net.
     */
    bool notifyLeave(SUMOTrafficObject& veh, double lastPos, MSMoveReminder::Notification reason, const MSLane* enteredLane = 0);
    /// @}


    /// @brief return the name for this type of device
    const std::string deviceName() const {
        return "tripinfo";
    }

    /** @brief Called on writing tripinfo output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     * @see MSDevice::generateOutput
     */
    void generateOutput() const;

    /** @brief Saves the state of the device
     *
     * @param[in] out The OutputDevice to write the information into
     */
    void saveState(OutputDevice& out) const;

    /** @brief Loads the state of the device from the given description
     *
     * @param[in] attrs XML attributes describing the current state
     */
    void loadState(const SUMOSAXAttributes& attrs);


private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_Tripinfo(SUMOVehicle& holder, const std::string& id);


protected:
    /** @brief Internal notification about the vehicle moves, see MSMoveReminder::notifyMoveInternal()
     *
     */
    void notifyMoveInternal(const SUMOTrafficObject& veh,
                            const double frontOnLane,
                            const double timeOnLane,
                            const double meanSpeedFrontOnLane,
                            const double meanSpeedVehicleOnLane,
                            const double travelledDistanceFrontOnLane,
                            const double travelledDistanceVehicleOnLane,
                            const double /* meanLengthOnLane */);

private:
    /// @brief The lane the vehicle departed at
    std::string myDepartLane;

    /// @brief The speed on departure
    double myDepartSpeed;

    /// @brief The lateral depart position
    double myDepartPosLat;

    /// @brief The overall waiting time
    SUMOTime myWaitingTime;

    /// @brief Whether the vehicle is currently waiting
    bool myAmWaiting;

    /// @brief The overall number of unintended stops
    int myWaitingCount;

    /// @brief The overall intentional stopping time
    SUMOTime myStoppingTime;

    /// @brief The time when parking started
    SUMOTime myParkingStarted;

    /// @brief The vehicle's arrival time
    SUMOTime myArrivalTime;

    /// @brief The lane the vehicle arrived at
    std::string myArrivalLane;

    /// @brief The position on the lane the vehicle arrived at
    double myArrivalPos;

    /// @brief The lateral position on the lane the vehicle arrived at
    double myArrivalPosLat;

    /// @brief The speed when arriving
    double myArrivalSpeed;

    /// @brief The time loss when compared to the desired and allowed speed
    SUMOTime myMesoTimeLoss;

    /// @brief The route length
    double myRouteLength;

    /// @brief devices which may still need to produce output
    static std::set<const MSDevice_Tripinfo*, ComparatorNumericalIdLess> myPendingOutput;

    /// @brief global tripinfo statistics
    static double myVehicleCount;
    static double myTotalRouteLength;
    static SUMOTime myTotalDuration;
    static SUMOTime myTotalWaitingTime;
    static SUMOTime myTotalTimeLoss;
    static SUMOTime myTotalDepartDelay;
    static SUMOTime myWaitingDepartDelay;

    static int myWalkCount;
    static double myTotalWalkRouteLength;
    static SUMOTime myTotalWalkDuration;
    static SUMOTime myTotalWalkTimeLoss;

    static int myRideCount;
    static int myRideBusCount;
    static int myRideRailCount;
    static int myRideBikeCount;
    static int myRideAbortCount;
    static double myTotalRideWaitingTime;
    static double myTotalRideRouteLength;
    static SUMOTime myTotalRideDuration;

private:
    /// @brief Invalidated copy constructor.
    MSDevice_Tripinfo(const MSDevice_Tripinfo&);

    /// @brief Invalidated assignment operator.
    MSDevice_Tripinfo& operator=(const MSDevice_Tripinfo&);


};


#endif

/****************************************************************************/

