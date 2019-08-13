/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEInspectorFrame.h
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
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
#include "GNEFrame.h"

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
    // class OverlappedInspection
    // ===========================================================================

    class OverlappedInspection : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEInspectorFrame::OverlappedInspection)

    public:
        /// @brief constructor
        OverlappedInspection(GNEInspectorFrame* inspectorFrameParent);

        /// @brief destructor
        ~OverlappedInspection();

        /// @brief show template editor
        void showOverlappedInspection(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const Position& clickedPosition);

        /// @brief hide template editor
        void hideOverlappedInspection();

        /// @brief check if overlappedInspection modul is shown
        bool overlappedInspectionShown() const;

        /// @brief check if given position is near to saved position
        bool checkSavedPosition(const Position& clickedPosition) const;

        /// @brief try to go to next element if clicked position is near to saved position
        bool nextElement(const Position& clickedPosition);

        /// @brief try to go to previous element if clicked position is near to saved position
        bool previousElement(const Position& clickedPosition);

        /// @name FOX-callbacks
        /// @{

        /// @brief Inspect next Element (from top to bot)
        long onCmdNextElement(FXObject*, FXSelector, void*);

        /// @brief Inspect previous element (from top to bot)
        long onCmdPreviousElement(FXObject*, FXSelector, void*);

        /// @brief show list of overlapped elements
        long onCmdShowList(FXObject*, FXSelector, void*);

        /// @brief called when a list item is selected
        long onCmdListItemSelected(FXObject*, FXSelector, void*);

        /// @brief Called when user press the help button
        long onCmdOverlappingHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        OverlappedInspection() {}

    private:
        /// @brief current GNEInspectorFrame parent
        GNEInspectorFrame* myInspectorFrameParent;

        /// @brief Previous element button
        FXButton* myPreviousElement;

        /// @brief Button for current index
        FXButton* myCurrentIndexButton;

        /// @brief Next element button
        FXButton* myNextElement;

        /// @brief list of overlapped elements
        FXList* myOverlappedElementList;

        /// @brief button for help
        FXButton* myHelpButton;

        /// @brief objects under cursor
        std::vector<GNEAttributeCarrier*> myOverlappedACs;

        /// @brief current index item
        size_t myItemIndex;

        /// @brief saved clicked position
        Position mySavedClickedPosition;

        /// @brief inspect attributeCarrier correspond to current index
        void inspectOverlappedAttributeCarrier();
    };

    // ===========================================================================
    // class NeteditAttributesEditor
    // ===========================================================================

    class NeteditAttributesEditor : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEInspectorFrame::NeteditAttributesEditor)

    public:
        /// @brief constructor
        NeteditAttributesEditor(GNEInspectorFrame* inspectorFrameParent);

        /// @brief destructor
        ~NeteditAttributesEditor();

        /// @brief show netedit attributes editor
        void showNeteditAttributesEditor();

        /// @brief hide netedit attributes editor
        void hideNeteditAttributesEditor();

        /// @brief refresh netedit attributes
        void refreshNeteditAttributesEditor(bool forceRefresh);

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user change the current GEO Attribute
        long onCmdSetNeteditAttribute(FXObject*, FXSelector, void*);

        /// @brief Called when user press the help button
        long onCmdNeteditAttributeHelp(FXObject*, FXSelector, void*);
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

        /// @brief button for help
        FXButton* myHelpButton;
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

        /// @brief show GEO attributes editor
        void showGEOAttributesEditor();

        /// @brief hide GEO attributes editor
        void hideGEOAttributesEditor();

        /// @brief refresh GEO attributes editor
        void refreshGEOAttributesEditor(bool forceRefresh);

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when user change the current GEO Attribute
        long onCmdSetGEOAttribute(FXObject*, FXSelector, void*);

        /// @brief Called when user press the help button
        long onCmdGEOAttributeHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        GEOAttributesEditor() {}

    private:
        /// @brief current GNEInspectorFrame parent
        GNEInspectorFrame* myInspectorFrameParent;

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

    // ===========================================================================
    // class TemplateEditor
    // ===========================================================================

    class TemplateEditor : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEInspectorFrame::TemplateEditor)

    public:
        /// @brief constructor
        TemplateEditor(GNEInspectorFrame* inspectorFrameParent);

        /// @brief destructor
        ~TemplateEditor();

        /// @brief show template editor
        void showTemplateEditor();

        /// @brief hide template editor
        void hideTemplateEditor();

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
        /// @}

    protected:
        /// @brief FOX needs this
        TemplateEditor() {}

    private:
        /// @brief current GNEInspectorFrame parent
        GNEInspectorFrame* myInspectorFrameParent;

        /// @brief copy template button
        FXButton* myCopyTemplateButton;

        /// @brief set template button
        FXButton* mySetTemplateButton;

        /// @brief the edge template
        GNEEdge* myEdgeTemplate;
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

    /// @brief hide inspector frame
    void hide();

    /**@brief process click over Viewnet in Supermode Network
    * @param[in] clickedPosition clicked position over ViewNet
    * @param[in] objectsUnderCursor objects under cursors
    * @return true if something was sucefully done
    */
    bool processNetworkSupermodeClick(const Position& clickedPosition, GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

    /**@brief process click over Viewnet in Supermode Demand
    * @param[in] clickedPosition clicked position over ViewNet
    * @param[in] objectsUnderCursor objects under cursors
    * @return true if something was sucefully done
    */
    bool processDemandSupermodeClick(const Position& clickedPosition, GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

    /// @brief Inspect a single element
    void inspectSingleElement(GNEAttributeCarrier* AC);

    /// @brief Inspect the given multi-selection
    void inspectMultisection(const std::vector<GNEAttributeCarrier*>& ACs);

    /// @brief inspect child of already inspected element
    void inspectChild(GNEAttributeCarrier* AC, GNEAttributeCarrier* previousElement);

    /// @brief inspect called from DeleteFrame
    void inspectFromDeleteFrame(GNEAttributeCarrier* AC, GNEAttributeCarrier* previousElement, bool previousElementWasMarked);

    /// @brief Clear all current inspected ACs
    void clearInspectedAC();

    /// @brief get AttributesEditor
    GNEFrameAttributesModuls::AttributesEditor* getAttributesEditor() const;

    /// @brief get template editor
    TemplateEditor* getTemplateEditor() const;

    /// @brief get OverlappedInspection modul
    OverlappedInspection* getOverlappedInspection() const;

    /// @name FOX-callbacks
    /// @{

    /// @brief called when user toogle the go back button
    long onCmdGoBack(FXObject*, FXSelector, void*);
    /// @}

    /// @brief function called after undo/redo in the current frame (can be reimplemented in frame children)
    void updateFrameAfterUndoRedo();

protected:
    /// @brief FOX needs this
    GNEInspectorFrame() {}

    /// @brief Inspect a singe element (the front of AC AttributeCarriers of ObjectUnderCursor
    void inspectClickedElement(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const Position& clickedPosition);

    /// @brief function called after set a valid attribute in AttributeEditor
    void attributeUpdated();

private:
    /// @brief Overlapped Inspection
    OverlappedInspection* myOverlappedInspection;

    /// @brief Attribute editor
    GNEFrameAttributesModuls::AttributesEditor* myAttributesEditor;

    /// @brief Netedit Attributes editor
    NeteditAttributesEditor* myNeteditAttributesEditor;

    /// @brief GEO Attributes editor
    GEOAttributesEditor* myGEOAttributesEditor;

    /// @brief Generic parameters editor
    GNEFrameAttributesModuls::GenericParametersEditor* myGenericParametersEditor;

    /// @brief Template editor
    TemplateEditor* myTemplateEditor;

    /// @brief Attribute Carrier Hierarchy
    GNEFrameModuls::AttributeCarrierHierarchy* myAttributeCarrierHierarchy;

    /// @brief back Button
    FXButton* myBackButton;

    /// @brief pointer to previous element called by Inspector Frame
    GNEAttributeCarrier* myPreviousElementInspect;

    /// @brief pointer to previous element called by Delete Frame
    GNEAttributeCarrier* myPreviousElementDelete;

    /// @brief flag to ckec if myPreviousElementDelete was marked in Delete Frame
    bool myPreviousElementDeleteWasMarked;
};


#endif

/****************************************************************************/

