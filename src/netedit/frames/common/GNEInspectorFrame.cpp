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
/// @file    GNEInspectorFrame.cpp
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2011
///
// The Widget for modifying network-element attributes (i.e. lane speed)
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/network/GNEEdgeTemplate.h>
#include <netedit/frames/GNEAttributesEditor.h>
#include <netedit/frames/GNEElementTree.h>
#include <netedit/frames/GNEOverlappedInspection.h>
#include <netedit/frames/network/GNECreateEdgeFrame.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEInspectorFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEInspectorFrame) GNEInspectorFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_INSPECTORFRAME_INSPECTPREVIOUSELEMENT,  GNEInspectorFrame::onCmdInspectPreviousElement)
};

FXDEFMAP(GNEInspectorFrame::TemplateEditor) TemplateEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_F1_TEMPLATE_SET,   GNEInspectorFrame::TemplateEditor::onCmdSetTemplate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_F2_TEMPLATE_COPY,  GNEInspectorFrame::TemplateEditor::onCmdCopyTemplate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_F3_TEMPLATE_CLEAR, GNEInspectorFrame::TemplateEditor::onCmdClearTemplate),
};

// Object implementation
FXIMPLEMENT(GNEInspectorFrame,                  FXVerticalFrame,    GNEInspectorFrameMap,   ARRAYNUMBER(GNEInspectorFrameMap))
FXIMPLEMENT(GNEInspectorFrame::TemplateEditor,  GNEGroupBoxModule,  TemplateEditorMap,      ARRAYNUMBER(TemplateEditorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEInspectorFrame::TemplateEditor - methods
// ---------------------------------------------------------------------------

GNEInspectorFrame::TemplateEditor::TemplateEditor(GNEInspectorFrame* inspectorFrameParent) :
    GNEGroupBoxModule(inspectorFrameParent, TL("Templates")),
    myInspectorFrameParent(inspectorFrameParent),
    myEdgeTemplate(nullptr) {
    // Create set template button
    mySetTemplateButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Set as Template"), "", "", nullptr, this, MID_HOTKEY_SHIFT_F1_TEMPLATE_SET, GUIDesignButton);
    // Create copy template button
    myCopyTemplateButton = GUIDesigns::buildFXButton(getCollapsableFrame(), "", "", "", nullptr, this, MID_HOTKEY_SHIFT_F2_TEMPLATE_COPY, GUIDesignButton);
    // Create copy template button
    myClearTemplateButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("clear Edge Template"), "", "", nullptr, this, MID_HOTKEY_SHIFT_F3_TEMPLATE_CLEAR, GUIDesignButton);
}


GNEInspectorFrame::TemplateEditor::~TemplateEditor() {
}


bool
GNEInspectorFrame::TemplateEditor::showTemplateEditor() {
    // show template editor only if we're editing an edge in Network mode AND we have at least one inspected edge
    if (myInspectorFrameParent->myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
        for (const auto& AC : myInspectorFrameParent->getViewNet()->getInspectedElements().getACs()) {
            if (AC->getTagProperty()->getTag() == SUMO_TAG_EDGE) {
                // update buttons and show module
                updateButtons();
                show();
                return true;
            }
        }
    }
    return false;
}


void
GNEInspectorFrame::TemplateEditor::hideTemplateEditor() {
    // hide template editor
    hide();
}


GNEEdgeTemplate*
GNEInspectorFrame::TemplateEditor::getEdgeTemplate() const {
    return myEdgeTemplate;
}


void
GNEInspectorFrame::TemplateEditor::setEdgeTemplate(const GNEEdge* edge) {
    // delete previous template edge
    if (myEdgeTemplate) {
        delete myEdgeTemplate;
        myEdgeTemplate = nullptr;
    }
    // update edge template
    if (edge) {
        myEdgeTemplate = new GNEEdgeTemplate(edge);
        // use template by default
        myInspectorFrameParent->myViewNet->getViewParent()->getCreateEdgeFrame()->setUseEdgeTemplate();
    }
}


void
GNEInspectorFrame::TemplateEditor::updateEdgeTemplate() {
    if (myEdgeTemplate) {
        myEdgeTemplate->updateLaneTemplates();
        // use template by default
        myInspectorFrameParent->myViewNet->getViewParent()->getCreateEdgeFrame()->setUseEdgeTemplate();
    }
}

void
GNEInspectorFrame::TemplateEditor::setTemplate() {
    // check if template editor AND mySetTemplateButton is enabled
    if (shown() && mySetTemplateButton->isEnabled()) {
        onCmdSetTemplate(nullptr, 0, nullptr);
    }
}


void
GNEInspectorFrame::TemplateEditor::copyTemplate() {
    // check if template editor AND myCopyTemplateButton is enabled
    if (shown() && myCopyTemplateButton->isEnabled()) {
        onCmdCopyTemplate(nullptr, 0, nullptr);
    }
}


void
GNEInspectorFrame::TemplateEditor::clearTemplate() {
    // check if template editor AND myClearTemplateButton is enabled
    if (shown() && myClearTemplateButton->isEnabled()) {
        onCmdClearTemplate(nullptr, 0, nullptr);
    }
}


long
GNEInspectorFrame::TemplateEditor::onCmdSetTemplate(FXObject*, FXSelector, void*) {
    // apply to all selected edges
    for (const auto& AC : myInspectorFrameParent->myViewNet->getInspectedElements().getACs()) {
        if (AC->getTagProperty()->getTag() == SUMO_TAG_EDGE) {
            // set template
            setEdgeTemplate(myInspectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->retrieveEdge(AC->getID()));
            // update buttons
            updateButtons();
        }
    }
    return 1;
}


long
GNEInspectorFrame::TemplateEditor::onCmdCopyTemplate(FXObject*, FXSelector, void*) {
    // first check
    if (myEdgeTemplate) {
        // begin copy template
        myInspectorFrameParent->myViewNet->getUndoList()->begin(myEdgeTemplate, "copy edge template");
        // iterate over inspected ACs
        for (const auto& AC : myInspectorFrameParent->myViewNet->getInspectedElements().getACs()) {
            // avoid copy template in the same edge
            if (AC->getID() != myEdgeTemplate->getID()) {
                // retrieve edge ID (and throw exception if edge doesn't exist)
                myInspectorFrameParent->myViewNet->getNet()->getAttributeCarriers()->retrieveEdge(AC->getID())->copyTemplate(myEdgeTemplate, myInspectorFrameParent->myViewNet->getUndoList());
            }
        }
        // end copy template
        myInspectorFrameParent->myViewNet->getUndoList()->end();
        // refresh inspector parent
        myInspectorFrameParent->myAttributesEditor->refreshAttributesEditor();
    }
    return 1;
}


long
GNEInspectorFrame::TemplateEditor::onCmdClearTemplate(FXObject*, FXSelector, void*) {
    // set null edge
    setEdgeTemplate(nullptr);
    // update buttons
    updateButtons();
    return 1;
}


void
GNEInspectorFrame::TemplateEditor::updateButtons() {
    const auto& inspectedElements = myInspectorFrameParent->getViewNet()->getInspectedElements();
    // only show set template button if we have exactly one inspected edge
    if (inspectedElements.isInspectingSingleElement() && (inspectedElements.getFirstAC()->getTagProperty()->getTag() == SUMO_TAG_EDGE)) {
        mySetTemplateButton->setText((TLF("Set edge '%' as Template", inspectedElements.getFirstAC()->getID())).c_str());
        mySetTemplateButton->show();
    } else {
        mySetTemplateButton->hide();
    }
    // enable or disable clear buttons depending of myEdgeTemplate
    if (myEdgeTemplate) {
        // update caption of copy button
        if (inspectedElements.isInspectingSingleElement()) {
            myCopyTemplateButton->setText(("Copy '" + myEdgeTemplate->getID() + "' into edge '" + inspectedElements.getFirstAC()->getID() + "'").c_str());
        } else {
            myCopyTemplateButton->setText(("Copy '" + myEdgeTemplate->getID() + "' into " + toString(inspectedElements.getACs().size()) + " selected edges").c_str());
        }
        // enable set and clear buttons
        myCopyTemplateButton->enable();
        myClearTemplateButton->enable();
    } else {
        // update caption of copy button
        myCopyTemplateButton->setText(TL("No edge Template Set"));
        // disable set and clear buttons
        myCopyTemplateButton->disable();
        myClearTemplateButton->disable();
    }
}

// ---------------------------------------------------------------------------
// GNEInspectorFrame - methods
// ---------------------------------------------------------------------------

GNEInspectorFrame::GNEInspectorFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, "Inspector") {

    // Create back button
    myBackButton = GUIDesigns::buildFXButton(myHeaderLeftFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::BIGARROWLEFT),
                   this, MID_GNE_INSPECTORFRAME_INSPECTPREVIOUSELEMENT, GUIDesignButtonRectangular);
    myHeaderLeftFrame->hide();
    myBackButton->hide();

    // Create Overlapped Inspection module
    myOverlappedInspection = new GNEOverlappedInspection(this, false);

    // Create Attributes Editor module
    myAttributesEditor = new GNEAttributesEditor(this, GNEAttributesEditorType::EditorType::EDITOR);

    // Create Template editor module
    myTemplateEditor = new TemplateEditor(this);

    // Create GNEElementTree module
    myHierarchicalElementTree = new GNEElementTree(this);
}


