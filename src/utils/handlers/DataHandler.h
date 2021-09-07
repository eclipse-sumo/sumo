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
/// @file    DataHandler.h
/// @author  Jakob Erdmann
/// @date    Jun 2021
///
// The XML-Handler for data elements loading
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/xml/CommonXMLStructure.h>
#include <utils/xml/SUMOSAXHandler.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class DataHandler
 * @brief The XML-Handler for network loading
 *
 * The SAX2-handler responsible for parsing networks and routes to load.
 * This is an extension of the MSRouteHandler as routes and vehicles may also
 *  be loaded from network descriptions.
 */
class DataHandler : private SUMOSAXHandler {

public:
    /** @brief Constructor
     * @param[in] file Name of the parsed file
     */
    DataHandler(const std::string& file);

    /// @brief Destructor
    ~DataHandler();

    /// @brief parse
    bool parse();

    /// @brief parse SumoBaseObject (it's called recursivelly)
    void parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj);

    /// @name build functions
    /// @{
    /**@brief Builds DataInterval
     * @param[in] sumoBaseObject sumo base object used for build
     * @param[in] dataSetID interval's dataSet
     * @param[in] begin interval begin
     * @param[in] end interval end
     */
    virtual void buildDataInterval(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& dataSetID,
                                   const double begin, const double end) = 0;

    /**@brief Builds edgeData
     * @param[in] sumoBaseObject sumo base object used for build
     * @param[in] edgeID edge ID
     * @param[in] parameters parameters map
     */
    virtual void buildEdgeData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& edgeID,
                               const std::map<std::string, std::string>& parameters) = 0;

    /**@brief Builds edgeRelationData
     * @param[in] sumoBaseObject sumo base object used for build
     * @param[in] fromEdge edge from
     * @param[in] toEdge edge to
     * @param[in] parameters parameters map
     */
    virtual void buildEdgeRelationData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& fromEdgeID,
                                       const std::string& toEdgeID, const std::map<std::string, std::string>& parameters) = 0;

    /**@brief Builds TAZRelationData
     * @param[in] sumoBaseObject sumo base object used for build
     * @param[in] fromTAZ TAZ from
     * @param[in] toTAZ TAZ to
     * @param[in] parameters parameters map
     */
    virtual void buildTAZRelationData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, const std::string& fromTAZID,
                                      const std::string& toTAZID, const std::map<std::string, std::string>& parameters) = 0;
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

    /// @name parse data attributes
    /// @{
    /// @brief parse interval attributes
    void parseInterval(const SUMOSAXAttributes& attrs);

    /// @brief parse edgeData attributes
    void parseEdgeData(const SUMOSAXAttributes& attrs);

    /// @brief parse edgeRelationData attributes
    void parseEdgeRelationData(const SUMOSAXAttributes& attrs);

    /// @brief parse TAZRelationData attributes
    void parseTAZRelationData(const SUMOSAXAttributes& attrs);
    /// @}

    /// @brief check parents
    void checkParent(const SumoXMLTag currentTag, const SumoXMLTag parentTag, bool& ok) const;

    /// @brief invalidate copy constructor
    DataHandler(const DataHandler& s) = delete;

    /// @brief invalidate assignment operator
    DataHandler& operator=(const DataHandler& s) = delete;
};
