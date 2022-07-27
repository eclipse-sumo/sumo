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


// ===========================================================================
// class definitions
// ===========================================================================

class ConfigHandler {

public:
    /// @brief Constructor
    ConfigHandler();

    /// @brief Destructor
    virtual ~ConfigHandler();

    /// @brief begin parse attributes
    bool beginParseAttributes(SumoXMLTag tag, const SUMOSAXAttributes& attrs);

    /// @brief end parse attributes
    void endParseAttributes();

private:
    /// @brief common XML Structure
    CommonXMLStructure myCommonXMLStructure;

    /// @brief parse config objects
    void parseConfigObject(CommonXMLStructure::SumoBaseObject* obj);

    /// @name parse SUMOConfig attributes
    /// @{
    /// @brief parse configuration attribute
    void parseConfiguration(const SUMOSAXAttributes& attrs);

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