GNEInspectorFrame::~GNEInspectorFrame() {}


void
GNEInspectorFrame::show() {
    refreshInspection();
    // stop reparenting
    myAttributesEditor->abortReparenting();
    // show
    GNEFrame::show();
}


void
GNEInspectorFrame::hide() {
    myViewNet->getInspectedElements().inspectACs({});
    GNEFrame::hide();
}


bool
GNEInspectorFrame::inspectClickedElements(GNEViewNetHelper::ViewObjectsSelector& viewObjects,
        const Position& clickedPosition, const bool shiftKeyPressed) {
    // get unlocked attribute carrier front
    auto AC = viewObjects.getAttributeCarrierFront();
    // first check if we have clicked over an Attribute Carrier
    if (AC) {
        // if Control key is Pressed, select instead inspect element
        if (myViewNet->getMouseButtonKeyPressed().controlKeyPressed()) {
            // toggle networkElement selection
            if (AC->isAttributeCarrierSelected()) {
                AC->unselectAttributeCarrier();
            } else {
                AC->selectAttributeCarrier();
            }
        } else {
            // show Overlapped Inspection module
            myOverlappedInspection->showOverlappedInspection(viewObjects, clickedPosition, shiftKeyPressed);
            // focus upper element of inspector frame
            focusUpperElement();
        }
        return true;
    } else {
        return false;
    }
}


