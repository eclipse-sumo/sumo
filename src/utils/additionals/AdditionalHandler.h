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

#include <utils/common/RGBColor.h>
#include <utils/geom/Position.h>
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
class AdditionalHandler : public SUMOSAXHandler {
public:
    /** @brief Constructor
     * @param[in] file Name of the parsed file
     */
    AdditionalHandler(const std::string& file);

    /// @brief Destructor
    virtual ~AdditionalHandler();

protected:
    /// @name inherited from GenericSAXHandler
    //@{
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
    //@}

protected:
    /// @brief common XML Structure
    CommonXMLStructure myCommonXMLStructure;

    /// @brief parse E1 attributes
    void parseE1Attributes(const SUMOSAXAttributes& attrs);

    /// @brief parse parameters
    void parseParameters(const SUMOSAXAttributes& attrs);

private:
    /// @brief invalidate copy constructor
    AdditionalHandler(const AdditionalHandler& s) = delete;

    /// @brief invalidate assignment operator
    AdditionalHandler& operator=(const AdditionalHandler& s) = delete;
};
