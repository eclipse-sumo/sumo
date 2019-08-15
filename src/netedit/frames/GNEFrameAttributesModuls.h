/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEFrameAttributesModuls.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
/// @version $Id$
///
// Auxiliar class for GNEFrame Moduls (only for attributes edition)
/****************************************************************************/
#ifndef GNEFrameAttributesModuls_h
#define GNEFrameAttributesModuls_h

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNEAttributeCarrier.h>
#include <netedit/GNEViewNetHelper.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFrameAttributesModuls {

public:
    // ===========================================================================
    // class declaration
    // ===========================================================================

    class AttributesCreator;
    class AttributesEditor;

    // ===========================================================================
    // class AttributesCreatorRow
    // ===========================================================================

    class AttributesCreatorRow : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributesModuls::AttributesCreatorRow)

    public:
        /// @brief constructor
        AttributesCreatorRow(AttributesCreator* AttributesCreatorParent, const GNEAttributeCarrier::AttributeProperties& attrProperties);

        /// @brief destroy AttributesCreatorRow (but don't delete)
        void destroy();

        /// @brief return Attr
        const GNEAttributeCarrier::AttributeProperties& getAttrProperties() const;

        /// @brief return value
        std::string getValue() const;

        /// @brief return status of radio button
        bool getAttributeRadioButtonCheck() const;

        /// @brief enable or disable radio button for disjoint attributes
        void setAttributeRadioButtonCheck(bool value);

        /// @brief return status of label checkbox button
        bool getAttributeCheckButtonCheck() const;

        /// @brief enable or disable label checkbox button for optional attributes
        void setAttributeCheckButtonCheck(bool value);

        /// @brief enable row
        void enableAttributesCreatorRow();

        /// @brief disable row
        void disableAttributesCreatorRow();

        /// @brief check if row is enabled
        bool isAttributesCreatorRowEnabled() const;

        /// @brief returns a empty string if current value is valid, a string with information about invalid value in other case
        const std::string& isAttributeValid() const;

        /// @brief get AttributesCreator parent
        AttributesCreator* getAttributesCreatorParent() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user set the value of an attribute of type int/float/string/bool
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief called when user press a check button
        long onCmdSelectCheckButton(FXObject*, FXSelector, void*);

        /// @brief called when user press the "Color" button
        long onCmdSelectColorButton(FXObject*, FXSelector, void*);

        /// @brief called when user press a radio button
        long onCmdSelectRadioButton(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        AttributesCreatorRow() {}

        /// @brief check if given complex attribute is valid
        std::string checkComplexAttribute(const std::string& value);

    private:
        /// @brief pointer to AttributesCreator
        AttributesCreator* myAttributesCreatorParent;

        /// @brief attribute properties
        const GNEAttributeCarrier::AttributeProperties myAttrProperties;

        /// @brief string which indicates the reason due current value is invalid
        std::string myInvalidValue;

        /// @brief Label with the name of the attribute
        FXLabel* myAttributeLabel;

        /// @brief Radio button for disjoint attributes
        FXRadioButton* myAttributeRadioButton;

        /// @brief check button to enable/disable Label attribute
        FXCheckButton* myAttributeCheckButton;

        /// @brief Button for open color editor
        FXButton* myAttributeColorButton;

        /// @brief textField to modify the default value of int/float/string parameters
        FXTextField* myValueTextFieldInt;

        /// @brief textField to modify the default value of real/times parameters
        FXTextField* myValueTextFieldReal;

        /// @brief textField to modify the default value of string parameters
        FXTextField* myValueTextFieldStrings;

        /// @brief check button to enable/disable the value of boolean parameters
        FXCheckButton* myValueCheckButton;
    };

    // ===========================================================================
    // class AttributesCreator
    // ===========================================================================

    class AttributesCreator : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributesModuls::AttributesCreator)

        // declare friend class
        friend class Row;

    public:
        /// @brief constructor
        AttributesCreator(GNEFrame* frameParent);

        /// @brief destructor
        ~AttributesCreator();

        /// @brief show AttributesCreator modul
        void showAttributesCreatorModul(const GNEAttributeCarrier::TagProperties& myTagProperties);

        /// @brief hide group box
        void hideAttributesCreatorModul();

        /// @brief get attributes and their values
        std::map<SumoXMLAttr, std::string> getAttributesAndValues(bool includeAll) const;

        /// @brief get current edited Tag Properties
        GNEAttributeCarrier::TagProperties getCurrentTagProperties() const;

        /// @brief check if parameters of attributes are valid
        bool areValuesValid() const;

        /// @brief show warning message with information about non-valid attributes
        void showWarningMessage(std::string extra = "") const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

        /// @brief update disjoint attributes
        void updateDisjointAttributes(AttributesCreatorRow* row);

    protected:
        /// @brief FOX needs this
        AttributesCreator() {};

    private:
        /// @brief pointer to Frame Parent
        GNEFrame* myFrameParent;

        /// @brief current edited Tag Properties
        GNEAttributeCarrier::TagProperties myTagProperties;

        /// @brief vector with the AttributesCreatorRow
        std::vector<AttributesCreatorRow*> myAttributesCreatorRows;

        /// @brief help button
        FXButton* myHelpButton;
    };

    // ===========================================================================
    // class AttributesEditorRow
    // ===========================================================================

    class AttributesEditorRow : protected FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributesModuls::AttributesEditorRow)

    public:
        /// @brief constructor
        AttributesEditorRow(AttributesEditor* attributeEditorParent, const GNEAttributeCarrier::AttributeProperties& ACAttr, const std::string& value, bool attributeEnabled);

        /// @brief destroy AttributesCreatorRow (but don't delete)
        void destroy();

        /// @brief refresh current row
        void refreshAttributesEditorRow(const std::string& value, bool forceRefresh, bool disjointAttributeEnabled);

        /// @brief check if current attribute of TextField/ComboBox is valid
        bool isAttributesEditorRowValid() const;

        /// @name FOX-callbacks
        /// @{

        /// @brief try to set new attribute value
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief called when user press a check button
        long onCmdSelectCheckButton(FXObject*, FXSelector, void*);

        /// @brief set new disjoint attribute
        long onCmdSelectRadioButton(FXObject*, FXSelector, void*);

        /// @brief open model dialog for more comfortable attribute editing
        long onCmdOpenAttributeDialog(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        AttributesEditorRow();

        /// @brief removed invalid spaces of Positions and shapes
        std::string stripWhitespaceAfterComma(const std::string& stringValue);

    private:
        /// @brief pointer to AttributesEditor parent
        AttributesEditor* myAttributesEditorParent;

        /// @brief current AC Attribute
        const GNEAttributeCarrier::AttributeProperties myACAttr;

        /// @brief flag to check if input element contains multiple values
        const bool myMultiple;

        /// @brief pointer to attribute label
        FXLabel* myAttributeLabel;

        /// @brief Radio button for disjoint attributes
        FXRadioButton* myAttributeRadioButton;

        /// @brief pointer to attribute  menu check
        FXCheckButton* myAttributeCheckButton;

        /// @brief pointer to buttonCombinableChoices
        FXButton* myAttributeButtonCombinableChoices;

        /// @brief Button for open color editor
        FXButton* myAttributeColorButton;

        /// @brief textField to modify the value of int attributes
        FXTextField* myValueTextFieldInt;

        /// @brief textField to modify the value of real/Time attributes
        FXTextField* myValueTextFieldReal;

        /// @brief textField to modify the value of string attributes
        FXTextField* myValueTextFieldStrings;

        /// @brief pointer to combo box choices
        FXComboBox* myValueComboBoxChoices;

        /// @brief pointer to menu check
        FXCheckButton* myValueCheckButton;
    };

    // ===========================================================================
    // class AttributesEditor
    // ===========================================================================

    class AttributesEditor : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributesModuls::AttributesEditor)

    public:
        /// @brief constructor
        AttributesEditor(GNEFrame* inspectorFrameParent);

        /// @brief show attributes of multiple ACs
        void showAttributeEditorModul(const std::vector<GNEAttributeCarrier*>& ACs, bool includeExtended);

        /// @brief hide attribute editor
        void hideAttributesEditorModul();

        /// @brief refresh attribute editor (only the valid values will be refresh)
        void refreshAttributeEditor(bool forceRefreshShape, bool forceRefreshPosition);

        /// @brief pointer to GNEFrame parent
        GNEFrame* getFrameParent() const;

        /// @brief get current edited ACs
        const std::vector<GNEAttributeCarrier*>& getEditedACs() const;

        /// @brief remove edited ACs
        void removeEditedAC(GNEAttributeCarrier* AC);

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user press the help button
        long onCmdAttributesEditorHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        AttributesEditor() {}

    private:
        /// @brief pointer to GNEFrame parent
        GNEFrame* myFrameParent;

        /// @brief list of Attribute editor rows
        std::vector<AttributesEditorRow*> myAttributesEditorRows;

        /// @brief button for help
        FXButton* myHelpButton;

        /// @brief the multi-selection currently being inspected
        std::vector<GNEAttributeCarrier*> myEditedACs;

        /// @brief flag used to mark if current edited ACs are bein edited including extended attribute
        bool myIncludeExtended;
    };

    // ===========================================================================
    // class AttributesEditorExtended
    // ===========================================================================

    class AttributesEditorExtended : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributesModuls::AttributesEditorExtended)

    public:
        /// @brief constructor
        AttributesEditorExtended(GNEFrame* frameParent);

        /// @brief destructor
        ~AttributesEditorExtended();

        /// @brief show AttributesEditorExtended modul
        void showAttributesEditorExtendedModul();

        /// @brief hide group box
        void hideAttributesEditorExtendedModul();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when open dialog button is clicked
        long onCmdOpenDialog(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        AttributesEditorExtended() {};

    private:
        /// @brief pointer to Frame Parent
        GNEFrame* myFrameParent;
    };

    // ===========================================================================
    // class GenericParametersEditor
    // ===========================================================================

    class GenericParametersEditor : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributesModuls::GenericParametersEditor)

    public:
        /// @brief constructor
        GenericParametersEditor(GNEFrame* frameParent);

        /// @brief destructor
        ~GenericParametersEditor();

        /// @brief show netedit attributes editor (used for edit generic parameters of an existent AC)
        void showGenericParametersEditor(GNEAttributeCarrier* AC);

        /// @brief show netedit attributes editor (used for edit generic parameters of an existent list of AC)
        void showGenericParametersEditor(std::vector<GNEAttributeCarrier*> ACs);

        /// @brief hide netedit attributes editor
        void hideGenericParametersEditor();

        /// @brief refresh netedit attributes
        void refreshGenericParametersEditor();

        /// @brief get generic parameters as string
        std::string getGenericParametersStr() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user clicks over add generic parameter
        long onCmdEditGenericParameter(FXObject*, FXSelector, void*);

        /// @brief Called when user udpate the generic parameter text field
        long onCmdSetGenericParameter(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        GenericParametersEditor() {}

    private:
        /// @brief pointer to inspector frame parent
        GNEFrame* myFrameParent;

        /// @brief edited Attribute Carrier
        GNEAttributeCarrier* myAC;

        /// @brief list of edited ACs
        std::vector<GNEAttributeCarrier*> myACs;

        /// @brief pointer to current vector of generic parameters
        std::vector<std::pair<std::string, std::string> >* myGenericParameters;

        /// @brief text field for write generic parameter
        FXTextField* myTextFieldGenericParameter;

        /// @brief button for add generic parameter
        FXButton* myEditGenericParameterButton;
    };

    // ===========================================================================
    // class DrawingShape
    // ===========================================================================

    class DrawingShape : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributesModuls::DrawingShape)

    public:
        /// @brief constructor
        DrawingShape(GNEFrame* frameParent);

        /// @brief destructor
        ~DrawingShape();

        /// @brief show Drawing mode
        void showDrawingShape();

        /// @brief hide Drawing mode
        void hideDrawingShape();

        /// @brief start drawing
        void startDrawing();

        /// @brief stop drawing and check if shape can be created
        void stopDrawing();

        /// @brief abort drawing
        void abortDrawing();

        /// @brief add new point to temporal shape
        void addNewPoint(const Position& P);

        /// @brief remove last added point
        void removeLastPoint();

        /// @brief get Temporal shape
        const PositionVector& getTemporalShape() const;

        /// @brief return true if currently a shape is drawed
        bool isDrawing() const;

        /// @brief enable or disable delete last created point
        void setDeleteLastCreatedPoint(bool value);

        /// @brief get flag delete last created point
        bool getDeleteLastCreatedPoint();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user press start drawing button
        long onCmdStartDrawing(FXObject*, FXSelector, void*);

        /// @brief Called when the user press stop drawing button
        long onCmdStopDrawing(FXObject*, FXSelector, void*);

        /// @brief Called when the user press abort drawing button
        long onCmdAbortDrawing(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        DrawingShape() {}

    private:
        /// @brief pointer to frame parent
        GNEFrame* myFrameParent;

        /// @brief flag to enable/disable delete point mode
        bool myDeleteLastCreatedPoint;

        /// @brief current drawed shape
        PositionVector myTemporalShapeShape;

        /// @brief button for start drawing
        FXButton* myStartDrawingButton;

        /// @brief button for stop drawing
        FXButton* myStopDrawingButton;

        /// @brief button for abort drawing
        FXButton* myAbortDrawingButton;

        /// @brief Label with information
        FXLabel* myInformationLabel;
    };

    // ===========================================================================
    // class NeteditAttributes
    // ===========================================================================

    class NeteditAttributes : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributesModuls::NeteditAttributes)

    public:
        /// @brief constructor
        NeteditAttributes(GNEFrame* frameParent);

        /// @brief destructor
        ~NeteditAttributes();

        /// @brief show Netedit attributes modul
        void showNeteditAttributesModul(const GNEAttributeCarrier::TagProperties& tagValue);

        /// @brief hide Netedit attributes modul
        void hideNeteditAttributesModul();

        /// @brief fill valuesMap with netedit attributes
        bool getNeteditAttributesAndValues(std::map<SumoXMLAttr, std::string>& valuesMap, const GNELane* lane) const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user changes some element of NeteditAttributes
        long onCmdSetNeteditAttribute(FXObject*, FXSelector, void*);

        /// @brief Called when user press the help button
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        NeteditAttributes() {}

    private:
        /// @brief list of the reference points
        enum AdditionalReferencePoint {
            GNE_ADDITIONALREFERENCEPOINT_LEFT,
            GNE_ADDITIONALREFERENCEPOINT_RIGHT,
            GNE_ADDITIONALREFERENCEPOINT_CENTER,
            GNE_ADDITIONALREFERENCEPOINT_INVALID
        };

        /// @brief obtain the Start position values of StoppingPlaces and E2 detector over the lane
        double setStartPosition(double positionOfTheMouseOverLane, double lengthOfAdditional) const;

        /// @brief obtain the End position values of StoppingPlaces and E2 detector over the lane
        double setEndPosition(double positionOfTheMouseOverLane, double lengthOfAdditional) const;

        /// @brief pointer to frame parent
        GNEFrame* myFrameParent;

        /// @brief match box with the list of reference points
        FXComboBox* myReferencePointMatchBox;

        /// @brief Label for length
        FXLabel* myLengthLabel;

        /// @brief textField for length
        FXTextField* myLengthTextField;

        /// @brief Label for block movement
        FXLabel* myBlockMovementLabel;

        /// @brief checkBox for block movement
        FXCheckButton* myBlockMovementCheckButton;

        /// @brief Label for block shape
        FXLabel* myBlockShapeLabel;

        /// @brief checkBox for block shape
        FXCheckButton* myBlockShapeCheckButton;

        /// @brief Label for open/close polygon
        FXLabel* myClosePolygonLabel;

        /// @brief checkbox to enable/disable closing polygon
        FXCheckButton* myCloseShapeCheckButton;

        /// @brief Button for help about the reference point
        FXButton* helpReferencePoint;

        /// @brief Flag to check if current length is valid
        bool myCurrentLengthValid;

        /// @brief actual additional reference point selected in the match Box
        AdditionalReferencePoint myActualAdditionalReferencePoint;
    };
};


#endif

/****************************************************************************/
