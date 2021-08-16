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
/// @file    RouteHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2021
///
// The XML-Handler for route elements loading
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/xml/CommonXMLStructure.h>
#include <utils/xml/SUMOSAXHandler.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RouteHandler
 * @brief The XML-Handler for network loading
 *
 * The SAX2-handler responsible for parsing networks and routes to load.
 * This is an extension of the MSRouteHandler as routes and vehicles may also
 *  be loaded from network descriptions.
 */
class RouteHandler : private SUMOSAXHandler {

public:
    /** @brief Constructor
     * @param[in] file Name of the parsed file
     */
    RouteHandler(const std::string& file);

    /// @brief Destructor
    ~RouteHandler();

    /// @brief parse
    bool parse();

    /// @brief parse SumoBaseObject (it's called recursivelly)
    void parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj);

    /// @name build functions
    /// @{
    /// @brief build a vehicle over an existent route
    static void buildVehicleOverRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters);

    /// @brief build a flow over an existent route
    static void buildFlowOverRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters);

    /// @brief build vehicle with a embedded route
    static void buildVehicleEmbeddedRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, SUMOVehicleParameter vehicleParameters, 
                                          const std::vector<std::string>& edges);

    /// @brief build flow with a embedded route
    static void buildFlowEmbeddedRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, SUMOVehicleParameter vehicleParameters, 
                                       const std::vector<std::string>& edges);

    /// @brief build trip
    static void buildTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters, 
                          const std::string &fromEdge, const std::string &toEdge, const std::vector<std::string>& via);

    /// @brief build flow
    static void buildFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters, 
                          const std::string &fromEdge, const std::string &toEdge, const std::vector<std::string>& via);

    /// @brief build stop
    static void buildStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter::Stop& stopParameters);

    /// @brief build person
    static void buildPerson(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& personParameters);

    /// @brief build person flow
    static void buildPersonFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& personFlowParameters);


    /// @brief build person trip
    static void buildPersonTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdge, const std::string &toEdge,
                                const std::string &toBusStop, double arrivalPos, const std::vector<std::string>& types, const std::vector<std::string>& modes);

    /// @brief build walk
    static void buildWalk(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdge, const std::string &toEdge,
                          const std::string &toBusStop, const std::vector<std::string>& edges, const std::string &route, double arrivalPos);

    /// @brief build ride
    static void buildRide(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdge, const std::string &toEdge, 
                          const std::string &toBusStop, double arrivalPos, const std::vector<std::string>& lines);

    /// @brief build person stop
    static void buildStopPerson(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &edge, const std::string &busStop, 
                                const SUMOVehicleParameter::Stop& stopParameters);

    /// @brief build container
    static void buildContainer(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& containerParameters);

    /// @brief build container flow
    static void buildContainerFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& containerFlowParameters);

    /// @brief build transport
    static void buildTransport(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdge, const std::string &toEdge,
                               const std::string &toBusStop, const std::vector<std::string>& lines, const double arrivalPos);

    /// @brief build tranship
    static void buildTranship(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdge, const std::string &toEdge,
                              const std::string &toBusStop, const std::vector<std::string>& edges, const double speed, const double departPosition, 
                              const double arrivalPosition);

    /// @brief build container stop
    static void buildStopContainer(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &edge, const std::string &containerStop, 
                                   const SUMOVehicleParameter::Stop& stopParameters);
    /// @}

private:
    /// @brief common XML Structure
    CommonXMLStructure myCommonXMLStructure;

    /// @name inherited from GenericSAXHandler
    /// @{
    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     * @todo Refactor/describe
     */
    virtual void myStartElement(int element, const SUMOSAXAttributes& attrs);

    /** @brief Called when a closing tag occurs
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     * @todo Refactor/describe
     */
    virtual void myEndElement(int element);
    /// @}

    /// @name parse route element attributes
    /// @{

    /// @brief parse busStop attributes
    void parseBusStopAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse trainStop attributes
    void parseTrainStopAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse access attributes
    void parseAccessAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse containerStop attributes
    void parseContainerStopAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse chargingStation attributes
    void parseChargingStationAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse parking area attributes
    void parseParkingAreaAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse parking space attributes
    void parseParkingSpaceAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse E1 attributes
    void parseE1Attributes(const SUMOSAXAttributes& attrs);

    /// @brief parse E2 attributes
    void parseE2Attributes(const SUMOSAXAttributes& attrs);

    /// @brief parse E3 attributes
    void parseE3Attributes(const SUMOSAXAttributes& attrs);

    /// @brief parse entry attributes
    void parseEntryAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse exist attributes
    void parseExitAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse E1 instant attributes
    void parseE1InstantAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse TAZ attributes
    void parseTAZAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse TAZ source attributes
    void parseTAZSourceAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse TAZ sink attributes
    void parseTAZSinkAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse variable speed sign attributes
    void parseVariableSpeedSignAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse variable speed sign step attributes
    void parseVariableSpeedSignStepAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse calibrator attributes
    void parseCalibratorAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse calibrator flow attributes
    void parseCalibratorFlowAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse rerouter attributes
    void parseRerouterAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse rerouter interval attributes
    void parseRerouterIntervalAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse closing lane reroute attributes
    void parseClosingLaneRerouteAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse closing reroute attributes
    void parseClosingRerouteAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse dest prob reroute attributes
    void parseDestProbRerouteAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse parking area reroute attributes
    void parseParkingAreaRerouteAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse route prob reroute attributes
    void parseRouteProbRerouteAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse route probe attributes
    void parseRouteProbeAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse vaporizer attributes
    void parseVaporizerAttributes(const SUMOSAXAttributes& attrs);

    // @brief parse poly attributes
    void parsePolyAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse POI attributes
    void parsePOIAttributes(const SUMOSAXAttributes& attrs);

    /// @brief parse generic parameters
    void parseParameters(const SUMOSAXAttributes& attrs);

    /// @}

    /// @brief check parents
    void checkParent(const SumoXMLTag currentTag, const SumoXMLTag parentTag, bool& ok) const;

    /// @brief invalidate copy constructor
    RouteHandler(const RouteHandler& s) = delete;

    /// @brief invalidate assignment operator
    RouteHandler& operator=(const RouteHandler& s) = delete;
};
