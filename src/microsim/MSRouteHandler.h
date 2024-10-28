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
/// @file    MSRouteHandler.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
///
// Parser and container for routes during their loading
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <utils/common/SUMOTime.h>
#include <utils/common/MapMatcher.h>
#include <utils/vehicle/SUMORouteHandler.h>
#include <microsim/transportables/MSPerson.h>
#include <microsim/transportables/MSTransportable.h>
#include "MSVehicle.h"


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
class MSRouteHandler : public SUMORouteHandler, public MapMatcher<MSEdge, MSLane, MSJunction> {
public:

    /// @brief enum for object type
    enum class ObjectTypeEnum {
        PERSON,
        CONTAINER,
        UNDEFINED
    };

    /// @brief standard constructor
    MSRouteHandler(const std::string& file, bool addVehiclesDirectly);

    /// @brief standard destructor
    virtual ~MSRouteHandler();

    /// @brief get parsing RNG
    static SumoRNG* getParsingRNG() {
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
                                const SUMOSAXAttributes& attrs) override;
    //@}

    /** @brief Called for parsing from and to and the corresponding taz attributes
     *
     * @param[in] element description of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     */
    void parseFromViaTo(SumoXMLTag tag, const SUMOSAXAttributes& attrs);

    /// @brief opens a type distribution for reading
    void openVehicleTypeDistribution(const SUMOSAXAttributes& attrs) override;

    /// @brief closes (ends) the building of a distribution
    void closeVehicleTypeDistribution() override;

    /// @brief opens a route for reading
    void openRoute(const SUMOSAXAttributes& attrs) override;

    /// @brief opens a flow for reading
    void openFlow(const SUMOSAXAttributes& attrs) override;

    /// @brief opens a route flow for reading
    void openRouteFlow(const SUMOSAXAttributes& attrs) override;

    /// @brief opens a trip for reading
    void openTrip(const SUMOSAXAttributes& attrs) override;

    /**@brief closes (ends) the building of a route.
     * @note Afterwards no edges may be added to it;
     *       this method may throw exceptions when
     *       a) the route is empty or
     *       b) another route with the same id already exists
     */
    void closeRoute(const bool mayBeDisconnected = false) override;

    /// @brief opens a route distribution for reading
    void openRouteDistribution(const SUMOSAXAttributes& attrs) override;

    /// @brief closes (ends) the building of a distribution
    void closeRouteDistribution() override;

    /// @brief Ends the processing of a vehicle (note: is virtual because is reimplemented in MSStateHandler)
    virtual void closeVehicle() override;

    /// @brief Ends the processing of a vehicle type
    void closeVType() override;

    /// @brief Ends the processing of a person
    void closePerson() override;

    /// @brief Ends the processing of a personFlow
    void closePersonFlow() override;

    /// @brief Ends the processing of a container
    void closeContainer() override;

    /// @brief Ends the processing of a containerFlow
    void closeContainerFlow() override;

    /// @brief Ends the processing of a flow
    void closeFlow() override;

    /// @brief Ends the processing of a trip
    void closeTrip() override;

    /// @brief Parse destination stop
    MSStoppingPlace* retrieveStoppingPlace(const SUMOSAXAttributes& attrs, const std::string& errorSuffix, SUMOVehicleParameter::Stop* stopParam = nullptr);

    /// @brief Processing of a stop
    Parameterised* addStop(const SUMOSAXAttributes& attrs) override;

    /// @brief add a routing request for a walking or intermodal person
    void addPersonTrip(const SUMOSAXAttributes& attrs) override;

    /// @brief add a fully specified walk
    void addWalk(const SUMOSAXAttributes& attrs) override;

    /// @brief Processing of a person
    void addTransportable(const SUMOSAXAttributes& attrs, const bool isPerson) override;

    /// @brief Processing of a ride
    void addRide(const SUMOSAXAttributes& attrs) override;

    /// @brief Processing of a tranship
    void addTranship(const SUMOSAXAttributes& attrs) override;

