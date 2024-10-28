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
/// @file    GNEFrameAttributeModules.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// Auxiliar class for GNEFrame Modules (only for attributes edition)
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/GNEViewNetHelper.h>
#include <utils/common/Parameterised.h>
#include <utils/foxtools/MFXGroupBoxModule.h>
#include <utils/foxtools/MFXToggleButtonTooltip.h>
#include <utils/foxtools/MFXTextFieldTooltip.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/xml/CommonXMLStructure.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;
class GNEViewParent;
class GNEFlowEditor;
class GNEInspectorFrame;
class GNETypeFrame;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFrameAttributeModules {

public:
    // ===========================================================================
    // class declaration
    // ===========================================================================

    class AttributesEditor;
    class AttributesEditorFlow;

    // ===========================================================================
    // class AttributesEditorRow
    // ===========================================================================

    class AttributesEditorRow : protected FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributeModules::AttributesEditorRow)

    public:
        /// @brief constructor
        AttributesEditorRow(AttributesEditor* attributeEditorParent, const GNEAttributeProperties& ACAttr, const std::string& value,
                            const bool attributeEnabled, const bool computed, GNEAttributeCarrier* ACParent);

        /// @brief destroy GNEAttributesCreatorRow (but don't delete)
        void destroy();

        /// @brief refresh current row
        void refreshAttributesEditorRow(const std::string& value, const bool forceRefreshAttribute, const bool attributeEnabled, const bool computed, GNEAttributeCarrier* ACParent);

        /// @brief check if current attribute of TextField/ComboBox is valid
        bool isAttributesEditorRowValid() const;

        /// @name FOX-callbacks
        /// @{

        /// @brief try to set new attribute value
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief called when user press a check button
        long onCmdSelectCheckButton(FXObject*, FXSelector, void*);

        /// @brief open model dialog for edit color
        long onCmdOpenColorDialog(FXObject*, FXSelector, void*);

        /// @brief open model dialog for edit allow
        long onCmdOpenAllowDialog(FXObject*, FXSelector, void*);

        /// @brief inspect parent
        long onCmdInspectParent(FXObject*, FXSelector, void*);

        /// @brief inspect vType/VTypeDistribution parent
        long onCmdMoveElementLaneUp(FXObject*, FXSelector, void*);

        /// @brief inspect vType/VTypeDistribution parent
        long onCmdMoveElementLaneDown(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief default constructor
        AttributesEditorRow();

        /// @brief removed invalid spaces of Positions and shapes
        std::string stripWhitespaceAfterComma(const std::string& stringValue);

        /// @brief check junction merging
        bool mergeJunction(SumoXMLAttr attr, const std::vector<GNEAttributeCarrier*>& inspectedACs, const std::string& newVal) const;

    private:
        /// @brief pointer to AttributesEditor parent
        AttributesEditor* myAttributesEditorParent;

        /// @brief current AC Attribute
        const GNEAttributeProperties myACAttr;

        /// @brief pointer to attribute label
        MFXLabelTooltip* myAttributeLabel = nullptr;

        /// @brief pointer to attribute  menu check
        FXCheckButton* myAttributeCheckButton = nullptr;

        /// @brief pointer to attributeAllowButton
        MFXButtonTooltip* myAttributeButton = nullptr;

        /// @brief textField to modify the value of string attributes
        MFXTextFieldTooltip* myValueTextField = nullptr;

        /// @brief pointer to combo box choices
        MFXComboBoxIcon* myValueComboBox = nullptr;

        /// @brief pointer to menu check
        FXCheckButton* myValueCheckButton = nullptr;

        /// @brief Button for move lane up
        MFXButtonTooltip* myValueLaneUpButton = nullptr;

        /// @brief Button for move lane down
        MFXButtonTooltip* myValueLaneDownButton = nullptr;

        /// @brief pointer to AC Parent
        GNEAttributeCarrier* myACParent = nullptr;

        /// @brief build Attribute elements
        void buildAttributeElements(const bool attributeEnabled, const bool computed);

        /// @brief build value elements();
        void buildValueElements(const bool attributeEnabled, const bool computed);

        /// @brief refresh Attribute elements
        void refreshAttributeElements(const std::string& value, const bool attributeEnabled, const bool computed);

        /// @brief build value elements();
        void refreshValueElements(const std::string& value,  const bool attributeEnabled, const bool computed, const bool forceRefreshAttribute);

        /// @brief fill comboBox with discrete values
        void fillComboBox(const std::string& value);

        /// @brief update move lane buttons
        void updateMoveLaneButtons(const std::string& value);

        /// @brief Invalidated copy constructor.
        AttributesEditorRow(AttributesEditorRow*) = delete;

        /// @brief Invalidated assignment operator.
        AttributesEditorRow& operator=(AttributesEditorRow*) = delete;
    };

    // ===========================================================================
    // class AttributesEditor
    // ===========================================================================

    class AttributesEditor : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributeModules::AttributesEditor)

    public:
        /// @brief constructor
        AttributesEditor(GNEFrame* inspectorFrameParent);

        /// @brief show attributes of multiple ACs
        void showAttributeEditorModule(bool includeExtended);

        /// @brief hide attribute editor
        void hideAttributesEditorModule();

        /// @brief refresh attribute editor (only the valid values will be refresh)
        void refreshAttributeEditor(bool forceRefreshShape, bool forceRefreshPosition);

        /// @brief pointer to GNEFrame parent
        GNEFrame* getFrameParent() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user press the help button
        long onCmdAttributesEditorHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief fox need this
        FOX_CONSTRUCTOR(AttributesEditor)

    private:
        /// @brief pointer to GNEFrame parent
        GNEFrame* myFrameParent;

        /// @brief GNEFlowEditor modul
        GNEFlowEditor* myAttributesEditorFlow = nullptr;

        /// @brief list of Attribute editor rows
        std::vector<AttributesEditorRow*> myAttributesEditorRows;

        /// @brief button for help
        FXButton* myHelpButton = nullptr;

        /// @brief flag used to mark if current edited ACs are bein edited including extended attribute
        bool myIncludeExtended;
    };

    // ===========================================================================
    // class AttributesEditorExtended
    // ===========================================================================

    class AttributesEditorExtended : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributeModules::AttributesEditorExtended)

    public:
        /// @brief constructor
        AttributesEditorExtended(GNEFrame* frameParent);

        /// @brief destructor
        ~AttributesEditorExtended();

        /// @brief show AttributesEditorExtended modul
        void showAttributesEditorExtendedModule();

        /// @brief hide group box
        void hideAttributesEditorExtendedModule();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when open dialog button is clicked
        long onCmdOpenDialog(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(AttributesEditorExtended)

    private:
        /// @brief pointer to Frame Parent
        GNEFrame* myFrameParent = nullptr;
    };

    // ===========================================================================
    // class GenericDataAttributes
    // ===========================================================================

    class GenericDataAttributes : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributeModules::GenericDataAttributes)

    public:
        /// @brief constructor
        GenericDataAttributes(GNEFrame* frameParent);

        /// @brief destructor
        ~GenericDataAttributes();

        /// @brief show netedit attributes EditorCreator
        void showGenericDataAttributes();

        /// @brief hide netedit attributes EditorCreator
        void hideGenericDataAttributes();

        /// @brief refresh netedit attributes
        void refreshGenericDataAttributes();

        /// @brief get parameters as map
        const Parameterised::Map& getParametersMap() const;

        /// @brief get parameters as string
        std::string getParametersStr() const;

        /// @brief get parameters as vector of strings
        std::vector<std::pair<std::string, std::string> > getParameters() const;

        /// @brief set parameters
        void setParameters(const std::vector<std::pair<std::string, std::string> >& parameters);

        /// @brief pointer to frame parent
        GNEFrame* getFrameParent() const;

        /// @brief check if current attributes are valid
        bool areAttributesValid() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user clicks over add parameter
        long onCmdEditParameters(FXObject*, FXSelector, void*);

        /// @brief Called when user udpate the parameter text field
        long onCmdSetParameters(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(GenericDataAttributes)

    private:
        /// @brief pointer to frame parent
        GNEFrame* myFrameParent = nullptr;

        /// @brief pointer to current map of parameters
        Parameterised::Map myParameters;

        /// @brief text field for write parameters
        FXTextField* myTextFieldParameters = nullptr;

        /// @brief button for edit parameters using specific dialog
        FXButton* myButtonEditParameters = nullptr;
    };

    // ===========================================================================
    // class ParametersEditor
    // ===========================================================================

    class ParametersEditor : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributeModules::ParametersEditor)

    public:
        /// @brief constructor for inspector frame
        ParametersEditor(GNEInspectorFrame* inspectorFrameParent);

        /// @brief constructor for type frame
        ParametersEditor(GNETypeFrame* typeFrameParent);

        /// @brief destructor
        ~ParametersEditor();

        /// @get viewNet
        GNEViewNet* getViewNet() const;

        /// @brief show netedit attributes EditorInspector
        void showParametersEditor();

        /// @brief hide netedit attributes EditorInspector
        void hideParametersEditor();

        /// @brief refresh netedit attributes
        void refreshParametersEditor();

        /// @brief get inspector frame parent
        GNEInspectorFrame* getInspectorFrameParent() const;

        /// @brief get type frame parent
        GNETypeFrame* getTypeFrameParent() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user clicks over add parameter
        long onCmdEditParameters(FXObject*, FXSelector, void*);

        /// @brief Called when user udpate the parameter text field
        long onCmdSetParameters(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(ParametersEditor)

    private:
        /// @brief inspector frame parent
        GNEInspectorFrame* myInspectorFrameParent = nullptr;

        /// @brief type frame parent
        GNETypeFrame* myTypeFrameParent = nullptr;

        /// @brief text field for write parameters
        FXTextField* myTextFieldParameters = nullptr;

        /// @brief button for edit parameters using specific dialog
        FXButton* myButtonEditParameters = nullptr;
    };

    /// @brief return true if AC can be edited in the current supermode
    static bool isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeCarrier* AC);

    /// @brief return true if give ACAttr can be edited in the current supermode
    static bool isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeProperties& ACAttr);
};
