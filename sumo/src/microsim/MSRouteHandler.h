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

    static MTRand* getParsingRNG() {
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

    ///@ brief parse depart- and arrival positions of a walk
    void parseWalkPositions(const SUMOSAXAttributes& attrs, const std::string& personID,
                            const MSEdge* fromEdge, const MSEdge*& toEdge,
                            SUMOReal& departPos, SUMOReal& arrivalPos, MSStoppingPlace*& bs, bool& ok);
    SUMOReal parseWalkPos(SumoXMLAttr attr, const std::string& id, const MSEdge* edge, const std::string& val);

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
    static MTRand myParsingRNG;

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

