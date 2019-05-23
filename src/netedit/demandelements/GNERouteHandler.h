/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERouteHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2019
/// @version $Id$
///
// Builds demand objects for netedit
/****************************************************************************/
#ifndef GNERouteHandler_h
#define GNERouteHandler_h

// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>

#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include <utils/vehicle/SUMORouteHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================

class GNEViewNet;
class GNEEdge;
class GNETAZ;
class GNEDemandElement;
class GNEVehicle;

// ===========================================================================
// class definitions
// ===========================================================================

/// @class GNERouteHandler
/// @brief Builds trigger objects for GNENet (busStops, chargingStations, detectors, etc..)
class GNERouteHandler : public SUMORouteHandler {
public:
    /// @brief Constructor
    GNERouteHandler(const std::string& file, GNEViewNet* viewNet, bool undoDemandElements = true);

    /// @brief Destructor
    ~GNERouteHandler();

    /// @brief check if there is already a vehicle (Vehicle, Trip, Flow or Flow) with the given ID
    static bool isVehicleIdDuplicated(GNEViewNet* viewNet, const std::string& id); 

    /// @brief build functions
    /// @{
    /// @brief build a vehicle over an existent route
    static void buildVehicleOverRoute(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter &vehicleParameters);

    /// @brief build a flow over an existent route
    static void buildFlowOverRoute(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter &vehicleParameters);

    /// @brief build vehicle with a embebbed route
    static void buildVehicleWithEmbebbedRoute(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter vehicleParameters, GNEDemandElement* embebbedRouteCopy);

    /// @brief build flow with a embebbed route
    static void buildFlowWithEmbebbedRoute(GNEViewNet* viewNet, bool undoDemandElements, SUMOVehicleParameter vehicleParameters, GNEDemandElement* embebbedRouteCopy);

    /// @brief build trip
    static void buildTrip(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter &vehicleParameters, const std::vector<GNEEdge*>& edges);

    /// @brief build flow
    static void buildFlow(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter &vehicleParameters, const std::vector<GNEEdge*>& edges);

    /// @brief build stop
    static void buildStop(GNEViewNet* viewNet, bool undoDemandElements, const SUMOVehicleParameter::Stop& stopParameters, GNEDemandElement* stopParent, bool friendlyPosition);

    /// @}

    /// @brief transform functions
    /// @{
    /// @brief transform to vehicle over an existent route
    static void transformToVehicleOverRoute(GNEVehicle* originalVehicle);

    /// @brief transform flow over an existent route
    static void transformToFlowOverRoute(GNEVehicle* originalVehicle);

    /// @brief transform to vehicle with a embedded route
    static void transformToVehicleWithEmbeddedRoute(GNEVehicle* originalVehicle);

    /// @brief transform to flow with a embedded route
    static void transformToFlowWithEmbeddedRoute(GNEVehicle* originalVehicle);

    /// @brief transform to trip
    static void transformToTrip(GNEVehicle* originalVehicle);

    /// @brief transform to flow
    static void transformToFlow(GNEVehicle* originalVehicle);

    /// @}

protected:
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
    /// @brief flag used for parsing route attribute
    bool myAbort;

    /// @brief string for saving parsed Route ID
    std::string myRouteID;

    /// @brief string for saving parsed edges
    std::string myEdgeIDs;

    /// @brief string for saving parsed route colors
    RGBColor myRouteColor;

    /// @brief string for saving parsed from edge ID
    std::string myFromID;

    /// @brief string for saving parsed to edge ID
    std::string myToID;

    /// @brief string for saving parsed via edges IDs
    std::string myViaIDs;

    /// @brief pointer to View's Net
    GNEViewNet* myViewNet;

    /// @brief flag to check if created demand elements must be undo and redo
    bool myUndoDemandElements;
};


#endif
