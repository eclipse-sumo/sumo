/****************************************************************************/
/// @file    MSContainerControl.h
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Mon, 16 Jun 2014
/// @version $Id: MSContainerControl.h 16525 2014-06-04 14:18:06Z kend-an $
///
// Stores all containers in the net and handles their waiting for cars.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSContainerControl_h
#define MSContainerControl_h


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
#include "MSContainer.h"
#include "MSVehicle.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 * @class MSContainerControl
 * The class is used to handle containers who are not using a transportation
 *  system but are waiting. Both is processed by waiting
 *  for the arrival time / the time the waiting is over.
 */
class MSContainerControl {
public:

    typedef std::vector<MSContainer*> ContainerVector;

    /// constructor
    MSContainerControl();

    /// destructor
    virtual ~MSContainerControl();

    /// adds a single container, returns false if an id clash occured
    bool add(const std::string& id, MSContainer* container);

    /// removes a single container
    virtual void erase(MSContainer* container);

    /// sets the arrival time for a waiting container
    void setDeparture(SUMOTime time, MSContainer* container);

    /// sets the arrival time for a waiting container
    void setWaitEnd(SUMOTime time, MSContainer* container);

    /// checks whether any containers waiting time is over
    void checkWaitingContainers(MSNet* net, const SUMOTime time);

    /// adds a container to the list of containers waiting for a vehicle on the specified edge
    void addWaiting(const MSEdge* edge, MSContainer* container);

    /** @brief load any applicable containers
     * Loads any container that is waiting on that edge for the given vehicle and removes them from myWaiting
     * @param[in] the edge on which the loading should take place
     * @param[in] the vehicle which is taking on containers
     * @return Whether any containers have been loaded
     */
    bool loadAnyWaiting(MSEdge* edge, MSVehicle* vehicle, MSVehicle::Stop* stop);

    /// checks whether any container waits to finish her plan
    bool hasContainers() const;

    /// checks whether any container is still engaged in walking / stopping
    bool hasNonWaiting() const;

    /// aborts the plan for any container that is still waiting for a ride
    void abortWaiting();


    /** @brief Builds a new container
     * @param[in] pars The parameter
     * @param[in] vtype The type (reusing vehicle type container here)
     * @param[in] plan This container's plan
     */
    virtual MSContainer* buildContainer(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSContainer::MSContainerPlan* plan) const;

    /// adds a container to myTranship 
    void setTranship(MSContainer* c);

    /// removes a container from myTranship
    void unsetTranship(MSContainer* c);
//
//    /// @brief returns whether the the given container is waiting for a vehicle on the given edge
//    bool isWaiting4Vehicle(const MSEdge* const edge, MSContainer* p) const;
//
//    const std::map<std::string, MSContainer*>& getContainers() const {
//        return myContainers;
//    }
//
private:
    /// all containers by id
    std::map<std::string, MSContainer*> myContainers;

    /// all containers being transhiped
    std::map<std::string, MSContainer*> myTranship;

    /// @brief Containers waiting for departure
    std::map<SUMOTime, ContainerVector> myWaiting4Departure;

    /// the lists of walking / stopping containers
    std::map<SUMOTime, ContainerVector> myWaitingUntil;

    /// the lists of waiting containers
    std::map<const MSEdge*, ContainerVector> myWaiting4Vehicle;
//
};


#endif

/****************************************************************************/
