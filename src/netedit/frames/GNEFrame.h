/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// Abstract class for lateral frames in NetEdit
/****************************************************************************/
#ifndef GNEFrame_h
#define GNEFrame_h

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include <netedit/GNEAttributeCarrier.h>
#include <utils/geom/PositionVector.h>
#include <netedit/GNEViewNetHelper.h>

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEFrame
 * Abstract class for lateral frames in NetEdit
 */
class GNEFrame : public FXVerticalFrame {

public:

    // ===========================================================================
    // class ItemSelector
    // ===========================================================================

    class ItemSelector : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrame::ItemSelector)

    public:
        /// @brief constructor
        ItemSelector(GNEFrame* frameParent, GNEAttributeCarrier::TagType type, bool onlyDrawables = true);

        /// @brief destructor
        ~ItemSelector();

        /// @brief get current type tag
        const GNEAttributeCarrier::TagProperties& getCurrentTagProperties() const;

        /// @brief set current type manually
        void setCurrentTypeTag(SumoXMLTag typeTag);

        /// @brief due myCurrentTagProperties is a Reference, we need to refresh it when frameParent is show
        void refreshTagProperties();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select another element in ComboBox
        long onCmdSelectItem(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        ItemSelector() {}

    private:
        /// @brief pointer to Frame Parent
        GNEFrame* myFrameParent;

        /// @brief comboBox with the list of elements type
        FXComboBox* myTypeMatchBox;

        /// @brief current tag properties
        GNEAttributeCarrier::TagProperties myCurrentTagProperties;

        /// @brief list of tags that will be shown in Match Box
        std::vector<SumoXMLTag> myListOfTags;

        /// @brief dummy tag properties used if user select an invalid tag
        GNEAttributeCarrier::TagProperties myInvalidTagProperty;
    };

    // ===========================================================================
    // class AttributesCreator
    // ===========================================================================

    class AttributesCreator : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrame::AttributesCreator)

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

        /// @brief check if parameters of attributes are valid
        bool areValuesValid() const;

        /// @brief show warning message with information about non-valid attributes
        void showWarningMessage(std::string extra = "") const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

        // ===========================================================================
        // class RowCreator
        // ===========================================================================

        class RowCreator : public FXHorizontalFrame {
            /// @brief FOX-declaration
            FXDECLARE(GNEFrame::AttributesCreator::RowCreator)

        public:
            /// @brief constructor
            RowCreator(AttributesCreator* AttributesCreatorParent);

            /// @brief show name and value of attribute of type string
            void showParameter(const GNEAttributeCarrier::AttributeProperties& attrProperties);

            /// @brief hide all parameters
            void hideParameter();

            /// @brief return Attr
            const GNEAttributeCarrier::AttributeProperties &getAttrProperties() const;

            /// @brief return value
            std::string getValue() const;

            /// @brief return status of radio button
            bool getRadioButtonCheck() const;

            /// @brief enable or disable radio button for disjoint attributes
            void setRadioButtonCheck(bool value);

            /// @brief check if row is enabled
            bool isRowEnabled() const;

            /// @brief returns a empty string if current value is valid, a string with information about invalid value in other case
            const std::string& isAttributeValid() const;

            /// @brief get AttributesCreator parent
            AttributesCreator* getAttributesCreatorParent() const;

            /// @name FOX-callbacks
            /// @{
            /// @brief called when user set the value of an attribute of type int/float/string
            long onCmdSetAttribute(FXObject*, FXSelector, void*);

            /// @brief called when user change the value of myBoolCheckButton
            long onCmdSetBooleanAttribute(FXObject*, FXSelector, void*);

            /// @brief called when user press the "Color" button
            long onCmdSetColorAttribute(FXObject*, FXSelector, void*);

            /// @brief called when user press a radio button
            long onCmdSelectRadioButton(FXObject*, FXSelector, void*);
            /// @}

        protected:
            /// @brief FOX needs this
            RowCreator() {}

        private:
            /// @brief pointer to AttributesCreator
            AttributesCreator* myAttributesCreatorParent;

            /// @brief attribute properties
            GNEAttributeCarrier::AttributeProperties myAttrProperties;

            /// @brief lael with the name of the parameter
            FXLabel* myLabel;

            /// @brief textField to modify the default value of int/float/string parameters
            FXTextField* myTextFieldInt;

            /// @brief textField to modify the default value of real/times parameters
            FXTextField* myTextFieldReal;

            /// @brief textField to modify the default value of string parameters
            FXTextField* myTextFieldStrings;

            /// @brief check button to enable/disable the value of boolean parameters
            FXCheckButton* myBoolCheckButton;

            /// @brief Button for open color editor
            FXButton* myColorEditor;

            /// @brief Radio button for disjoint attributes
            FXRadioButton* myRadioButton;

            /// @brief string which indicates the reason due current value is invalid
            std::string myInvalidValue;
        };

        /// @brief update disjoint attributes
        void updateDisjointAttributes(RowCreator *row);

    protected:
        /// @brief FOX needs this
        AttributesCreator() {};

    private:
        /// @brief pointer to Polygon Frame Parent
        GNEFrame* myFrameParent;

        /// @brief current edited Tag Properties
        GNEAttributeCarrier::TagProperties myTagProperties;

        /// @brief vector with the ACAttribute Rows
        std::vector<RowCreator*> myRows;
    };

    // ===========================================================================
    // class AttributesEditor
    // ===========================================================================

    class AttributesEditor : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrame::AttributesEditor)

    public:

        // ===========================================================================
        // class RowEditor
        // ===========================================================================

        class RowEditor : private FXHorizontalFrame {
            /// @brief FOX-declaration
            FXDECLARE(GNEFrame::AttributesEditor::RowEditor)

        public:
            /// @brief constructor
            RowEditor(GNEFrame::AttributesEditor* attributeEditorParent);

            /// @brief show row attribute
            void showRow(const GNEAttributeCarrier::AttributeProperties &ACAttr, const std::string& value, bool disjointAttributeEnabled);

            /// @brief show row attribute
            void hideRow();

            /// @brief refresh current row
            void refreshRow(const std::string& value, bool forceRefresh, bool disjointAttributeEnabled);

            /// @brief check if current attribute of TextField/ComboBox is valid
            bool isRowValid() const;

            /// @name FOX-callbacks
            /// @{

            /// @brief try to set new attribute value
            long onCmdSetAttribute(FXObject*, FXSelector, void*);

            /// @brief set new disjoint attribute
            long onCmdSetDisjointAttribute(FXObject*, FXSelector, void*);

            /// @brief open model dialog for more comfortable attribute editing
            long onCmdOpenAttributeDialog(FXObject*, FXSelector, void*);
            /// @}

        protected:
            /// @brief FOX needs this
            RowEditor() {}

            /// @brief removed invalid spaces of Positions and shapes
            std::string stripWhitespaceAfterComma(const std::string& stringValue);
            
            /// @brief enable row elements
            void enableRowElements();

            /// @brief disable row elements
            void disableRowElements();

        private:
            /// @brief pointer to AttributesEditor parent
            GNEFrame::AttributesEditor* myAttributesEditorParent;

            /// @brief current AC Attribute
            GNEAttributeCarrier::AttributeProperties myACAttr;

            /// @brief flag to check if input element contains multiple values
            bool myMultiple;

            /// @brief pointer to attribute label
            FXLabel* myLabel;

            /// @brief Radio button for disjoint attributes
            FXRadioButton* myRadioButton;

            /// @brief pointer to buttonCombinableChoices
            FXButton* myButtonCombinableChoices;

            /// @brief Button for open color editor
            FXButton* myColorEditor;

            /// @brief textField to modify the value of int attributes
            FXTextField* myTextFieldInt;

            /// @brief textField to modify the value of real/Time attributes
            FXTextField* myTextFieldReal;

            /// @brief textField to modify the value of string attributes
            FXTextField* myTextFieldStrings;

            /// @brief pointer to combo box choices
            FXComboBox* myChoicesCombo;

            /// @brief pointer to menu check
            FXCheckButton* myBoolCheckButton;
        };

        /// @brief constructor
        AttributesEditor(GNEFrame* inspectorFrameParent);

        /// @brief show attributes of multiple ACs
        void showAttributeEditorModul(const std::vector<GNEAttributeCarrier*>& ACs, bool includeExtended);

        /// @brief hide attribute editor
        void hideAttributesEditorModul();

        /// @brief refresh attribute editor (only the valid values will be refresh)
        void refreshAttributeEditor(bool forceRefreshShape, bool forceRefreshPosition);

        /// @brief get current edited ACs
        const std::vector<GNEAttributeCarrier*> &getEditedACs() const;

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

        /// @brief list of Attribute inputs rows
        std::vector<RowEditor*> myVectorOfRows;

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
        FXDECLARE(GNEFrame::AttributesEditorExtended)

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
        /// @brief pointer to Polygon Frame Parent
        GNEFrame* myFrameParent;
    };

    // ===========================================================================
    // class ACHierarchy
    // ===========================================================================

    class ACHierarchy : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrame::ACHierarchy)

    public:
        /// @brief constructor
        ACHierarchy(GNEFrame* frameParent);

        /// @brief destructor
        ~ACHierarchy();

        /// @brief show ACHierarchy
        void showACHierarchy(GNEAttributeCarrier* AC);

        /// @brief hide ACHierarchy
        void hideACHierarchy();

        /// @brief refresh ACHierarchy
        void refreshACHierarchy();

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user press right click over an item of list of childs
        long onCmdShowChildMenu(FXObject*, FXSelector, void* data);

        /// @brief called when user select option "center item" of child Menu
        long onCmdCenterItem(FXObject*, FXSelector, void*);

        /// @brief called when user select option "inspect item" of child menu
        long onCmdInspectItem(FXObject*, FXSelector, void*);

        /// @brief called when user select option "delte item" of child menu
        long onCmdDeleteItem(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        ACHierarchy() {}

        // @brief create pop-up menu in the positions X-Y for the attribute carrier ac
        void createPopUpMenu(int X, int Y, GNEAttributeCarrier* ac);

        /// @brief show child of current attributeCarrier
        FXTreeItem* showAttributeCarrierParents();

        /// @brief show child of current attributeCarrier
        void showAttributeCarrierChilds(GNEAttributeCarrier* AC, FXTreeItem* itemParent);

        /// @brief add item into list
        FXTreeItem* addListItem(GNEAttributeCarrier* AC, FXTreeItem* itemParent = nullptr, std::string prefix = "", std::string sufix = "");

        /// @brief add item into list
        FXTreeItem* addListItem(FXTreeItem* itemParent, const std::string &text, FXIcon* icon, bool expanded);
    private:
        /// @brief Frame Parent
        GNEFrame* myFrameParent;

        /// @brief Attribute carrier
        GNEAttributeCarrier* myAC;

        /// @brief tree list to show the childs of the element to erase
        FXTreeList* myTreelist;

        /// @brief map used to save the Tree items with their AC
        std::map<FXTreeItem*, GNEAttributeCarrier*> myTreeItemToACMap;

        /// @brief set used to save tree items without AC assigned, the Incoming/Outcoming connections
        std::set<FXTreeItem*> myTreeItemsConnections;

        /// @brief pointer to current right clicked Attribute Carrier
        GNEAttributeCarrier* myRightClickedAC;
    };

    // ===========================================================================
    // class GenericParametersEditor
    // ===========================================================================

    class GenericParametersEditor : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrame::GenericParametersEditor)

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
        FXDECLARE(GNEFrame::DrawingShape)

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
        FXDECLARE(GNEFrame::NeteditAttributes)

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
        bool getNeteditAttributesAndValues(std::map<SumoXMLAttr, std::string>& valuesMap, GNELane* lane) const;

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

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     * @brief frameLabel label of the frame
     */
    GNEFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet, const std::string& frameLabel);

    /// @brief destructor
    ~GNEFrame();

    /// @brief focus upper element of frame
    void focusUpperElement();

    /**@brief show Frame
     * @note some GNEFrames needs a re-implementation
     */
    virtual void show();

    /**@brief hide Frame
     * @note some GNEFrames needs a re-implementation
     */
    virtual void hide();

    /// @brief set width of GNEFrame
    void setFrameWidth(int newWidth);

    /// @brief get view net
    GNEViewNet* getViewNet() const;

    /// @brief get the label for the frame's header
    FXLabel* getFrameHeaderLabel() const;

    /// @brief get font of the header's frame
    FXFont* getFrameHeaderFont() const;