    /// @brief Processing of a transport
    void addTransport(const SUMOSAXAttributes& attrs) override;

    ///@ brief parse depart- and arrival positions of a walk
    void parseWalkPositions(const SUMOSAXAttributes& attrs, const std::string& personID,
                            const MSEdge* fromEdge, const MSEdge*& toEdge,
                            double& departPos, double& arrivalPos, MSStoppingPlace*& bs,
                            const MSStage* const lastStage, bool& ok);

    void initLaneTree(NamedRTree* tree) override;

    MSEdge* retrieveEdge(const std::string& id) override;

protected:
    /// @brief The current route
    ConstMSEdgeVector myActiveRoute;

    /// @brief number of repetitions of the active route
    int myActiveRouteRepeat;
    SUMOTime myActiveRoutePeriod;

    /// @brief whether the active route is stored indefinitely (used by state loader)
    bool myActiveRoutePermanent;

    /// @brief The time at which this route was replaced (from vehroute-output)
    SUMOTime myActiveRouteReplacedAtTime;

    /// @brief The index at which this route was replaced (from vehroute-output)
    int myActiveRouteReplacedIndex;

    /// @brief The type of the current object
    ObjectTypeEnum myActiveType;

    /// @brief The name of the current object type
    std::string myActiveTypeName;

    /// @brief Wether an object with 'via'-attribute is being parsed
    bool myHaveVia;

    /// @brief The plan of the current transportable (person or container)
    MSTransportable::MSTransportablePlan* myActiveTransportablePlan;

    /// @brief Information whether vehicles shall be directly added to the network or kept within the buffer
    bool myAddVehiclesDirectly;

    /// @brief The currently parsed distribution of vehicle types (probability->vehicle type)
    RandomDistributor<MSVehicleType*>* myCurrentVTypeDistribution;

    /// @brief The id of the currently parsed vehicle type distribution
    std::string myCurrentVTypeDistributionID;

    /// @brief The currently parsed distribution of routes (probability->route)
    RandomDistributor<ConstMSRoutePtr>* myCurrentRouteDistribution;

    /// @brief The id of the currently parsed route distribution
    std::string myCurrentRouteDistributionID;

    /// @brief whether a state file is being loaded
    bool myAmLoadingState;

    /// @brief prefix when copying vehicles with --scale
    std::string myScaleSuffix;

    /// @brief whether loaded rerouting events shall be replayed
    bool myReplayRerouting;

    /// @brief whether we are loading a personFlow that is starting triggered in a vehicle flow
    bool myStartTriggeredInFlow;

    /// @brief A random number generator used to choose from vtype/route distributions and computing the speed factors
    static SumoRNG myParsingRNG;

private:
    /// @brief delete already created MSTransportablePlans if error occurs before handing over responsibility to a MSTransportable.
    void deleteActivePlanAndVehicleParameter();

    /// @brief reset MSTransportablePlans after transportable tag closes
    void resetActivePlanAndVehicleParameter();

    /// @brief ends the flow of a transportable
    void closeTransportableFlow();

    /// @brief ends the processing of a transportable (as person or container)
    void closeTransportable();

    /// @brief delete already created MSTransportablePlans if error occurs before handing over responsibility to a MSTransportable.
    int addFlowTransportable(SUMOTime depart, MSVehicleType* type, const std::string& baseID, int i);

    double interpretDepartPosLat(const std::string& value, int departLane, const std::string& element);

    /// @brief adapt implicit route (edges derived from stops) to additional vehicle-stops
    ConstMSRoutePtr addVehicleStopsToImplicitRoute(ConstMSRoutePtr route, bool isPermanent);

    /// @brief Invalidated copy constructor
    MSRouteHandler(const MSRouteHandler& s) = delete;

    /// @brief Invalidated assignment operator
    MSRouteHandler& operator=(const MSRouteHandler& s) = delete;

    /// @brief Processing of a transport
    void addRideOrTransport(const SUMOSAXAttributes& attrs, const SumoXMLTag modeTag);

};
