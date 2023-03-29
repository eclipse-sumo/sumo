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
/// @file    OptionHandler.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2023
///
// The XML-Handler for loading options
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/xml/CommonXMLStructure.h>


// ===========================================================================
// class definitions
// ===========================================================================

class OptionHandler {

public:
    /// @brief Constructor
    OptionHandler();

    /// @brief Destructor
    virtual ~OptionHandler();

    /// @brief begin parse attributes
    bool beginParseAttributes(SumoXMLTag tag, const SUMOSAXAttributes& attrs);

    /// @brief end parse attributes
    void endParseAttributes();

    /// @brief parse SumoBaseObject (it's called recursivelly)
    void parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj);

    /// @brief get flag for check if a element wasn't created
    bool isErrorCreatingElement() const;

protected:
    /// @brief write error and enable error creating element
    void writeError(const std::string& error);

private:
    /// @brief common XML Structure
    CommonXMLStructure myCommonXMLStructure;

    /// @brief flag for check if a element wasn't created
    bool myErrorCreatingElement = false;

    /// @brief invalidate copy constructor
    OptionHandler(const OptionHandler& s) = delete;

    /// @brief invalidate assignment operator
    OptionHandler& operator=(const OptionHandler& s) = delete;
};
