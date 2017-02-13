/****************************************************************************/
/// @file    GNEInspectorFrame.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// The Widget for modifying network-element attributes (i.e. lane speed)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
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
    // class AttrInput
    // ===========================================================================

    class AttrInput : public FXMatrix  {
        /// @brief FOX-declaration
        FXDECLARE(GNEInspectorFrame::AttrInput)

    public:
        /// @brief constructor
        AttrInput(FXComposite* parent, GNEInspectorFrame* inspectorFrameParent);

        /// @brief show attribute
        void showAttribute(SumoXMLTag tag, SumoXMLAttr attr, const std::string& value);

        /// @brief show attribute
        void hideAttribute();

        /// @brief get current tag
        SumoXMLTag getTag() const;

        /// @brief get current Attr
        SumoXMLAttr getAttr() const;

        /// @brief try to set new attribute value
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief open model dialog for more comfortable attribute editing
        long onCmdOpenAttributeEditor(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        AttrInput() {}

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

        /// @brief textField to modify the value of real attributes
        FXTextField* myTextFieldReal;

        /// @rief SpinDial to modify the value of time attributes
        FXSpinner* myTimeSpinDial;

        /// @brief textField to modify the value of string attributes
        FXTextField* myTextFieldStrings;

        /// @brief pointer to combo box choices
        FXComboBox* myChoicesCombo;

        /// @brief pointer to menu check
        FXMenuCheck* myCheckBox;

        /// @brief pointer to buttonCombinableChoices
        FXButton* myButtonCombinableChoices;

        /// @brief set show as private function
        void show();

        /// @brief set hide as private function
        void hide();
    };

    // ===========================================================================
    // class AttrEditor
    // ===========================================================================

    class AttrEditor : public FXDialogBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEInspectorFrame::AttrEditor)

    public:
        /// @brief constructor
        AttrEditor(AttrInput* attrInputParent, FXTextField* textFieldAttr);

        /// @brief destructor
        ~AttrEditor();

        /// @brief call when user press button reset
        long onCmdReset(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        AttrEditor() {}

    private:
        /// @brief Pointer to AttrInput parent
        AttrInput* myAttrInputParent;

        // @brief Pointer to TexField in which write attribute
        FXTextField* myTextFieldAttr;

        // @brief Matrix in that CheckBoxs will be inserted
        FXMatrix* myCheckBoxMatrix;

        /// @brief vector of Menuchecks
        std::vector<FXMenuCheck*> myVectorOfCheckBox;

        /// @brief frame for the buttons
        FXHorizontalFrame* frameButtons;

        /// @brief Button Accept
        FXButton* myAcceptButton;

        /// @brief Button Cancel
        FXButton* myCancelButton;

        /// @brief Button Reset
        FXButton* myResetButton;
    };

public:
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

    /// @brief called when user toogle the blocking button
    long onCmdSetBlocking(FXObject*, FXSelector, void*);

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

private:
    /// @brief groupBox for attributes
    FXGroupBox* myGroupBoxForAttributes;

    /// @brief list of Attribute inputs
    std::vector<GNEInspectorFrame::AttrInput*> vectorOfAttrInput;

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

    /// @brief GropuBox for editor attributes
    FXGroupBox* myGroupBoxForEditor;

    /// @brief pointer to menu check block
    FXMenuCheck* myCheckBlocked;

    /// @brief pointer to additional element
    GNEAdditional* myAdditional;

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
    std::set<FXTreeItem*> myTreeItesmWithoutAC;

    /// @brief pointer to current right clicked Attribute Carrier
    GNEAttributeCarrier* myRightClickedAC;
};


#endif

/****************************************************************************/

