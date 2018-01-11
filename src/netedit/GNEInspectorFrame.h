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

    class AttributeInput : public FXHorizontalFrame  {
        /// @brief FOX-declaration
        FXDECLARE(GNEInspectorFrame::AttributeInput)

    public:
        /// @brief constructor
        AttributeInput(FXComposite* parent, GNEInspectorFrame* inspectorFrameParent);

        /// @brief show attribute of ac
        void showAttribute(SumoXMLTag ACTag, SumoXMLAttr ACAttribute, const std::string& value);

        /// @brief show attribute
        void hideAttribute();

        /// @brief refresh attribute
        void refreshAttribute();

        /// @brief get current tag
        SumoXMLTag getTag() const;

        /// @brief get current Attr
        SumoXMLAttr getAttr() const;

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
        /// @brief pointer to GNEInspectorFrame parent
        GNEInspectorFrame* myInspectorFrameParent;

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

        /// @brief set show as private function
        void show();

        /// @brief set hide as private function
        void hide();
    };

    // ===========================================================================
    // class editorParameters
    // ===========================================================================

    class NeteditParameters : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEInspectorFrame::NeteditParameters)

    public:
        /// @brief constructor
        NeteditParameters(GNEInspectorFrame* inspectorFrameParent);

        /// @brief destructor
        ~NeteditParameters();

        /// @brief show NeteditParameters
        void show();

        /// @brief hide all NeteditParameters
        void hide();

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user change the parent of an additional
        long onCmdChangeAdditionalParent(FXObject*, FXSelector, void*);

        /// @brief called when user toogle the blocking movement CheckBox
        long onCmdSetBlockingMovement(FXObject*, FXSelector, void*);

        /// @brief called when user toogle the blocking shape CheckBox
        long onCmdSetBlockingShape(FXObject*, FXSelector, void*);

        /// @brief called when user toogle the closiong shape CheckBox
        long onCmdSetClosingShape(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        NeteditParameters() {}

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
    /// @brief groupBox for attributes
    FXGroupBox* myGroupBoxForAttributes;

    /// @brief list of Attribute inputs
    std::vector<GNEInspectorFrame::AttributeInput*> myVectorOfAttributeInputs;

    /// @brief Netedit Parameters
    NeteditParameters* myNeteditParameters;

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

