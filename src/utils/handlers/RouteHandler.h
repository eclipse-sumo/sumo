/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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

#include <utils/xml/CommonXMLStructure.h>


// ===========================================================================
// class definitions
// ===========================================================================

class RouteHandler {

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

    /// @name build functions
    /// @{

    /// @brief build vType
    virtual void buildVType(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVTypeParameter& vTypeParameter) = 0;

    /// @brief build vType distribution
    virtual void buildVTypeDistribution(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id,
                                        const int deterministic, const std::vector<std::string>& vTypes) = 0;

    /// @brief build route
    virtual void buildRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id, SUMOVehicleClass vClass,
                            const std::vector<std::string>& edgeIDs, const RGBColor& color, const int repeat, const SUMOTime cycleTime,
                            const Parameterised::Map& routeParameters) = 0;

    /// @brief build embedded route
    virtual void buildEmbeddedRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::vector<std::string>& edgeIDs,
                                    const RGBColor& color, const int repeat, const SUMOTime cycleTime,
                                    const Parameterised::Map& routeParameters) = 0;

    /// @brief build route distribution
    virtual void buildRouteDistribution(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& id) = 0;

    /// @brief build a vehicle over an existent route
    virtual void buildVehicleOverRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters) = 0;

    /// @brief build a flow over an existent route
    virtual void buildFlowOverRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters) = 0;

    /// @brief build trip (from-to edges)
    virtual void buildTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                           const std::string& fromEdgeID, const std::string& toEdgeID) = 0;

    /// @brief build trip (from-to junctions)
    virtual void buildTripJunctions(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                                    const std::string& fromJunctionID, const std::string& toJunctionID) = 0;

    /// @brief build flow (from-to edges)
    virtual void buildFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                           const std::string& fromEdgeID, const std::string& toEdgeID) = 0;

    /// @brief build flow (from-to junctions)
    virtual void buildFlowJunctions(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                                    const std::string& fromJunctionID, const std::string& toJunctionID) = 0;

    /// @brief build person
    virtual void buildPerson(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& personParameters) = 0;

    /// @brief build person flow
    virtual void buildPersonFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& personFlowParameters) = 0;

    /// @brief build person trip
    virtual void buildPersonTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& fromEdgeID, const std::string& toEdgeID,
                                 const std::string& fromJunctionID, const std::string& toJunctionID, const std::string& toBusStopID, double arrivalPos,
                                 const std::vector<std::string>& types, const std::vector<std::string>& modes, const std::vector<std::string>& lines) = 0;

    /// @brief build walk
    virtual void buildWalk(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& fromEdgeID, const std::string& toEdgeID,
                           const std::string& fromJunctionID, const std::string& toJunctionID, const std::string& toBusStopID,
                           const std::vector<std::string>& edgeIDs, const std::string& routeID, double arrivalPos) = 0;

    /// @brief build ride
    virtual void buildRide(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& fromEdgeID, const std::string& toEdgeID,
                           const std::string& toBusStopID, double arrivalPos, const std::vector<std::string>& lines) = 0;

    /// @brief build container
    virtual void buildContainer(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& containerParameters) = 0;

    /// @brief build container flow
    virtual void buildContainerFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& containerFlowParameters) = 0;

    /// @brief build transport
    virtual void buildTransport(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& fromEdgeID, const std::string& toEdgeID,
                                const std::string& toBusStopID, const std::vector<std::string>& lines, const double arrivalPos) = 0;

    /// @brief build tranship
    virtual void buildTranship(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& fromEdgeID, const std::string& toEdgeID,
                               const std::string& toBusStopID, const std::vector<std::string>& edgeIDs, const double speed, const double departPosition,
                               const double arrivalPosition) = 0;

    /// @brief build stop
    virtual void buildStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter::Stop& stopParameters) = 0;

    /// @}

    /// @brief get flag for check if a element wasn't created
    bool isErrorCreatingElement() const;

protected:
    /// @brief write error and enable error creating element
    void writeError(const std::string& error);

private:
    /// @brief filename (needed for parsing vTypes)
    const std::string myFilename;

    /// @brief enable or disable hardFail (stop parsing if parameter aren't correct)
    const bool myHardFail;

    /// @brief The default value for flow begins
    SUMOTime myFlowBeginDefault;

    /// @brief The default value for flow ends
    SUMOTime myFlowEndDefault;

    /// @brief common XML Structure
    CommonXMLStructure myCommonXMLStructure;

    /// @brief flag for check if a element wasn't created
    bool myErrorCreatingElement = false;

    /// @brief write error "invalid id"
    void writeErrorInvalidID(const SumoXMLTag tag, const std::string& id);

    /// @name parse route element attributes
    /// @{
    /// @brief parse vType
    void parseVType(const SUMOSAXAttributes& attrs);

    /// @brief parse vType distribution
    void parseVTypeDistribution(const SUMOSAXAttributes& attrs);

    /// @brief parse route
    void parseRoute(const SUMOSAXAttributes& attrs);

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

    /// @brief parse generic parameters
    void parseParameters(const SUMOSAXAttributes& attrs);

    /// @brief parse nested CarFollowingModel
    bool parseNestedCFM(const SumoXMLTag tag, const SUMOSAXAttributes& attrs);

    /// @}

    /// @brief parse stop parameters
    bool parseStopParameters(SUMOVehicleParameter::Stop& stop, const SUMOSAXAttributes& attrs);

    /// @brief check embedded route
    bool isEmbeddedRoute(const SUMOSAXAttributes& attrs) const;

    /// @brief check parents
    void checkParent(const SumoXMLTag currentTag, const std::vector<SumoXMLTag>& parentTags, bool& ok);

    /// @brief invalidate copy constructor
    RouteHandler(const RouteHandler& s) = delete;

    /// @brief invalidate assignment operator
    RouteHandler& operator=(const RouteHandler& s) = delete;
};
