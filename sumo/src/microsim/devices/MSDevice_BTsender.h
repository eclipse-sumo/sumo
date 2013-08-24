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
 * @class MSDevice_BTsender
 * @brief A BT sender
 *
 * @see MSDevice
 */
class MSDevice_BTsender : public MSDevice {
public:
    /** @brief Inserts MSDevice_BTsender-options
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
    /** @brief Called on writing tripinfo output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     * @see MSDevice::generateOutput
     */
    void generateOutput() const;


    /// @brief Destructor.
    ~MSDevice_BTsender();


    /** @brief Says the device the holder shall report his route
     */
    void reportRoute() {
        myReportRoute = true;
    }


private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_BTsender(SUMOVehicle& holder, const std::string& id);


private:
    /// @brief Whether the vehicle shall report it's route
    bool myReportRoute;


private:
    /// @brief Invalidated copy constructor.
    MSDevice_BTsender(const MSDevice_BTsender&);

    /// @brief Invalidated assignment operator.
    MSDevice_BTsender& operator=(const MSDevice_BTsender&);


};


#endif

/****************************************************************************/

