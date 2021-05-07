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
// class definitions
// ===========================================================================

class CommonXMLStructure {

public:
    /// @brief SumoBaseObject
    class SumoBaseObject {

    public:
        /// @brief constructor
        SumoBaseObject(SumoBaseObject* sumoBaseObjectParent, const SumoXMLTag tag);

        /// @brief destructor
        ~SumoBaseObject();

        /// @name get functions
        /// @{
        /// @brief get XML myTag
        const SumoXMLTag getTag() const;

        /// @brief get pointer to mySumoBaseObjectParent SumoBaseObject (if is null, then is the root)
        SumoBaseObject* getParentSumoBaseObject() const;

        /// @brief get string attribute
        const std::string &getStringAttribute(const SumoXMLAttr attr) const;

        /// @brief get int attribute
        int getIntAttribute(const SumoXMLAttr attr) const;

        /// @brief get double attribute
        double getDoubleAttribute(const SumoXMLAttr attr) const;

        /// @brief get bool attribute
        bool getBoolAttribute(const SumoXMLAttr attr) const;

        /// @brief get SUMOTime attribute
        SUMOTime getSUMOTimeAttribute(const SumoXMLAttr attr) const;

        /// @brief get parameters
        const std::map<std::string, std::string> &getParameters() const;

        /// @brief get SumoBaseObject children
        const std::vector<SumoBaseObject*> &getSumoBaseObjectChildren() const;
        /// @}

        /// @name add functions
        /// @{
        /// @brief add string attribute into current SumoBaseObject node
        void addStringAttribute(const SumoXMLAttr attr, const std::string &value);

        /// @brief add int attribute into current SumoBaseObject node
        void addIntAttribute(const SumoXMLAttr attr, const int value);

        /// @brief add double attribute into current SumoBaseObject node
        void addDoubleAttribute(const SumoXMLAttr attr, const double value);

        /// @brief add bool attribute into current SumoBaseObject node
        void addBoolAttribute(const SumoXMLAttr attr, const bool value);

        /// @brief add SUMOTime attribute into current SumoBaseObject node
        void addSUMOTimeAttribute(const SumoXMLAttr attr, const SUMOTime value);

        /// @brief add parameter into current SumoBaseObject node
        void addParameter(const std::string &key, const std::string &value);
        /// @}

    protected:
        /// @brief pointer to SumoBaseObject parent (If is null, then is the root)
        SumoBaseObject* mySumoBaseObjectParent;

        /// @brief XML myTag
        const SumoXMLTag myTag;

        /// @brief attributes string
        std::map<const SumoXMLAttr, std::string> myStringAttributes;

        /// @brief attributes int
        std::map<const SumoXMLAttr, int> myIntAttributes;

        /// @brief attributes double
        std::map<const SumoXMLAttr, double> myDoubleAttributes;

        /// @brief attributes bool
        std::map<const SumoXMLAttr, bool> myBoolAttributes;

        /// @brief attributes SUMOTime
        std::map<const SumoXMLAttr, SUMOTime> mySUMOTimeAttributes;

        /// @brief myParameters
        std::map<std::string, std::string> myParameters;

        /// @brief SumoBaseObject children
        std::vector<SumoBaseObject*> mySumoBaseObjectChildren;

        /// @brief check duplicated attribute (probably we can delete this function)
        bool checkDuplicatedAttribute(const SumoXMLAttr attr) const;

        /// @brief add SumoBaseObject child
        void addSumoBaseObjectChild(SumoBaseObject* sumoBaseObject);

        /// @brief remove SumoBaseObject child
        void removeSumoBaseObjectChild(SumoBaseObject* sumoBaseObject);

    private:
        /// @brief invalidate copy constructor
        SumoBaseObject(const SumoBaseObject& s) = delete;

        /// @brief invalidate assignment operator
        SumoBaseObject& operator=(const SumoBaseObject& s) = delete;
    };

    /// @brief Constructor
    CommonXMLStructure();

    /// @brief Destructor
    ~CommonXMLStructure();

    /// @brief open myTag
    void openTag(const SumoXMLTag myTag);

    /// @brief close myTag
    void closeTag();

    /// @brief get SumoBaseObject root
    CommonXMLStructure::SumoBaseObject* getSumoBaseObjectRoot() const;

    /// @brief get last inserted SumoBaseObject
    CommonXMLStructure::SumoBaseObject* getLastInsertedSumoBaseObject() const;

protected:
    /// @brief SumoBaseObject root
    CommonXMLStructure::SumoBaseObject* mySumoBaseObjectRoot;

    /// @brief last inserted SumoBaseObject
    CommonXMLStructure::SumoBaseObject* myLastInsertedSumoBaseObject;

private:
    /// @brief invalidate copy constructor
    CommonXMLStructure(const CommonXMLStructure& s) = delete;

    /// @brief invalidate assignment operator
    CommonXMLStructure& operator=(const CommonXMLStructure& s) = delete;
};
