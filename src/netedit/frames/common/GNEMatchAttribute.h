/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEMatchAttribute.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2020
///
// The Widget for modifying selections of network-elements
// (some elements adapted from GUIDialog_GLChosenEditor)
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEGroupBoxModule.h"

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAttributeProperties;
class GNESelectorFrame;
class GNETagProperties;
class GNETagPropertiesDatabase;
class GNEComboBoxAttrProperty;
class GNEComboBoxTagProperty;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMatchAttribute : public GNEGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNEMatchAttribute)

public:
    /// @brief constructor
    GNEMatchAttribute(GNESelectorFrame* selectorFrameParent);

    /// @brief destructor
    ~GNEMatchAttribute();

    /// @brief enable match attributes
    void enableMatchAttribute();

    /// @brief disable match attributes
    void disableMatchAttribute();

    /// @brief show match attributes
    void showMatchAttribute();

    /// @brief hide match attributes
    void hideMatchAttribute();

    /// @brief refresh match attribute
    void refreshMatchAttribute();

    /// @name FOX-callbacks
    /// @{

    /// @brief Called when the user selects a tag in the match box
    long onCmdTagSelected(FXObject* obj, FXSelector, void*);

    /// @brief Called when the user selects an attribute in the match box
    long onCmdAttributeSelected(FXObject*, FXSelector, void*);

    /// @brief Called when the user toogle the only common checkbox
    long onCmdToogleOnlyCommon(FXObject*, FXSelector, void*);

    /// @brief Called when the user enters a new selection expression
    long onCmdProcessString(FXObject*, FXSelector, void*);

    /// @brief Called when the user clicks the help button
    long onCmdHelp(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEMatchAttribute)

    /**@brief return ACs of the given type with matching attrs
     * @param[in] compOp One of {<,>,=} for matching against val or '@' for matching against expr
     */
    std::vector<GNEAttributeCarrier*> getMatches(const char compOp, const double val, const std::string& expr);

    /**@brief return GenericDatas of the given type with matching attrs
     * @param[in] genericDatas list of filter generic datas
     * @param[in] attr XML Attribute used to filter
     * @param[in] compOp One of {<,>,=} for matching against val or '@' for matching against expr
     */
    std::vector<GNEAttributeCarrier*> getGenericMatches(const std::vector<GNEGenericData*>& genericDatas, const std::string& attr, const char compOp, const double val, const std::string& expr);


private:
    /// @brief container with current edited properties
    class CurrentEditedProperties {

    public:
        /// @brief constructor
        CurrentEditedProperties(const GNEMatchAttribute* matchAttributeParent);

        /// @brief destructor
        ~CurrentEditedProperties();

        /// @brief get special tag <all>
        const GNETagProperties* getTagPropertiesAll() const;

        /// @brief get attr properties no common
        const GNEAttributeProperties* getAttributePropertiesNoCommon() const;

        /// @brief get tag property (depending of supermode)
        const GNETagProperties* getTagProperties() const;

        /// @brief get attribute property (depending of supermode)
        const GNEAttributeProperties* getAttributeProperties() const;

        /// @brief get match value (depending of supermode)
        const std::string& getMatchValue() const;

        /// @brief set tag property (depending of supermode)
        void setTagProperties(const GNETagProperties* tagProperty);

        /// @brief set attribute property (depending of supermode)
        void setAttributeProperties(const GNEAttributeProperties* attrProperty);

        /// @brief set match value (depending of supermode)
        void setMatchValue(const std::string value);

    private:
        /// @brief pointer to match attribute parent
        const GNEMatchAttribute* myMatchAttributeParent;

        /// @brief current network tag properties
        std::vector<const GNETagProperties*> myNetworkTagProperties;

        /// @brief current network attribute properties
        const GNEAttributeProperties* myNetworkAttributeProperties;

        /// @brief current network match value
        std::string myNetworkMatchValue;

        /// @brief current demand tag properties
        std::vector<const GNETagProperties*> myDemandTagProperties;

        /// @brief current demand attribute properties
        const GNEAttributeProperties* myDemandAttributeProperties;

        /// @brief current demand match value
        std::string myDemandMatchValue;

        /// @brief current data tag properties
        std::vector<const GNETagProperties*> myDataTagProperties;

        /// @brief current data attribute properties
        const GNEAttributeProperties* myDataAttributeProperties;

        /// @brief current data match value
        std::string myDataMatchValue;

        /// @brief tag properties <all>
        GNETagProperties* myTagPropertiesAllAttributes = nullptr;

        /// @brief attribute properties no common
        const GNEAttributeProperties* myAttributePropertiesNoCommon = nullptr;

        /// @brief default constructor
        CurrentEditedProperties() = delete;

        /// @brief Invalidated copy constructor.
        CurrentEditedProperties(const CurrentEditedProperties&) = delete;

        /// @brief Invalidated assignment operator
        CurrentEditedProperties& operator=(const CurrentEditedProperties& src) = delete;
    };

    /// @brief pointer to selector frame parent
    GNESelectorFrame* mySelectorFrameParent = nullptr;

    /// @brief vector with tag property comboBoxes
    std::vector <GNEComboBoxTagProperty*> myTagComboBoxVector;

    /// @brief checkbox for enable/disable show only common attributes
    FXCheckButton* myShowOnlyCommonAttributes = nullptr;

    /// @brief attribute property comboBox
    GNEComboBoxAttrProperty* myAttributeComboBox = nullptr;

    /// @brief string of the match
    FXTextField* myMatchString = nullptr;

    /// @brief match string button
    FXButton* myMatchStringButton = nullptr;

    /// @brief current edited properties
    CurrentEditedProperties* myCurrentEditedProperties;

    /// @brief Invalidated copy constructor.
    GNEMatchAttribute(const GNEMatchAttribute&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMatchAttribute& operator=(const GNEMatchAttribute&) = delete;
};