void
GNEInspectorFrame::inspectElement(GNEAttributeCarrier* AC, GNEAttributeCarrier* previousInspectedAC) {
    std::vector<GNEAttributeCarrier*> itemsToInspect;
    // Use the implementation of inspect for multiple AttributeCarriers to avoid repetition of code
    if (AC) {
        if (AC->isAttributeCarrierSelected() && !myViewNet->getMouseButtonKeyPressed().altKeyPressed()) {
            // obtain selected ACs depending of current supermode
            const auto selectedACs = myViewNet->getNet()->getAttributeCarriers()->getSelectedAttributeCarriers(false);
            // reserve space
            itemsToInspect.reserve(selectedACs.size());
            // iterate over selected ACs
            for (const auto& selectedAC : selectedACs) {
                // filter ACs to inspect using Tag as criterion
                if (selectedAC->getTagProperty()->getTag() == AC->getTagProperty()->getTag()) {
                    itemsToInspect.push_back(selectedAC);
                }
            }
        } else {
            itemsToInspect.push_back(AC);
        }
    }
    inspectElements(itemsToInspect, previousInspectedAC);
}

void
GNEInspectorFrame::inspectElements(const std::vector<GNEAttributeCarrier*>& ACs, GNEAttributeCarrier* previousInspectedAC) {
    myViewNet->getInspectedElements().inspectACs(ACs);
    myPreviousInspectedAC = previousInspectedAC;
    refreshInspection();
}


void
GNEInspectorFrame::clearInspection() {
    // simply clear overlapped inspection (it refresh inspector frame)
    myOverlappedInspection->clearOverlappedInspection();
}


