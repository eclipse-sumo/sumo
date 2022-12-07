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
// A SAX-Handler for loading templates
/****************************************************************************/
#pragma once
#include <config.h>

#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================

class OptionsCont;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TemplateHandler
 * @brief A SAX-Handler for loading options
 */
class TemplateHandler : public XERCES_CPP_NAMESPACE::HandlerBase {

public:
    /// @brief run parser
    static void parseTemplate(OptionsCont& options, const std::string &path);

private:
    /// @brief Constructor
    TemplateHandler(OptionsCont& options);

    /// @brief destructor
    ~TemplateHandler();

    /// @name Handlers for the SAX DocumentHandler interface
    /// @{

    /** @brief Called on the occurence of the beginning of a tag
     *
     * Sets the name of the last item
     */
    void startElement(const XMLCh* const name, XERCES_CPP_NAMESPACE::AttributeList& attributes);

    /** @brief Called on the end of an element
     *
     * Resets the element name
     */
    void endElement(const XMLCh* const name);

    /// @}

    /// @name Handlers for the SAX ErrorHandler interface
    /// @{

    /** @brief Called on an XML-warning
     *
     * The warning is reported to the the warning-instance of MsgHandler
     */
    void warning(const XERCES_CPP_NAMESPACE::SAXParseException& exception);

    /** @brief Called on an XML-error
     *
     * The warning is reported to the the error-instance of MsgHandler
     */
    void error(const XERCES_CPP_NAMESPACE::SAXParseException& exception);

    /** @brief Called on an XML-fatal error
     *
     * The warning is reported to the the error-instance of MsgHandler
     */
    void fatalError(const XERCES_CPP_NAMESPACE::SAXParseException& exception);

    /// @}

    /// @brief The information whether an error occurred
    bool myError;

    /// @brief The options to fill
    OptionsCont& myOptions;

    /// @brief The name of the currently parsed option
    std::string myTag;

    /** @brief Tries to set the named option to the given value
     *
     * Also evaluates whether it is a boolean or a filename option and
     *  does the relevant checks / modifications.
     *
     * @param[in] key The name of the option to set
     * @param[in] value The new value for the option
     */
    void setValue(const std::string& key, const std::string& value);

    /** @brief Tries to set the named option to the given value
     *
     * Checks the item whether it was default before setting it.
     * Returns the information whether the item was set before (was not a default value)
     *
     * @param[in] name The name of the option to set
     * @param[in] value The new value for the option
     * @return Whether the option could be set
     */
    bool addOption(const std::string& name, const std::string& value) const;

    /// @brief invalid copy constructor
    TemplateHandler(const TemplateHandler& s) = delete;

    /// @brief invalid assignment operator
    TemplateHandler& operator=(const TemplateHandler& s) = delete;
};
