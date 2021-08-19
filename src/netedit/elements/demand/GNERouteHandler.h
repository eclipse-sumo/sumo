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
#include <utils/handlers/RouteHandler.h>


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
class GNERouteHandler : public RouteHandler {

public:
    /// @brief Constructor
    GNERouteHandler(const std::string& file, GNENet* net, bool undoDemandElements = true);

    /// @brief Destructor
    ~GNERouteHandler();

/**************************************/

    /// @name build functions
    /// @{

    /// @brief build vType
    void buildVType(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVTypeParameter& vTypeParameter,
                    const std::map<std::string, std::string> &parameters);

    /// @brief build vType distribution
    void buildVTypeDistribution(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &id);

    /// @brief build route
    void buildRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &id, SUMOVehicleClass vClass, 
                    const std::vector<std::string> &edges, const RGBColor &color, const int repeat, const SUMOTime cycleTime, 
                    const std::map<std::string, std::string> &parameters);

    /// @brief build route distribution
    void buildRouteDistribution(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &id);

    /// @brief build a vehicle over an existent route
    void buildVehicleOverRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                                const std::map<std::string, std::string> &parameters);

    /// @brief build a flow over an existent route
    void buildFlowOverRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                            const std::map<std::string, std::string> &parameters);

    /// @brief build vehicle with a embedded route
    void buildVehicleEmbeddedRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, SUMOVehicleParameter vehicleParameters, 
                                    const std::vector<std::string>& edgeIDs, const std::map<std::string, std::string> &parameters);

    /// @brief build flow with a embedded route
    void buildFlowEmbeddedRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, SUMOVehicleParameter vehicleParameters, 
                                const std::vector<std::string>& edgeIDs, const std::map<std::string, std::string> &parameters);

    /// @brief build trip
    void buildTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters, 
                   const std::string &fromEdgeID, const std::string &toEdgeID, const std::vector<std::string>& viaIDs,
                   const std::map<std::string, std::string> &parameters);

    /// @brief build flow
    void buildFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters, 
                   const std::string &fromEdgeID, const std::string &toEdgeID, const std::vector<std::string>& viaIDs,
                   const std::map<std::string, std::string> &parameters);

    /// @brief build stop
    void buildStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter::Stop& stopParameters);

    /// @brief build person
    void buildPerson(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& personParameters,
                     const std::map<std::string, std::string> &parameters);

    /// @brief build person flow
    void buildPersonFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& personFlowParameters,
                         const std::map<std::string, std::string> &parameters);

    /// @brief build person trip
    void buildPersonTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdgeID, const std::string &toEdgeID,
                         const std::string &toBusStopID, double arrivalPos, const std::vector<std::string>& types, const std::vector<std::string>& modes);

    /// @brief build walk
    void buildWalk(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdgeID, const std::string &toEdgeID,
                   const std::string &toBusStopID, const std::vector<std::string>& edgeIDs, const std::string &routeID, double arrivalPos);

    /// @brief build ride
    void buildRide(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdgeID, const std::string &toEdgeID, 
                   const std::string &toBusStopID, double arrivalPos, const std::vector<std::string>& lines);

    /// @brief build container
    void buildContainer(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& containerParameters,
                        const std::map<std::string, std::string> &parameters);

    /// @brief build container flow
    void buildContainerFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& containerFlowParameters,
                            const std::map<std::string, std::string> &parameters);

    /// @brief build transport
    void buildTransport(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdgeID, const std::string &toEdgeID,
                                const std::string &toBusStopID, const std::vector<std::string>& lines, const double arrivalPos);

    /// @brief build tranship
    void buildTranship(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdgeID, const std::string &toEdgeID,
                       const std::string &toBusStopID, const std::vector<std::string>& edgeIDs, const double speed, const double departPosition, 
                       const double arrivalPosition);
    /// @}

/**************************************/

    /// @brief check if there is already a vehicle (Vehicle, Trip, Flow or Flow) with the given ID
    static bool isVehicleIdDuplicated(GNENet* net, const std::string& id);

    /// @brief check if there is already a person (Person or PersonFlow) with the given ID
    static bool isPersonIdDuplicated(GNENet* net, const std::string& id);

    /// @brief check if there is already a container (Container or ContainerFlow) with the given ID
    static bool isContainerIdDuplicated(GNENet* net, const std::string& id);

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
    /// @brief parse edge
    GNEEdge* parseEdge(const SumoXMLTag tag, const std::string& edgeID) const;

    /// @brief parse edges
    std::vector<GNEEdge*> parseEdges(const SumoXMLTag tag, const std::vector<std::string>& edgeIDs) const;

    /// @brief get person parent
    GNEDemandElement *getPersonParent(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const;

    /// @brief get container parent
    GNEDemandElement *getContainerParent(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const;

private:
    /// @brief pointer to GNENet
    GNENet* myNet;

    /// @brief flag to check if created demand elements must be undo and redo
    bool myUndoDemandElements;
};


