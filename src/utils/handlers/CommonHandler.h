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
/// @file    CommonHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2024
///
// Collection of functions used in handlers
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/xml/CommonXMLStructure.h>
#include <utils/xml/SUMOSAXHandler.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CommonHandler
 * @brief The XML-Handler for network loading
 *
 * The SAX2-handler responsible for parsing networks and routes to load.
 * This is an extension of the MSRouteHandler as routes and vehicles may also
 *  be loaded from network descriptions.
 */
class CommonHandler {

public:
    /// @brief Constructor
    CommonHandler();

    /// @brief Destructor
    virtual ~CommonHandler();

    /// @brief get flag for check if a element wasn't created
    bool isErrorCreatingElement() const;

protected:
    /// @brief write error and enable error creating element
    bool writeError(const std::string& error);

private:
    /// @brief flag for check if a element wasn't created
    bool myErrorCreatingElement = false;

    /// @brief invalidate copy constructor
    CommonHandler(const CommonHandler& s) = delete;

    /// @brief invalidate assignment operator
    CommonHandler& operator=(const CommonHandler& s) = delete;
};