protected:
    /// @brief FOX needs this
    GNEFrame() {}

    /**@brief build a shaped element using the drawed shape (can be reimplemented in frame childs)
     * return true if was sucesfully created
     * @note called when user stop drawing shape
     */
    virtual bool buildShape();

    /// @brief enable moduls depending of item selected in ItemSelector (can be reimplemented in frame childs)
    virtual void enableModuls(const GNEAttributeCarrier::TagProperties& tagProperties);

    /// @brief disable moduls if element selected in itemSelector isn't valid (can be reimplemented in frame childs)
    virtual void disableModuls();

    /// @brief function called after set a valid attribute in AttributeCreator/AttributeEditor/GenericParametersEditor/...
    virtual void updateFrameAfterChangeAttribute();

    /// @brief open AttributesCreator extended dialog (can be reimplemented in frame childs)
    virtual void openAttributesEditorExtendedDialog();

    /// @brief Open help attributes dialog
    void openHelpAttributesDialog(const GNEAttributeCarrier::TagProperties& tagProperties) const;

    /// @brief get edge candidate color
    const RGBColor& getEdgeCandidateColor() const;

    /// @brief get selected color
    const RGBColor& getEdgeCandidateSelectedColor() const;
    
    /// @brief get predefinedTagsMML
    const std::map<int, std::string> &getPredefinedTagsMML() const;

    /// @brief View Net for changes
    GNEViewNet* myViewNet;

    /// @brief Vertical frame that holds all widgets of frame
    FXVerticalFrame* myContentFrame;

    /// @brief fame for header elements
    FXHorizontalFrame* myHeaderFrame;

    /// @brief fame for left header elements
    FXHorizontalFrame* myHeaderLeftFrame;

    /// @brief fame for right header elements
    FXHorizontalFrame* myHeaderRightFrame;

private:
    /// @brief scroll windows that holds the content frame
    FXScrollWindow* myScrollWindowsContents;

    /// @brief static Font for the Header (it's common for all headers, then create only one time)
    static FXFont* myFrameHeaderFont;

    /// @brief the label for the frame's header
    FXLabel* myFrameHeaderLabel;

    /// @brief edge candidate color (used by some modulds to mark edges)
    RGBColor myEdgeCandidateColor;

    /// @brief selected edge candidate color (used by some modulds to selected mark edges)
    RGBColor myEdgeCandidateSelectedColor;

    /// @brief Map of attribute ids to their (readable) string-representation (needed for SUMOSAXAttributesImpl_Cached)
    std::map<int, std::string> myPredefinedTagsMML;

    /// @brief Invalidated copy constructor.
    GNEFrame(const GNEFrame&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFrame& operator=(const GNEFrame&) = delete;
};


#endif

/****************************************************************************/
