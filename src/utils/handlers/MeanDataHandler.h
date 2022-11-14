/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    MeanDataHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// The XML-Handler for meanMeanData elements loading
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/xml/CommonXMLStructure.h>
#include <utils/xml/SUMOSAXHandler.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MeanDataHandler
 * @brief The XML-Handler for network loading
 *
 * The SAX2-handler responsible for parsing networks and routes to load.
 * This is an extension of the MSRouteHandler as routes and vehicles may also
 *  be loaded from network descriptions.
 */
class MeanDataHandler : private SUMOSAXHandler {

public:
    /** @brief Constructor
     * @param[in] file Name of the parsed file
     */
    MeanDataHandler(const std::string& file);

    /// @brief Destructor
    ~MeanDataHandler();

    /// @brief parse
    bool parse();

    /// @brief parse SumoBaseObject (it's called recursivelly)
    void parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj);

    /**@brief Builds edgeMeanData
     * @param[in] sumoBaseObject sumo base object used for build
     * @param[in] edgeID edge id
     * @param[in] file path to file output
     */
    virtual void buildEdgeMeanData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, 
        const std::string& edgeID, const std::string& file) = 0;

    /**@brief Builds laneMeanData
     * @param[in] sumoBaseObject sumo base object used for build
     * @param[in] laneID lane id
     * @param[in] file path to file output
     */
    virtual void buildLaneMeanData(const CommonXMLStructure::SumoBaseObject* sumoBaseObject, 
        const std::string& laneID, const std::string& file) = 0;

    /// @}

protected:
    /// @brief write error and enable error creating element
    void writeError(const std::string& error);

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

    /// @name parse meanMeanData attributes
    /// @{
    /// @brief parse edgeMeanData attributes
    void parseEdgeMeanData(const SUMOSAXAttributes& attrs);

    /// @brief parse laneMeanData attributes
    void parseLaneMeanData(const SUMOSAXAttributes& attrs);

    /// @}

    /// @brief invalidate copy constructor
    MeanDataHandler(const MeanDataHandler& s) = delete;

    /// @brief invalidate assignment operator
    MeanDataHandler& operator=(const MeanDataHandler& s) = delete;
};
