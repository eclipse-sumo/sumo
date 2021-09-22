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
/// @file    GNEGeneralHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Sep 2021
///
// General element handler for NETEDIT
/****************************************************************************/
#pragma once
#include <config.h>


// ===========================================================================
// included modules
// ===========================================================================

#include <utils/xml/SUMOSAXHandler.h>
#include <netedit/elements/additional/GNEAdditionalHandler.h>
#include <netedit/elements/demand/GNERouteHandler.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEGeneralHandler : private SUMOSAXHandler {

public:
    /**@brief Constructor
     * @param[in] net GNENet
     * @param[in] file Name of the parsed file
     * @param[in] allowUndoRedo enable or disable undoRedo
     */
    GNEGeneralHandler(GNENet* net, const std::string& file, const bool allowUndoRedo);

    /// @brief Destructor
    ~GNEGeneralHandler();

    /// @brief parse
    bool parse();

protected:
    /// @brief additional handler
    GNEAdditionalHandler myAdditionalHandler;

    /// @brief demand handler
    GNERouteHandler myDemandHandler;

private:
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
    GNEGeneralHandler(const GNEGeneralHandler& s) = delete;

    /// @brief invalidate assignment operator
    GNEGeneralHandler& operator=(const GNEGeneralHandler& s) = delete;
};

/****************************************************************************/

