/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNERouteHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
///
// Builds demand objects for netedit
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrameAttributesModuls.h>
#include <netedit/frames/GNEFrameModuls.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/vehicle/SUMORouteHandler.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class declarations
// ===========================================================================

class GNEViewNet;
class GNEEdge;
class GNETAZ;
class GNEDemandElement;
class GNEVehicle;
class GNEPerson;
class GNEContainer;
class GNEUndoList;

// ===========================================================================
// class definitions
// ===========================================================================

/// @class GNERouteHandler
/// @brief Builds trigger objects for GNENet (busStops, chargingStations, detectors, etc..)
class GNERouteHandler : public SUMORouteHandler {
public:
    /// @brief struct for saving route parameters
    struct RouteParameter {

        /// @brief constructor
        RouteParameter();

        /// @brief parameter constructor (use values of originalDemandElement)
        RouteParameter(GNEDemandElement* originalDemandElement);

        /// @brief set edges (list of consecutive edges)
        void setEdges(GNENet* net, const std::string& edgeIDs);

        /// @brief set edges (from, to and via edges)
        void setEdges(GNENet* net, const std::string& vehicleID, const std::string& fromID, const std::string& toID, const std::string& viaIDs);

        /// @brief clear edges
        void clearEdges();

        /// @brief string for saving parsed Route ID
        std::string routeID;

        /// @brief flag to check if route was loaded
        bool loadedID;

        /// @brief edges
        std::vector<GNEEdge*> edges;

        /// @brief vClass used by this route
        SUMOVehicleClass vClass;

        /// @brief string for saving parsed route colors
        RGBColor color;

        /// @brief repeat
        int repeat;

        /// @brief cycleTime
        SUMOTime cycleTime;

        /// @brief parameters
        Parameterised parameters;
    };

    /// @brief Constructor
    GNERouteHandler(const std::string& file, GNENet* net, bool undoDemandElements = true);

    /// @brief Destructor
    ~GNERouteHandler();

    /// @brief check if there is already a vehicle (Vehicle, Trip, Flow or Flow) with the given ID
    static bool isVehicleIdDuplicated(GNENet* net, const std::string& id);

    /// @brief check if there is already a person (Person or PersonFlow) with the given ID
    static bool isPersonIdDuplicated(GNENet* net, const std::string& id);

    /// @brief check if there is already a container (Container or ContainerFlow) with the given ID
    static bool isContainerIdDuplicated(GNENet* net, const std::string& id);

    /// @name build routes
    /// @{

    /// @brief build route
    static void buildRoute(GNENet* net, bool undoDemandElements, const RouteParameter& routeParameters, const std::vector<SUMOVehicleParameter::Stop>& activeStops);

    /// @}

    /// @name build vehicles
    /// @{

    /// @brief build a vehicle over an existent route
    static void buildVehicleOverRoute(GNENet* net, bool undoDemandElements, const SUMOVehicleParameter& vehicleParameters);

    /// @brief build a flow over an existent route
    static void buildFlowOverRoute(GNENet* net, bool undoDemandElements, const SUMOVehicleParameter& vehicleParameters);

    /// @brief build vehicle with a embedded route
    static void buildVehicleEmbeddedRoute(GNENet* net, bool undoDemandElements, SUMOVehicleParameter vehicleParameters, const std::vector<GNEEdge*>& edges);

    /// @brief build flow with a embedded route
    static void buildFlowEmbeddedRoute(GNENet* net, bool undoDemandElements, SUMOVehicleParameter vehicleParameters, const std::vector<GNEEdge*>& edges);

    /// @brief build trip
    static void buildTrip(GNENet* net, bool undoDemandElements, const SUMOVehicleParameter& vehicleParameters, GNEEdge* fromEdge, GNEEdge* toEdge, const std::vector<GNEEdge*>& via);

    /// @brief build flow
    static void buildFlow(GNENet* net, bool undoDemandElements, const SUMOVehicleParameter& vehicleParameters, GNEEdge* fromEdge, GNEEdge* toEdge, const std::vector<GNEEdge*>& via);

    /// @brief build stop
    static void buildStop(GNENet* net, bool undoDemandElements, const SUMOVehicleParameter::Stop& stopParameters, GNEDemandElement* stopParent);

    /// @}

    /// @name build person
    /// @{
    /// @brief build person
    static void buildPerson(GNENet* net, bool undoDemandElements, const SUMOVehicleParameter& personParameters);

    /// @brief build person flow
    static void buildPersonFlow(GNENet* net, bool undoDemandElements, const SUMOVehicleParameter& personFlowParameters);

