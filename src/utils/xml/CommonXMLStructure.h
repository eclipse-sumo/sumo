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
/// @file    CommonXMLStructure.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2021
///
// Structure for common XML Parsing
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/common/SUMOTime.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/PositionVector.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
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
        SumoBaseObject(SumoBaseObject* sumoBaseObjectParent);

        /// @brief destructor
        ~SumoBaseObject();

        /// @brief set SumoBaseObject tag
        void setTag(const SumoXMLTag tag);

        /// @name get functions
        /// @{
        /// @brief get XML myTag
        SumoXMLTag getTag() const;

        /// @brief get pointer to mySumoBaseObjectParent SumoBaseObject (if is null, then is the root)
        SumoBaseObject* getParentSumoBaseObject() const;

        /// @brief get string attribute
        const std::string& getStringAttribute(const SumoXMLAttr attr) const;

        /// @brief get int attribute
        int getIntAttribute(const SumoXMLAttr attr) const;

        /// @brief get double attribute
        double getDoubleAttribute(const SumoXMLAttr attr) const;

        /// @brief get bool attribute
        bool getBoolAttribute(const SumoXMLAttr attr) const;

        /// @brief get Position attribute
        const Position& getPositionAttribute(const SumoXMLAttr attr) const;

        /// @brief get time attribute
        SUMOTime getTimeAttribute(const SumoXMLAttr attr) const;

        /// @brief get color attribute
        const RGBColor& getColorAttribute(const SumoXMLAttr attr) const;

        /// @brief get string list attribute
        const std::vector<std::string>& getStringListAttribute(const SumoXMLAttr attr) const;

        /// @brief get PositionVector attribute
        const PositionVector& getPositionVectorAttribute(const SumoXMLAttr attr) const;

        /// @brief vehicle class
        SUMOVehicleClass getVClass() const;

        /// @brief get current vType
        const SUMOVTypeParameter &getVehicleTypeParameter() const;

        /// @brief get vehicle parameters
        const SUMOVehicleParameter &getVehicleParameter() const;

        /// @brief get stop parameters
        const SUMOVehicleParameter::Stop &getStopParameter() const;

        /// @brief get parameters
        const std::map<std::string, std::string>& getParameters() const;

        /// @brief get SumoBaseObject children
        const std::vector<SumoBaseObject*>& getSumoBaseObjectChildren() const;
        /// @}

        /// @brief has function
        /// @{

        /// @brief check if current SumoBaseObject has the given string attribute
        bool hasStringAttribute(const SumoXMLAttr attr) const;

        /// @brief check if current SumoBaseObject has the given int attribute
        bool hasIntAttribute(const SumoXMLAttr attr) const;

        /// @brief check if current SumoBaseObject has the given double attribute
        bool hasDoubleAttribute(const SumoXMLAttr attr) const;

        /// @brief check if current SumoBaseObject has the given bool attribute
        bool hasBoolAttribute(const SumoXMLAttr attr) const;

        /// @brief check if current SumoBaseObject has the given bool attribute
        bool hasPositionAttribute(const SumoXMLAttr attr) const;

        /// @brief check if current SumoBaseObject has the given time attribute
        bool hasTimeAttribute(const SumoXMLAttr attr) const;

        /// @brief check if current SumoBaseObject has the given color attribute
        bool hasColorAttribute(const SumoXMLAttr attr) const;

        /// @brief check if current SumoBaseObject has the given string list attribute
        bool hasStringListAttribute(const SumoXMLAttr attr) const;

        /// @brief check if current SumoBaseObject has the given positionVector attribute
        bool hasPositionVectorAttribute(const SumoXMLAttr attr) const;

        /// @}

        /// @name add functions
        /// @{
        /// @brief add string attribute into current SumoBaseObject node
        void addStringAttribute(const SumoXMLAttr attr, const std::string& value);

        /// @brief add int attribute into current SumoBaseObject node
        void addIntAttribute(const SumoXMLAttr attr, const int value);

        /// @brief add double attribute into current SumoBaseObject node
        void addDoubleAttribute(const SumoXMLAttr attr, const double value);

        /// @brief add bool attribute into current SumoBaseObject node
        void addBoolAttribute(const SumoXMLAttr attr, const bool value);

        /// @brief add Position attribute into current SumoBaseObject node
        void addPositionAttribute(const SumoXMLAttr attr, const Position& value);

        /// @brief add time attribute into current SumoBaseObject node
        void addTimeAttribute(const SumoXMLAttr attr, const SUMOTime value);

        /// @brief add color attribute into current SumoBaseObject node
        void addColorAttribute(const SumoXMLAttr attr, const RGBColor& value);

        /// @brief add string list attribute into current SumoBaseObject node
        void addStringListAttribute(const SumoXMLAttr attr, const std::vector<std::string>& value);

        /// @brief add PositionVector attribute into current SumoBaseObject node
        void addPositionVectorAttribute(const SumoXMLAttr attr, const PositionVector& value);

        /// @brief set vehicle class
        void setVClass(SUMOVehicleClass vClass);

        /// @brief set vehicle type parameters
        void setVehicleTypeParameter(const SUMOVTypeParameter* vehicleTypeParameter);

        /// @brief set vehicle parameters
        void setVehicleParameter(const SUMOVehicleParameter* vehicleParameter);

        /// @brief add stop parameters
        void setStopParameter(const SUMOVehicleParameter::Stop &stopParameter);

        /// @brief add parameter into current SumoBaseObject node
        void addParameter(const std::string& key, const std::string& value);
        /// @}

    protected:
        /// @brief pointer to SumoBaseObject parent (If is null, then is the root)
        SumoBaseObject* mySumoBaseObjectParent;

        /// @brief XML myTag
        SumoXMLTag myTag;

        /// @brief string attributes
        std::map<const SumoXMLAttr, std::string> myStringAttributes;

        /// @brief int attributes
        std::map<const SumoXMLAttr, int> myIntAttributes;

        /// @brief double attributes
        std::map<const SumoXMLAttr, double> myDoubleAttributes;

        /// @brief bool attributes
        std::map<const SumoXMLAttr, bool> myBoolAttributes;

        /// @brief Position attributes
        std::map<const SumoXMLAttr, Position> myPositionAttributes;

        /// @brief SUMOTime attributes
        std::map<const SumoXMLAttr, SUMOTime> myTimeAttributes;

        /// @brief RGBColor attributes
        std::map<const SumoXMLAttr, RGBColor> myColorAttributes;

        /// @brief stringList attributes
        std::map<const SumoXMLAttr, std::vector<std::string> > myStringListAttributes;

        /// @brief PositionVector attributes
        std::map<const SumoXMLAttr, PositionVector> myPositionVectorAttributes;

        /// @brief myParameters
        std::map<std::string, std::string> myParameters;

        /// @brief SumoBaseObject children
        std::vector<SumoBaseObject*> mySumoBaseObjectChildren;

        /// @brief vehicle class
        SUMOVehicleClass myVClass;

        /// @brief vehicle type parameter
        SUMOVTypeParameter myVehicleTypeParameter;

        /// @brief vehicle parameter
        SUMOVehicleParameter myVehicleParameter;

        /// @brief stop parameter
        SUMOVehicleParameter::Stop myStopParameter;

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

    /// @brief open SUMOBaseOBject
    void openSUMOBaseOBject();

    /// @brief close myTag
    void closeSUMOBaseOBject();

    /// @brief get SumoBaseObject root
    CommonXMLStructure::SumoBaseObject* getSumoBaseObjectRoot() const;

    /// @brief get current editedSumoBaseObject
    CommonXMLStructure::SumoBaseObject* getCurrentSumoBaseObject() const;

protected:
    /// @brief SumoBaseObject root
    CommonXMLStructure::SumoBaseObject* mySumoBaseObjectRoot;

    /// @brief last inserted SumoBaseObject
    CommonXMLStructure::SumoBaseObject* myCurrentSumoBaseObject;

private:
    /// @brief invalidate copy constructor
    CommonXMLStructure(const CommonXMLStructure& s) = delete;

    /// @brief invalidate assignment operator
    CommonXMLStructure& operator=(const CommonXMLStructure& s) = delete;
};
