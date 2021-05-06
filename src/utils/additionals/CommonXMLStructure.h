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
/// @author  Pablo Alvarez Lopez
/// @date    May 2021
///
// Structure for common XML Parsing
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class declarations
// ===========================================================================


// ===========================================================================
// class definitions
// ===========================================================================

class CommonXMLStructure {

public:
    /// @brief XMLNode
    class XMLNode {

    public:
        /// @brief constructor
        XMLNode(XMLNode* parent, const SumoXMLTag tag);

        /// @brief destructor
        ~XMLNode();

        XMLNode* parent;

        /// @brief XML tag
        const SumoXMLTag tag;

        /// @brief attributes
        std::map<const SumoXMLAttr, std::string> attributes;

        /// @brief parameters
        std::map<std::string, std::string> parameters;

        /// @brief child XML nodes
        std::vector<XMLNode*> children;

    private:
        /// @brief invalidate copy constructor
        XMLNode(const XMLNode& s) = delete;

        /// @brief invalidate assignment operator
        XMLNode& operator=(const XMLNode& s) = delete;
    };

    /// @brief Constructor
    CommonXMLStructure();

    /// @brief Destructor
    ~CommonXMLStructure();

    /// @brief open tag
    void openTag(const SumoXMLTag tag);

    /// @brief close tag
    void closeTag();

    /// @brief add attribute into current node
    void addAttribute(const SumoXMLAttr attr, const std::string &value);

    /// @brief add parameter into current node
    void addParameter(const std::string &attr, const std::string &value);

protected:
    /// @brief root
    CommonXMLStructure::XMLNode* myRoot;

    /// @brief last inserted node
    CommonXMLStructure::XMLNode* myLastInsertedNode;

private:
    /// @brief invalidate copy constructor
    CommonXMLStructure(const CommonXMLStructure& s) = delete;

    /// @brief invalidate assignment operator
    CommonXMLStructure& operator=(const CommonXMLStructure& s) = delete;
};
