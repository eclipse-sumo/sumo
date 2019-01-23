/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEInspectorFrame.cpp
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2011
/// @version $Id$
///
// The Widget for modifying network-element attributes (i.e. lane speed)
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/foxtools/MFXUtils.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNEPOI.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/dialogs/GNEDialog_AllowDisallow.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>

#include "GNEInspectorFrame.h"
#include "GNEDeleteFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEInspectorFrame::OverlappedInspection) OverlappedInspectionMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_INSPECTORFRAME_NEXT,            GNEInspectorFrame::OverlappedInspection::onCmdNextElement),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_INSPECTORFRAME_PREVIOUS,        GNEInspectorFrame::OverlappedInspection::onCmdPreviousElement),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_INSPECTORFRAME_SHOWLIST,        GNEInspectorFrame::OverlappedInspection::onCmdShowList),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_INSPECTORFRAME_ITEMSELECTED,    GNEInspectorFrame::OverlappedInspection::onCmdListItemSelected),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                               GNEInspectorFrame::OverlappedInspection::onCmdOverlappingHelp),
};

FXDEFMAP(GNEInspectorFrame) GNEInspectorFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_INSPECTORFRAME_GOBACK,  GNEInspectorFrame::onCmdGoBack),
};

FXDEFMAP(GNEInspectorFrame::AttributesEditor::AttributeInput) AttributeInputMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEInspectorFrame::AttributesEditor::AttributeInput::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEInspectorFrame::AttributesEditor::AttributeInput::onCmdOpenAttributeDialog)
};

FXDEFMAP(GNEInspectorFrame::AttributesEditor) AttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNEInspectorFrame::AttributesEditor::onCmdAttributeHelp),
};

FXDEFMAP(GNEInspectorFrame::NeteditAttributesEditor) NeteditAttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEInspectorFrame::NeteditAttributesEditor::onCmdSetNeteditAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNEInspectorFrame::NeteditAttributesEditor::onCmdNeteditAttributeHelp),
};

FXDEFMAP(GNEInspectorFrame::GEOAttributesEditor) GEOAttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEInspectorFrame::GEOAttributesEditor::onCmdSetGEOAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNEInspectorFrame::GEOAttributesEditor::onCmdGEOAttributeHelp),
};

FXDEFMAP(GNEInspectorFrame::TemplateEditor) TemplateEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_INSPECTORFRAME_COPYTEMPLATE,    GNEInspectorFrame::TemplateEditor::onCmdCopyTemplate),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_INSPECTORFRAME_SETTEMPLATE,     GNEInspectorFrame::TemplateEditor::onCmdSetTemplate),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_INSPECTORFRAME_COPYTEMPLATE,    GNEInspectorFrame::TemplateEditor::onUpdCopyTemplate),
};


// Object implementation
FXIMPLEMENT(GNEInspectorFrame,                                      FXVerticalFrame,    GNEInspectorFrameMap,       ARRAYNUMBER(GNEInspectorFrameMap))
FXIMPLEMENT(GNEInspectorFrame::OverlappedInspection,                FXGroupBox,         OverlappedInspectionMap,    ARRAYNUMBER(OverlappedInspectionMap))
FXIMPLEMENT(GNEInspectorFrame::AttributesEditor::AttributeInput,    FXHorizontalFrame,  AttributeInputMap,          ARRAYNUMBER(AttributeInputMap))
FXIMPLEMENT(GNEInspectorFrame::AttributesEditor,                    FXGroupBox,         AttributesEditorMap,        ARRAYNUMBER(AttributesEditorMap))
FXIMPLEMENT(GNEInspectorFrame::NeteditAttributesEditor,             FXGroupBox,         NeteditAttributesEditorMap, ARRAYNUMBER(NeteditAttributesEditorMap))
FXIMPLEMENT(GNEInspectorFrame::GEOAttributesEditor,                 FXGroupBox,         GEOAttributesEditorMap,     ARRAYNUMBER(GEOAttributesEditorMap))
FXIMPLEMENT(GNEInspectorFrame::TemplateEditor,                      FXGroupBox,         TemplateEditorMap,          ARRAYNUMBER(TemplateEditorMap))


// ===========================================================================
// method definitions
// ===========================================================================

GNEInspectorFrame::GNEInspectorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet):
    GNEFrame(horizontalFrameParent, viewNet, "Inspector"),
    myPreviousElementInspect(nullptr),
    myPreviousElementDelete(nullptr) {

    // Create back button
    myBackButton = new FXButton(myHeaderLeftFrame, "", GUIIconSubSys::getIcon(ICON_NETEDITARROWLEFT), this, MID_GNE_INSPECTORFRAME_GOBACK, GUIDesignButtonIconRectangular);
    myHeaderLeftFrame->hide();
    myBackButton->hide();

    // Create Overlapped Inspection modul
    myOverlappedInspection = new OverlappedInspection(this);

    // Create Attributes Editor modul
    myAttributesEditor = new AttributesEditor(this);

    // Create GEO Parameters Editor modul
    myGEOAttributesEditor = new GEOAttributesEditor(this);

    // create Generic parameters Editor modul
    myGenericParametersEditor = new GenericParametersEditor(this);

    // Create Netedit Attributes Editor modul
    myNeteditAttributesEditor = new NeteditAttributesEditor(this);

    // Create Template editor modul
    myTemplateEditor = new TemplateEditor(this);

    // Create ACHierarchy modul
    myACHierarchy = new GNEFrame::ACHierarchy(this);
}


GNEInspectorFrame::~GNEInspectorFrame() {}


void
GNEInspectorFrame::show() {
    // inspect a null element to reset inspector frame
    inspectSingleElement(nullptr);
    GNEFrame::show();
}


void
GNEInspectorFrame::hide() {
    myInspectedACs.clear();
    myViewNet->setDottedAC(nullptr);
    GNEFrame::hide();
}


bool
GNEInspectorFrame::processNetworkSupermodeClick(const Position& clickedPosition, GNEViewNet::ObjectsUnderCursor& objectsUnderCursor) {
    // first check if we have clicked over an Attribute Carrier
    if (objectsUnderCursor.getAttributeCarrierFront()) {
        // change the selected attribute carrier if mySelectEdges is enabled and clicked element is a getLaneFront() and shift key isn't pressed
        if (!myViewNet->getKeyPressed().shiftKeyPressed() && myViewNet->getViewOptions().selectEdges() && (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_LANE)) {
            objectsUnderCursor.swapLane2Edge();
        }
        // if Control key is Pressed, select instead inspect element
        if (myViewNet->getKeyPressed().controlKeyPressed()) {
            // Check if this GLobject type is locked
            if (!myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->IsObjectTypeLocked(objectsUnderCursor.getGlTypeFront())) {
                // toogle netElement selection
                if (objectsUnderCursor.getAttributeCarrierFront()->isAttributeCarrierSelected()) {
                    objectsUnderCursor.getAttributeCarrierFront()->unselectAttributeCarrier();
                } else {
                    objectsUnderCursor.getAttributeCarrierFront()->selectAttributeCarrier();
                }
            }
        } else {
            // first check if we clicked over a OverlappedInspection point
            if (myViewNet->getKeyPressed().shiftKeyPressed()) {
                if (!myOverlappedInspection->previousElement(clickedPosition)) {
                    // inspect attribute carrier, (or multiselection if AC is selected)
                    inspectClickedElement(objectsUnderCursor, clickedPosition);
                }
            } else  if (!myOverlappedInspection->nextElement(clickedPosition)) {
                // inspect attribute carrier, (or multiselection if AC is selected)
                inspectClickedElement(objectsUnderCursor, clickedPosition);
            }
            // focus upper element of inspector frame
            focusUpperElement();
        }
        return true;
    } else {
        return false;
    }
}


bool
GNEInspectorFrame::processDemandSupermodeClick(const Position& clickedPosition, GNEViewNet::ObjectsUnderCursor& objectsUnderCursor) {
    // first check if we have clicked over a demand element
    if (objectsUnderCursor.getDemandElementFront()) {
        // if Control key is Pressed, select instead inspect element
        if (myViewNet->getKeyPressed().controlKeyPressed()) {
            // Check if this GLobject type is locked
            if (!myViewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->IsObjectTypeLocked(objectsUnderCursor.getGlTypeFront())) {
                // toogle netElement selection
                if (objectsUnderCursor.getAttributeCarrierFront()->isAttributeCarrierSelected()) {
                    objectsUnderCursor.getAttributeCarrierFront()->unselectAttributeCarrier();
                } else {
                    objectsUnderCursor.getAttributeCarrierFront()->selectAttributeCarrier();
                }
            }
        } else {
            // first check if we clicked over a OverlappedInspection point
            if (myViewNet->getKeyPressed().shiftKeyPressed()) {
                if (!myOverlappedInspection->previousElement(clickedPosition)) {
                    // inspect attribute carrier, (or multiselection if AC is selected)
                    inspectClickedElement(objectsUnderCursor, clickedPosition);
                }
            } else  if (!myOverlappedInspection->nextElement(clickedPosition)) {
                // inspect attribute carrier, (or multiselection if AC is selected)
                inspectClickedElement(objectsUnderCursor, clickedPosition);
            }
            // focus upper element of inspector frame
            focusUpperElement();
        }
        return true;
    } else {
        return false;
    }
}


void
GNEInspectorFrame::inspectSingleElement(GNEAttributeCarrier* AC) {
    // Use the implementation of inspect for multiple AttributeCarriers to avoid repetition of code
    std::vector<GNEAttributeCarrier*> itemsToInspect;
    if (AC != nullptr) {
        myViewNet->setDottedAC(AC);
        if (AC->isAttributeCarrierSelected()) {
            // obtain selected ACs depending of current supermode
            std::vector<GNEAttributeCarrier*> selectedACs = myViewNet->getNet()->getSelectedAttributeCarriers(false);
            // iterate over selected ACs
            for (const auto& i : selectedACs) {
                // filter ACs to inspect using Tag as criterium
                if (i->getTagProperty().getTag() == AC->getTagProperty().getTag()) {
                    itemsToInspect.push_back(i);
                }
            }
        } else {
            itemsToInspect.push_back(AC);
        }
    }
    inspectMultisection(itemsToInspect);
}


