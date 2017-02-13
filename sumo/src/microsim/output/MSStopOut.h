/****************************************************************************/
/// @file    MSStopOut.h
/// @author  Jakob Erdmann
/// @date    Wed, 21.12.2016
/// @version $Id$
///
// Ouput information about planned vehicle stop
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSStopOut_h
#define MSStopOut_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <utils/common/SUMOTime.h>
#include <microsim/MSVehicle.h>


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

    static MSStopOut* getInstance() {
        return myInstance;
    }

    /// @brief constructor.
    MSStopOut(OutputDevice& dev);

    /// @brief Destructor.
    virtual ~MSStopOut();

    void stopStarted(const SUMOVehicle* veh, int numPersons, int numContainers);

    void loadedPersons(const SUMOVehicle* veh, int n);
    void unloadedPersons(const SUMOVehicle* veh, int n);

    void loadedContainers(const SUMOVehicle* veh, int n);
    void unloadedContainers(const SUMOVehicle* veh, int n);

    void stopEnded(const SUMOVehicle* veh, const MSVehicle::Stop& stop);


private:
    struct StopInfo {

        StopInfo(SUMOTime t, int numPersons, int numContainers) :
            started(t),
            initialNumPersons(numPersons),
            loadedPersons(0),
            unloadedPersons(0),
            initialNumContainers(numContainers),
            loadedContainers(0),
            unloadedContainers(0) {
        }

        // @note: need default constructor or std::map doesn't work
        StopInfo() :
            started(-1),
            initialNumPersons(0),
            loadedPersons(0),
            unloadedPersons(0),
            initialNumContainers(0),
            loadedContainers(0),
            unloadedContainers(0) {
        }

        SUMOTime started;
        int initialNumPersons;
        int loadedPersons;
        int unloadedPersons;
        int initialNumContainers;
        int loadedContainers;
        int unloadedContainers;
    };

    typedef std::map<const SUMOVehicle*, StopInfo> Stopped;
    Stopped myStopped;

    OutputDevice& myDevice;

    static MSStopOut* myInstance;

    /// @brief Invalidated copy constructor.
    MSStopOut(const MSStopOut&);

    /// @brief Invalidated assignment operator.
    MSStopOut& operator=(const MSStopOut&);


};


#endif

/****************************************************************************/

