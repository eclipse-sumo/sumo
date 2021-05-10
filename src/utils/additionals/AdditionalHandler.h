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
/// @file    AdditionalHandler.h
/// @author  Jakob Erdmann
/// @date    Feb 2015
///
// The XML-Handler for network loading
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/xml/SUMOSAXHandler.h>

#include "CommonXMLStructure.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class AdditionalHandler
 * @brief The XML-Handler for network loading
 *
 * The SAX2-handler responsible for parsing networks and routes to load.
 * This is an extension of the MSRouteHandler as routes and vehicles may also
 *  be loaded from network descriptions.
 */
class AdditionalHandler : private SUMOSAXHandler {

public:
    /** @brief Constructor
     * @param[in] file Name of the parsed file
     */
    AdditionalHandler(const std::string& file);

    /// @brief Destructor
    ~AdditionalHandler();

    /// @brief parse
    bool parse();

    /**@brief Builds a induction loop detector (E1)
     * @param[in] id The id of the detector
     * @param[in] lane The lane the detector is placed on
     * @param[in] pos position of the detector on the lane
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] filename The path to the output file.
     * @param[in] vtypes list of vehicle types to be reported
     * @param[in] name E1 detector name
     * @param[in] friendlyPos enable or disable friendly position
     * @param[in] parameters generic parameters
     */
    virtual void buildE1Detector(const CommonXMLStructure::SumoBaseObject* sumoBaseObject,
        const std::string &id, const std::string &laneId, const double position,
        const SUMOTime frequency, const std::string &file, const std::string &vehicleTypes,
        const std::string &name, const bool friendlyPos, const std::map<std::string, std::string> &parameters) = 0;

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

    /// @name parse additional attributes
    /// @{

    /// @brief parse busStop attributes
    void parseBusStopAttributes(const SUMOSAXAttributes& attrs);

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

    /// @brief parse generic parameters
    void parseParameters(const SUMOSAXAttributes& attrs);

    /// @}

    /// @brief parse SumoBaseObject (it's called recursivelly)
    void parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj);

    /// @brief invalidate copy constructor
    AdditionalHandler(const AdditionalHandler& s) = delete;

    /// @brief invalidate assignment operator
    AdditionalHandler& operator=(const AdditionalHandler& s) = delete;
};