void
GNEInspectorFrame::inspectMultisection(const std::vector<GNEAttributeCarrier*>& ACs) {
    // hide back button
    myHeaderLeftFrame->hide();
    myBackButton->hide();
    // Assing ACs to myACs
    myInspectedACs = ACs;
    // Hide all elements
    myAttributesEditor->hideAttributesEditorModul();
    myNeteditAttributesEditor->hideNeteditAttributesEditor();
    myGEOAttributesEditor->hideGEOAttributesEditor();
    myGenericParametersEditor->hideGenericParametersEditor();
    myTemplateEditor->hideTemplateEditor();
    myACHierarchy->hideACHierarchy();
    myOverlappedInspection->hideOverlappedInspection();
    // If vector of attribute Carriers contain data
    if (myInspectedACs.size() > 0) {
        // Set header
        std::string headerString;
        if (dynamic_cast<GNENetElement*>(myInspectedACs.front())) {
            headerString = "Net: ";
        } else if (dynamic_cast<GNEAdditional*>(myInspectedACs.front())) {
            headerString = "Additional: ";
        } else if (dynamic_cast<GNEShape*>(myInspectedACs.front())) {
            headerString = "Shape: ";
        }
        if (myInspectedACs.size() > 1) {
            headerString += toString(myInspectedACs.size()) + " ";
        }
        headerString += myInspectedACs.front()->getTagStr();
        if (myInspectedACs.size() > 1) {
            headerString += "s";
        }
        // Set headerString into header label
        getFrameHeaderLabel()->setText(headerString.c_str());

        // Show attributes editor
        myAttributesEditor->showAttributeEditorModul();

        // show netedit attributes editor if  we're inspecting elements with Netedit Attributes
        myNeteditAttributesEditor->showNeteditAttributesEditor();

        // Show GEO Attributes Editor if we're inspecting elements with GEO Attributes
        myGEOAttributesEditor->showGEOAttributesEditor();

        // show generic attributes editor
        if (myInspectedACs.size() == 1) {
            myGenericParametersEditor->showGenericParametersEditor(myInspectedACs.front());
        } else {
            myGenericParametersEditor->showGenericParametersEditor(myInspectedACs);
        }

        // If attributes correspond to an Edge and we aren't in demand mode, show template editor
        myTemplateEditor->showTemplateEditor();

        // if we inspect a single Attribute carrier vector, show their childs
        if (myInspectedACs.size() == 1) {
            myACHierarchy->showACHierarchy(myInspectedACs.front());
        }
    } else {
        getFrameHeaderLabel()->setText("Inspect");
        myContentFrame->recalc();
    }
}


void
GNEInspectorFrame::inspectChild(GNEAttributeCarrier* AC, GNEAttributeCarrier* previousElement) {
    // Show back button if myPreviousElementInspect was defined
    myPreviousElementInspect = previousElement;
    if (myPreviousElementInspect != nullptr) {
        // disable myPreviousElementDelete to avoid inconsistences
        myPreviousElementDelete = nullptr;
        inspectSingleElement(AC);
        myHeaderLeftFrame->show();
        myBackButton->show();
    }
}


void
GNEInspectorFrame::inspectFromDeleteFrame(GNEAttributeCarrier* AC, GNEAttributeCarrier* previousElement, bool previousElementWasMarked) {
    myPreviousElementDelete = previousElement;
    myPreviousElementDeleteWasMarked = previousElementWasMarked;
    // Show back button if myPreviousElementDelete is valid
    if (myPreviousElementDelete != nullptr) {
        // disable myPreviousElementInspect to avoid inconsistences
        myPreviousElementInspect = nullptr;
        inspectSingleElement(AC);
        myHeaderLeftFrame->show();
        myBackButton->show();
    }
}

void
GNEInspectorFrame::removeInspectedAC(GNEAttributeCarrier* ac) {
    // Only remove if there is inspected ACs
    if (myInspectedACs.size() > 0) {
        // Try to find AC in myACs
        auto i = std::find(myInspectedACs.begin(), myInspectedACs.end(), ac);
        // if was found
        if (i != myInspectedACs.end()) {
            // erase AC from inspected ACs
            myInspectedACs.erase(i);
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG("Removed inspected element from Inspected ACs. " + toString(myInspectedACs.size()) + " ACs remains.");
            // Inspect multi selection again
            inspectMultisection(myInspectedACs);
        }
    }
}


void
GNEInspectorFrame::clearInspectedAC() {
    // Only remove if there is inspected ACs
    if (myInspectedACs.size() > 0) {
        // clear ACs
        myInspectedACs.clear();
        myViewNet->setDottedAC(nullptr);
        // Inspect multi selection again (to hide all Editors)
        inspectMultisection(myInspectedACs);
    }
}


GNEFrame::ACHierarchy*
GNEInspectorFrame::getACHierarchy() const {
    return myACHierarchy;
}


GNEInspectorFrame::TemplateEditor*
GNEInspectorFrame::getTemplateEditor() const {
    return myTemplateEditor;
}


GNEInspectorFrame::OverlappedInspection*
GNEInspectorFrame::getOverlappedInspection() const {
    return myOverlappedInspection;
}


long
GNEInspectorFrame::onCmdGoBack(FXObject*, FXSelector, void*) {
    // Inspect previous element or go back to Delete Frame
    if (myPreviousElementInspect) {
        inspectSingleElement(myPreviousElementInspect);
        myPreviousElementInspect = nullptr;
    } else if (myPreviousElementDelete != nullptr) {
        myPreviousElementDelete = nullptr;
        // Hide inspect frame and show delete frame
        hide();
        myViewNet->getViewParent()->getDeleteFrame()->show();
    }
    return 1;
}


const std::vector<GNEAttributeCarrier*>&
GNEInspectorFrame::getInspectedACs() const {
    return myInspectedACs;
}


void
GNEInspectorFrame::inspectClickedElement(const GNEViewNet::ObjectsUnderCursor& objectsUnderCursor, const Position& clickedPosition) {
    if (objectsUnderCursor.getAttributeCarrierFront()) {
        // inspect front element
        inspectSingleElement(objectsUnderCursor.getAttributeCarrierFront());
        // if element has overlapped elements, show Overlapped Inspection modul
        if (objectsUnderCursor.getClickedAttributeCarriers().size() > 1) {
            myOverlappedInspection->showOverlappedInspection(objectsUnderCursor, clickedPosition);
        } else {
            myOverlappedInspection->hideOverlappedInspection();
        }
    }
}

// ---------------------------------------------------------------------------
// GNEInspectorFrame::OverlappedInspection - methods
// ---------------------------------------------------------------------------

