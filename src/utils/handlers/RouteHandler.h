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

    /// @brief build vType
    virtual void buildVType(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVTypeParameter& vTypeParameter,
                            const std::map<std::string, std::string> &parameters) = 0;

    /// @brief build vType distribution
    virtual void buildVTypeDistribution(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &id) = 0;

    /// @brief build route
    virtual void buildRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &id, const std::vector<std::string> &edges, 
                            const RGBColor &color, const int repeat, const SUMOTime cycleTime, const std::map<std::string, std::string> &parameters) = 0;

    /// @brief build route distribution
    virtual void buildRouteDistribution(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &id) = 0;

    /// @brief build a vehicle over an existent route
    virtual void buildVehicleOverRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                                        const std::map<std::string, std::string> &parameters) = 0;

    /// @brief build a flow over an existent route
    virtual void buildFlowOverRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters,
                                    const std::map<std::string, std::string> &parameters) = 0;

    /// @brief build vehicle with a embedded route
    virtual void buildVehicleEmbeddedRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, SUMOVehicleParameter vehicleParameters, 
                                           const std::vector<std::string>& edges, const std::map<std::string, std::string> &parameters) = 0;

    /// @brief build flow with a embedded route
    virtual void buildFlowEmbeddedRoute(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, SUMOVehicleParameter vehicleParameters, 
                                        const std::vector<std::string>& edges, const std::map<std::string, std::string> &parameters) = 0;

    /// @brief build trip
    virtual void buildTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters, 
                           const std::string &fromEdge, const std::string &toEdge, const std::vector<std::string>& via,
                           const std::map<std::string, std::string> &parameters) = 0;

    /// @brief build flow
    virtual void buildFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& vehicleParameters, 
                           const std::string &fromEdge, const std::string &toEdge, const std::vector<std::string>& via,
                           const std::map<std::string, std::string> &parameters) = 0;

    /// @brief build stop
    virtual void buildStop(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter::Stop& stopParameters) = 0;

    /// @brief build person
    virtual void buildPerson(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& personParameters,
                             const std::map<std::string, std::string> &parameters) = 0;

    /// @brief build person flow
    virtual void buildPersonFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& personFlowParameters,
                                 const std::map<std::string, std::string> &parameters) = 0;

    /// @brief build person trip
    virtual void buildPersonTrip(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdge, const std::string &toEdge,
                                 const std::string &toBusStop, double arrivalPos, const std::vector<std::string>& types, const std::vector<std::string>& modes) = 0;

    /// @brief build walk
    virtual void buildWalk(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdge, const std::string &toEdge,
                           const std::string &toBusStop, const std::vector<std::string>& edges, const std::string &route, double arrivalPos) = 0;

    /// @brief build ride
    virtual void buildRide(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdge, const std::string &toEdge, 
                           const std::string &toBusStop, double arrivalPos, const std::vector<std::string>& lines) = 0;

    /// @brief build container
    virtual void buildContainer(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& containerParameters,
                                const std::map<std::string, std::string> &parameters) = 0;

    /// @brief build container flow
    virtual void buildContainerFlow(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const SUMOVehicleParameter& containerFlowParameters,
                                    const std::map<std::string, std::string> &parameters) = 0;

    /// @brief build transport
    virtual void buildTransport(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdge, const std::string &toEdge,
                                const std::string &toBusStop, const std::vector<std::string>& lines, const double arrivalPos) = 0;

    /// @brief build tranship
    virtual void buildTranship(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string &fromEdge, const std::string &toEdge,
                               const std::string &toBusStop, const std::vector<std::string>& edges, const double speed, const double departPosition, 
                               const double arrivalPosition) = 0;
    /// @}

private:
    /// @brif enable or disable hardFail (stop parsing if parameter aren't correct)
    const bool myHardFail;

    /// @brief The default value for flow begins
    SUMOTime myFlowBeginDefault;

    /// @brief The default value for flow ends
    SUMOTime myFlowEndDefault;

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

    /// @}

    /// @brief parse stop parameters
    bool parseStopParameters(SUMOVehicleParameter::Stop &stop, const SUMOSAXAttributes& attrs) const;

    /// @brief check parents
    void checkParent(const SumoXMLTag currentTag, const SumoXMLTag parentTag, bool& ok) const;

    /// @brief invalidate copy constructor
    RouteHandler(const RouteHandler& s) = delete;

    /// @brief invalidate assignment operator
    RouteHandler& operator=(const RouteHandler& s) = delete;
};
