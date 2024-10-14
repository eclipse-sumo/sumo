/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2013-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSDevice_GLOSA.h
/// @author  Jakob Erdmann
/// @date    21.04.2021
///
// A device for Green Light Optimal Speed Advisory
/****************************************************************************/
#pragma once
#include <config.h>

#include "MSVehicleDevice.h"
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOTrafficObject;
class MSLink;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_GLOSA
 * @brief A device which collects info on the vehicle trip (mainly on departure and arrival)
 *
 * Each device collects departure time, lane and speed and the same for arrival.
 *
 * @see MSDevice
 */
class MSDevice_GLOSA : public MSVehicleDevice {
public:
    /** @brief Inserts MSDevice_GLOSA-options
     * @param[filled] oc The options container to add the options to
     */
    static void insertOptions(OptionsCont& oc);


    /** @brief Build devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether a example-device shall be built
     *  for the given vehicle.
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[filled] into The vector to store the built device in
     */
    static void buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into);

    /// @brief resets counters
    static void cleanup();

public:
    /// @brief Destructor.
    ~MSDevice_GLOSA();



    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{

    /** @brief updates distance and computes speed advice
     *
     * @param[in] veh Vehicle that asks this reminder.
     * @param[in] oldPos Position before move.
     * @param[in] newPos Position after move with newSpeed.
     * @param[in] newSpeed Moving speed.
     *
     * @return True (always).
     */
    bool notifyMove(SUMOTrafficObject& veh, double oldPos,
                    double newPos, double newSpeed);


    /** @brief updates next tls link
     *
     * @param[in] veh The entering vehicle.
     * @param[in] reason how the vehicle enters the lane
     * @return Always true
     * @see MSMoveReminder::notifyEnter
     * @see MSMoveReminder::Notification
     */
    bool notifyEnter(SUMOTrafficObject& veh, MSMoveReminder::Notification reason, const MSLane* enteredLane = 0);


    /// @}


    /// @brief return the name for this type of device
    const std::string deviceName() const {
        return "glosa";
    }

    /** @brief Returns the precomputed, original factor by which the driver
               wants to be faster than the speed limit
     * @return Speed limit factor
     */
    inline double getOriginalSpeedFactor() const {
        return myOriginalSpeedFactor;
    }

    /** @brief Returns if the traffic light stop calculation of the CF model shall be ignored
     * @return Override stop calculation before traffic light
     */
    inline bool getOverrideSafety() const {
        return myOverrideSafety;
    }

    /** @brief Returns if the GLOSA device is currently changing the speedFactor
     * @return If speedFactor has been changed by GLOSA
     */
    inline bool isSpeedAdviceActive() const {
        return mySpeedAdviceActive;
    }

    /// @brief try to retrieve the given parameter from this device. Throw exception for unsupported key
    std::string getParameter(const std::string& key) const;

    /// @brief try to set the given parameter for this device. Throw exception for unsupported key
    void setParameter(const std::string& key, const std::string& value);

    /** @brief Called on writing tripinfo output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     * @see MSDevice::generateOutput
     */
    void generateOutput(OutputDevice* tripinfoOut) const;



private:

    /// @brief compute time to next (relevant) switch
    static double getTimeToSwitch(const MSLink* tlsLink, int& countOld);

    /// @brief compute time to next (relevant) switch the vehicle can reach
    static double getTimeToNextSwitch(const MSLink* tlsLink, bool &currentPhaseGreen, bool &currentPhaseStop, int& countOld);

    static double timeGreen(const MSLink* tlsLink);

    /// @brief return minimum number of seconds to reach the junction
    double earliest_arrival(double speed, double distance);

    /// @brief return maximum number of seconds to reach the junction
    double latest_arrival(double speed, double distance, double earliest);

    double distance_at_continuous_accel(double speed, double time);

    double time_to_junction_at_continuous_accel(double d, double v);

    /// @brief adapt speed to reach junction at green
    void adaptSpeed(double distance, double timeToJunction, double timeToSwitch, bool &solved);

    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_GLOSA(SUMOVehicle& holder, const std::string& id, double minSpeed, double range, double maxSpeedFactor,
        double addSwitchTime, bool useQueue,  bool overrideSafety, bool ignoreCFModel);



private:
    /// @brief myHolder cast to needed type
    MSVehicle& myVeh;

    /// @brief the upcoming traffic light
    const MSLink* myNextTLSLink;
    /// @brief the distance to the upcoming traffic light
    double myDistance;

    /// @brief minimum approach speed towards red light
    double myMinSpeed;
    /// @brief maximum communication range
    double myRange;
    /// @brief maximum speed factor when trying to reach green light
    double myMaxSpeedFactor;
    /// @brief Additional time the vehicle shall need to reach the intersection after the signal turns green
    double myAddSwitchTime;
    /// @brief if true ignore the current light state, always follow GLOSA's predicted state
    bool myOverrideSafety;
    /// @brief if true ignore non-critical speed calculations from the CF model, follow GLOSA's perfect speed calculation
    bool myIgnoreCFModel;

    /// @brief original speed factor
    double myOriginalSpeedFactor;

    /// @brief If speedFactor is currently beeing changed by the GLOSA device
    bool mySpeedAdviceActive;
    /// @brief if true the queue in front of the TLS is used for calculation
    bool  myUseQueue;


private:
    /// @brief Invalidated copy constructor.
    MSDevice_GLOSA(const MSDevice_GLOSA&);

    /// @brief Invalidated assignment operator.
    MSDevice_GLOSA& operator=(const MSDevice_GLOSA&);


};
