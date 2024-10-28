/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSStopOut.h
/// @author  Jakob Erdmann
/// @date    Wed, 21.12.2016
///
// Ouput information about planned vehicle stop
/****************************************************************************/
#pragma once
#include <config.h>

#include <map>
#include <utils/common/SUMOTime.h>
#include <utils/vehicle/SUMOVehicleParameter.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSStopOut
 * @brief Realises dumping the complete network state
 *
 * The class offers a static method, which writes the complete dump of
 *  the given network into the given OutputDevice.
 *
 * @todo consider error-handling on write (using IOError)
 */
class MSStopOut {
public:

    /** @brief Static intialization
     */
    static void init();

    static bool active() {
        return myInstance != 0;
    }

    static void cleanup();

    static MSStopOut* getInstance() {
        return myInstance;
    }

    /// @brief constructor.
    MSStopOut(OutputDevice& dev);

    /// @brief Destructor.
    virtual ~MSStopOut();

    void stopStarted(const SUMOVehicle* veh, int numPersons, int numContainers, SUMOTime time);

    void loadedPersons(const SUMOVehicle* veh, int n);
    void unloadedPersons(const SUMOVehicle* veh, int n);

    void loadedContainers(const SUMOVehicle* veh, int n);
    void unloadedContainers(const SUMOVehicle* veh, int n);

    void stopEnded(const SUMOVehicle* veh, const SUMOVehicleParameter::Stop& stop, const std::string& laneOrEdgeID, bool simEnd = false);

    /// @brief generate output for vehicles which are still stopped at simulation end
    void generateOutputForUnfinished();

private:
    struct StopInfo {

        StopInfo(int numPersons, int numContainers) :
            initialNumPersons(numPersons),
            loadedPersons(0),
            unloadedPersons(0),
            initialNumContainers(numContainers),
            loadedContainers(0),
            unloadedContainers(0) {
        }

        int initialNumPersons;
        int loadedPersons;
        int unloadedPersons;
        int initialNumContainers;
        int loadedContainers;
        int unloadedContainers;
    };

    std::map<const SUMOVehicle*, StopInfo, ComparatorNumericalIdLess> myStopped;

    OutputDevice& myDevice;

    static MSStopOut* myInstance;

    /// @brief Invalidated copy constructor.
    MSStopOut(const MSStopOut&);

    /// @brief Invalidated assignment operator.
    MSStopOut& operator=(const MSStopOut&);


};
