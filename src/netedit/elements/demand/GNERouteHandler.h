/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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

#include <utils/handlers/RouteHandler.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEAttributesEditor;
class GNEContainer;
class GNEDemandElement;
class GNEEdge;
class GNEJunction;
class GNENet;
class GNEPerson;
class GNEPlanCreator;
class GNETAZ;
class GNEUndoList;
class GNEVehicle;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

class GNERouteHandler : public RouteHandler {

public:
    /// @brief Constructor
    GNERouteHandler(GNENet* net, FileBucket* bucket, const bool allowUndoRedo);

    /// @brief Destructor
    virtual ~GNERouteHandler();

    /// @brief run post parser tasks
    bool postParserTasks();

    /// @name build functions
    /// @{

    /// @brief build vType
    bool buildVType(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVTypeParameter& vTypeParameter);

    /// @brief build vType ref
    bool buildVTypeRef(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& vTypeID, const double probability);

    /// @brief build vType distribution
    bool buildVTypeDistribution(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const int deterministic);

    /// @brief build route
    bool buildRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, SUMOVehicleClass vClass,
                    const std::vector<std::string>& edgeIDs, const RGBColor& color, const int repeat, const SUMOTime cycleTime,
                    const double probability, const Parameterised::Map& routeParameters);

    /// @brief build route ref
    bool buildRouteRef(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& routeID, const double probability);

    /// @brief build route distribution
    bool buildRouteDistribution(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id);

    /// @brief build a vehicle over an existent route
    bool buildVehicleOverRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters);

    /// @brief build a vehicle with an embedded route
    bool buildVehicleEmbeddedRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                                   const std::vector<std::string>& edgeIDs, const RGBColor& color, const int repeat, const SUMOTime cycleTime,
                                   const Parameterised::Map& routeParameters);

    /// @brief build a flow over an existent route
    bool buildFlowOverRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters);

    /// @brief build a flow with an embedded route
    bool buildFlowEmbeddedRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                                const std::vector<std::string>& edgeIDs, const RGBColor& color, const int repeat, const SUMOTime cycleTime,
                                const Parameterised::Map& routeParameters);

    /// @brief build trip
    bool buildTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                   const std::string& fromEdgeID, const std::string& toEdgeID);

    /// @brief build trip over junctions
    bool buildTripJunctions(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                            const std::string& fromJunctionID, const std::string& toJunctionID);

    /// @brief build trip over TAZs
    bool buildTripTAZs(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                       const std::string& fromTAZID, const std::string& toTAZID);

    /// @brief build flow
    bool buildFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                   const std::string& fromEdgeID, const std::string& toEdgeIDs);

    /// @brief build flow over junctions
    bool buildFlowJunctions(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                            const std::string& fromJunctionID, const std::string& toJunctionID);

    /// @brief build flow over junctions
    bool buildFlowTAZs(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                       const std::string& fromTAZID, const std::string& toTAZID);

    /// @brief build person
    bool buildPerson(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& personParameters);

    /// @brief build person flow
    bool buildPersonFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& personFlowParameters);

    /// @brief build person trip
    bool buildPersonTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const CommonXMLStructure::PlanParameters& planParameters,
                         const double arrivalPos, const std::vector<std::string>& types, const std::vector<std::string>& modes,
                         const std::vector<std::string>& lines, const double walkFactor, const std::string& group);

    /// @brief build walk
    bool buildWalk(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const CommonXMLStructure::PlanParameters& planParameters,
                   const double arrivalPos, const double speed, const SUMOTime duration);

    /// @brief build ride
    bool buildRide(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const CommonXMLStructure::PlanParameters& planParameters,
                   const double arrivalPos, const std::vector<std::string>& lines, const std::string& group);

    /// @brief build container
    bool buildContainer(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& containerParameters);

    /// @brief build container flow
    bool buildContainerFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& containerFlowParameters);

    /// @brief build transport
    bool buildTransport(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const CommonXMLStructure::PlanParameters& planParameters,
                        const double arrivalPos, const std::vector<std::string>& lines, const std::string& group);

    /// @brief build tranship
    bool buildTranship(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const CommonXMLStructure::PlanParameters& planParameters,
                       const double arrivalPosition, const double departPosition, const double speed, const SUMOTime duration);

    /// @brief build person stop
    bool buildPersonStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const CommonXMLStructure::PlanParameters& planParameters,
                         const double endPos, const SUMOTime duration, const SUMOTime until,
                         const std::string& actType, const bool friendlyPos, const int parameterSet);

    /// @brief build container stop
    bool buildContainerStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const CommonXMLStructure::PlanParameters& planParameters,
                            const double endPos, const SUMOTime duration, const SUMOTime until, const std::string& actType, const bool friendlyPos,
                            const int parameterSet);

    /// @brief build stop
    bool buildStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const CommonXMLStructure::PlanParameters& planParameters,
                   const SUMOVehicleParameter::Stop& stopParameters);

    /// @brief build person plan
    bool buildPersonPlan(const GNEDemandElement* planTemplate, GNEDemandElement* personParent, GNEAttributesEditor* personPlanAttributesEditor,
                         GNEPlanCreator* planCreator, const bool centerAfterCreation);

    /// @brief build container plan
    bool buildContainerPlan(const GNEDemandElement* planTemplate, GNEDemandElement* containerParent, GNEAttributesEditor* containerPlanAttributesEditor,
                            GNEPlanCreator* planCreator, const bool centerAfterCreation);

    /// @}

    /// @brief duplicate given plan in new parent
    void duplicatePlan(const GNEDemandElement* originalPlan, GNEDemandElement* newParent);

    /// @brief check if via attribute is valid
    bool checkViaAttribute(const SumoXMLTag tag, const std::string& id, const std::vector<std::string>& via);

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

    /// @brief transform to trip over junctions
    static void transformToTripJunctions(GNEVehicle* originalVehicle);

    /// @brief transform to flow over junctions
    static void transformToFlowJunctions(GNEVehicle* originalVehicle);

    /// @brief transform to trip over TAZs
    static void transformToTripTAZs(GNEVehicle* originalVehicle);

    /// @brief transform to flow over TAZs
    static void transformToFlowTAZs(GNEVehicle* originalVehicle);

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

    /// @brief reverse functions
    /// @{

    /// @brief check if the given vehicle can be reversed
    static bool canReverse(const GNEDemandElement* element);

    /// @brief check if the given list of edges can be reversed
    static bool canReverse(GNENet* net, SUMOVehicleClass vClass, const std::vector<GNEEdge*>& edges);

    /// @brief reverse given demand element
    static void reverse(GNEDemandElement* element);

    /// @brief add reverse for given demand element
    static void addReverse(GNEDemandElement* element);

    /// @}

