/****************************************************************************/
/// @file    RORouteHandler.h
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
#ifndef RORouteHandler_h
#define RORouteHandler_h


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
#include <utils/common/RandomDistributor.h>
#include <utils/common/SUMOTime.h>
#include <utils/vehicle/PedestrianRouter.h>
#include <utils/xml/SUMORouteHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice_String;
class ROEdge;
class ROLane;
class RONet;
class ROPerson;
class RORoute;
class RORouteDef;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORouteHandler
 * @brief Parser and container for routes during their loading
 *
 * RORouteHandler is the container for routes while they are build until
 * their transfering to the MSNet::RouteDict
 * The result of the operations are single MSNet::Route-instances
 */
class RORouteHandler : public SUMORouteHandler {
public:
    /// standard constructor
    RORouteHandler(RONet& net, const std::string& file,
                   const bool tryRepair,
                   const bool emptyDestinationsAllowed,
                   const bool ignoreErrors);

    /// standard destructor
    virtual ~RORouteHandler();

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
    void closeVehicle();

    /// Ends the processing of a person
    void closePerson();

    /// Ends the processing of a container
    void closeContainer();

    /// Ends the processing of a flow
    void closeFlow();

    /// Processing of a stop
    void addStop(const SUMOSAXAttributes& attrs);

    /// Parse edges from strings
    void parseEdges(const std::string& desc, ConstROEdgeVector& into,
                    const std::string& rid);

    /// @brief add a routing request for a walking person
    bool addPersonTrip(const SUMOSAXAttributes& attrs);

protected:
    /// @brief The current route
    RONet& myNet;

    /// @brief The current route
    ConstROEdgeVector myActiveRoute;

    /// @brief The plan of the current person
    ROPerson* myActivePerson;

    /// @brief The plan of the current container
    OutputDevice_String* myActiveContainerPlan;

    /// @brief The number of stages in myActiveContainerPlan
    int myActiveContainerPlanSize;

    /// @brief Information whether routes shall be repaired
    const bool myTryRepair;

    /// @brief Information whether the "to" attribute is mandatory
    const bool myEmptyDestinationsAllowed;

    /// @brief Depending on the "ignore-errors" option different outputs are used
    MsgHandler* const myErrorOutput;

    /// @brief The currently parsed distribution of vehicle types (probability->vehicle type)
    RandomDistributor<SUMOVTypeParameter*>* myCurrentVTypeDistribution;

    /// @brief The id of the currently parsed vehicle type distribution
    std::string myCurrentVTypeDistributionID;

    /// @brief The currently parsed route alternatives
    RORouteDef* myCurrentAlternatives;

private:
    /// @brief Invalidated copy constructor
    RORouteHandler(const RORouteHandler& s);

    /// @brief Invalidated assignment operator
    RORouteHandler& operator=(const RORouteHandler& s);

};


#endif

/****************************************************************************/

