/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEInspectorFrame.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// The Widget for modifying network-element attributes (i.e. lane speed)
/****************************************************************************/
#ifndef GNEInspectorFrame_h
#define GNEInspectorFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEFrame.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEAttributeCarrier;
class GNEAdditional;
class GNEEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEInspectorFrame
 * The Widget for modifying network-element attributes (i.e. lane speed)
 */
class GNEInspectorFrame : public GNEFrame {
    /// @brief FOX-declaration
    FXDECLARE(GNEInspectorFrame)

public:
    
    // ===========================================================================
    // class AttributeInput
    // ===========================================================================
    
    class AttributesEditor;
    
    class AttributeInput : protected FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEInspectorFrame::AttributeInput)

    public:
        /// @brief constructor
        AttributeInput(GNEInspectorFrame::AttributesEditor* attributeEditorParent);

        /// @brief show attribute of ac
        void showAttribute(SumoXMLTag ACTag, SumoXMLAttr ACAttribute, const std::string& value);

        /// @brief show attribute
        void hideAttribute();

        /// @brief refresh attribute
        void refreshAttribute();

        /// @brief get current Attr
        SumoXMLAttr getEditedAttr() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief try to set new attribute value
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief open model dialog for more comfortable attribute editing
        long onCmdOpenAllowDisallowEditor(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        AttributeInput() {}

        /// @brief removed invalid spaces of Positions and shapes
        std::string stripWhitespaceAfterComma(const std::string& stringValue);

    private:
        /// @brief pointer to AttributesEditor parent
        GNEInspectorFrame::AttributesEditor * myAttributesEditorParent;

        /// @brief current tag
        SumoXMLTag myTag;

        /// @brief current Attr
        SumoXMLAttr myAttr;

        /// @brief pointer to label
        FXLabel* myLabel;

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

        /// @brief pointer to buttonCombinableChoices
        FXButton* myButtonCombinableChoices;
    };

    // ===========================================================================
    // class AttributesEditor
    // ===========================================================================

    class AttributesEditor : public FXGroupBox {

    public:
        /// @brief constructor
        AttributesEditor(GNEInspectorFrame* inspectorFrameParent);

        /// @brief show attribute of ac
        void showAttribute(SumoXMLTag ACTag, SumoXMLAttr ACAttribute, const std::string& value);

        /// @brief show attribute
        void hideAttributesEditor();

        /// @brief refresh attribute
        void refreshAttributes(bool onlyAllowdisallow = false);

        /// @brief get InspectorFrame Parent
        GNEInspectorFrame* getInspectorFrameParent() const;

    private:
        /// @brief pointer to GNEInspectorFrame parent
        GNEInspectorFrame * myInspectorFrameParent;

        /// @brief list of Attribute inputs
        std::vector<GNEInspectorFrame::AttributeInput*> myVectorOfAttributeInputs;

        /// @brief current parameter index
        int myCurrentIndex = 0;
    };

    // ===========================================================================
    // class NeteditAttributesEditor
    // ===========================================================================

    class NeteditAttributesEditor : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEInspectorFrame::NeteditAttributesEditor)

    public:
        /// @brief constructor
        NeteditAttributesEditor(GNEInspectorFrame* inspectorFrameParent);

        /// @brief destructor
        ~NeteditAttributesEditor();

        /// @brief show attribute of ac
        void showNeteditAttributes(const std::vector<GNEAttributeCarrier*>& ACs);

        /// @brief show attribute
        void hideNeteditAttributesEditor();

        /// @name FOX-callbacks
        /// @{
        /// @brief try to set new attribute value
        long onCmdSetAttribute(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        NeteditAttributesEditor() {}

    private:
        /// @brief pointer to inspector frame parent
        GNEInspectorFrame* myInspectorFrameParent;

        /// @frame horizontal frame for change additional parent
        FXHorizontalFrame* myHorizontalFrameAdditionalParent;

        /// @brief Label for additional parent
        FXLabel* myLabelAdditionalParent;

        /// @brief pointer for change additional parent
        FXTextField* myTextFieldAdditionalParent;

        /// @frame horizontal frame for block movement
        FXHorizontalFrame* myHorizontalFrameBlockMovement;

        /// @brief Label for Check blocked movement
        FXLabel* myLabelBlockMovement;

        /// @brief pointer to check box "Block movement"
        FXCheckButton* myCheckBoxBlockMovement;

        /// @frame horizontal frame for block shape
        FXHorizontalFrame* myHorizontalFrameBlockShape;

        /// @brief Label for Check blocked shape
        FXLabel* myLabelBlockShape;

        /// @brief pointer to check box "Block Shape"
        FXCheckButton* myCheckBoxBlockShape;

        /// @frame horizontal frame for close shape
        FXHorizontalFrame* myHorizontalFrameCloseShape;

        /// @brief Label for close shape
        FXLabel* myLabelCloseShape;

        /// @brief pointer to check box "Block movement"
        FXCheckButton* myCheckBoxCloseShape;
    };

    // ===========================================================================
    // class GEOAttributesEditor
    // ===========================================================================

    class GEOAttributesEditor : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEInspectorFrame::GEOAttributesEditor)

    public:
        /// @brief constructor
        GEOAttributesEditor(GNEInspectorFrame* inspectorFrameParent);

        /// @brief destructor
        ~GEOAttributesEditor();

        /// @brief show GEOAttribute for the current AttributeCarriers
        void showGEOAttributesEditor(const std::vector<GNEAttributeCarrier*>& ACs);

        /// @brief hide GEOAttributesEditor
        void hideGEOAttributesEditor();

        /// @brief refresh TextFields with the new GEO Attributes
        void refreshGEOAttributesEditor();

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when user change the current GEO Attribute
        long onCmdSetGEOAttribute(FXObject*, FXSelector, void*);

        /// @brief Called when user enters a new length
        long onCmdUseGEOParameters(FXObject*, FXSelector, void*);

        /// @brief Called when user press the help button
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        GEOAttributesEditor() {}

    private:
        /// @brief current GNEInspectorFrame parent
        GNEInspectorFrame* myInspectorFrameParent;

        /// @brief type of GEO Attribute
        SumoXMLAttr myGEOAttribute;

        /// @brief horizontal frame for GEOAttribute
        FXHorizontalFrame* myGEOAttributeFrame;

        /// @brief Label for GEOAttribute
        FXLabel* myGEOAttributeLabel;

        /// @brief textField for GEOAttribute
        FXTextField* myGEOAttributeTextField;

        /// @brief horizontal frame for use GEO
        FXHorizontalFrame* myUseGEOFrame;

        /// @brief Label for use GEO
        FXLabel* myUseGEOLabel;

        /// @brief checkBox for use GEO
        FXCheckButton* myUseGEOCheckButton;

        /// @brief button for help
        FXButton* myHelpButton;
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEInspectorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEInspectorFrame();

    /// @brief show inspector frame
    void show();

    /// @brief Inspect a single element
    void inspectElement(GNEAttributeCarrier* AC);

    /// @brief Inspect the given multi-selection
    void inspectMultisection(const std::vector<GNEAttributeCarrier*>& ACs);

    /// @brief inspect child of already inspected element
    void inspectChild(GNEAttributeCarrier* AC, GNEAttributeCarrier* previousElement);

    /// @brief inspect called from DeleteFrame
    void inspectFromDeleteFrame(GNEAttributeCarrier* AC, GNEAttributeCarrier* previousElement, bool previousElementWasMarked);

    /// @brief Refresh inspected values (used when values can be changed externally by other modul)
    void refreshValues();

    /// @brief get current list of ACs
    const std::vector<GNEAttributeCarrier*>& getACs() const;

    /// @brief get the template edge (to copy attributes from)
    GNEEdge* getEdgeTemplate() const;

    /// @brief seh the template edge (we assume shared responsibility via reference counting)
    void setEdgeTemplate(GNEEdge* tpl);

    /// @name FOX-callbacks
    /// @{
    /// @brief copy edge attributes from edge template
    long onCmdCopyTemplate(FXObject*, FXSelector, void*);

    /// @brief set current edge as new template
    long onCmdSetTemplate(FXObject*, FXSelector, void*);

    /// @brief update the copy button with the name of the template
    long onUpdCopyTemplate(FXObject*, FXSelector, void*);

    /// @brief called when user toogle the go back button
    long onCmdGoBack(FXObject*, FXSelector, void*);

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
    GNEInspectorFrame() {}

    // @brief create pop-up menu in the positions X-Y for the attribute carrier ac
    void createPopUpMenu(int X, int Y, GNEAttributeCarrier* ac);

    /// @brief show child of current attributeCarrier
    void showAttributeCarrierChilds();

    /// @brief get reference to current inspected Attribute carriers
    const std::vector<GNEAttributeCarrier*>& getInspectedACs() const;

