/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
#include <netedit/GNEViewNetHelper.h>
#include <utils/foxtools/MFXGroupBoxModule.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAttributesEditor;
class GNEEdgeTemplate;
class GNEElementTree;
class GNEOverlappedInspection;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEInspectorFrame : public GNEFrame {
    /// @brief FOX-declaration
    FXDECLARE(GNEInspectorFrame)

public:
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

    /**@brief process click over Viewnet
     * @param[in] viewObjects objects under cursors
     * @return true if something was sucefully done
     */
    bool inspectClickedElements(GNEViewNetHelper::ViewObjectsSelector& viewObjects, const Position& clickedPosition,
                                const bool shiftKeyPressed);

    /// @brief Inspect a single element
    void inspectElement(GNEAttributeCarrier* AC, GNEAttributeCarrier* previousInspectedAC = nullptr);

    /// @brief Inspect the given elements
    void inspectElements(const std::vector<GNEAttributeCarrier*>& ACs, GNEAttributeCarrier* previousInspectedAC = nullptr);

    /// @brief clear inspection
    void clearInspection();

    /// @brief refresh current inspection
    void refreshInspection();

    /// @brief get AttributesEditor
    GNEAttributesEditor* getAttributesEditor() const;

    /// @brief get template editor
    TemplateEditor* getTemplateEditor() const;

    /// @brief get GNEOverlappedInspection modul
    GNEOverlappedInspection* getOverlappedInspection() const;

    /// @brief get GNEElementTree modul
    GNEElementTree* getHierarchicalElementTree() const;

    /// @name FOX-callbacks
    /// @{

    /// @brief called when user press inspet previous elemnt button
    long onCmdInspectPreviousElement(FXObject*, FXSelector, void*);
    /// @}

    /// @brief function called after undo/redo in the current frame (can be reimplemented in frame children)
    void updateFrameAfterUndoRedo();

    /// @brief open GNEAttributesCreator extended dialog (can be reimplemented in frame children)
    void selectedOverlappedElement(GNEAttributeCarrier* AC);

protected:
    /// @brief FOX need this
    FOX_CONSTRUCTOR(GNEInspectorFrame)

private:
    /// @brief Overlapped Inspection
    GNEOverlappedInspection* myOverlappedInspection = nullptr;

    /// @brief Attributes editor
    GNEAttributesEditor* myAttributesEditor = nullptr;

    /// @brief Template editor
    TemplateEditor* myTemplateEditor = nullptr;

    /// @brief Attribute Carrier Hierarchy
    GNEElementTree* myHierarchicalElementTree = nullptr;

    /// @brief Back Button
    FXButton* myBackButton = nullptr;

    /// @brief Pointer to previous element inspected
    GNEAttributeCarrier* myPreviousInspectedAC = nullptr;
};
