/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSRouteHandler.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Parser and container for routes during their loading
/****************************************************************************/
#ifndef MSRouteHandler_h
#define MSRouteHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <microsim/pedestrians/MSPerson.h>
#include "MSContainer.h"
#include "MSVehicle.h"
#include <utils/xml/SUMORouteHandler.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSVehicleType;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSRouteHandler
 * @brief Parser and container for routes during their loading
 *
 * MSRouteHandler is the container for routes while they are build until
 * their transfering to the MSNet::RouteDict
 * The result of the operations are single MSNet::Route-instances
 */
class MSRouteHandler : public SUMORouteHandler {
public:
    /// standard constructor
    MSRouteHandler(const std::string& file,
                   bool addVehiclesDirectly);

    /// standard destructor
    virtual ~MSRouteHandler();

    static std::mt19937* getParsingRNG() {
        return &myParsingRNG;
    }

protected:
    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    virtual void myStartElement(int element,
                                const SUMOSAXAttributes& attrs);


    /** @brief Called when a closing tag occurs
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    virtual void myEndElement(int element);
    //@}


    /** @brief Called for parsing from and to and the corresponding taz attributes
     *
     * @param[in] element description of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     */
    void parseFromViaTo(std::string element,
                        const SUMOSAXAttributes& attrs);


    /** opens a type distribution for reading */
    void openVehicleTypeDistribution(const SUMOSAXAttributes& attrs);

    /** closes (ends) the building of a distribution */
    void closeVehicleTypeDistribution();

    /** opens a route for reading */
    void openRoute(const SUMOSAXAttributes& attrs);

    /** closes (ends) the building of a route.
        Afterwards no edges may be added to it;
        this method may throw exceptions when
        a) the route is empty or
        b) another route with the same id already exists */
    void closeRoute(const bool mayBeDisconnected = false);

    /** opens a route distribution for reading */
    void openRouteDistribution(const SUMOSAXAttributes& attrs);

    /** closes (ends) the building of a distribution */
    void closeRouteDistribution();

    /// Ends the processing of a vehicle
    virtual void closeVehicle();

    /// Ends the processing of a person
    void closePerson();

    /// Ends the processing of a container
    void closeContainer();

    /// Ends the processing of a flow
    void closeFlow();

    /// Processing of a stop
    void addStop(const SUMOSAXAttributes& attrs);

    /// @brief add a routing request for a walking or intermodal person
    void addPersonTrip(const SUMOSAXAttributes& attrs);

    /// @brief add a fully specified walk
    void addWalk(const SUMOSAXAttributes& attrs);

    ///@ brief parse depart- and arrival positions of a walk
    void parseWalkPositions(const SUMOSAXAttributes& attrs, const std::string& personID,
                            const MSEdge* fromEdge, const MSEdge*& toEdge,
                            double& departPos, double& arrivalPos, MSStoppingPlace*& bs,
                            const MSTransportable::Stage* const lastStage, bool& ok);

protected:
    /// @brief The current route
    ConstMSEdgeVector myActiveRoute;

    /// @brief The plan of the current person
    MSTransportable::MSTransportablePlan* myActivePlan;

    /// @brief The plan of the current container
    MSTransportable::MSTransportablePlan* myActiveContainerPlan;

    /// @brief Information whether vehicles shall be directly added to the network or kept within the buffer
    bool myAddVehiclesDirectly;

    /// @brief The currently parsed distribution of vehicle types (probability->vehicle type)
    RandomDistributor<MSVehicleType*>* myCurrentVTypeDistribution;

    /// @brief The id of the currently parsed vehicle type distribution
    std::string myCurrentVTypeDistributionID;

    /// @brief The currently parsed distribution of routes (probability->route)
    RandomDistributor<const MSRoute*>* myCurrentRouteDistribution;

    /// @brief The id of the currently parsed route distribution
    std::string myCurrentRouteDistributionID;

    /// @brief whether a state file is being loaded
    bool myAmLoadingState;

    /// @brief A random number generator used to choose from vtype/route distributions and computing the speed factors
    static std::mt19937 myParsingRNG;

private:
    /// @brief delete already created MSTransportablePlans if error occurs before handing over responsibility to a MSTransportable.
    void deleteActivePlans();

    /// @brief Invalidated copy constructor
    MSRouteHandler(const MSRouteHandler& s);

    /// @brief Invalidated assignment operator
    MSRouteHandler& operator=(const MSRouteHandler& s);

};


#endif

/****************************************************************************/