    /// @}

    /// @name build personPlan
    /// @{
    /// @brief build person plan functions (used in Person / PersonPlan frames)
    static bool buildPersonPlan(SumoXMLTag tag, GNEDemandElement* personParent, GNEFrameAttributesModuls::AttributesCreator* personPlanAttributes, GNEFrameModuls::PathCreator* pathCreator);

    /// @brief build person trip
    static void buildPersonTrip(GNENet* net, bool undoDemandElements, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEEdge* toEdge, GNEAdditional* toBusStop,
                                double arrivalPos, const std::vector<std::string>& types, const std::vector<std::string>& modes);

    /// @brief build walk
    static void buildWalk(GNENet* net, bool undoDemandElements, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEEdge* toEdge,
                          GNEAdditional* toBusStop, const std::vector<GNEEdge*>& edges, GNEDemandElement* route, double arrivalPos);

    /// @brief build ride
    static void buildRide(GNENet* net, bool undoDemandElements, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEEdge* toEdge, GNEAdditional* toBusStop,
                          double arrivalPos, const std::vector<std::string>& lines);

    /// @brief build person stop
    static void buildStopPerson(GNENet* net, bool undoDemandElements, GNEDemandElement* personParent, GNEEdge* edge, GNEAdditional* busStop, const SUMOVehicleParameter::Stop& stopParameters);

    /// @}

    /// @name build container
    /// @{
    /// @brief build container
    static void buildContainer(GNENet* net, bool undoDemandElements, const SUMOVehicleParameter& containerParameters);

    /// @brief build container flow
    static void buildContainerFlow(GNENet* net, bool undoDemandElements, const SUMOVehicleParameter& containerFlowParameters);

    /// @}

    /// @name build containerPlan
    /// @{
    /// @brief build container plan functions (used in Container / ContainerPlan frames)
    static bool buildContainerPlan(SumoXMLTag tag, GNEDemandElement* containerParent, GNEFrameAttributesModuls::AttributesCreator* containerPlanAttributes, GNEFrameModuls::PathCreator* pathCreator);

    /// @brief build transport
    static void buildTransport(GNENet* net, bool undoDemandElements, GNEDemandElement* containerParent, GNEEdge* fromEdge, GNEEdge* toEdge,
                          GNEAdditional* toBusStop, double arrivalPos);

    /// @brief build tranship
    static void buildTranship(GNENet* net, bool undoDemandElements, GNEDemandElement* containerParent, GNEEdge* fromEdge, GNEEdge* toEdge,
                          GNEAdditional* toBusStop, const std::vector<GNEEdge*>& edges, double arrivalPos);

    /// @brief build container stop
    static void buildStopContainer(GNENet* net, bool undoDemandElements, GNEDemandElement* containerParent, GNEEdge* edge, GNEAdditional* busStop, const SUMOVehicleParameter::Stop& stopParameters);

    /// @}


    /// @brief transform vehicle functions
    /// @{

    /// @brief transform to vehicle over an existent route
    static void transformToVehicle(GNEVehicle* originalVehicle, bool createEmbeddedRoute);

    /// @brief transform routeFlow over an existent route
    static void transformToRouteFlow(GNEVehicle* originalVehicle, bool createEmbeddedRoute);

    /// @brief transform to trip
    static void transformToTrip(GNEVehicle* originalVehicle);

    /// @brief transform to flow
    static void transformToFlow(GNEVehicle* originalVehicle);

    /// @}

    /// @brief transform person functions
    /// @{

    /// @brief transform to vehicle over an existent route
    static void transformToPerson(GNEPerson* originalPerson);

    /// @brief transform routeFlow over an existent route
    static void transformToPersonFlow(GNEPerson* originalPerson);

    /// @}

    /// @brief transform container functions
    /// @{

    /// @brief transform to vehicle over an existent route
    static void transformToContainer(GNEContainer* originalContainer);

    /// @brief transform routeFlow over an existent route
    static void transformToContainerFlow(GNEContainer* originalContainer);

    /// @}

    /// @brief configure flow parameters
    static void setFlowParameters(const SumoXMLAttr attribute, int& parameters);

protected:
    /// @brief opens a type distribution for reading
    void openVehicleTypeDistribution(const SUMOSAXAttributes& attrs);

    /// @brief closes (ends) the building of a distribution
    void closeVehicleTypeDistribution();

    /// @brief opens a route for reading
    void openRoute(const SUMOSAXAttributes& attrs);

    /// @brief opens a routeFlow for reading
    void openFlow(const SUMOSAXAttributes& attrs);

