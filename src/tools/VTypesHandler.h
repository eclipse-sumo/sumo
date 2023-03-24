/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2023 German Aerospace Center (DLR) and others.
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
/// @file    VTypesHandler.h
/// @author  Jakob Erdmann
/// @date    12.01.2022
///
// An XML-Handler for reading vTypes
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <utility>
#include <utils/vehicle/SUMORouteHandler.h>

class EnergyParams;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class VTypesHandler
 * @brief An XML-Handler for amitran and netstate trajectories
 *
 * This SUMOSAXHandler parses vehicles and their speeds.
 */
class VTypesHandler : public SUMORouteHandler {
public:
    static const int INVALID_VALUE = -999999;

public:
    /** @brief Constructor
     *
     * @param[in] file The file that will be processed
     */
    VTypesHandler(const std::string& file, std::map<std::string, SUMOVTypeParameter*>& vTypes);


    /// @brief Destructor
    ~VTypesHandler();

    void closeVType();
    void openVehicleTypeDistribution(const SUMOSAXAttributes& attrs) {
        UNUSED_PARAMETER(attrs);
    }
    void closeVehicleTypeDistribution() {}
    void openRoute(const SUMOSAXAttributes& attrs) {
        UNUSED_PARAMETER(attrs);
    }
    void openFlow(const SUMOSAXAttributes& attrs) {
        UNUSED_PARAMETER(attrs);
    }
    void openRouteFlow(const SUMOSAXAttributes& attrs) {
        UNUSED_PARAMETER(attrs);
    }
    void openTrip(const SUMOSAXAttributes& attrs) {
        UNUSED_PARAMETER(attrs);
    }
    void closeRoute(const bool mayBeDisconnected = false) {
        UNUSED_PARAMETER(mayBeDisconnected);
    }
    void openRouteDistribution(const SUMOSAXAttributes& attrs) {
        UNUSED_PARAMETER(attrs);
    }
    void closeRouteDistribution() {}
    void closeVehicle() {}
    void closePerson() {}
    void closePersonFlow() {}
    void closeContainer() {}
    void closeContainerFlow() {}
    void closeFlow() {}
    void closeTrip() {}
    void addStop(const SUMOSAXAttributes& attrs) {
        UNUSED_PARAMETER(attrs);
    }
    void addPersonTrip(const SUMOSAXAttributes& attrs) {
        UNUSED_PARAMETER(attrs);
    }
    void addWalk(const SUMOSAXAttributes& attrs) {
        UNUSED_PARAMETER(attrs);
    }
    void addPerson(const SUMOSAXAttributes& attrs) {
        UNUSED_PARAMETER(attrs);
    }
    void addRide(const SUMOSAXAttributes& attrs) {
        UNUSED_PARAMETER(attrs);
    }
    void addContainer(const SUMOSAXAttributes& attrs) {
        UNUSED_PARAMETER(attrs);
    }
    void addTransport(const SUMOSAXAttributes& attrs) {
        UNUSED_PARAMETER(attrs);
    }
    void addTranship(const SUMOSAXAttributes& attrs) {
        UNUSED_PARAMETER(attrs);
    }

protected:


private:
    std::map<std::string, SUMOVTypeParameter*>& myVTypes;


private:
    /// @brief invalidated copy constructor
    VTypesHandler(const VTypesHandler& s);

    /// @brief invalidated assignment operator
    VTypesHandler& operator=(const VTypesHandler& s);


};
