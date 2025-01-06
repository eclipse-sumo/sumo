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

    /// @brief get flag for mark if a element wasn't created
    bool isErrorCreatingElement() const;

protected:
    /// @brief common XML Structure
    CommonXMLStructure myCommonXMLStructure;

    /// @brief flag for mark if a element wasn't created
    bool myErrorCreatingElement = false;

    /// @brief parse generic parameters
    void parseParameters(const SUMOSAXAttributes& attrs);

    /// @name check functions
    /// @{
    /// @brief check parsed parents
    void checkParsedParent(const SumoXMLTag currentTag, const std::vector<SumoXMLTag>& parentTags, bool& ok);

    /// @brief check list of IDs
    bool checkListOfVehicleTypes(const SumoXMLTag tag, const std::string& id, const std::vector<std::string>& vTypeIDs);

    /// @brief check vehicle parents
    bool checkVehicleParents(CommonXMLStructure::SumoBaseObject* obj);

    /// @brief check if the given int value is NOT negative
    bool checkNegative(const SumoXMLTag tag, const std::string& id, const SumoXMLAttr attribute, const int value, const bool canBeZero);

    /// @brief check if the given double value is NOT negative
    bool checkNegative(const SumoXMLTag tag, const std::string& id, const SumoXMLAttr attribute, const double value, const bool canBeZero);

    /// @brief check if the given SUMOTime value is NOT negative
    bool checkNegative(const SumoXMLTag tag, const std::string& id, const SumoXMLAttr attribute, const SUMOTime value, const bool canBeZero);

    /// @brief check if the given filename is valid
    bool checkFileName(const SumoXMLTag tag, const std::string& id, const SumoXMLAttr attribute, const std::string &value);

    /// @brief check if the given additional ID is valid
    bool checkValidAdditionalID(const SumoXMLTag tag, const std::string& value);

    /// @brief check if the given detector ID is valid
    bool checkValidDetectorID(const SumoXMLTag tag, const std::string& value);

    /// @brief check if the given demand elmement ID is valid
    bool checkValidDemandElementID(const SumoXMLTag tag, const std::string& value);

    /// @}

    /// @brief write error and enable error creating element
    bool writeError(const std::string& error);

    /// @brief write error "invalid position"
    bool writeErrorInvalidPosition(const SumoXMLTag tag, const std::string& id);

    /// @brief write error "duplicated additional"
    bool writeErrorDuplicated(const SumoXMLTag tag, const std::string& id, const SumoXMLTag checkedTag);

    /// @brief write error "invalid list of lanes"
    bool writeErrorInvalidLanes(const SumoXMLTag tag, const std::string& id);

    /// @brief write error "invalid distribution"
    bool writeErrorInvalidDistribution(const SumoXMLTag tag, const std::string& id);

    /// @brief write error "invalid parent element" giving ids of current and parent element
    bool writeErrorInvalidParent(const SumoXMLTag tag, const std::string& id, const SumoXMLTag parentTag, const std::string& parentID);

    /// @brief write error "invalid parent element" giving only the id of parent element
    bool writeErrorInvalidParent(const SumoXMLTag tag, const SumoXMLTag parentTag, const std::string& parentID);

    /// @brief write error "invalid parent element" without giving IDs
    bool writeErrorInvalidParent(const SumoXMLTag tag, const SumoXMLTag parentTag);



private:
    /// @brief invalidate copy constructor
    CommonHandler(const CommonHandler& s) = delete;

    /// @brief invalidate assignment operator
    CommonHandler& operator=(const CommonHandler& s) = delete;
};
