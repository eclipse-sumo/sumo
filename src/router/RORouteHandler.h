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
/// @file    RORouteHandler.h
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
#include <vector>
#include <utils/distribution/RandomDistributor.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/MapMatcher.h>
#include <utils/router/PedestrianRouter.h>
#include <utils/vehicle/SUMORouteHandler.h>
#include "ROPerson.h"


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice_String;
class ROEdge;
class ROLane;
class RONet;
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
class RORouteHandler : public SUMORouteHandler, public MapMatcher<ROEdge, ROLane, RONode> {
public:
    /// @brief standard constructor
    RORouteHandler(RONet& net, const std::string& file,
                   const bool tryRepair,
                   const bool emptyDestinationsAllowed,
                   const bool ignoreErrors,
                   const bool checkSchema);

    /// @brief standard destructor
    ~RORouteHandler() override;

    /// @brief Checks whether the route file is sorted by departure time if needed
    bool checkLastDepart() override;

protected:
    void deleteActivePlanAndVehicleParameter();

    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    void myStartElement(int element,
                        const SUMOSAXAttributes& attrs) override;
    //@}

    /** @brief Called for parsing from and to and the corresponding taz attributes
     *
     * @param[in] element description of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     */
    void parseFromViaTo(SumoXMLTag tag, const SUMOSAXAttributes& attrs, bool& ok);

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

    /// @brief Ends the processing of a vehicle
    void closeVehicle() override;

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

    /// @brief retrieve stopping place element
    const SUMOVehicleParameter::Stop* retrieveStoppingPlace(const SUMOSAXAttributes& attrs, const std::string& errorSuffix, std::string& id, const SUMOVehicleParameter::Stop* stopParam = nullptr);

    /// @brief Processing of a stop
    Parameterised* addStop(const SUMOSAXAttributes& attrs) override;

    /// @brief Processing of a person from a personFlow
    void addFlowPerson(const std::string& typeID, SUMOTime depart, const std::string& baseID, int i);

    /// @brief Processing of a ride
    void addRide(const SUMOSAXAttributes& attrs) override;

    /// @brief Processing of a transport
    void addTransport(const SUMOSAXAttributes& attrs) override;

    /// @brief Processing of a tranship
    void addTranship(const SUMOSAXAttributes& attrs) override;

    /// @brief Parse edges from strings
    void parseEdges(const std::string& desc, ConstROEdgeVector& into,
                    const std::string& rid, bool& ok);

    /// @brief add a routing request for a walking or intermodal person
    void addPersonTrip(const SUMOSAXAttributes& attrs) override;

    /// @brief add a fully specified walk
    void addWalk(const SUMOSAXAttributes& attrs) override;

    ///@ brief parse depart- and arrival positions of a walk
    void parseWalkPositions(const SUMOSAXAttributes& attrs, const std::string& personID,
                            const ROEdge* fromEdge, const ROEdge*& toEdge,
                            double& departPos, double& arrivalPos, std::string& busStopID,
                            const ROPerson::PlanItem* const lastStage, bool& ok);

    void initLaneTree(NamedRTree* tree) override;

    ROEdge* retrieveEdge(const std::string& id) override;

protected:
    /// @brief The current route
    RONet& myNet;

    /// @brief The current route
    ConstROEdgeVector myActiveRoute;

    /// @brief number of repetitions of the active route
    int myActiveRouteRepeat;
    SUMOTime myActiveRoutePeriod;

    /// @brief The plan of the current person
    std::vector<ROPerson::PlanItem*>* myActivePlan;

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

    /// @brief The begin time
    const SUMOTime myBegin;

    /// @brief whether to keep the vtype distribution in output
    const bool myKeepVTypeDist;

    /// @brief whether input is read all at once (no sorting check is necessary)
    const bool myUnsortedInput;

    /// @brief The currently parsed distribution of vehicle types (probability->vehicle type)
    RandomDistributor<SUMOVTypeParameter*>* myCurrentVTypeDistribution;

    /// @brief The id of the currently parsed vehicle type distribution
    std::string myCurrentVTypeDistributionID;

    /// @brief The currently parsed route alternatives
    RORouteDef* myCurrentAlternatives;

private:
    /// @brief Invalidated copy constructor
    RORouteHandler(const RORouteHandler& s) = delete;

    /// @brief Invalidated assignment operator
    RORouteHandler& operator=(const RORouteHandler& s) = delete;
};
