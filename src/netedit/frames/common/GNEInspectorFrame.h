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
/// @file    GNEInspectorFrame.h
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2011
///
// The Widget for modifying network-element attributes (i.e. lane speed)
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>
#include <netedit/frames/GNEOverlappedInspection.h>
#include <netedit/frames/GNEElementTree.h>

// ===========================================================================
// class declaration
// ===========================================================================
class GNEEdgeTemplate;

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

    /// @brief declare friend class
    friend class GNEFrameAttributeModules::ParametersEditor;

public:
    // ===========================================================================
    // class NeteditAttributesEditor
    // ===========================================================================

    class NeteditAttributesEditor : public MFXGroupBoxModule {
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

        /// @brief check if we're selecting a new parent
        bool isSelectingParent() const;

        /// @brief set new parent
        void setNewParent(GNEAttributeCarrier* clickedAC);

        /// @brief stop select new parent
        void stopSelectParent();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user change the current GEO Attribute
        long onCmdSetNeteditAttribute(FXObject*, FXSelector, void*);

        /// @brief Called when user press button "Mark front element"
        long onCmdMarkFrontElement(FXObject*, FXSelector, void*);

        /// @brief Called when user press the help button
        long onCmdNeteditAttributeHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(NeteditAttributesEditor)

    private:
        /// @brief pointer to inspector frame parent
        GNEInspectorFrame* myInspectorFrameParent;

        /// @brief button for set element as front button
        FXButton* myMarkFrontElementButton;

        /// @brief Label for parent additional
        FXLabel* myLabelParentAdditional;

        /// @brief pointer for replace the parent additional
        FXTextField* myTextFieldParentAdditional;

        /// @brief button for set new parent
        MFXCheckableButton* mySetNewParentButton;

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

    class GEOAttributesEditor : public MFXGroupBoxModule {
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
        /// @brief FOX need this
        FOX_CONSTRUCTOR(GEOAttributesEditor)

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

    class TemplateEditor : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEInspectorFrame::TemplateEditor)

    public:
        /// @brief constructor
        TemplateEditor(GNEInspectorFrame* inspectorFrameParent);

        /// @brief destructor
        ~TemplateEditor();

        /// @brief show template editor
        bool showTemplateEditor();

        /// @brief hide template editor
        void hideTemplateEditor();

        /// @brief get edge template (to copy attributes from)
        GNEEdgeTemplate* getEdgeTemplate() const;

        /// @brief set edge template
        void setEdgeTemplate(const GNEEdge* edge);

        /// @brief update edge template
        void updateEdgeTemplate();

        /// @brief set template (used by shortcut)
        void setTemplate();

        /// @brief copy template (used by shortcut)
        void copyTemplate();

        /// @brief clear template (used by shortcut)
        void clearTemplate();

        /// @name FOX-callbacks
        /// @{
        /// @brief set current edge as new template
        long onCmdSetTemplate(FXObject*, FXSelector, void*);

        /// @brief copy edge attributes from edge template
        long onCmdCopyTemplate(FXObject*, FXSelector, void*);

        /// @brief clear current edge template
        long onCmdClearTemplate(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(TemplateEditor)

        /// @brief update frame buttons
        void updateButtons();

    private:
        /// @brief current GNEInspectorFrame parent
        GNEInspectorFrame* myInspectorFrameParent;

        /// @brief set template button
        FXButton* mySetTemplateButton;

        /// @brief copy template button
        FXButton* myCopyTemplateButton;

        /// @brief clear template button
        FXButton* myClearTemplateButton;

        /// @brief edge Template
        GNEEdgeTemplate* myEdgeTemplate;
    };

    // ===========================================================================
    // class AdditionalDialog
    // ===========================================================================

    class AdditionalDialog : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEInspectorFrame::AdditionalDialog)

    public:
        /// @brief constructor
        AdditionalDialog(GNEInspectorFrame* inspectorFrameParent);

        /// @brief destructor
        ~AdditionalDialog();

        /// @brief show netedit attributes editor
        void showAdditionalDialog();

        /// @brief hide netedit attributes editor
        void hideAdditionalDialog();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user click over open additional dialog
        long onCmdOpenAdditionalDialog(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(AdditionalDialog)

    private:
        /// @brief pointer to inspector frame parent
        GNEInspectorFrame* myInspectorFrameParent;

        /// @brief button for open additional dialog
        FXButton* myOpenAdditionalDialog;
    };

    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief net net that uses this GNEFrame
     */
    GNEInspectorFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEInspectorFrame();

    /// @brief show inspector frame
    void show();

    /// @brief hide inspector frame
    void hide();

    /**@brief process click over Viewnet in Supermode Network
     * @param[in] clickedPosition clicked position over ViewNet
     * @param[in] viewObjects objects under cursors
     * @return true if something was sucefully done
     */
    bool processNetworkSupermodeClick(const Position& clickedPosition, GNEViewNetHelper::ViewObjectsSelector& viewObjects);

    /**@brief process click over Viewnet in Supermode Demand
     * @param[in] clickedPosition clicked position over ViewNet
     * @param[in] viewObjects objects under cursors
     * @return true if something was sucefully done
     */
    bool processDemandSupermodeClick(const Position& clickedPosition, GNEViewNetHelper::ViewObjectsSelector& viewObjects);

    /**@brief process click over Viewnet in Supermode Data
     * @param[in] clickedPosition clicked position over ViewNet
     * @param[in] viewObjects objects under cursors
     * @return true if something was sucefully done
     */
    bool processDataSupermodeClick(const Position& clickedPosition, GNEViewNetHelper::ViewObjectsSelector& viewObjects);

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
    GNEFrameAttributeModules::AttributesEditor* getAttributesEditor() const;

    /// @brief get Netedit Attributes editor
    GNEInspectorFrame::NeteditAttributesEditor* getNeteditAttributesEditor() const;

    /// @brief get template editor
    TemplateEditor* getTemplateEditor() const;

    /// @brief get GNEOverlappedInspection modul
    GNEOverlappedInspection* getOverlappedInspection() const;

    /// @brief get GNEElementTree modul
    GNEElementTree* getHierarchicalElementTree() const;

    /// @name FOX-callbacks
    /// @{

    /// @brief called when user toggle the go back button
    long onCmdGoBack(FXObject*, FXSelector, void*);
    /// @}

    /// @brief function called after undo/redo in the current frame (can be reimplemented in frame children)
    void updateFrameAfterUndoRedo();

    /// @brief open GNEAttributesCreator extended dialog (can be reimplemented in frame children)
    void selectedOverlappedElement(GNEAttributeCarrier* AC);

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEInspectorFrame)

    /// @brief Inspect a singe element (the front of AC AttributeCarriers of ObjectUnderCursor
    void inspectClickedElement(const GNEViewNetHelper::ViewObjectsSelector& viewObjects, const Position& clickedPosition);

    /// @brief function called after set a valid attribute in AttributeEditor
    void attributeUpdated(SumoXMLAttr attribute);

private:
    /// @brief Overlapped Inspection
    GNEOverlappedInspection* myOverlappedInspection;

    /// @brief Attribute editor
    GNEFrameAttributeModules::AttributesEditor* myAttributesEditor;

    /// @brief Netedit Attributes editor
    NeteditAttributesEditor* myNeteditAttributesEditor;

    /// @brief GEO Attributes editor
    GEOAttributesEditor* myGEOAttributesEditor;

    /// @brief Parameters editor inspector
    GNEFrameAttributeModules::ParametersEditor* myParametersEditor;

    /// @brief Additional dialog
    AdditionalDialog* myAdditionalDialog;

    /// @brief Template editor
    TemplateEditor* myTemplateEditor;

    /// @brief Attribute Carrier Hierarchy
    GNEElementTree* myHierarchicalElementTree;

    /// @brief Back Button
    FXButton* myBackButton;

    /// @brief Pointer to previous element called by Inspector Frame
    GNEAttributeCarrier* myPreviousElementInspect;

    /// @brief pointer to previous element called by Delete Frame
    GNEAttributeCarrier* myPreviousElementDelete;

    /// @brief flag to ckec if myPreviousElementDelete was marked in Delete Frame
    bool myPreviousElementDeleteWasMarked;
};
