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
/// @file    ConfigHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2022
///
// The XML-Handler for SUMOConfig loading
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/xml/CommonXMLStructure.h>
#include <utils/xml/SUMOSAXHandler.h>

// ===========================================================================
// class definitions
// ===========================================================================

/// @brief handler for SUMOConfigs
class ConfigHandler : private SUMOSAXHandler {

public:
    /// @brief Constructor
    ConfigHandler(const std::string& file);

    /// @brief Destructor
    virtual ~ConfigHandler();

    /// @brief parse
    bool parse();

    /// @brief parse SumoBaseObject (it's called recursivelly)
    void parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj);

    /// @name load functions
    /// @{
    /**@brief Load net file
     * @param[in] sumoBaseObject sumo base object used for build
     * @param[in] file net file
     */
    virtual void loadNetFile(const std::string& file) = 0;

    /**@brief Load additional files
     * @param[in] sumoBaseObject sumo base object used for build
     * @param[in] files additional files
     */
    virtual void loadAdditionalFiles(const std::string& files) = 0;

    /**@brief Load route files
     * @param[in] sumoBaseObject sumo base object used for build
     * @param[in] files route files
     */
    virtual void loadRouteFiles(const std::string& files) = 0;

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

    /// @name parse SUMOConfig attributes
    /// @{

    /// @brief parse parse netFile attribute
    void parseNetFile(const SUMOSAXAttributes& attrs);

    /// @brief parse additional files attribute
    void parseAdditionalFiles(const SUMOSAXAttributes& attrs);

    /// @brief parse route files attribute
    void parseRouteFiles(const SUMOSAXAttributes& attrs);

    /// @}

    /// @brief invalidate copy constructor
    ConfigHandler(const ConfigHandler& s) = delete;

    /// @brief invalidate assignment operator
    ConfigHandler& operator=(const ConfigHandler& s) = delete;
};
