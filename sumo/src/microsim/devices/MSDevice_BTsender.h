/****************************************************************************/
/// @file    MSDevice_BTsender.h
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
#ifndef MSDevice_BTsender_h
#define MSDevice_BTsender_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <set>
#include <string>
#include "MSDevice.h"
#include <utils/common/SUMOTime.h>
#include <utils/geom/Position.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_BTsender
 * @brief A BT sender
 *
 * @see MSDevice
 */
class MSDevice_BTsender : public MSDevice {
public:
    /** @brief Inserts MSDevice_BTsender-options
     * @param[filled] oc The options container to add the options to
     */
    static void insertOptions(OptionsCont& oc);


    /** @brief Build devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether a bt-sender-device shall be built
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
    ~MSDevice_BTsender();


    bool notifyEnter(SUMOVehicle& veh, Notification reason);
    bool notifyLeave(SUMOVehicle& veh, SUMOReal lastPos, Notification reason);

    /** @class ArrivedVehicleInformation
     * @brief Stores the information of a removed device
     */
    class ArrivedVehicleInformation {
    public:
        /** @brief Constructor
         * @param[in] _id The id of the removed vehicle
         * @param[in] _speed The speed of the removed vehicle at removal time
         * @param[in] _position The position of the removed vehicle at removal time
         */
        ArrivedVehicleInformation(const std::string &_id, SUMOReal _speed, const Position &_position) 
            : id(_id), speed(_speed), position(_position) {}

        /// @brief Destructor
        ~ArrivedVehicleInformation() {}

        /// @brief The id of the removed vehicle
        std::string id;
        /// @brief The speed of the removed vehicle at removal time
        SUMOReal speed;
        /// @brief The position of the removed vehicle at removal time
        Position position;

    };
private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_BTsender(SUMOVehicle& holder, const std::string& id);


public: // !!!

    static std::set<MSVehicle*> sRunningVehicles;
    static std::set<ArrivedVehicleInformation*> sArrivedVehicles;

private:
    /// @brief Invalidated copy constructor.
    MSDevice_BTsender(const MSDevice_BTsender&);

    /// @brief Invalidated assignment operator.
    MSDevice_BTsender& operator=(const MSDevice_BTsender&);


};


#endif

/****************************************************************************/