GNEInspectorFrame::OverlappedInspection::OverlappedInspection(GNEInspectorFrame* inspectorFrameParent) :
    FXGroupBox(inspectorFrameParent->myContentFrame, "Overlapped elements", GUIDesignGroupBoxFrame),
    myInspectorFrameParent(inspectorFrameParent),
    myItemIndex(0) {
    FXHorizontalFrame* frameButtons = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // Create previous Item Button
    myPreviousElement = new FXButton(frameButtons, "", GUIIconSubSys::getIcon(ICON_NETEDITARROWLEFT), this, MID_GNE_INSPECTORFRAME_PREVIOUS, GUIDesignButtonIconRectangular);
    // create current index button
    myCurrentIndexButton = new FXButton(frameButtons, "", nullptr, this, MID_GNE_INSPECTORFRAME_SHOWLIST, GUIDesignButton);
    // Create next Item Button
    myNextElement = new FXButton(frameButtons, "", GUIIconSubSys::getIcon(ICON_NETEDITARROWRIGHT), this, MID_GNE_INSPECTORFRAME_NEXT, GUIDesignButtonIconRectangular);
    // Create list of overlapped elements (by default hidden)
    myOverlappedElementList = new FXList(this, this, MID_GNE_INSPECTORFRAME_ITEMSELECTED, GUIDesignListSingleElement);
    // disable vertical scrolling
    myOverlappedElementList->setScrollStyle(VSCROLLING_OFF);
    // by default list of overlapped elements is hidden)
    myOverlappedElementList->hide();
    // Create help button
    myHelpButton = new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEInspectorFrame::OverlappedInspection::~OverlappedInspection() {}


void
GNEInspectorFrame::OverlappedInspection::showOverlappedInspection(const GNEViewNet::ObjectsUnderCursor& objectsUnderCursor, const Position& clickedPosition) {
    myOverlappedACs = objectsUnderCursor.getClickedAttributeCarriers();
    mySavedClickedPosition = clickedPosition;
    // by default we inspect first element
    myItemIndex = 0;
    // update text of current index button
    myCurrentIndexButton->setText(("1 / " + toString(myOverlappedACs.size())).c_str());
    // clear and fill list again
    myOverlappedElementList->clearItems();
    for (int i = 0; i < (int)objectsUnderCursor.getClickedAttributeCarriers().size(); i++) {
        myOverlappedElementList->insertItem(i, objectsUnderCursor.getClickedAttributeCarriers().at(i)->getID().c_str(), objectsUnderCursor.getClickedAttributeCarriers().at(i)->getIcon());
    }
    // set first element as selected element
    myOverlappedElementList->getItem(0)->setSelected(TRUE);
    // by default list hidden
    myOverlappedElementList->hide();
    // show template editor
    show();
}


void
GNEInspectorFrame::OverlappedInspection::hideOverlappedInspection() {
    // hide template editor
    hide();
}


bool
GNEInspectorFrame::OverlappedInspection::overlappedInspectionShown() const {
    return shown();
}


bool
GNEInspectorFrame::OverlappedInspection::checkSavedPosition(const Position& clickedPosition) const {
    return (mySavedClickedPosition.distanceSquaredTo2D(clickedPosition) < 0.25);
}


bool
GNEInspectorFrame::OverlappedInspection::nextElement(const Position& clickedPosition) {
    // first check if OverlappedInspection is shown
    if (shown()) {
        // check if given position is near saved position
        if (checkSavedPosition(clickedPosition)) {
            // inspect next element
            onCmdNextElement(0, 0, 0);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GNEInspectorFrame::OverlappedInspection::previousElement(const Position& clickedPosition) {
    // first check if OverlappedInspection is shown
    if (shown()) {
        // check if given position is near saved position
        if (checkSavedPosition(clickedPosition)) {
            // inspect previousElement
            onCmdPreviousElement(0, 0, 0);
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


long
GNEInspectorFrame::OverlappedInspection::onCmdPreviousElement(FXObject*, FXSelector, void*) {
    // unselect current list element
    myOverlappedElementList->getItem((int)myItemIndex)->setSelected(FALSE);
    // set index (it works as a ring)
    if (myItemIndex > 0) {
        myItemIndex--;
    } else {
        myItemIndex = (myOverlappedACs.size() - 1);
    }
    // selected current list element
    myOverlappedElementList->getItem((int)myItemIndex)->setSelected(TRUE);
    // inspect overlapped attribute carrier
    inspectOverlappedAttributeCarrier();
    return 1;
}


long
GNEInspectorFrame::OverlappedInspection::onCmdNextElement(FXObject*, FXSelector, void*) {
    // unselect current list element
    myOverlappedElementList->getItem((int)myItemIndex)->setSelected(FALSE);
    // set index (it works as a ring)
    myItemIndex = (myItemIndex + 1) % myOverlappedACs.size();
    // selected current list element
    myOverlappedElementList->getItem((int)myItemIndex)->setSelected(TRUE);
    // inspect overlapped attribute carrier
    inspectOverlappedAttributeCarrier();
    return 1;
}


long
GNEInspectorFrame::OverlappedInspection::onCmdShowList(FXObject*, FXSelector, void*) {
    // show or hidde element list
    if (myOverlappedElementList->shown()) {
        myOverlappedElementList->hide();
    } else {
        myOverlappedElementList->show();
    }
    myOverlappedElementList->recalc();
    // recalc and update frame
    recalc();
    return 1;
}

long
GNEInspectorFrame::OverlappedInspection::onCmdListItemSelected(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myOverlappedElementList->getNumItems(); i++) {
        if (myOverlappedElementList->getItem(i)->isSelected()) {
            myItemIndex = i;
            // inspect overlapped attribute carrier
            inspectOverlappedAttributeCarrier();
            return 1;
        }
    }
    return 0;
}


long
GNEInspectorFrame::OverlappedInspection::onCmdOverlappingHelp(FXObject*, FXSelector, void*) {
    FXDialogBox* helpDialog = new FXDialogBox(this, "GEO attributes Help", GUIDesignDialogBox);
    std::ostringstream help;
    help
            << " - Click in the same position\n"
            << "   for inspect next element\n"
            << " - Shift + Click in the same\n"
            << "   position for inspect\n"
            << "   previous element";
    new FXLabel(helpDialog, help.str().c_str(), nullptr, GUIDesignLabelFrameInformation);
    // "OK"
    new FXButton(helpDialog, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), helpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    helpDialog->create();
    helpDialog->show();
    return 1;
}


void
GNEInspectorFrame::OverlappedInspection::inspectOverlappedAttributeCarrier() {
    // change current inspected item
    GNEAttributeCarrier* AC = myOverlappedACs.at(myItemIndex);
    // if is an lane and selectEdges checkBox is enabled, inspect their edge
    if (AC->getTagProperty().getTag() == SUMO_TAG_LANE && myInspectorFrameParent->getViewNet()->getViewOptions().selectEdges()) {
        myInspectorFrameParent->inspectSingleElement(&dynamic_cast<GNELane*>(AC)->getParentEdge());
    } else {
        myInspectorFrameParent->inspectSingleElement(AC);
    }
    // show OverlappedInspection again (because it's hidden in inspectSingleElement)
    show();
    // update current index button
    myCurrentIndexButton->setText((toString(myItemIndex + 1) + " / " + toString(myOverlappedACs.size())).c_str());
    // update view (due dotted contour)
    myInspectorFrameParent->getViewNet()->update();
}

// ---------------------------------------------------------------------------
// GNEInspectorFrame::AttributesEditor::AttributeInput - methods
// ---------------------------------------------------------------------------

GNEInspectorFrame::AttributesEditor::AttributeInput::AttributeInput(GNEInspectorFrame::AttributesEditor* attributeEditorParent) :
    FXHorizontalFrame(attributeEditorParent, GUIDesignAuxiliarHorizontalFrame),
    myAttributesEditorParent(attributeEditorParent),
    myTag(SUMO_TAG_NOTHING),
    myAttr(SUMO_ATTR_NOTHING),
    myMultiple(false) {
    // Create and hide label
    myLabel = new FXLabel(this, "attributeLabel", nullptr, GUIDesignLabelAttribute);
    myLabel->hide();
    // Create and hide ButtonCombinableChoices
    myButtonCombinableChoices = new FXButton(this, "AttributeButton", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myButtonCombinableChoices->hide();
    // create and hidde color editor
    myColorEditor = new FXButton(this, "ColorButton", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myColorEditor->hide();
    // Create and hide textField for int attributes
    myTextFieldInt = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldInt);
    myTextFieldInt->hide();
    // Create and hide textField for real/time attributes
    myTextFieldReal = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myTextFieldReal->hide();
    // Create and hide textField for string attributes
    myTextFieldStrings = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myTextFieldStrings->hide();
    // Create and hide ComboBox
    myChoicesCombo = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    myChoicesCombo->hide();
    // Create and hide checkButton
    myBoolCheckButton = new FXCheckButton(this, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myBoolCheckButton->hide();
}


void
GNEInspectorFrame::AttributesEditor::AttributeInput::showAttribute(SumoXMLTag ACTag, SumoXMLAttr ACAttr, const std::string& value) {
    // start enabling all elements
    myTextFieldInt->enable();
    myTextFieldReal->enable();
    myTextFieldStrings->enable();
    myChoicesCombo->enable();
    myBoolCheckButton->enable();
    myButtonCombinableChoices->enable();
    myColorEditor->enable();
    // Set current Tag
    myTag = ACTag;
    // Set current Attribute
    myAttr = ACAttr;
    // set multiple
    myMultiple = GNEAttributeCarrier::parse<std::vector<std::string>>(value).size() > 1;
    // obtain attribute property (only for improve code legibility)
    const auto& attrValue = GNEAttributeCarrier::getTagProperties(myTag).getAttributeProperties(myAttr);
    // enable all input values
    enableAttributeInputElements();
    if (attrValue.isColor()) {
        myColorEditor->setTextColor(FXRGB(0, 0, 0));
        myColorEditor->setText(toString(ACAttr).c_str());
        myColorEditor->show();
    } else {
        // Show attribute Label
        myLabel->setText(toString(myAttr).c_str());
        myLabel->show();
    }
    // Set field depending of the type of value
    if (attrValue.isBool()) {
        // first we need to check if all boolean values are equal
        bool allBooleanValuesEqual = true;
        // obtain boolean vector
        auto booleanVector = GNEAttributeCarrier::parse<std::vector<bool> >(value);
        // iterate over pased booleans comparing all element with the first
        for (const auto& i : booleanVector) {
            if (i != booleanVector.front()) {
                allBooleanValuesEqual = false;
            }
        }
        // use checkbox or textfield depending if all booleans are equal
        if (allBooleanValuesEqual) {
            // set check button
            if (booleanVector.front()) {
                myBoolCheckButton->setCheck(true);
                myBoolCheckButton->setText("true");
            } else {
                myBoolCheckButton->setCheck(false);
                myBoolCheckButton->setText("false");
            }
            // show check button
            myBoolCheckButton->show();
            // enable or disable depending if attribute is editable
            if (attrValue.isNonEditable()) {
                myBoolCheckButton->disable();
            } else {
                myBoolCheckButton->enable();
            }
        } else {
            // show list of bools (0 1)
            myTextFieldStrings->setText(value.c_str());
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->show();
            // enable or disable depending if attribute is editable
            if (attrValue.isNonEditable()) {
                myTextFieldStrings->disable();
            } else {
                myTextFieldStrings->enable();
            }
        }
    } else if (attrValue.isDiscrete()) {
        // Check if are combinable choices
        if ((attrValue.getDiscreteValues().size() > 0) && attrValue.isCombinable()) {
            // hide label
            myLabel->hide();
            // Show button combinable choices
            myButtonCombinableChoices->setText(toString(myAttr).c_str());
            myButtonCombinableChoices->show();
            // Show string with the values
            myTextFieldStrings->setText(value.c_str());
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->show();
        } else if (!myMultiple) {
            // fill comboBox
            myChoicesCombo->clearItems();
            for (const auto& it : attrValue.getDiscreteValues()) {
                myChoicesCombo->appendItem(it.c_str());
            }
            // show combo box with values
            myChoicesCombo->setNumVisible((int)attrValue.getDiscreteValues().size());
            myChoicesCombo->setCurrentItem(myChoicesCombo->findItem(value.c_str()));
            myChoicesCombo->setTextColor(FXRGB(0, 0, 0));
            myChoicesCombo->show();
            // enable or disable depending if attribute is editable
            if (attrValue.isNonEditable()) {
                myChoicesCombo->disable();
            } else {
                myChoicesCombo->enable();
            }
        } else {
            // represent combinable choices in multiple selections always with a textfield instead with a comboBox
            myTextFieldStrings->setText(value.c_str());
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->show();
            // enable or disable depending if attribute is editable
            if (attrValue.isNonEditable()) {
                myTextFieldStrings->disable();
            } else {
                myTextFieldStrings->enable();
            }
        }
    } else if (attrValue.isFloat() || attrValue.isTime()) {
        // show TextField for real/time values
        myTextFieldReal->setText(value.c_str());
        myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        myTextFieldReal->show();
        // enable or disable depending if attribute is editable
        if (attrValue.isNonEditable()) {
            myTextFieldReal->disable();
        } else {
            myTextFieldReal->enable();
        }
    } else if (attrValue.isInt()) {
        // Show textField for int attributes
        myTextFieldInt->setText(value.c_str());
        myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        myTextFieldInt->show();
        // enable or disable depending if attribute is editable
        if (attrValue.isNonEditable()) {
            myTextFieldInt->disable();
        } else {
            myTextFieldInt->enable();
        }
        // we need an extra check for connection attribute "TLIndex", because it cannot be edited if junction's connection doesn' have a TLS
        if ((ACTag == SUMO_TAG_CONNECTION) && (ACAttr == SUMO_ATTR_TLLINKINDEX) && (value == "No TLS")) {
            myTextFieldInt->disable();
        }
    } else {
        // In any other case (String, list, etc.), show value as String
        myTextFieldStrings->setText(value.c_str());
        myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        myTextFieldStrings->show();
        // enable or disable depending if attribute is editable
        if (attrValue.isNonEditable()) {
            myTextFieldStrings->disable();
        } else {
            myTextFieldStrings->enable();
        }
    }
    // if Tag correspond to an network element but we're in demand mode, disable all elements
    if ((myAttributesEditorParent->myInspectorFrameParent->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && !GNEAttributeCarrier::getTagProperties(myTag).isDemandElement()) {
        myTextFieldInt->disable();
        myTextFieldReal->disable();
        myTextFieldStrings->disable();
        myChoicesCombo->disable();
        myBoolCheckButton->disable();
        myButtonCombinableChoices->disable();
        myColorEditor->disable();
    }
    // Show AttributeInput
    show();
    // recalc after show elements
    recalc();
}


void
GNEInspectorFrame::AttributesEditor::AttributeInput::hideAttribute() {
    // Hide all elements
    myLabel->hide();
    myTextFieldInt->hide();
    myTextFieldReal->hide();
    myTextFieldStrings->hide();
    myChoicesCombo->hide();
    myBoolCheckButton->hide();
    myButtonCombinableChoices->hide();
    myColorEditor->hide();
    // hide AttributeInput
    hide();
    // recalc after hide all elements
    recalc();
}


void
GNEInspectorFrame::AttributesEditor::AttributeInput::refreshAttributeInput(const std::string& value, bool forceRefresh) {
    if (myTextFieldInt->shown()) {
        // set last valid value and restore color if onlyValid is disabled
        if (myTextFieldInt->getTextColor() == FXRGB(0, 0, 0) || forceRefresh) {
            myTextFieldInt->setText(value.c_str());
            myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        }
    } else if (myTextFieldReal->shown()) {
        // set last valid value and restore color if onlyValid is disabled
        if (myTextFieldReal->getTextColor() == FXRGB(0, 0, 0) || forceRefresh) {
            myTextFieldReal->setText(value.c_str());
            myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        }
    } else if (myTextFieldStrings->shown()) {
        // set last valid value and restore color if onlyValid is disabled
        if (myTextFieldStrings->getTextColor() == FXRGB(0, 0, 0) || forceRefresh) {
            myTextFieldStrings->setText(value.c_str());
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        }
    } else if (myChoicesCombo->shown()) {
        // set last valid value and restore color if onlyValid is disabled
        if (myChoicesCombo->getTextColor() == FXRGB(0, 0, 0) || forceRefresh) {
            myChoicesCombo->setText(value.c_str());
            myChoicesCombo->setTextColor(FXRGB(0, 0, 0));
        }
    } else if (myBoolCheckButton->shown()) {
        myBoolCheckButton->setCheck(GNEAttributeCarrier::parse<bool>(value));
    }
}


bool
GNEInspectorFrame::AttributesEditor::AttributeInput::isCurrentAttributeValid() const {
    return ((myTextFieldInt->getTextColor() == FXRGB(0, 0, 0)) && (myTextFieldReal->getTextColor() == FXRGB(0, 0, 0)) &&
            (myTextFieldStrings->getTextColor() == FXRGB(0, 0, 0)) && (myChoicesCombo->getTextColor() == FXRGB(0, 0, 0)));
}


long
GNEInspectorFrame::AttributesEditor::AttributeInput::onCmdOpenAttributeDialog(FXObject* obj, FXSelector, void*) {
    if (obj == myColorEditor) {
        // create FXColorDialog
        FXColorDialog colordialog(this, tr("Color Dialog"));
        colordialog.setTarget(this);
        // If previous attribute wasn't correct, set black as default color
        if (GNEAttributeCarrier::canParse<RGBColor>(myTextFieldStrings->getText().text())) {
            colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myTextFieldStrings->getText().text())));
        } else {
            colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().front()->getTagProperty().getDefaultValue(myAttr))));
        }
        // execute dialog to get a new color
        if (colordialog.execute()) {
            std::string newValue = toString(MFXUtils::getRGBColor(colordialog.getRGBA()));
            myTextFieldStrings->setText(newValue.c_str());
            if (myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().front()->isValid(myAttr, newValue)) {
                // if its valid for the first AC than its valid for all (of the same type)
                if (myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().size() > 1) {
                    myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList()->p_begin("Change multiple attributes");
                }
                // Set new value of attribute in all selected ACs
                for (const auto& it_ac : myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs()) {
                    it_ac->setAttribute(myAttr, newValue, myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList());
                }
                // If previously value was incorrect, change font color to black
                myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
                myTextFieldStrings->killFocus();
            }
        }
        return 0;
    } else if (obj == myButtonCombinableChoices) {
        // if its valid for the first AC than its valid for all (of the same type)
        if (myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().size() > 1) {
            myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList()->p_begin("Change multiple attributes");
        }
        // open GNEDialog_AllowDisallow
        GNEDialog_AllowDisallow(myAttributesEditorParent->getInspectorFrameParent()->getViewNet(), myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().front()).execute();
        std::string allowed = myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().front()->getAttribute(SUMO_ATTR_ALLOW);
        // Set new value of attribute in all selected ACs
        for (const auto& it_ac : myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs()) {
            it_ac->setAttribute(SUMO_ATTR_ALLOW, allowed, myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList());
        }
        // finish change multiple attributes
        if (myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().size() > 1) {
            myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList()->p_end();
        }
        return 1;
    } else {
        throw ProcessError("Invalid call to onCmdOpenAttributeDialog");
    }
}


long
GNEInspectorFrame::AttributesEditor::AttributeInput::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // Declare changed value
    std::string newVal;
    bool refreshGEOAndNeteditEditors = false;
    // get Tag and attribute Values (only for improve efficiency)
    const auto& tagValues = GNEAttributeCarrier::getTagProperties(myTag);
    const auto& attrValues = tagValues.getAttributeProperties(myAttr);
    // First, obtain the string value of the new attribute depending of their type
    if (attrValues.isBool()) {
        // first check if we're editing boolean as a list of string or as a checkbox
        if (myBoolCheckButton->shown()) {
            // Set true o false depending of the checkBox
            if (myBoolCheckButton->getCheck()) {
                myBoolCheckButton->setText("true");
                newVal = "true";
            } else {
                myBoolCheckButton->setText("false");
                newVal = "false";
            }
        } else {
            // obtain boolean value of myTextFieldStrings (because we're inspecting multiple attribute carriers with different values)
            newVal = myTextFieldStrings->getText().text();
        }
    } else if (attrValues.isDiscrete()) {
        // Check if are combinable choices (for example, Vehicle Types)
        if ((attrValues.getDiscreteValues().size() > 0) &&
                attrValues.isCombinable()) {
            // Get value obtained using AttributesEditor
            newVal = myTextFieldStrings->getText().text();
        } else if (!myMultiple) {
            // Get value of ComboBox
            newVal = myChoicesCombo->getText().text();
        } else {
            // due this is a multiple selection, obtain value of myTextFieldStrings instead of comboBox
            newVal = myTextFieldStrings->getText().text();
        }
    } else if (attrValues.isFloat() || attrValues.isTime()) {
        // Check if default value of attribute must be set
        if (myTextFieldReal->getText().empty() && attrValues.hasDefaultValue()) {
            newVal = tagValues.getDefaultValue(myAttr);
            myTextFieldReal->setText(newVal.c_str());
        } else {
            // obtain value of myTextFieldReal
            newVal = myTextFieldReal->getText().text();
        }
    } else if (attrValues.isInt()) {
        // Check if default value of attribute must be set
        if (myTextFieldInt->getText().empty() && attrValues.hasDefaultValue()) {
            newVal = tagValues.getDefaultValue(myAttr);
            myTextFieldInt->setText(newVal.c_str());
        } else {
            // obtain value of myTextFieldInt
            newVal = myTextFieldInt->getText().text();
        }
    } else if (attrValues.isString()) {
        // Check if default value of attribute must be set
        if (myTextFieldStrings->getText().empty() && attrValues.hasDefaultValue()) {
            newVal = tagValues.getDefaultValue(myAttr);
            myTextFieldStrings->setText(newVal.c_str());
        } else {
            // obtain value of myTextFieldStrings
            newVal = myTextFieldStrings->getText().text();
        }
    }

    // we need a extra check for Position and Shape Values, due #2658
    if ((myAttr == SUMO_ATTR_POSITION) || (myAttr == SUMO_ATTR_SHAPE)) {
        newVal = stripWhitespaceAfterComma(newVal);
        // due we're changing a Position and Shape attribute, GEO and Netedit Editors must be refresh
        refreshGEOAndNeteditEditors = true;
    }

    // Check if attribute must be changed
    if (myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().front()->isValid(myAttr, newVal)) {
        // if its valid for the first AC than its valid for all (of the same type)
        if (myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().size() > 1) {
            myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList()->p_begin("Change multiple attributes");
        } else if (myAttr == SUMO_ATTR_ID) {
            // IDs attribute has to be encapsulated
            myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList()->p_begin("change " + toString(myTag) + " attribute");
        }
        // Set new value of attribute in all selected ACs
        for (const auto& it_ac : myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs()) {
            it_ac->setAttribute(myAttr, newVal, myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList());
        }
        // finish change multiple attributes or ID Attributes
        if (myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().size() > 1) {
            myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList()->p_end();
        } else if (myAttr == SUMO_ATTR_ID) {
            myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList()->p_end();
        }
        // If previously value was incorrect, change font color to black
        if (attrValues.isCombinable()) {
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->killFocus();
            // in this case, we need to refresh the other values (For example, allow/Disallow objects)
            myAttributesEditorParent->refreshAttributeEditor(false, false);
        } else if (attrValues.isDiscrete()) {
            myChoicesCombo->setTextColor(FXRGB(0, 0, 0));
            myChoicesCombo->killFocus();
        } else if (attrValues.isFloat() || attrValues.isTime()) {
            myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
            myTextFieldReal->killFocus();
        } else if (attrValues.isInt() && myTextFieldStrings != nullptr) {
            myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
            myTextFieldInt->killFocus();
        } else if (myTextFieldStrings != nullptr) {
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->killFocus();
        }
        // Check if GEO and Netedit editors must be refresh
        if (refreshGEOAndNeteditEditors) {
            myAttributesEditorParent->getInspectorFrameParent()->myNeteditAttributesEditor->refreshNeteditAttributesEditor(true);
            myAttributesEditorParent->getInspectorFrameParent()->myGEOAttributesEditor->refreshGEOAttributesEditor(true);
        }
    } else {
        // If value of TextField isn't valid, change color to Red depending of type
        if (attrValues.isCombinable()) {
            myTextFieldStrings->setTextColor(FXRGB(255, 0, 0));
            myTextFieldStrings->killFocus();
        } else if (attrValues.isDiscrete()) {
            myChoicesCombo->setTextColor(FXRGB(255, 0, 0));
            myChoicesCombo->killFocus();
        } else if (attrValues.isFloat() || attrValues.isTime()) {
            myTextFieldReal->setTextColor(FXRGB(255, 0, 0));
        } else if (attrValues.isInt() && myTextFieldStrings != nullptr) {
            myTextFieldInt->setTextColor(FXRGB(255, 0, 0));
        } else if (myTextFieldStrings != nullptr) {
            myTextFieldStrings->setTextColor(FXRGB(255, 0, 0));
        }
        // Write Warning in console if we're in testing mode
        WRITE_DEBUG("Value '" + newVal + "' for attribute " + toString(myAttr) + " of " + toString(myTag) + " isn't valid");
    }
    return 1;
}


std::string
GNEInspectorFrame::AttributesEditor::AttributeInput::stripWhitespaceAfterComma(const std::string& stringValue) {
    std::string result(stringValue);
    while (result.find(", ") != std::string::npos) {
        result = StringUtils::replace(result, ", ", ",");
    }
    return result;
}


void
GNEInspectorFrame::AttributesEditor::AttributeInput::enableAttributeInputElements() {
    myTextFieldInt->enable();
    myTextFieldReal->enable();
    myTextFieldStrings->enable();
    myChoicesCombo->enable();
    myBoolCheckButton->enable();
}


void
GNEInspectorFrame::AttributesEditor::AttributeInput::disableAttributeInputElements() {
    myTextFieldInt->disable();
    myTextFieldReal->disable();
    myTextFieldStrings->disable();
    myChoicesCombo->disable();
    myBoolCheckButton->disable();
}

// ---------------------------------------------------------------------------
// GNEInspectorFrame::AttributesEditor - methods
// ---------------------------------------------------------------------------

GNEInspectorFrame::AttributesEditor::AttributesEditor(GNEInspectorFrame* inspectorFrameParent) :
    FXGroupBox(inspectorFrameParent->myContentFrame, "Internal attributes", GUIDesignGroupBoxFrame),
    myInspectorFrameParent(inspectorFrameParent) {
    // Create sufficient AttributeInput for all types of AttributeCarriers
    for (int i = 0; i < (int)GNEAttributeCarrier::getHigherNumberOfAttributes(); i++) {
        myVectorOfAttributeInputs.push_back(new AttributeInput(this));
    }
    // Create help button
    myHelpButton = new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


void
GNEInspectorFrame::AttributesEditor::showAttributeEditorModul() {
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        // Gets tag (only for simplify code)
        SumoXMLTag ACFrontTag = myInspectorFrameParent->getInspectedACs().front()->getTagProperty().getTag();
        //  check if current AC is a Junction without TLSs (needed to hidde TLS options)
        bool disableTLSinJunctions = (dynamic_cast<GNEJunction*>(myInspectorFrameParent->getInspectedACs().front()) && (dynamic_cast<GNEJunction*>(myInspectorFrameParent->getInspectedACs().front())->getNBNode()->getControllingTLS().empty()));
        // Iterate over attributes
        for (const auto& i : GNEAttributeCarrier::getTagProperties(ACFrontTag)) {
            // disable editing for unique attributes in case of multi-selection
            if ((myInspectorFrameParent->getInspectedACs().size() > 1) && i.second.isUnique()) {
                continue;
            }
            // Declare a set of occuring values and insert attribute's values of item (note: We use a set to avoid repeated values)
            std::set<std::string> occuringValues;
            for (const auto& it_ac : myInspectorFrameParent->getInspectedACs()) {
                occuringValues.insert(it_ac->getAttribute(i.first));
            }
            // get current value
            std::ostringstream oss;
            for (auto it_val = occuringValues.begin(); it_val != occuringValues.end(); it_val++) {
                if (it_val != occuringValues.begin()) {
                    oss << " ";
                }
                oss << *it_val;
            }
            std::string value = oss.str();
            if ((ACFrontTag == SUMO_TAG_CONNECTION) && (i.first == SUMO_ATTR_TLLINKINDEX)
                    && value == toString(NBConnection::InvalidTlIndex)) {
                // possibly the connections are newly created (allow assigning
                // tlIndex if the junction(s) have a traffic light
                for (const auto& it_ac : myInspectorFrameParent->getInspectedACs()) {
                    if (!it_ac->isValid(SUMO_ATTR_TLLINKINDEX, "0")) {
                        value =  "No TLS";
                        break;
                    }
                }
            }
            // Show attribute
            if ((disableTLSinJunctions && (ACFrontTag == SUMO_TAG_JUNCTION) && ((i.first == SUMO_ATTR_TLTYPE) || (i.first == SUMO_ATTR_TLID))) == false) {
                // first show AttributesEditor
                show();
                // show attribute
                myVectorOfAttributeInputs[i.second.getPositionListed()]->showAttribute(ACFrontTag, i.first, value);
            }
        }
    }
}


void
GNEInspectorFrame::AttributesEditor::hideAttributesEditorModul() {
    // hide al attributes
    for (const auto& i : myVectorOfAttributeInputs) {
        i->hideAttribute();
    }
    // hide also AttributesEditor
    hide();
}


void
GNEInspectorFrame::AttributesEditor::refreshAttributeEditor(bool forceRefreshShape, bool forceRefreshPosition) {
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        // Declare pointer for allow/Disallow vehicles
        std::pair<GNEInspectorFrame::AttributesEditor::AttributeInput*, std::string> myAllowAttribute(nullptr, "");
        std::pair<GNEInspectorFrame::AttributesEditor::AttributeInput*, std::string> myDisallowAttribute(nullptr, "");
        // Gets tag (only for simplify code)
        SumoXMLTag ACFrontTag = myInspectorFrameParent->getInspectedACs().front()->getTagProperty().getTag();
        //  check if current AC is a Junction without TLSs (needed to hidde TLS options)
        bool disableTLSinJunctions = (dynamic_cast<GNEJunction*>(myInspectorFrameParent->getInspectedACs().front()) && (dynamic_cast<GNEJunction*>(myInspectorFrameParent->getInspectedACs().front())->getNBNode()->getControllingTLS().empty()));
        // Iterate over attributes
        for (const auto& i : myInspectorFrameParent->getInspectedACs().front()->getTagProperty()) {
            // disable editing for unique attributes in case of multi-selection
            if ((myInspectorFrameParent->getInspectedACs().size() > 1) && i.second.isUnique()) {
                continue;
            }
            // Declare a set of occuring values and insert attribute's values of item
            std::set<std::string> occuringValues;
            for (const auto& it_ac : myInspectorFrameParent->getInspectedACs()) {
                occuringValues.insert(it_ac->getAttribute(i.first));
            }
            // get current value
            std::ostringstream oss;
            for (auto it_val = occuringValues.begin(); it_val != occuringValues.end(); it_val++) {
                if (it_val != occuringValues.begin()) {
                    oss << " ";
                }
                oss << *it_val;
            }
            // Show attribute
            if ((disableTLSinJunctions && (ACFrontTag == SUMO_TAG_JUNCTION) && ((i.first == SUMO_ATTR_TLTYPE) || (i.first == SUMO_ATTR_TLID))) == false) {
                // refresh attribute, with a special case for allow/disallow vehicles
                if (i.first  == SUMO_ATTR_ALLOW) {
                    myAllowAttribute.first = myVectorOfAttributeInputs[i.second.getPositionListed()];
                    myAllowAttribute.second = oss.str();
                } else if (i.first  == SUMO_ATTR_DISALLOW) {
                    myDisallowAttribute.first = myVectorOfAttributeInputs[i.second.getPositionListed()];
                    myDisallowAttribute.second = oss.str();
                } else {
                    // Check if refresh of Position or Shape has to be forced
                    if ((i.first  == SUMO_ATTR_SHAPE) && forceRefreshShape) {
                        myVectorOfAttributeInputs[i.second.getPositionListed()]->refreshAttributeInput(oss.str(), true);
                    } else if ((i.first  == SUMO_ATTR_POSITION) && forceRefreshPosition) {
                        // Refresh attributes maintain invalid values
                        myVectorOfAttributeInputs[i.second.getPositionListed()]->refreshAttributeInput(oss.str(), true);
                    } else {
                        // Refresh attributes maintain invalid values
                        myVectorOfAttributeInputs[i.second.getPositionListed()]->refreshAttributeInput(oss.str(), false);
                    }
                }
            }
        }
        // Check special case for Allow/Disallow attributes
        if (myAllowAttribute.first && myDisallowAttribute.first) {
            // if allow attribute is valid but disallow attribute is invalid
            if (myAllowAttribute.first->isCurrentAttributeValid() && !myDisallowAttribute.first->isCurrentAttributeValid()) {
                // force refresh of disallow attribute
                myDisallowAttribute.first->refreshAttributeInput(myDisallowAttribute.second, true);
            }
            // if disallow attribute is valid but allow attribute is invalid
            if (myDisallowAttribute.first->isCurrentAttributeValid() && !myAllowAttribute.first->isCurrentAttributeValid()) {
                // force refresh of disallow attribute
                myAllowAttribute.first->refreshAttributeInput(myAllowAttribute.second, true);
            }
        }
    }
}


GNEInspectorFrame*
GNEInspectorFrame::AttributesEditor::getInspectorFrameParent() const {
    return myInspectorFrameParent;
}


long
GNEInspectorFrame::AttributesEditor::onCmdAttributeHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog if there is inspected ACs
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        // open Help attributes dialog
        myInspectorFrameParent->openHelpAttributesDialog(myInspectorFrameParent->getInspectedACs().front()->getTagProperty());
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEInspectorFrame::NeteditAttributesEditor - methods
// ---------------------------------------------------------------------------

GNEInspectorFrame::NeteditAttributesEditor::NeteditAttributesEditor(GNEInspectorFrame* inspectorFrameParent) :
    FXGroupBox(inspectorFrameParent->myContentFrame, "Netedit attributes", GUIDesignGroupBoxFrame),
    myInspectorFrameParent(inspectorFrameParent) {

    // Create elements for additional parent
    myHorizontalFrameAdditionalParent = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLabelAdditionalParent = new FXLabel(myHorizontalFrameAdditionalParent, "Block move", nullptr, GUIDesignLabelAttribute);
    myTextFieldAdditionalParent = new FXTextField(myHorizontalFrameAdditionalParent, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);

    // Create elements for block movement
    myHorizontalFrameBlockMovement = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLabelBlockMovement = new FXLabel(myHorizontalFrameBlockMovement, "Block move", nullptr, GUIDesignLabelAttribute);
    myCheckBoxBlockMovement = new FXCheckButton(myHorizontalFrameBlockMovement, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);

    // Create elements for block shape
    myHorizontalFrameBlockShape = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLabelBlockShape = new FXLabel(myHorizontalFrameBlockShape, "Block shape", nullptr, GUIDesignLabelAttribute);
    myCheckBoxBlockShape = new FXCheckButton(myHorizontalFrameBlockShape, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);

    // Create elements for close shape
    myHorizontalFrameCloseShape = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLabelCloseShape = new FXLabel(myHorizontalFrameCloseShape, "Close shape", nullptr, GUIDesignLabelAttribute);
    myCheckBoxCloseShape = new FXCheckButton(myHorizontalFrameCloseShape, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);

    // Create help button
    myHelpButton = new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEInspectorFrame::NeteditAttributesEditor::~NeteditAttributesEditor() {}


void
GNEInspectorFrame::NeteditAttributesEditor::showNeteditAttributesEditor() {
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        // enable all editable elements
        myTextFieldAdditionalParent->enable();
        myCheckBoxBlockMovement->enable();
        myCheckBoxBlockShape->enable();
        myCheckBoxCloseShape->enable();
        // obtain tag property (only for improve code legibility)
        const auto& tagValue = myInspectorFrameParent->getInspectedACs().front()->getTagProperty();
        // Check if item can be moved
        if (tagValue.canBlockMovement()) {
            // show NeteditAttributesEditor
            show();
            // Iterate over AC to obtain values
            bool value = true;
            for (const auto& i : myInspectorFrameParent->getInspectedACs()) {
                value &= GNEAttributeCarrier::parse<bool>(i->getAttribute(GNE_ATTR_BLOCK_MOVEMENT));
            }
            // show block movement frame
            myHorizontalFrameBlockMovement->show();
            // set check box value and update label
            if (value) {
                myCheckBoxBlockMovement->setCheck(true);
                myCheckBoxBlockMovement->setText("true");
            } else {
                myCheckBoxBlockMovement->setCheck(false);
                myCheckBoxBlockMovement->setText("false");
            }
        }
        // check if item can block their shape
        if (tagValue.canBlockShape()) {
            // show NeteditAttributesEditor
            show();
            // Iterate over AC to obtain values
            bool value = true;
            for (const auto& i : myInspectorFrameParent->getInspectedACs()) {
                value &= GNEAttributeCarrier::parse<bool>(i->getAttribute(GNE_ATTR_BLOCK_SHAPE));
            }
            // show block shape frame
            myHorizontalFrameBlockShape->show();
            // set check box value and update label
            if (value) {
                myCheckBoxBlockShape->setCheck(true);
                myCheckBoxBlockShape->setText("true");
            } else {
                myCheckBoxBlockShape->setCheck(false);
                myCheckBoxBlockShape->setText("false");
            }
        }
        // check if item can block their shape
        if (tagValue.canCloseShape()) {
            // show NeteditAttributesEditor
            show();
            // Iterate over AC to obtain values
            bool value = true;
            for (const auto& i : myInspectorFrameParent->getInspectedACs()) {
                value &= GNEAttributeCarrier::parse<bool>(i->getAttribute(GNE_ATTR_CLOSE_SHAPE));
            }
            // show close shape frame
            myHorizontalFrameCloseShape->show();
            // set check box value and update label
            if (value) {
                myCheckBoxCloseShape->setCheck(true);
                myCheckBoxCloseShape->setText("true");
            } else {
                myCheckBoxCloseShape->setCheck(false);
                myCheckBoxCloseShape->setText("false");
            }
        }
        // Check if item has another item as parent and can be reparemt
        if (tagValue.hasParent() && tagValue.canBeReparent()) {
            // show NeteditAttributesEditor
            show();
            // obtain additional Parent
            std::set<std::string> parents;
            for (const auto& i : myInspectorFrameParent->getInspectedACs()) {
                parents.insert(i->getAttribute(GNE_ATTR_PARENT));
            }
            // show additional parent frame
            myHorizontalFrameAdditionalParent->show();
            // set Label and TextField with the Tag and ID of parent
            myLabelAdditionalParent->setText((toString(myInspectorFrameParent->getInspectedACs().front()->getTagProperty().getParentTag()) + " parent").c_str());
            myTextFieldAdditionalParent->setText(toString(parents).c_str());
        }
        // disable all editable elements if we're in demand mode and inspected AC isn't a demand element
        if ((myInspectorFrameParent->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && !myInspectorFrameParent->getInspectedACs().front()->getTagProperty().isDemandElement()) {
            myTextFieldAdditionalParent->disable();
            myCheckBoxBlockMovement->disable();
            myCheckBoxBlockShape->disable();
            myCheckBoxCloseShape->disable();
        }
    }
}


void
GNEInspectorFrame::NeteditAttributesEditor::hideNeteditAttributesEditor() {
    // hide all elements of GroupBox
    myHorizontalFrameAdditionalParent->hide();
    myHorizontalFrameBlockMovement->hide();
    myHorizontalFrameBlockShape->hide();
    myHorizontalFrameCloseShape->hide();
    // hide groupbox
    hide();
}


void
GNEInspectorFrame::NeteditAttributesEditor::refreshNeteditAttributesEditor(bool forceRefresh) {
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        // refresh block movement
        if (myHorizontalFrameBlockMovement->shown()) {
            // Iterate over AC to obtain values
            bool value = true;
            for (const auto& i : myInspectorFrameParent->getInspectedACs()) {
                value &= GNEAttributeCarrier::parse<bool>(i->getAttribute(GNE_ATTR_BLOCK_MOVEMENT));
            }
            // set check box value and update label
            if (value) {
                myCheckBoxBlockMovement->setCheck(true);
                myCheckBoxBlockMovement->setText("true");
            } else {
                myCheckBoxBlockMovement->setCheck(false);
                myCheckBoxBlockMovement->setText("false");
            }
        }
        // refresh block shape
        if (myHorizontalFrameBlockShape->shown()) {
            // Iterate over AC to obtain values
            bool value = true;
            for (const auto& i : myInspectorFrameParent->getInspectedACs()) {
                value &= GNEAttributeCarrier::parse<bool>(i->getAttribute(GNE_ATTR_BLOCK_SHAPE));
            }
            // set check box value and update label
            if (value) {
                myCheckBoxBlockShape->setCheck(true);
                myCheckBoxBlockShape->setText("true");
            } else {
                myCheckBoxBlockShape->setCheck(false);
                myCheckBoxBlockShape->setText("false");
            }
        }
        // refresh close shape
        if (myHorizontalFrameCloseShape->shown()) {
            // Iterate over AC to obtain values
            bool value = true;
            for (const auto& i : myInspectorFrameParent->getInspectedACs()) {
                value &= GNEAttributeCarrier::parse<bool>(i->getAttribute(GNE_ATTR_CLOSE_SHAPE));
            }
            // set check box value and update label
            if (value) {
                myCheckBoxCloseShape->setCheck(true);
                myCheckBoxCloseShape->setText("true");
            } else {
                myCheckBoxCloseShape->setCheck(false);
                myCheckBoxCloseShape->setText("false");
            }
        }
        // Check if item has another item as parent (Currently only for single Additionals)
        if (myHorizontalFrameAdditionalParent->shown() && ((myTextFieldAdditionalParent->getTextColor() == FXRGB(0, 0, 0)) || forceRefresh)) {
            // set Label and TextField with the Tag and ID of parent
            myLabelAdditionalParent->setText((toString(myInspectorFrameParent->getInspectedACs().front()->getTagProperty().getParentTag()) + " parent").c_str());
            myTextFieldAdditionalParent->setText(myInspectorFrameParent->getInspectedACs().front()->getAttribute(GNE_ATTR_PARENT).c_str());
        }
    }
}


long
GNEInspectorFrame::NeteditAttributesEditor::onCmdSetNeteditAttribute(FXObject* obj, FXSelector, void*) {
    // make sure that ACs has elements
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        // check if we're changing multiple attributes
        if (myInspectorFrameParent->getInspectedACs().size() > 1) {
            myInspectorFrameParent->getViewNet()->getUndoList()->p_begin("Change multiple attributes");
        }
        if (obj == myCheckBoxBlockMovement) {
            // set new values in all inspected Attribute Carriers
            for (const auto& i : myInspectorFrameParent->getInspectedACs()) {
                if (myCheckBoxBlockMovement->getCheck() == 1) {
                    i->setAttribute(GNE_ATTR_BLOCK_MOVEMENT, "true", myInspectorFrameParent->getViewNet()->getUndoList());
                    myCheckBoxBlockMovement->setText("true");
                } else {
                    i->setAttribute(GNE_ATTR_BLOCK_MOVEMENT, "false", myInspectorFrameParent->getViewNet()->getUndoList());
                    myCheckBoxBlockMovement->setText("false");
                }
            }
        } else if (obj == myCheckBoxBlockShape) {
            // set new values in all inspected Attribute Carriers
            for (const auto& i : myInspectorFrameParent->getInspectedACs()) {
                if (myCheckBoxBlockShape->getCheck() == 1) {
                    i->setAttribute(GNE_ATTR_BLOCK_SHAPE, "true", myInspectorFrameParent->getViewNet()->getUndoList());
                    myCheckBoxBlockShape->setText("true");
                } else {
                    i->setAttribute(GNE_ATTR_BLOCK_SHAPE, "false", myInspectorFrameParent->getViewNet()->getUndoList());
                    myCheckBoxBlockShape->setText("false");
                }
            }
        } else if (obj == myCheckBoxCloseShape) {
            // set new values in all inspected Attribute Carriers
            for (const auto& i : myInspectorFrameParent->getInspectedACs()) {
                if (myCheckBoxCloseShape->getCheck() == 1) {
                    i->setAttribute(GNE_ATTR_CLOSE_SHAPE, "true", myInspectorFrameParent->getViewNet()->getUndoList());
                    myCheckBoxCloseShape->setText("true");
                } else {
                    i->setAttribute(GNE_ATTR_CLOSE_SHAPE, "false", myInspectorFrameParent->getViewNet()->getUndoList());
                    myCheckBoxCloseShape->setText("false");
                }
            }
        } else if (obj == myTextFieldAdditionalParent) {
            if (myInspectorFrameParent->getInspectedACs().front()->isValid(GNE_ATTR_PARENT, myTextFieldAdditionalParent->getText().text())) {
                // change parent of all inspected elements
                for (const auto& i : myInspectorFrameParent->getInspectedACs()) {
                    i->setAttribute(GNE_ATTR_PARENT, myTextFieldAdditionalParent->getText().text(), myInspectorFrameParent->getViewNet()->getUndoList());
                }
                myTextFieldAdditionalParent->setTextColor(FXRGB(0, 0, 0));
            } else {
                myTextFieldAdditionalParent->setTextColor(FXRGB(255, 0, 0));
                myTextFieldAdditionalParent->killFocus();
            }
        }
        // finish change multiple attributes
        if (myInspectorFrameParent->getInspectedACs().size() > 1) {
            myInspectorFrameParent->getViewNet()->getUndoList()->p_end();
        }
        // force refresh values of AttributesEditor and GEOAttributesEditor
        myInspectorFrameParent->myAttributesEditor->refreshAttributeEditor(true, true);
        myInspectorFrameParent->myGEOAttributesEditor->refreshGEOAttributesEditor(true);
    }
    return 1;
}


