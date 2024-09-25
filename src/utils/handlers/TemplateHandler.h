/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
    static void parseTemplate(OptionsCont& options, const std::string& templateString);

private:
    /// @brief Constructor
    TemplateHandler(OptionsCont& options);

    /// @brief destructor
    ~TemplateHandler();

    /// @name Handlers for the SAX DocumentHandler interface
    /// @{

    /** @brief Called on the occurrence of the beginning of a tag
     *
     * Sets the name of the last item
     */
    void startElement(const XMLCh* const name, XERCES_CPP_NAMESPACE::AttributeList& attributes);

    /// @brief add option
    bool addOption(std::string value, const std::string& synonymes, const std::string& type,
                   const std::string& help, bool required, bool positional, const std::string& listSep) const;

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
     * The warning is reported to the warning-instance of MsgHandler
     */
    void warning(const XERCES_CPP_NAMESPACE::SAXParseException& exception);

    /** @brief Called on an XML-error
     *
     * The warning is reported to the error-instance of MsgHandler
     */
    void error(const XERCES_CPP_NAMESPACE::SAXParseException& exception);

    /** @brief Called on an XML-fatal error
     *
     * The warning is reported to the error-instance of MsgHandler
     */
    void fatalError(const XERCES_CPP_NAMESPACE::SAXParseException& exception);

    /// @}

    /// @brief The information whether an error occurred
    bool myError;

    /// @brief The nesting level of parsed elements
    int myLevel;

    /// @brief The options to fill
    OptionsCont& myOptions;

    /// @brief The name of the current option
    std::string myOptionName;

    /// @brief current subtopic
    std::string mySubTopic;

    /// @brief invalid int in string format
    static const std::string INVALID_INT_STR;

    /// @brief invalid double in string format
    static const std::string INVALID_DOUBLE_STR;

    /// @brief invalid copy constructor
    TemplateHandler(const TemplateHandler& s) = delete;

    /// @brief invalid assignment operator
    TemplateHandler& operator=(const TemplateHandler& s) = delete;
};
