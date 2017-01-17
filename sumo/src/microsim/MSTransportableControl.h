/****************************************************************************/
/// @file    MSTransportableControl.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Stores all persons or containers in the net and handles their waiting for cars.
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
#ifndef MSTransportableControl_h
#define MSTransportableControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>
#include "MSTransportable.h"
#include "MSVehicle.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 * @class MSTransportableControl
 * The class is used to handle transportables (persons and containers)
 *  who are not using a transportation
 *  system but are walking or waiting. This includes waiting
 *  for the arrival or departure time / the time the waiting is over.
 */
class MSTransportableControl {
public:
    /// @brief Definition of a list of transportables
    typedef std::vector<MSTransportable*> TransportableVector;

    /// @brief Definition of the internal transportables map iterator
    typedef std::map<std::string, MSTransportable*>::const_iterator constVehIt;


public:
    /// @brief Constructor
    MSTransportableControl();


    /// @brief Destructor
    virtual ~MSTransportableControl();


    /** @brief Adds a single transportable, returns false if an id clash occured
     * @param[in] transportable The transportable to add
     * @return Whether the transportable could be added (none with the same id existed before)
     */
    bool add(MSTransportable* transportable);


    /** @brief Returns the named transportable, if existing
     * @param[in] id The id of the transportable
     * @return The named transportable, if existing, otherwise 0
     */
    MSTransportable* get(const std::string& id) const;


    /// removes a single transportable
    virtual void erase(MSTransportable* transportable);

    /// sets the arrival time for a waiting transportable
    void setWaitEnd(SUMOTime time, MSTransportable* transportable);

    /// checks whether any transportables waiting time is over
    void checkWaiting(MSNet* net, const SUMOTime time);

    /// adds a transportable to the list of transportables waiting for a vehicle on the specified edge
    void addWaiting(const MSEdge* edge, MSTransportable* person);

    /** @brief board any applicable persons
     * Boards any people who wait on that edge for the given vehicle and removes them from myWaiting
     * @param[in] the edge on which the boarding should take place
     * @param[in] the vehicle which is taking on passengers / goods
     * @param[in] the stop at which the vehicle is stopping
     * @return Whether any transportables have been boarded
     */
    bool boardAnyWaiting(MSEdge* edge, MSVehicle* vehicle, MSVehicle::Stop* stop);

    /** @brief load any applicable containers
    * Loads any container that is waiting on that edge for the given vehicle and removes them from myWaiting
    * @param[in] the edge on which the loading should take place
    * @param[in] the vehicle which is taking on containers
    * @return Whether any containers have been loaded
    */
    bool loadAnyWaiting(MSEdge* edge, MSVehicle* vehicle, MSVehicle::Stop* stop);

    /// checks whether any transportable waits to finish her plan
    bool hasTransportables() const;

    /// checks whether any transportable is still engaged in walking / stopping
    bool hasNonWaiting() const;

    /// aborts the plan for any transportable that is still waiting for a ride
    void abortWaiting();


    /** @brief Builds a new person
     * @param[in] pars The parameter
     * @param[in] vtype The type (reusing vehicle type container here)
     * @param[in] plan This person's plan
     */
    virtual MSTransportable* buildPerson(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan) const;

    /** @brief Builds a new container
    * @param[in] pars The parameter
    * @param[in] vtype The type (reusing vehicle type container here)
    * @param[in] plan This container's plan
    */
    virtual MSTransportable* buildContainer(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSTransportable::MSTransportablePlan* plan) const;

    /** @brief Returns the begin of the internal transportables map
     * @return The begin of the internal transportables map
     */
    constVehIt loadedBegin() const {
        return myTransportables.begin();
    }


    /** @brief Returns the end of the internal transportables map
     * @return The end of the internal transportables map
     */
    constVehIt loadedEnd() const {
        return myTransportables.end();
    }


    /** @brief Returns the number of known transportables
     * @return The number of stored transportables
     */
    int size() const {
        return (int)myTransportables.size();
    }

    /// @brief register a jammed transportable
    void registerJammed() {
        myJammedNumber++;
    }

    /// @name Retrieval of transportable statistics (always accessable)
    /// @{

    /** @brief Returns the number of build transportables
     * @return The number of loaded (build) transportables
     */
    int getLoadedNumber() const {
        return myLoadedNumber;
    }


    /** @brief Returns the number of build and inserted, but not yet deleted transportables
     * @return The number of simulated transportables
     */
    int getRunningNumber() const {
        return myRunningNumber;
    }

    /** @brief Returns the number of times a transportables was jammed
     * @return The number of times transportables were jammed
     */
    int getJammedNumber() const {
        return myJammedNumber;
    }

    /// @}

protected:
    /// all currently created transportables by id
    std::map<std::string, MSTransportable*> myTransportables;

    /// @brief Transportables waiting for departure
    std::map<SUMOTime, TransportableVector> myWaiting4Departure;

    /// the lists of walking / stopping transportables
    std::map<SUMOTime, TransportableVector> myWaitingUntil;

    /// the lists of waiting transportables
    std::map<const MSEdge*, TransportableVector> myWaiting4Vehicle;

    /// @brief The number of build transportables
    int myLoadedNumber;

    /// @brief The number of transportables within the network (build and inserted but not removed)
    int myRunningNumber;

    /// @brief The number of jammed transportables
    int myJammedNumber;

    /// @brief The number of transportables waiting for vehicles
    int myWaitingForVehicleNumber;

    /// @brief whether a new transportable waiting for a vehicle has been added in the last step
    bool myHaveNewWaiting;

};


#endif

/****************************************************************************/
