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
/// @file    GeneralHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2021
///
// General element handler
/****************************************************************************/
#pragma once
#include <config.h>


// ===========================================================================
// included modules
// ===========================================================================
#include <utils/xml/SUMOSAXHandler.h>


// ===========================================================================
// class definitions
// ===========================================================================

class GeneralHandler : public SUMOSAXHandler {

public:
    /**@brief Constructor
     * @param[in] file Name of the parsed file
     */
    GeneralHandler(const std::string& file);

    /// @brief Destructor
    ~GeneralHandler();

    /// @brief parse
    bool parse();

private:
    /// @brief start element
    virtual void beginTag(SumoXMLTag tag, const SUMOSAXAttributes& attrs) = 0;

    /// @brief end element
    virtual void endTag() = 0;

    /// @name inherited from SUMOSAXHandler
    /// @{
    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     * @todo Refactor/describe
     */
    void myStartElement(int element, const SUMOSAXAttributes& attrs);

    /** @brief Called when a closing tag occurs
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     * @todo Refactor/describe
     */
    void myEndElement(int element);
    /// @}

    /// @brief invalidate copy constructor
    GeneralHandler(const GeneralHandler& s) = delete;

    /// @brief invalidate assignment operator
    GeneralHandler& operator=(const GeneralHandler& s) = delete;
};

/****************************************************************************/

