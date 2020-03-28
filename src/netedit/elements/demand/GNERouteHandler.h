/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
        void setEdges(GNEViewNet* viewNet, const std::string& edgeIDs);

        /// @brief set edges (from, to and via edges)
        void setEdges(GNEViewNet* viewNet, const std::string& vehicleID, const std::string& fromID, const std::string& toID, const std::string& viaIDs);

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

        /// @brief parameters
        Parameterised parameters;
    };

    /// @brief Constructor
    GNERouteHandler(const std::string& file, GNEViewNet* viewNet, bool undoDemandElements = true);

    /// @brief Destructor
    ~GNERouteHandler();

    /// @brief check if there is already a vehicle (Vehicle, Trip, Flow or Flow) with the given ID
    static bool isVehicleIdDuplicated(GNEViewNet* viewNet, const std::string& id);

    /// @brief check if there is already a person (Person or PersonFlow) with the given ID
    static bool isPersonIdDuplicated(GNEViewNet* viewNet, const std::string& id);

    /// @brief build functions
    /// @{
    /// @brief build a vehicle over an existent route
    static void buildVehicleOverRoute(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter& vehicleParameters);

    /// @brief build a flow over an existent route
    static void buildFlowOverRoute(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter& vehicleParameters);

    /// @brief build vehicle with a embedded route
    static void buildVehicleWithEmbeddedRoute(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter vehicleParameters, GNEDemandElement* embeddedRouteCopy);

    /// @brief build flow with a embedded route
    static void buildFlowWithEmbeddedRoute(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter vehicleParameters, GNEDemandElement* embeddedRouteCopy);

    /// @brief build trip
    static void buildTrip(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter& vehicleParameters, GNEEdge* fromEdge, GNEEdge* toEdge, const std::vector<GNEEdge*>& via);

    /// @brief build flow
    static void buildFlow(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter& vehicleParameters, GNEEdge* fromEdge, GNEEdge* toEdge, const std::vector<GNEEdge*>& via);

    /// @brief build stop
    static void buildStop(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter::Stop& stopParameters, GNEDemandElement* stopParent);

    /// @brief build person
    static void buildPerson(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter& personParameters);

    /// @brief build person flow
    static void buildPersonFlow(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter& personFlowParameters);

    /// @brief build trip using a from-to edges
    static void buildPersonTripFromTo(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEEdge* toEdge, double arrivalPos,
                                      const std::vector<std::string>& types, const std::vector<std::string>& modes);

    /// @brief build trip using a from edge and a busStop
    static void buildPersonTripBusStop(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEAdditional* busStop,
                                       const std::vector<std::string>& types, const std::vector<std::string>& modes);

    /// @brief build walk using a list of consecutive edges
    static void buildWalkEdges(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, const std::vector<GNEEdge*>& edges, double arrivalPos);

    /// @brief build walk using a from-to edges
    static void buildWalkFromTo(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEEdge* toEdge, double arrivalPos);

    /// @brief build walk using a from edge an a busStop
    static void buildWalkBusStop(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEAdditional* busStop);

    /// @brief build walk using a list of consecutive edges
    static void buildWalkRoute(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, GNEDemandElement* route, double arrivalPos);

    /// @brief build ride using a from-to edges
    static void buildRideFromTo(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEEdge* toEdge, const std::vector<std::string>& lines, double arrivalPos);

    /// @brief build ride using a from edge and a busStop
    static void buildRideBusStop(GNEViewNet* viewNet, bool undoDemandElements, GNEDemandElement* personParent, GNEEdge* fromEdge, GNEAdditional* busStop, const std::vector<std::string>& lines);

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

    /// @brief configure flow parameters
    static void setFlowParameters(const SumoXMLAttr attribute, int& parameters);

protected:
    /// @brief embebbe route within a vehicle
    static void embebbeRoute(GNEVehicle* vehicle, GNEUndoList* undoList);

    /// @brief separate vehicle and embedded route
    static GNEVehicle* separateEmbeddedRoute(GNEVehicle* vehicle, GNEUndoList* undoList);

    /// @brief opens a type distribution for reading
    void openVehicleTypeDistribution(const SUMOSAXAttributes& attrs);

    /// @brief closes (ends) the building of a distribution
    void closeVehicleTypeDistribution();

    /// @brief opens a route for reading
    void openRoute(const SUMOSAXAttributes& attrs);

    /// @brief opens a routeFlow for reading
    void openFlow(const SUMOSAXAttributes& attrs);

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

    /// @brief Processing of a container
    void addContainer(const SUMOSAXAttributes& attrs);

    /// @brief Processing of a ride
    void addRide(const SUMOSAXAttributes& attrs);

    /// @brief Processing of a transport
    void addTransport(const SUMOSAXAttributes& attrs);

    /// @brief Processing of a tranship
    void addTranship(const SUMOSAXAttributes& attrs);

private:
    /// @brief struct used for load person plans (Rides, Walks, etc.)
    struct PersonPlansValues {
        /// @brief default constructor
        PersonPlansValues();

        /// @brief return last valid edge (used to create consecutive person plans)
        GNEEdge* getLastEdge() const;

        /// @brief walk tag
        SumoXMLTag tag;

        /// @brief from edge
        GNEEdge* from;

        /// @brief to edge
        GNEEdge* to;

        /// @brief via edges
        std::vector<GNEEdge*> via;

        /// @brief list of edges
        std::vector<GNEEdge*> edges;

        /// @brief busStop
        GNEAdditional* busStop;

        /// @brief containerStop
        GNEAdditional* containerStop;

        /// @brief chargingStation
        GNEAdditional* chargingStation;

        /// @brief parkingArea
        GNEAdditional* parkingArea;

        /// @brief arrival route
        GNEDemandElement* route;

        /// @brief vehicle types
        std::vector<std::string> vTypes;

        /// @brief modes
        std::vector<std::string> modes;

        /// @brief lines
        std::vector<std::string> lines;

        /// @brief arrival pos
        double arrivalPos;

        /// @brief laneStop
        GNELane* laneStop;

        /// @brief stop parameters
        SUMOVehicleParameter::Stop stopParameters;
    };

    /// @brief pointer to View's Net
    GNEViewNet* myViewNet;

    /// @brief container for person trips loaded values
    std::vector<PersonPlansValues> myPersonPlanValues;

    /// @brief NETEDIT Route Parameters
    RouteParameter myRouteParameter;

    /// @brief flag to check if created demand elements must be undo and redo
    bool myUndoDemandElements;

    /// @brief Pointer to loaded vehicle with embebbed route (needed for GNEStops)
    GNEDemandElement* myLoadedVehicleWithEmbebbedRoute;

    /// @brief flag used for parsing values
    bool myAbort;
};