    /// @brief opens a routeFlow for reading
    void openRouteFlow(const SUMOSAXAttributes& attrs);

    /// @brief opens a trip for reading
    void openTrip(const SUMOSAXAttributes& attrs);

    /**closes (ends) the building of a route.
     * Afterwards no edges may be added to it;
     * this method may throw exceptions when
     * a) the route is empty or
     * b) another route with the same id already exists
     */
    void closeRoute(const bool mayBeDisconnected = false);

    /// @brief opens a route distribution for reading
    void openRouteDistribution(const SUMOSAXAttributes& attrs);

    /// @brief closes (ends) the building of a distribution
    void closeRouteDistribution();

    /// @brief Ends the processing of a vehicle
    void closeVehicle();

    /// @brief Ends the processing of a vehicle Type
    void closeVType();

    /// @brief Ends the processing of a person
    void closePerson();

    /// @brief Ends the processing of a personFlow
    void closePersonFlow();

    /// @brief Ends the processing of a container
    void closeContainer();

    /// @brief Ends the processing of a containerFlow
    void closeContainerFlow();

    /// @brief Ends the processing of a routeFlow
    void closeFlow();

    /// @brief Ends the processing of a trip
    void closeTrip();

    /// @brief Processing of a stop
    void addStop(const SUMOSAXAttributes& attrs);

    /// @brief add a routing request for a walking or intermodal person
    void addPersonTrip(const SUMOSAXAttributes& attrs);

    /// @brief add a fully specified walk
    void addWalk(const SUMOSAXAttributes& attrs);

    /// @brief Processing of a person
    void addPerson(const SUMOSAXAttributes& attrs);

    /// @brief Processing of a ride
    void addRide(const SUMOSAXAttributes& attrs);

    /// @brief Processing of a container
    void addContainer(const SUMOSAXAttributes& attrs);

    /// @brief Processing of a tranship
    void addTranship(const SUMOSAXAttributes& attrs);

    /// @brief Processing of a transport
    void addTransport(const SUMOSAXAttributes& attrs);

private:
    /// @brief struct used for load person plans (Rides, Walks, etc.)
    struct PersonPlansValues {
        /// @brief default constructor
        PersonPlansValues();

        /// @brief update tag
        void updateGNETag();

        /// @brief check integrity
        bool checkIntegrity() const;

        /// @brief is first person plan
        bool isFirstPersonPlan() const;

        /// @brief return last valid edge (used to create consecutive person plans)
        GNEEdge* getLastEdge() const;

        /// @brief walk tag
        SumoXMLTag tag;

        /// @brief from edge
        GNEEdge* fromEdge;

        /// @brief to edge
        GNEEdge* toEdge;

        /// @brief to busStop
        GNEAdditional* toBusStop;

        /// @brief list of edges
        std::vector<GNEEdge*> edges;

        /// @brief arrival route
        GNEDemandElement* route;

        /// @brief arrival pos
        double arrivalPos;

        /// @brief vehicle types
        std::vector<std::string> vTypes;

        /// @brief modes
        std::vector<std::string> modes;

        /// @brief lines
        std::vector<std::string> lines;

        /// @brief stop parameters
        SUMOVehicleParameter::Stop stopParameters;

        /// @brief bus stop (stop)
        GNEAdditional* busStop;

        /// @brief edge stop
        GNEEdge* edgeStop;

        /// @brief lane stop
        GNELane* laneStop;

    private:
        /// @brief Invalidated copy constructor.
        PersonPlansValues(PersonPlansValues*) = delete;

        /// @brief Invalidated assignment operator.
        PersonPlansValues& operator=(PersonPlansValues*) = delete;
    };

    /// @brief person value
    struct PersonValue {
        /// @brief add person plan value (
        bool addPersonValue(GNENet* net, SumoXMLTag tag, const SUMOSAXAttributes& attrs);

        /// @brief check person plan loaded (this will change tags, set begin and end elements, etc.)
        bool checkPersonPlanValues();

        /// @brief container for person trips loaded values
        std::vector<PersonPlansValues> myPersonPlanValues;
    };

    /// @brief pointer to GNENet
    GNENet* myNet;

    /// @brief NETEDIT person values
    PersonValue myPersonValues;

    /// @brief NETEDIT Route Parameters
    RouteParameter myRouteParameter;

    /// @brief flag to check if created demand elements must be undo and redo
    bool myUndoDemandElements;

    /// @brief Pointer to loaded vehicle with embebbed route (needed for GNEStops)
    GNEDemandElement* myLoadedVehicleWithEmbebbedRoute;

    /// @brief flag used for parsing values
    bool myAbort;
};


