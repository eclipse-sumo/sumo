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
/// @file    RouteHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2021
///
// The XML-Handler for route elements loading
/****************************************************************************/
#pragma once
#include <config.h>

#include "CommonHandler.h"

// ===========================================================================
// class definitions
// ===========================================================================

class RouteHandler : public CommonHandler {

public:
    /**@brief Constructor
     * @param[in] filename Name of the parsed file
     * @param[in] hardFail enable or disable hardFails (continue handling demand elements if there is an error)
     */
    RouteHandler(const std::string& filename, const bool hardFail);

    /// @brief Destructor
    virtual ~RouteHandler();

    /// @brief begin parse attributes
    bool beginParseAttributes(SumoXMLTag tag, const SUMOSAXAttributes& attrs);

    /// @brief end parse attributes
    void endParseAttributes();

    /// @brief parse SumoBaseObject (it's called recursivelly)
    void parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj);

    /// @brief run post parser tasks
    virtual bool postParserTasks() = 0;

    /// @name build functions
    /// @{

    /// @brief build vType
    virtual bool buildVType(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVTypeParameter& vTypeParameter) = 0;

    /// @brief build vType ref
    virtual bool buildVTypeRef(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& vTypeID, const double probability) = 0;

    /// @brief build vType distribution
    virtual bool buildVTypeDistribution(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, const int deterministic) = 0;

    /// @brief build route
    virtual bool buildRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, SUMOVehicleClass vClass,
                            const std::vector<std::string>& edgeIDs, const RGBColor& color, const int repeat, const SUMOTime cycleTime,
                            const double probability, const Parameterised::Map& routeParameters) = 0;

    /// @brief build route ref
    virtual bool buildRouteRef(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& routeID, const double probability) = 0;

    /// @brief build route distribution
    virtual bool buildRouteDistribution(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id) = 0;

    /// @brief build a vehicle over an existent route
    virtual bool buildVehicleOverRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters) = 0;

    /// @brief build a vehicle with an embedded route
    virtual bool buildVehicleEmbeddedRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                                           const std::vector<std::string>& edgeIDs, const RGBColor& color, const int repeat, const SUMOTime cycleTime,
                                           const Parameterised::Map& routeParameters) = 0;

    /// @brief build a flow over an existent route
    virtual bool buildFlowOverRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters) = 0;

    /// @brief build a flow with an embedded route
    virtual bool buildFlowEmbeddedRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                                        const std::vector<std::string>& edgeIDs, const RGBColor& color, const int repeat, const SUMOTime cycleTime,
                                        const Parameterised::Map& routeParameters) = 0;

    /// @brief build trip (from-to edges)
    virtual bool buildTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                           const std::string& fromEdgeID, const std::string& toEdgeID) = 0;

    /// @brief build trip (from-to junctions)
    virtual bool buildTripJunctions(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                                    const std::string& fromJunctionID, const std::string& toJunctionID) = 0;

    /// @brief build trip (from-to TAZs)
    virtual bool buildTripTAZs(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                               const std::string& fromTazID, const std::string& toTazID) = 0;

    /// @brief build flow (from-to edges)
    virtual bool buildFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                           const std::string& fromEdgeID, const std::string& toEdgeID) = 0;

    /// @brief build flow (from-to junctions)
    virtual bool buildFlowJunctions(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                                    const std::string& fromJunctionID, const std::string& toJunctionID) = 0;

    /// @brief build flow (from-to TAZs)
    virtual bool buildFlowTAZs(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                               const std::string& fromTAZID, const std::string& toTAZID) = 0;

    /// @brief build person
    virtual bool buildPerson(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& personParameters) = 0;

    /// @brief build person flow
    virtual bool buildPersonFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& personFlowParameters) = 0;

    /// @brief build person trip
    virtual bool buildPersonTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const CommonXMLStructure::PlanParameters& planParameters,
                                 const double arrivalPos, const std::vector<std::string>& types, const std::vector<std::string>& modes,
                                 const std::vector<std::string>& lines, const double walkFactor, const std::string& group) = 0;

    /// @brief build walk
    virtual bool buildWalk(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const CommonXMLStructure::PlanParameters& planParameters,
                           const double arrivalPos, const double speed, const SUMOTime duration) = 0;

    /// @brief build ride
    virtual bool buildRide(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const CommonXMLStructure::PlanParameters& planParameters,
                           const double arrivalPos, const std::vector<std::string>& lines, const std::string& group) = 0;

    /// @brief build container
    virtual bool buildContainer(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& containerParameters) = 0;

    /// @brief build container flow
    virtual bool buildContainerFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& containerFlowParameters) = 0;

    /// @brief build transport
    virtual bool buildTransport(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const CommonXMLStructure::PlanParameters& planParameters,
                                const double arrivalPos, const std::vector<std::string>& lines, const std::string& group) = 0;

    /// @brief build tranship
    virtual bool buildTranship(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const CommonXMLStructure::PlanParameters& planParameters,
                               const double arrivalPosition, const double departPosition, const double speed, const SUMOTime duration) = 0;

    /// @brief build stop
    virtual bool buildStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const CommonXMLStructure::PlanParameters& planParameters,
                           const SUMOVehicleParameter::Stop& stopParameters) = 0;

    /// @}