long
GNEInspectorFrame::NeteditAttributesEditor::onCmdNeteditAttributeHelp(FXObject*, FXSelector, void*) {
    return 0;
}

// ---------------------------------------------------------------------------
// GNEInspectorFrame::GEOAttributesEditor - methods
// ---------------------------------------------------------------------------

GNEInspectorFrame::GEOAttributesEditor::GEOAttributesEditor(GNEInspectorFrame* inspectorFrameParent) :
    FXGroupBox(inspectorFrameParent->myContentFrame, "GEO Attributes", GUIDesignGroupBoxFrame),
    myInspectorFrameParent(inspectorFrameParent) {

    // Create Frame for GEOAttribute
    myGEOAttributeFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myGEOAttributeLabel = new FXLabel(myGEOAttributeFrame, "Undefined GEO Attribute", nullptr, GUIDesignLabelAttribute);
    myGEOAttributeTextField = new FXTextField(myGEOAttributeFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);

    // Create Frame for use GEO
    myUseGEOFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myUseGEOLabel = new FXLabel(myUseGEOFrame, toString(SUMO_ATTR_GEO).c_str(), nullptr, GUIDesignLabelAttribute);
    myUseGEOCheckButton = new FXCheckButton(myUseGEOFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);

    // Create help button
    myHelpButton = new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEInspectorFrame::GEOAttributesEditor::~GEOAttributesEditor() {}


void
GNEInspectorFrame::GEOAttributesEditor::showGEOAttributesEditor() {
    // make sure that ACs has elements
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        // enable all editable elements
        myGEOAttributeTextField->enable();
        myUseGEOCheckButton->enable();
        // obtain tag property (only for improve code legibility)
        const auto& tagProperty = myInspectorFrameParent->getInspectedACs().front()->getTagProperty();
        // check if item can use a geo position
        if (tagProperty.hasGEOPosition() || tagProperty.hasGEOShape()) {
            // show GEOAttributesEditor
            show();
            // Iterate over AC to obtain values
            bool value = true;
            for (const auto& i : myInspectorFrameParent->getInspectedACs()) {
                value &= GNEAttributeCarrier::parse<bool>(i->getAttribute(SUMO_ATTR_GEO));
            }
            // show use geo frame
            myUseGEOFrame->show();
            // set UseGEOCheckButton value of and update label (only if geo conversion is defined)
            if (GeoConvHelper::getFinal().getProjString() != "!") {
                myUseGEOCheckButton->enable();
                if (value) {
                    myUseGEOCheckButton->setCheck(true);
                    myUseGEOCheckButton->setText("true");
                } else {
                    myUseGEOCheckButton->setCheck(false);
                    myUseGEOCheckButton->setText("false");
                }
            } else {
                myUseGEOCheckButton->disable();
            }
            // now specify if a single position or an entire shape must be shown (note: cannot be shown both at the same time, and GEO Shape/Position only works for single selections)
            if (tagProperty.hasGEOPosition() && myInspectorFrameParent->getInspectedACs().size() == 1) {
                myGEOAttributeFrame->show();
                myGEOAttributeLabel->setText(toString(SUMO_ATTR_GEOPOSITION).c_str());
                myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
                // only allow edit if geo conversion is defined
                if (GeoConvHelper::getFinal().getProjString() != "!") {
                    myGEOAttributeTextField->enable();
                    myGEOAttributeTextField->setText(myInspectorFrameParent->getInspectedACs().front()->getAttribute(SUMO_ATTR_GEOPOSITION).c_str());
                } else {
                    myGEOAttributeTextField->disable();
                    myGEOAttributeTextField->setText("No geo-conversion defined");
                }
            } else if (tagProperty.hasGEOShape() && myInspectorFrameParent->getInspectedACs().size() == 1) {
                myGEOAttributeFrame->show();
                myGEOAttributeLabel->setText(toString(SUMO_ATTR_GEOSHAPE).c_str());
                myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
                // only allow edit if geo conversion is defined
                if (GeoConvHelper::getFinal().getProjString() != "!") {
                    myGEOAttributeTextField->enable();
                    myGEOAttributeTextField->setText(myInspectorFrameParent->getInspectedACs().front()->getAttribute(SUMO_ATTR_GEOSHAPE).c_str());
                } else {
                    myGEOAttributeTextField->disable();
                    myGEOAttributeTextField->setText("No geo-conversion defined");
                }
            }
        }
        // disable all editable elements if we're in demand mode and inspected AC isn't a demand element
        if ((myInspectorFrameParent->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && !myInspectorFrameParent->getInspectedACs().front()->getTagProperty().isDemandElement()) {
            myGEOAttributeTextField->disable();
            myUseGEOCheckButton->disable();
        }
    }
}


void
GNEInspectorFrame::GEOAttributesEditor::hideGEOAttributesEditor() {
    // hide all elements of GroupBox
    myGEOAttributeFrame->hide();
    myUseGEOFrame->hide();
    // hide groupbox
    hide();
}


void
GNEInspectorFrame::GEOAttributesEditor::refreshGEOAttributesEditor(bool forceRefresh) {
    // obtain tag property (only for improve code legibility)
    const auto& tagProperty = myInspectorFrameParent->getInspectedACs().front()->getTagProperty();
    // Check that myGEOAttributeFrame is shown
    if ((GeoConvHelper::getFinal().getProjString() != "!") && myGEOAttributeFrame->shown() && ((myGEOAttributeTextField->getTextColor() == FXRGB(0, 0, 0)) || forceRefresh)) {
        if (tagProperty.hasGEOPosition()) {
            myGEOAttributeTextField->setText(myInspectorFrameParent->getInspectedACs().front()->getAttribute(SUMO_ATTR_GEOPOSITION).c_str());
        } else if (tagProperty.hasGEOShape()) {
            myGEOAttributeTextField->setText(myInspectorFrameParent->getInspectedACs().front()->getAttribute(SUMO_ATTR_GEOSHAPE).c_str());
        }
        myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
    }
}


long
GNEInspectorFrame::GEOAttributesEditor::onCmdSetGEOAttribute(FXObject* obj, FXSelector, void*) {
    // make sure that ACs has elements
    if ((GeoConvHelper::getFinal().getProjString() != "!") && (myInspectorFrameParent->getInspectedACs().size() > 0)) {
        if (obj == myGEOAttributeTextField) {
            // obtain tag property (only for improve code legibility)
            const auto& tagProperty = myInspectorFrameParent->getInspectedACs().front()->getTagProperty();
            // Change GEO Attribute depending of type (Position or shape)
            if (tagProperty.hasGEOPosition()) {
                if (myInspectorFrameParent->getInspectedACs().front()->isValid(SUMO_ATTR_GEOPOSITION, myGEOAttributeTextField->getText().text())) {
                    myInspectorFrameParent->getInspectedACs().front()->setAttribute(SUMO_ATTR_GEOPOSITION, myGEOAttributeTextField->getText().text(), myInspectorFrameParent->getViewNet()->getUndoList());
                    myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
                } else {
                    myGEOAttributeTextField->setTextColor(FXRGB(255, 0, 0));
                    myGEOAttributeTextField->killFocus();
                }
            } else if (tagProperty.hasGEOShape()) {
                if (myInspectorFrameParent->getInspectedACs().front()->isValid(SUMO_ATTR_GEOSHAPE, myGEOAttributeTextField->getText().text())) {
                    myInspectorFrameParent->getInspectedACs().front()->setAttribute(SUMO_ATTR_GEOSHAPE, myGEOAttributeTextField->getText().text(), myInspectorFrameParent->getViewNet()->getUndoList());
                    myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
                } else {
                    myGEOAttributeTextField->setTextColor(FXRGB(255, 0, 0));
                    myGEOAttributeTextField->killFocus();
                }
            } else {
                throw ProcessError("myGEOAttributeTextField must be hidden becaurse there isn't GEO Attribute to edit");
            }
        } else if (obj == myUseGEOCheckButton) {
            // update GEO Attribute of entire selection
            for (const auto& i : myInspectorFrameParent->getInspectedACs()) {
                if (myUseGEOCheckButton->getCheck() == 1) {
                    i->setAttribute(SUMO_ATTR_GEO, "true", myInspectorFrameParent->getViewNet()->getUndoList());
                    myUseGEOCheckButton->setText("true");
                } else {
                    i->setAttribute(SUMO_ATTR_GEO, "false", myInspectorFrameParent->getViewNet()->getUndoList());
                    myUseGEOCheckButton->setText("false");
                }
            }
        }
        // force refresh values of Attributes editor and NeteditAttributesEditor
        myInspectorFrameParent->myAttributesEditor->refreshAttributeEditor(true, true);
        myInspectorFrameParent->myNeteditAttributesEditor->refreshNeteditAttributesEditor(true);
    }
    return 1;
}


long
GNEInspectorFrame::GEOAttributesEditor::onCmdGEOAttributeHelp(FXObject*, FXSelector, void*) {
    FXDialogBox* helpDialog = new FXDialogBox(this, "GEO attributes Help", GUIDesignDialogBox);
    std::ostringstream help;
    help
            << " SUMO uses the World Geodetic System 84 (WGS84/UTM).\n"
            << " For a GEO-referenced network, geo coordinates are represented as pairs of Longitude and Latitude\n"
            << " in decimal degrees without extra symbols. (N,W..)\n"
            << " - Longitude: East-west position of a point on the Earth's surface.\n"
            << " - Latitude: North-south position of a point on the Earth's surface.\n"
            << " - CheckBox 'geo' enables or disables saving position in GEO coordinates\n";
    new FXLabel(helpDialog, help.str().c_str(), nullptr, GUIDesignLabelFrameInformation);
    // "OK"
    new FXButton(helpDialog, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), helpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    helpDialog->create();
    helpDialog->show();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEInspectorFrame::TemplateEditor - methods
// ---------------------------------------------------------------------------

GNEInspectorFrame::TemplateEditor::TemplateEditor(GNEInspectorFrame* inspectorFrameParent) :
    FXGroupBox(inspectorFrameParent->myContentFrame, "Templates", GUIDesignGroupBoxFrame),
    myInspectorFrameParent(inspectorFrameParent),
    myEdgeTemplate(nullptr) {

    // Create copy template button
    myCopyTemplateButton = new FXButton(this, "", nullptr, this, MID_GNE_INSPECTORFRAME_COPYTEMPLATE, GUIDesignButton);
    myCopyTemplateButton->hide();

    // Create set template button
    mySetTemplateButton = new FXButton(this, "Set as Template\t\t", nullptr, this, MID_GNE_INSPECTORFRAME_SETTEMPLATE, GUIDesignButton);
    mySetTemplateButton->hide();
}


GNEInspectorFrame::TemplateEditor::~TemplateEditor() {
    if (myEdgeTemplate) {
        // delete template
        myEdgeTemplate->decRef("GNEInspectorFrame::~GNEInspectorFrame");
        if (myEdgeTemplate->unreferenced()) {
            delete myEdgeTemplate;
        }
    }
}


void
GNEInspectorFrame::TemplateEditor::showTemplateEditor() {
    if ((myInspectorFrameParent->getViewNet()->getEditModes().currentSupermode != GNE_SUPERMODE_DEMAND) && (myInspectorFrameParent->getInspectedACs().front()->getTagProperty().getTag() == SUMO_TAG_EDGE)) {
        // show template editor
        show();
        // show "Copy Template" (caption supplied via onUpdate)
        myCopyTemplateButton->show();
        // show "Set As Template"
        if (myInspectorFrameParent->getInspectedACs().size() == 1) {
            mySetTemplateButton->show();
            mySetTemplateButton->setText(("Set edge '" + myInspectorFrameParent->getInspectedACs().front()->getID() + "' as Template").c_str());
        }
    }
}


void
GNEInspectorFrame::TemplateEditor::hideTemplateEditor() {
    // hide buttons
    myCopyTemplateButton->hide();
    mySetTemplateButton->hide();
    // hide template editor
    hide();
}


GNEEdge*
GNEInspectorFrame::TemplateEditor::getEdgeTemplate() const {
    return myEdgeTemplate;
}


void
GNEInspectorFrame::TemplateEditor::setEdgeTemplate(GNEEdge* tpl) {
    if (myEdgeTemplate) {
        myEdgeTemplate->decRef("GNEInspectorFrame::setEdgeTemplate");
        if (myEdgeTemplate->unreferenced()) {
            delete myEdgeTemplate;
        }
    }
    myEdgeTemplate = tpl;
    myEdgeTemplate->incRef("GNEInspectorFrame::setEdgeTemplate");
}


long
GNEInspectorFrame::TemplateEditor::onCmdCopyTemplate(FXObject*, FXSelector, void*) {
    for (const auto& it : myInspectorFrameParent->getInspectedACs()) {
        GNEEdge* edge = dynamic_cast<GNEEdge*>(it);
        assert(edge);
        edge->copyTemplate(myEdgeTemplate, myInspectorFrameParent->getViewNet()->getUndoList());
        myInspectorFrameParent->inspectMultisection(myInspectorFrameParent->getInspectedACs());
    }
    return 1;
}


long
GNEInspectorFrame::TemplateEditor::onCmdSetTemplate(FXObject*, FXSelector, void*) {
    assert(myInspectorFrameParent->getInspectedACs().size() == 1);
    GNEEdge* edge = dynamic_cast<GNEEdge*>(myInspectorFrameParent->getInspectedACs().front());
    assert(edge);
    setEdgeTemplate(edge);
    return 1;
}


long
GNEInspectorFrame::TemplateEditor::onUpdCopyTemplate(FXObject* sender, FXSelector, void*) {
    // declare caption for button
    FXString caption;
    if (myEdgeTemplate) {
        if (myInspectorFrameParent->getInspectedACs().size() == 1) {
            caption = ("Copy '" + myEdgeTemplate->getMicrosimID() + "' into edge '" + myInspectorFrameParent->getInspectedACs().front()->getID() + "'").c_str();
        } else {
            caption = ("Copy '" + myEdgeTemplate->getMicrosimID() + "' into " + toString(myInspectorFrameParent->getInspectedACs().size()) + " selected edges").c_str();
        }
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        caption = "No edge Template Set";
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
    sender->handle(this, FXSEL(SEL_COMMAND, FXLabel::ID_SETSTRINGVALUE), (void*)&caption);
    return 1;
}

/****************************************************************************/