protected:
    /// @brief parse junction
    GNEJunction* parseJunction(const SumoXMLTag tag, const std::string& id, const std::string& junctionID);

    /// @brief parse TAZ
    GNEAdditional* parseTAZ(const SumoXMLTag tag, const std::string& id, const std::string& TAZID);

    /// @brief parse edge
    GNEEdge* parseEdge(const SumoXMLTag tag, const std::string& id, const std::string& edgeID,
                       const CommonXMLStructure::SumoBaseObject* sumoBaseObject,
                       const bool firstEdge);

    /// @brief parse stop edge
    GNEEdge* parseStopEdge(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const;

    /// @brief parse edge from lane ID
    GNEEdge* parseEdgeFromLaneID(const std::string& laneID) const;

    /// @brief parse edges
    std::vector<GNEEdge*> parseEdges(const SumoXMLTag tag, const std::string& id, const std::vector<std::string>& edgeIDs);

    /// @brief get type (Either type o typeDistribution)
    GNEDemandElement* getType(const std::string& id) const;

    /// @brief get route (Either route o routeDistribution)
    GNEDemandElement* getRoute(const std::string& id) const;

    /// @brief get person parent
    GNEDemandElement* getPersonParent(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const;

    /// @brief get container parent
    GNEDemandElement* getContainerParent(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const;

    /// @brief get route distribution parent
    GNEDemandElement* getRouteDistributionParent(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const;

    /// @brief get vType distribution parent
    GNEDemandElement* getVTypeDistributionParent(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const;

    /// @brief get distribution elements
    bool getDistributionElements(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, SumoXMLTag distributionElementTag,
                                 const std::vector<std::string>& distributionElementIDs, const std::vector<double>& probabilities,
                                 std::vector<const GNEDemandElement*>& elements);

    /// @brief get element by ID
    GNEDemandElement* retrieveDemandElement(const std::vector<SumoXMLTag> tags, const std::string& id);

    /// @brief check if element exist, and if overwrite
    bool checkElement(const SumoXMLTag tag, GNEDemandElement* demandElement);

private:
    /// @brief pointer to GNENet
    GNENet* myNet;

    /// @brief pointer for person and container plans
    CommonXMLStructure::SumoBaseObject* myPlanObject;

    /// @brief demand element parentplans (person and containers, used in postParserTasks)
    std::set<GNEDemandElement*> myParentPlanElements;

    /// @brief allow undo/redo
    const bool myAllowUndoRedo;

    /// @brief invalidate default onstructor
    GNERouteHandler() = delete;

    /// @brief invalidate copy constructor
    GNERouteHandler(const GNERouteHandler& s) = delete;

    /// @brief invalidate assignment operator
    GNERouteHandler& operator=(const GNERouteHandler& s) = delete;
};