private:
    /// @brief enable or disable hardFail (stop parsing if parameter aren't correct)
    const bool myHardFail;

    /// @brief The default value for flow begins
    SUMOTime myFlowBeginDefault;

    /// @brief The default value for flow ends
    SUMOTime myFlowEndDefault;

    /// @name parse route element attributes
    /// @{
    /// @brief parse vType
    void parseVType(const SUMOSAXAttributes& attrs);

    /// @brief parse vType reference
    void parseVTypeRef(const SUMOSAXAttributes& attrs);

    /// @brief parse vType distribution
    void parseVTypeDistribution(const SUMOSAXAttributes& attrs);

    /// @brief parse route
    void parseRoute(const SUMOSAXAttributes& attrs);

    /// @brief parse route reference
    void parseRouteRef(const SUMOSAXAttributes& attrs);

    /// @brief parse embedded route
    void parseRouteEmbedded(const SUMOSAXAttributes& attrs);

    /// @brief parse route distribution
    void parseRouteDistribution(const SUMOSAXAttributes& attrs);

    /// @brief parse trip
    void parseTrip(const SUMOSAXAttributes& attrs);

    /// @brief parse vehicle (including vehicles over routes and vehicles with embedded routes)
    void parseVehicle(const SUMOSAXAttributes& attrs);

    /// @brief parse flow (including flows, flows over routes and flows with embedded routes)
    void parseFlow(const SUMOSAXAttributes& attrs);

    /// @brief parse stop
    void parseStop(const SUMOSAXAttributes& attrs);

    /// @brief parse person
    void parsePerson(const SUMOSAXAttributes& attrs);

    /// @brief parse person flow
    void parsePersonFlow(const SUMOSAXAttributes& attrs);

    /// @brief parse person trip
    void parsePersonTrip(const SUMOSAXAttributes& attrs);

    /// @brief parse walk
    void parseWalk(const SUMOSAXAttributes& attrs);

    /// @brief parse ride
    void parseRide(const SUMOSAXAttributes& attrs);

    /// @brief parse container
    void parseContainer(const SUMOSAXAttributes& attrs);

    /// @brief parse container flow
    void parseContainerFlow(const SUMOSAXAttributes& attrs);

    /// @brief parse transport
    void parseTransport(const SUMOSAXAttributes& attrs);

    /// @brief parse tranship
    void parseTranship(const SUMOSAXAttributes& attrs);

    /// @brief parse interval
    void parseInterval(const SUMOSAXAttributes& attrs);

    /// @brief parse nested CarFollowingModel
    bool parseNestedCFM(const SumoXMLTag tag, const SUMOSAXAttributes& attrs,
                        CommonXMLStructure::SumoBaseObject* vTypeObject);

    /// @brief parse stop parameters
    bool parseStopParameters(SUMOVehicleParameter::Stop& stop, const SUMOSAXAttributes& attrs);

    /// @}

    /// @name check functions used for simplify code in handlers
    /// @{

    /// @brief check if element is defined over from-to edges
    bool isOverFromToEdges(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const;

    /// @brief check if element is defined over from-to junctions
    bool isOverFromToJunctions(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const;

    /// @brief check if element is defined over from-to junctions
    bool isOverFromToTAZs(const CommonXMLStructure::SumoBaseObject* sumoBaseObject) const;

    /// @}

    /// @brief adjust types and probabilities
    void adjustTypesAndProbabilities(std::vector<std::string>& vTypes, std::vector<double>& probabilities);

    /// @brief invalidate default onstructor
    RouteHandler() = delete;

    /// @brief invalidate copy constructor
    RouteHandler(const RouteHandler& s) = delete;

    /// @brief invalidate assignment operator
    RouteHandler& operator=(const RouteHandler& s) = delete;
};
