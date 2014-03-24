/****************************************************************************/
/// @file    MSDevice.h
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Tue, 04 Dec 2007
/// @version $Id$
///
// Abstract in-vehicle device
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2007-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSDevice_h
#define MSDevice_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <map>
#include <set>
#include <microsim/MSMoveReminder.h>
#include <utils/common/Named.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class SUMOVehicle;
class OptionsCont;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice
 * @brief Abstract in-vehicle device
 *
 * The MSDevice-interface brings the following interfaces to a vehicle that
 *  may be overwritten by real devices:
 * @arg Retrieval of the vehicle that holds the device
 * @arg Building and retrieval of a device id
 * @arg Methods called on vehicle movement / state change
 *
 * The "methods called on vehicle movement / state change" are called for each
 *  device within the corresponding vehicle methods. MSDevice brings already
 *  an empty (nothing doing) implementation of these.
 */
class MSDevice : public MSMoveReminder, public Named {
public:
    /** @brief Inserts options for building devices
     * @param[filled] oc The options container to add the options to
     */
    static void insertOptions(OptionsCont& oc);


    /** @brief Build devices for the given vehicle, if needed
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[filled] into The vector to store the built device in
     */
    static void buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into);



public:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice(SUMOVehicle& holder, const std::string& id) :
        MSMoveReminder(id), Named(id), myHolder(holder) {
    }


    /// @brief Destructor
    virtual ~MSDevice() { }


    /** @brief Returns the vehicle that holds this device
     *
     * @return The vehicle that holds this device
     */
    SUMOVehicle& getHolder() const {
        return myHolder;
    }


    /** @brief Called on writing tripinfo output
     *
     * The device may write some statistics into the tripinfo output. It
     *  is assumed that the written information is a valid xml-snipplet, which
     *  will be embedded within the vehicle's information.
     *
     * The device should use the openTag / closeTag methods of the OutputDevice
     *  for correct indentation.
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     */
    virtual void generateOutput() const {
    }



protected:
    /// @name Helper methods for device assignment
    /// @{

    /** @brief Adds common command options that allow to assign devices to vehicles
     *
     * @param[in] deviceName The name of the device type
     * @param[in] optionsTopic The options topic into which the options shall be added
     * @param[filled] oc The options container to add the options to
     */
    static void insertDefaultAssignmentOptions(const std::string& deviceName, const std::string& optionsTopic, OptionsCont& oc);


    /** @brief Determines whether a vehicle should get a certain device
     *
     * @param[in] oc The options container to get the information about assignment from
     * @param[in] deviceName The name of the device type
     * @param[in] v The vehicle to determine whether it shall be equipped or not
     */
    static bool equippedByDefaultAssignmentOptions(const OptionsCont& oc, const std::string& deviceName, SUMOVehicle& v);
    /// @}



protected:
    /// @brief The vehicle that stores the device
    SUMOVehicle& myHolder;



private:
    /// @brief vehicles which explicitly carry a device, sorted by device, first
    static std::map<std::string, std::set<std::string> > myExplicitIDs;


private:
    /// @brief Invalidated copy constructor.
    MSDevice(const MSDevice&);

    /// @brief Invalidated assignment operator.
    MSDevice& operator=(const MSDevice&);

};


#endif

/****************************************************************************/

