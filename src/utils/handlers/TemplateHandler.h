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
/// @file    TemplateHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2022
///
// The XML-Handler for templates
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/xml/CommonXMLStructure.h>
#include <utils/xml/SUMOSAXHandler.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TemplateHandler
 * @brief The XML-Handler for network loading
 *
 * The SAX2-handler responsible for parsing networks and routes to load.
 * This is an extension of the MSRouteHandler as routes and vehicles may also
 *  be loaded from network descriptions.
 */
class TemplateHandler : private SUMOSAXHandler {

public:
    /** @brief Constructor
     * @param[in] file Name of the parsed file
     */
    TemplateHandler(OptionsCont &options, const std::string& file);

    /// @brief Destructor
    ~TemplateHandler();

    /// @brief parse
    bool parse();

private:
    /// @brief struct used for loading options
    struct Option {
        /// @brief option name
        std::string name;

        /// @brief option value
        std::string value;

        /// @brief option synonymes
        std::string synonymes;

        /// @brief option type
        std::string type;

        /// @brief option help
        std::string help;
    };

    /// @brief list of loaded options
    std::vector<Option> myLoadedOptions;

    /// @brief option containers
    OptionsCont &myOptions;

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

    /// @brief invalidate copy constructor
    TemplateHandler(const TemplateHandler& s) = delete;

    /// @brief invalidate assignment operator
    TemplateHandler& operator=(const TemplateHandler& s) = delete;
};