void
GNEInspectorFrame::refreshInspection() {
    const auto& inspectedElements = myViewNet->getInspectedElements();
    // check if show back button
    if (myPreviousInspectedAC) {
        myHeaderLeftFrame->show();
        myBackButton->show();
    } else {
        myHeaderLeftFrame->hide();
        myBackButton->hide();
    }
    // Show all attribute editors (will be automatically hidden if there are no elements to inspect)
    myAttributesEditor->showAttributesEditor(inspectedElements.getACs(), true);
    // Hide other moduls
    myTemplateEditor->hideTemplateEditor();
    myHierarchicalElementTree->hideHierarchicalElementTree();
    // If vector of attribute Carriers contain data
    if (inspectedElements.isInspectingElements()) {
        // Set header
        std::string headerString;
        if (inspectedElements.getFirstAC()->getTagProperty()->isNetworkElement()) {
            headerString = "Net: ";
        } else if (inspectedElements.getFirstAC()->getTagProperty()->isAdditionalElement()) {
            headerString = "Additional: ";
        } else if (inspectedElements.getFirstAC()->getTagProperty()->isShapeElement()) {
            headerString = "Shape: ";
        } else if (inspectedElements.getFirstAC()->getTagProperty()->isTAZElement()) {
            headerString = "TAZ: ";
        } else if (inspectedElements.getFirstAC()->getTagProperty()->isWireElement()) {
            headerString = "WIRE: ";
        } else if (inspectedElements.getFirstAC()->getTagProperty()->isVehicle()) {
            headerString = "Vehicle: ";
        } else if (inspectedElements.getFirstAC()->getTagProperty()->isRoute()) {
            headerString = "Route: ";
        } else if (inspectedElements.getFirstAC()->getTagProperty()->isPerson()) {
            headerString = "Person: ";
        } else if (inspectedElements.getFirstAC()->getTagProperty()->isPlanPerson()) {
            headerString = "PersonPlan: ";
        } else if (inspectedElements.getFirstAC()->getTagProperty()->isContainer()) {
            headerString = "Container: ";
        } else if (inspectedElements.getFirstAC()->getTagProperty()->isPlanContainer()) {
            headerString = "ContainerPlan: ";
        } else if (inspectedElements.getFirstAC()->getTagProperty()->isVehicleStop()) {
            headerString = "Stop: ";
        } else if (inspectedElements.getFirstAC()->getTagProperty()->isDataElement()) {
            headerString = "Data: ";
        }
        if (myViewNet->getInspectedElements().isInspectingMultipleElements()) {
            headerString += toString(inspectedElements.getACs().size()) + " ";
        }
        headerString += inspectedElements.getFirstAC()->getTagStr();
        if (myViewNet->getInspectedElements().isInspectingMultipleElements()) {
            headerString += "s";
        }
        // Set headerString into header label
        getFrameHeaderLabel()->setText(headerString.c_str());

        // If attributes correspond to an Edge and we aren't in demand mode, show template editor
        myTemplateEditor->showTemplateEditor();

        // if we inspect a single Attribute carrier vector, show their children
        if (inspectedElements.isInspectingSingleElement()) {
            myHierarchicalElementTree->showHierarchicalElementTree(inspectedElements.getFirstAC());
        }
    } else {
        getFrameHeaderLabel()->setText(TL("Inspect"));
        myOverlappedInspection->hiderOverlappedInspection();
    }
    // update frame width
    setFrameWidth(myViewNet->getViewParent()->getFrameAreaWidth());
    // update viewNet
    myViewNet->update();
}


GNEAttributesEditor*
GNEInspectorFrame::getAttributesEditor() const {
    return myAttributesEditor;
}


GNEInspectorFrame::TemplateEditor*
GNEInspectorFrame::getTemplateEditor() const {
    return myTemplateEditor;
}


GNEOverlappedInspection*
GNEInspectorFrame::getOverlappedInspection() const {
    return myOverlappedInspection;
}


GNEElementTree*
GNEInspectorFrame::getHierarchicalElementTree() const {
    return myHierarchicalElementTree;
}


long
GNEInspectorFrame::onCmdInspectPreviousElement(FXObject*, FXSelector, void*) {
    inspectElement(myPreviousInspectedAC, nullptr);
    return 1;
}


void
GNEInspectorFrame::updateFrameAfterUndoRedo() {
    refreshInspection();
}


void
GNEInspectorFrame::selectedOverlappedElement(GNEAttributeCarrier* AC) {
    inspectElement(AC);
    // update view (due dotted contour)
    myViewNet->updateViewNet();
}

/****************************************************************************/
