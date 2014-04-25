/****************************************************************************/
/// @file    MSDevice_BTreceiver.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    14.08.2013
/// @version $Id$
///
// A BT receiver
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2013-2014 DLR (http://www.dlr.de/) and contributors
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
#include "MSDevice_BTsender.h"
#include <microsim/MSNet.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/Command.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_BTreceiver
 * @brief A BT receiver
 *
 * @see MSDevice
 */
class MSDevice_BTreceiver : public MSDevice {
public:
    /** @brief Inserts MSDevice_BTreceiver-options
     * @param[filled] oc The options container to add the options to
     */
    static void insertOptions(OptionsCont& oc);


    /** @brief Build devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether a bt-receiver-device shall be built
     *  for the given vehicle.
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[filled] into The vector to store the built device in
     */
    static void buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into);



public:
    /// @brief Destructor.
    ~MSDevice_BTreceiver();



    /// @name Methods inherited from MSMoveReminder.
    /// @{

    /** @brief Adds the vehicle to running vehicles if it (re-) enters the network
     *
     * @param[in] veh The entering vehicle.
     * @param[in] reason how the vehicle enters the lane
     * @return Always true
     * @see MSMoveReminder::notifyEnter
     * @see MSMoveReminder::Notification
     */
    bool notifyEnter(SUMOVehicle& veh, Notification reason);


    /** @brief Checks whether the reminder still has to be notified about the vehicle moves
     *
     * Indicator if the reminders is still active for the passed
     * vehicle/parameters. If false, the vehicle will erase this reminder
     * from it's reminder-container.
     *
     * @param[in] veh Vehicle that asks this reminder.
     * @param[in] oldPos Position before move.
     * @param[in] newPos Position after move with newSpeed.
     * @param[in] newSpeed Moving speed.
     *
     * @return True if vehicle hasn't passed the reminder completely.
     */
    bool notifyMove(SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed);


    /** @brief Moves (the known) vehicle from running to arrived vehicles' list
     *
     * @param[in] veh The leaving vehicle.
     * @param[in] lastPos Position on the lane when leaving.
     * @param[in] isArrival whether the vehicle arrived at its destination
     * @param[in] isLaneChange whether the vehicle changed from the lane
     * @see leaveDetectorByLaneChange
     * @see MSMoveReminder
     * @see MSMoveReminder::notifyLeave
     */
    bool notifyLeave(SUMOVehicle& veh, SUMOReal lastPos, Notification reason);
    //@}



    /** @class MeetingPoint
     * @brief Holds the information about exact positions/speeds/time of the begin/end of a meeting
     */
    class MeetingPoint {
    public:
        /** @brief Constructor
         * @param[in] _t The time of the meeting
         * @param[in] _observerPos The position the observer had at the time
         * @param[in] _observerSpeed The speed the observer had at the time
         * @param[in] _observerLaneID The lane the observer was at
         * @param[in] _observerLanePos The position at the lane of the observer
         * @param[in] _seenPos The position the seen vehicle had at the time
         * @param[in] _seenSpeed The speed the vehicle had at the time
         * @param[in] _seenLaneID The lane the vehicle was at
         * @param[in] _seenLanePos The position at the lane of the vehicle
         */
        MeetingPoint(SUMOReal _t, const Position& _observerPos, SUMOReal _observerSpeed,
                     const std::string& _observerLaneID, SUMOReal _observerLanePos,
                     const Position& _seenPos, SUMOReal _seenSpeed,
                     const std::string& _seenLaneID, SUMOReal _seenLanePos)
            : t(_t), observerPos(_observerPos), observerSpeed(_observerSpeed), observerLaneID(_observerLaneID), observerLanePos(_observerLanePos),
              seenPos(_seenPos), seenSpeed(_seenSpeed), seenLaneID(_seenLaneID), seenLanePos(_seenLanePos) {}

        /// @brief Destructor
        ~MeetingPoint() {}

    public:
        /// @brief The time of the meeting
        SUMOReal t;
        /// @brief The position the observer had at the time
        Position observerPos;
        /// @brief The speed the observer had at the time
        SUMOReal observerSpeed;
        /// @brief The lane the observer was at
        std::string observerLaneID;
        /// @brief The position at the lane of the observer
        SUMOReal observerLanePos;
        /// @brief The position the seen vehicle had at the time
        Position seenPos;
        /// @brief The speed the vehicle had at the time
        SUMOReal seenSpeed;
        /// @brief The lane the vehicle was at
        std::string seenLaneID;
        /// @brief The position at the lane of the vehicle
        SUMOReal seenLanePos;

    };



    /** @class SeenDevice
     * @brief Class representing a single seen device
     */
    class SeenDevice {
    public:
        /** @brief Constructor
         * @param[in] meetingBegin_ Description of the meeting's begin
         */
        SeenDevice(const MeetingPoint& meetingBegin_)
            : meetingBegin(meetingBegin_), meetingEnd(meetingBegin_), lastView(meetingBegin_.t) {}

        /// @brief Destructor
        ~SeenDevice() {
            for (std::vector<MeetingPoint*>::iterator i = recognitionPoints.begin(); i != recognitionPoints.end(); ++i) {
                delete *i;
            }
            recognitionPoints.clear();
        }


    public:
        /// @brief Description of the meeting's begin
        MeetingPoint meetingBegin;
        /// @brief Description of the meeting's end
        MeetingPoint meetingEnd;
        /// @brief Last recognition point
        SUMOReal lastView;
        /// @brief List of recognition points
        std::vector<MeetingPoint*> recognitionPoints;

    };



    /** @brief Clears the given containers deleting the stored items
     * @param[in] c The currently seen container to clear
     * @param[in] s The seen container to clear
     */
    static void cleanUp(std::map<std::string, SeenDevice*>& c, std::map<std::string, std::vector<SeenDevice*> >& s);



protected:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_BTreceiver(SUMOVehicle& holder, const std::string& id, SUMOReal range);



private:
    /// @brief The range of the device
    SUMOReal myRange;

    /// @brief Whether the bt-system was already initialised
    static bool myWasInitialised;



    /** @class VehicleInformation
     * @brief Stores the information of a vehicle
     */
    class VehicleState {
    public:
        /** @brief Constructor
         * @param[in] _time The current time
         * @param[in] _speed The speed of the vehicle
         * @param[in] _angle The angle of the vehicle
         * @param[in] _position The position of the vehicle
         * @param[in] _laneID The id of the lane the vehicle is located at
         * @param[in] _lanePos The position of the vehicle along the lane
         */
        VehicleState(SUMOReal _time, SUMOReal _speed, SUMOReal _angle, const Position& _position, const std::string& _laneID, SUMOReal _lanePos)
            : time(_time), speed(_speed), angle(_angle), position(_position), laneID(_laneID), lanePos(_lanePos) {}

        /// @brief Destructor
        ~VehicleState() {}

        /// @brief The current time
        SUMOReal time;
        /// @brief The speed of the vehicle
        SUMOReal speed;
        /// @brief The angle of the vehicle
        SUMOReal angle;
        /// @brief The position of the vehicle
        Position position;
        /// @brief The lane the vehicle was at
        std::string laneID;
        /// @brief The position at the lane of the vehicle
        SUMOReal lanePos;

    };



    /** @class VehicleInformation
     * @brief Stores the information of a vehicle
     */
    class VehicleInformation : public Named {
    public:
        /** @brief Constructor
         * @param[in] id The id of the vehicle
         * @param[in] range Recognition range of the vehicle
         */
        VehicleInformation(const std::string& id, const SUMOReal _range) : Named(id), range(_range), amOnNet(true), haveArrived(false) {}

        /// @brief Destructor
        ~VehicleInformation() {}


        /** @brief Returns the boundary of passed positions
         * @return The positions boundary
         */
        Boundary getBoxBoundary() const {
            Boundary ret;
            for (std::vector<VehicleState>::const_iterator i = updates.begin(); i != updates.end(); ++i) {
                ret.add((*i).position);
            }
            return ret;
        }

        /// @brief Recognition range of the vehicle
        const SUMOReal range;

        /// @brief List of position updates during last step
        std::vector<VehicleState> updates;

        /// @brief Whether the vehicle is within the simulated network
        bool amOnNet;

        /// @brief Whether the vehicle was removed from the simulation
        bool haveArrived;

        /// @brief The map of devices seen by the vehicle at removal time
        std::map<std::string, SeenDevice*> currentlySeen;

        /// @brief The past episodes of removed vehicle
        std::map<std::string, std::vector<SeenDevice*> > seen;

    private:
        /// @brief Invalidated copy constructor.
        VehicleInformation(const VehicleInformation&);

        /// @brief Invalidated assignment operator.
        VehicleInformation& operator=(const VehicleInformation&);

    };



    /** @class BTreceiverUpdate
     * @brief A global update performer
     */
    class BTreceiverUpdate : public Command {
    public:
        /// @brief Constructor
        BTreceiverUpdate();

        /// @brief Destructor
        ~BTreceiverUpdate();

        /** @brief Performs the update
         * @param[in] currentTime The current simulation time
         * @return Always DELTA_T - the time to being called back
         */
        SUMOTime execute(SUMOTime currentTime);


        /** @brief Rechecks the visibility for a given receiver/sender pair
         * @param[in] receiver Definition of the receiver vehicle
         * @param[in] sender Definition of the sender vehicle
         * @param[in] receiverPos The initial receiver position
         * @param[in] receiverD The direction vector of the receiver
         */
        void updateVisibility(VehicleInformation& receiver, MSDevice_BTsender::VehicleInformation& sender,
                              const Position& receiverPos, const Position& receiverD);


        /** @brief Informs the receiver about a sender entering it's radius
         * @param[in] atOffset The time offset to the current time step
         * @param[in] thisPos The receiver's position at the time
         * @param[in] thisSpeed The receiver's speed at the time
         * @param[in] thisLaneID The lane the observer was at
         * @param[in] thisLanePos The position at the lane of the observer
         * @param[in] otherID The ID of the entering sender
         * @param[in] otherPos The position of the entering sender
         * @param[in] otherSpeed The speed of the entering sender
         * @param[in] otherLaneID The lane the sender was at
         * @param[in] otherLanePos The position at the lane of the sender
         * @param[in] currentlySeen The container storing episodes
         */
        void enterRange(SUMOReal atOffset, const Position& thisPos, SUMOReal thisSpeed, const std::string& thisLaneID, SUMOReal thisLanePos,
                        const std::string& otherID, const Position& otherPos, SUMOReal otherSpeed, const std::string& otherLaneID, SUMOReal otherLanePos,
                        std::map<std::string, SeenDevice*>& currentlySeen);


        /** @brief Removes the sender from the currently seen devices to past episodes
         * @param[in] currentlySeen The currently seen devices
         * @param[in] seen The lists of episodes to add this one to
         * @param[in] thisPos The receiver's position at the time
         * @param[in] thisSpeed The receiver's speed at the time
         * @param[in] thisLaneID The lane the observer was at
         * @param[in] thisLanePos The position at the lane of the observer
         * @param[in] otherID The ID of the entering sender
         * @param[in] otherPos The position of the entering sender
         * @param[in] otherSpeed The speed of the entering sender
         * @param[in] otherLaneID The lane the sender was at
         * @param[in] otherLanePos The position at the lane of the sender
         * @param[in] tOffset The time offset to the current time step
         * @param[in] remove Whether the sender shall be removed from this device's myCurrentlySeen
         */
        void leaveRange(std::map<std::string, SeenDevice*>& currentlySeen, std::map<std::string, std::vector<SeenDevice*> >& seen,
                        const Position& thisPos, SUMOReal thisSpeed, const std::string& thisLaneID, SUMOReal thisLanePos,
                        const std::string& otherID, const Position& otherPos, SUMOReal otherSpeed, const std::string& otherLaneID, SUMOReal otherLanePos,
                        SUMOReal tOffset);




        /** @brief Adds a point of recognition
         * @param[in] tEnd The time of the recognition
         * @param[in] thisPos The receiver's position at the time
         * @param[in] thisSpeed The receiver's speed at the time
         * @param[in] thisLaneID The lane the observer was at
         * @param[in] thisLanePos The position at the lane of the observer
         * @param[in] otherPos The position of the entering sender
         * @param[in] otherSpeed The speed of the entering sender
         * @param[in] otherLaneID The lane the sender was at
         * @param[in] otherLanePos The position at the lane of the sender
         * @param[in] otherDevice The device of the entering sender
         */
        void addRecognitionPoint(const SUMOReal tEnd, const Position& thisPos, const SUMOReal thisSpeed, const std::string& thisLaneID, const SUMOReal thisLanePos,
                                 const Position& otherPos, const SUMOReal otherSpeed, const std::string& otherLaneID, const SUMOReal otherLanePos,
                                 SeenDevice* otherDevice) const;


        /** @brief Writes the output
         * @param[in] id The id of the receiver
         * @param[in] seen The information about seen senders
         * @param[in] allRecognitions Whether all recognitions shall be written
         */
        void writeOutput(const std::string& id, const std::map<std::string, std::vector<SeenDevice*> >& seen,
                         bool allRecognitions);




    };


    /// @brief A random number generator used to determine whether the opposite was recognized
    static MTRand sRecognitionRNG;

    /// @brief The list of arrived receivers
    static std::map<std::string, VehicleInformation*> sVehicles;



private:
    /// @brief Invalidated copy constructor.
    MSDevice_BTreceiver(const MSDevice_BTreceiver&);

    /// @brief Invalidated assignment operator.
    MSDevice_BTreceiver& operator=(const MSDevice_BTreceiver&);


};

#endif

/****************************************************************************/

