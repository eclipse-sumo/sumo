/****************************************************************************/
/// @file    MSDevice_BTreceiver.h
/// @author  Daniel Krajzewicz
/// @date    14.08.2013
/// @version $Id$
///
// A BT sender
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSDevice_BTreceiver_h
#define MSDevice_BTreceiver_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSDevice.h"
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_BTreceiver
 * @brief A BT sender
 *
 * @see MSDevice
 */
class MSDevice_BTreceiver : public MSDevice {
public:
    /** @brief Inserts MSDevice_BTreceiver-options
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
     * @param[in, filled] into The vector to store the built device in
     */
    static void buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into);


public:
    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{

    /** @brief Checks for waiting steps when the vehicle moves
     *
     * @param[in] veh Vehicle that asks this reminder.
     * @param[in] oldPos Position before move.
     * @param[in] newPos Position after move with newSpeed.
     * @param[in] newSpeed Moving speed.
     *
     * @return True (always).
     */
    bool notifyMove(SUMOVehicle& veh, SUMOReal oldPos,
                    SUMOReal newPos, SUMOReal newSpeed);
    /// @}


    /** @brief Called on writing tripinfo output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     * @see MSDevice::generateOutput
     */
    void generateOutput() const;


    /// @brief Destructor.
    ~MSDevice_BTreceiver();


private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_BTreceiver(SUMOVehicle& holder, const std::string& id, SUMOReal range);


private:
    /// @brief The range of the device
    SUMOReal myRange;


    class MeetingPoint {
    public:
        MeetingPoint(SUMOTime t_, const Position& observerPos_, SUMOReal observerSpeed_,
                     const Position& seenPos_, SUMOReal seenSpeed_)
            : t(t_), observerPos(observerPos_), observerSpeed(observerSpeed_),
              seenPos(seenPos_), seenSpeed(seenSpeed_) {}

        ~MeetingPoint() {}

    public:
        SUMOTime t;
        Position observerPos;
        SUMOReal observerSpeed;
        Position seenPos;
        SUMOReal seenSpeed;
    };

    /** @class SeenDevice
     * @brief Class representing a single seen device
     */
    class SeenDevice {
    public:
        /** @brief Constructor
         * @param[in] tBegin_ The first time the device was seen
         * @param[in] observerPos_ The position of the observer at tBegin
         * @param[in] observerSpeed_ The speed of the observer at tBegin
         * @param[in] seenPos_ The position of the seen device at tBegin
         * @param[in] seenSpeed_ The position of the seen device at tBegin
         */
        SeenDevice(const MeetingPoint& meetingBegin_)
            : meetingBegin(meetingBegin_), meetingEnd(meetingBegin_) {}

        ~SeenDevice() {}

    public:
        MeetingPoint meetingBegin;
        MeetingPoint meetingEnd;

    };


    std::map<std::string, SeenDevice*> myCurrentlySeen;
    std::map<std::string, std::vector<SeenDevice*> > mySeen;

private:
    /// @brief Invalidated copy constructor.
    MSDevice_BTreceiver(const MSDevice_BTreceiver&);

    /// @brief Invalidated assignment operator.
    MSDevice_BTreceiver& operator=(const MSDevice_BTreceiver&);


};


#endif

/****************************************************************************/