private:
    /// @brief Attribute editor
    AttributesEditor* myAttributesEditor;

    /// @brief Netedit Attributes editor
    NeteditAttributesEditor* myNeteditAttributesEditor;

    /// @brief GEO Attributes editor
    GEOAttributesEditor* myGEOAttributesEditor;

    /// @brief back Button
    FXButton* myBackButton;

    /// @brief groupBox for templates
    FXGroupBox* myGroupBoxForTemplates;

    /// @brief copy template button
    FXButton* myCopyTemplateButton;

    /// @brief set template button
    FXButton* mySetTemplateButton;

    /// @brief the edge template
    GNEEdge* myEdgeTemplate;

    /// @brief pointer to previous element called by Inspector Frame
    GNEAttributeCarrier* myPreviousElementInspect;

    /// @brief pointer to previous element called by Delete Frame
    GNEAttributeCarrier* myPreviousElementDelete;

    /// @brief flag to ckec if myPreviousElementDelete was marked in Delete Frame
    bool myPreviousElementDeleteWasMarked;

    /// @brief the multi-selection currently being inspected
    std::vector<GNEAttributeCarrier*> myACs;

    /// @brief groupBox for AttrConnection
    FXGroupBox* myGroupBoxForTreeList;

    /// @brief tree list to show the childs of the element to erase
    FXTreeList* myTreelist;

    /// @brief map used to save the Tree items with their AC
    std::map<FXTreeItem*, GNEAttributeCarrier*> myTreeItemToACMap;

    /// @brief set used to save tree items without AC assigned (for example, Incoming/Outcoming connections)
    std::set<FXTreeItem*> myTreeItemsWithoutAC;

    /// @brief pointer to current right clicked Attribute Carrier
    GNEAttributeCarrier* myRightClickedAC;
};


#endif

/****************************************************************************/

