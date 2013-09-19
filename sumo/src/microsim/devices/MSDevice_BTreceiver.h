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
 * @brief A BT sender
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



    /** @class MeetingPoint
     * @brief Holds the information about exact positions/speeds/time of the begin/end of a meeting
     */
    class MeetingPoint {
    public:
        /** @brief Constructor
         * @param[in] t_ The time of the meeting
         * @param[in] observerPos_ The position the observer had at the time
         * @param[in] observerSpeed_ The speed the observer had at the time
         * @param[in] seenPos_ The position the seen vehicle had at the time
         * @param[in] seenSpeed_ The speed the vehicle had at the time
         */
        MeetingPoint(SUMOReal t_, const Position& observerPos_, SUMOReal observerSpeed_,
                     const Position& seenPos_, SUMOReal seenSpeed_)
            : t(t_), observerPos(observerPos_), observerSpeed(observerSpeed_),
              seenPos(seenPos_), seenSpeed(seenSpeed_) {}

        /// @brief Destructor
        ~MeetingPoint() {}

    public:
        /// @brief The time of the meeting
        SUMOReal t;
        /// @brief The position the observer had at the time
        Position observerPos;
        /// @brief The speed the observer had at the time
        SUMOReal observerSpeed;
        /// @brief The position the seen vehicle had at the time
        Position seenPos;
        /// @brief The speed the vehicle had at the time
        SUMOReal seenSpeed;

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
            for(std::vector<MeetingPoint*>::iterator i=recognitionPoints.begin(); i!=recognitionPoints.end(); ++i) {
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



    /** @brief Returns the currently seen devices
     * @return the map from ID of holder to seen device
     */
    const std::map<std::string, SeenDevice*> &getCurrentlySeen() const {
        return myCurrentlySeen;
    }


    /** @brief Returns the list of meetings so far
     * @return the map from ID of holder to the list of meetings
     */
    const std::map<std::string, std::vector<SeenDevice*> > &getSeen() const {
        return mySeen;
    }


    /** @brief Clears the given containers deleting the stored items
     * @param[in] c The currently seen container to clear
     * @param[in] s The seen container to clear
     */
    static void cleanUp(std::map<std::string, SeenDevice*> &c, std::map<std::string, std::vector<SeenDevice*> > &s);


    /** @brief Clears containers after copying them
     */
    void unref() {
        myCurrentlySeen.clear();
        mySeen.clear();
    }


protected:
    /** @brief Updates the currently seen devices
     */
    void updateNeighbors();


    /** @brief Informs the receiver about a sender entering it's radius
     * @param[in] thisPos The receiver's position at the time
     * @param[in] thisSpeed The receiver's speed at the time
     * @param[in] otherID The ID of the entering sender
     * @param[in] otherPos The position of the entering sender
     * @param[in] otherSpeed The speed of the entering sender
     * @param[in] tOffset The time offset to the current time step
     */
    void enterRange(const Position &thisPos, SUMOReal thisSpeed, 
        const std::string &otherID, const Position &otherPos, SUMOReal otherSpeed, SUMOReal tOffset);


    /** @brief Informs the receiver about a sender leaving it's radius
     * @param[in] thisPos The receiver's position at the time
     * @param[in] thisSpeed The receiver's speed at the time
     * @param[in] otherID The ID of the entering sender
     * @param[in] otherPos The position of the entering sender
     * @param[in] otherSpeed The speed of the entering sender
     * @param[in] tOffset The time offset to the current time step
     * @param[in] remove Whether the sender shall be removed from this device's myCurrentlySeen
     */
    void leaveRange(const Position &thisPos, SUMOReal thisSpeed, 
        const std::string &otherID, const Position &otherPos, SUMOReal otherSpeed, SUMOReal tOffset, bool remove = false);


    /** @brief Removes the sender from the currently seen devices to past episodes
     * @param[in] currentlySeen The currently seen devices
     * @param[in] seen The lists of episodes to add this one to
     * @param[in] thisPos The receiver's position at the time
     * @param[in] thisSpeed The receiver's speed at the time
     * @param[in] otherID The ID of the entering sender
     * @param[in] otherPos The position of the entering sender
     * @param[in] otherSpeed The speed of the entering sender
     * @param[in] tOffset The time offset to the current time step
     * @param[in] remove Whether the sender shall be removed from this device's myCurrentlySeen
     */
    static void leaveRange(std::map<std::string, SeenDevice*> &currentlySeen, std::map<std::string, std::vector<SeenDevice*> > &seen,
        const Position &thisPos, SUMOReal thisSpeed, 
        const std::string &otherID, const Position &otherPos, SUMOReal otherSpeed, 
        SUMOReal tOffset, bool remove);


    /** @brief Determines whether the other vehicle got visible until the given time
     * @param[in] otherID The ID of the other vehicle
     * @param[in] tEnd The end of contact
     * @param[changed] currentlySeen Contact information, updated if the sender was recognized
     * @return The recognition time
     */
    static SUMOReal recognizedAt(const std::string &otherID, SUMOReal tEnd, std::map<std::string, SeenDevice*> &currentlySeen);


    /** @brief Adds a point of recognition
     * @param[in] thisPos The receiver's position at the time
     * @param[in] thisSpeed The receiver's speed at the time
     * @param[in] otherID The ID of the entering sender
     * @param[in] otherPos The position of the entering sender
     * @param[in] otherSpeed The speed of the entering sender
     * @param[in] tOffset The time offset to the current time step
     * @param[filled] currentlySeen The contact information storage for saving the contact point
     */
    static void addRecognitionPoint(const Position &thisPos, SUMOReal thisSpeed, 
        const std::string &otherID, const Position &otherPos, SUMOReal otherSpeed, SUMOReal t, 
        std::map<std::string, SeenDevice*> &currentlySeen);


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
    
    /// @brief The currently seen devices
    std::map<std::string, SeenDevice*> myCurrentlySeen;

    /// @brief The devices seen so far
    std::map<std::string, std::vector<SeenDevice*> > mySeen;

    /// @brief Whether the complete bt-system was already initialised
    static bool myWasInitialised;


    /** @class ArrivedVehicleInformation
     * @brief Stores the information of a removed device
     */
    class ArrivedVehicleInformation {
    public:
        /** @brief Constructor
         * @param[in] _id The id of the removed vehicle
         * @param[in] _speed The speed of the removed vehicle at removal time
         * @param[in] _position The position of the removed vehicle at removal time
         * @param[in] _currentlySeen The map of devices seen by the vehicle at removal time
         * @param[in] _seen The past episodes of removed vehicle
         */
        ArrivedVehicleInformation(const std::string &_id, SUMOReal _speed, const Position &_position, 
            const std::map<std::string, SeenDevice*> &_currentlySeen, const std::map<std::string, std::vector<SeenDevice*> > &_seen) 
            : id(_id), speed(_speed), position(_position), currentlySeen(_currentlySeen), seen(_seen) {}

        /// @brief Destructor
        ~ArrivedVehicleInformation() {}

        /// @brief The id of the removed vehicle
        std::string id;
        /// @brief The speed of the removed vehicle at removal time
        SUMOReal speed;
        /// @brief The position of the removed vehicle at removal time
        Position position;
        /// @brief The map of devices seen by the vehicle at removal time
        std::map<std::string, SeenDevice*> currentlySeen;
        /// @brief The past episodes of removed vehicle
        std::map<std::string, std::vector<SeenDevice*> > seen;

    };



    /** @class BTreceiverUpdate
     * @brief A global update performer
     */
    class BTreceiverUpdate : public MSNet::VehicleStateListener, public Command {
    public:
        /// @brief Constructor
        BTreceiverUpdate();

        /// @brief Destructor
        ~BTreceiverUpdate();

        /** @brief Informs the updater about a vehicle entering/leaving the simulation
         * @pram[in] vehicle The vehicle that changes it's state
         * @pram[in] to the new state of this vehicle
         */
        void vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to);

        /** @brief Performs the update
         * @param[in] currentTime The current simulation time
         * @return Always DELTA_T - the time to being called back
         */
        SUMOTime execute(SUMOTime currentTime);


        /// @brief The list of running receivers
        std::set<MSVehicle*> myRunningReceiverVehicles;

        /// @brief The list of running senders
        std::set<MSVehicle*> myRunningSenderVehicles;

        /// @brief The list of arrived receivers
        std::set<ArrivedVehicleInformation*> myArrivedReceiverVehicles;

        /// @brief The list of arrived senders
        std::set<ArrivedVehicleInformation*> myArrivedSenderVehicles;

    };


    /// @brief A random number generator used to determine whether the opposite was recognized
    static MTRand myRecognitionRNG;

private:
    /// @brief Invalidated copy constructor.
    MSDevice_BTreceiver(const MSDevice_BTreceiver&);

    /// @brief Invalidated assignment operator.
    MSDevice_BTreceiver& operator=(const MSDevice_BTreceiver&);


};

#endif

/****************************************************************************/

