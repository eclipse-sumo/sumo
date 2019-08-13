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

#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>

#include "GNEInspectorFrame.h"
#include "GNEDeleteFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEInspectorFrame) GNEInspectorFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_INSPECTORFRAME_GOBACK,  GNEInspectorFrame::onCmdGoBack)
};

FXDEFMAP(GNEInspectorFrame::OverlappedInspection) OverlappedInspectionMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OVERLAPPED_NEXT,            GNEInspectorFrame::OverlappedInspection::onCmdNextElement),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OVERLAPPED_PREVIOUS,        GNEInspectorFrame::OverlappedInspection::onCmdPreviousElement),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OVERLAPPED_SHOWLIST,        GNEInspectorFrame::OverlappedInspection::onCmdShowList),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OVERLAPPED_ITEMSELECTED,    GNEInspectorFrame::OverlappedInspection::onCmdListItemSelected),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                           GNEInspectorFrame::OverlappedInspection::onCmdOverlappingHelp)
};

FXDEFMAP(GNEInspectorFrame::NeteditAttributesEditor) NeteditAttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEInspectorFrame::NeteditAttributesEditor::onCmdSetNeteditAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNEInspectorFrame::NeteditAttributesEditor::onCmdNeteditAttributeHelp)
};

FXDEFMAP(GNEInspectorFrame::GEOAttributesEditor) GEOAttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEInspectorFrame::GEOAttributesEditor::onCmdSetGEOAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNEInspectorFrame::GEOAttributesEditor::onCmdGEOAttributeHelp)
};

FXDEFMAP(GNEInspectorFrame::TemplateEditor) TemplateEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TEMPLATE_COPY,  GNEInspectorFrame::TemplateEditor::onCmdCopyTemplate),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TEMPLATE_COPY,  GNEInspectorFrame::TemplateEditor::onUpdCopyTemplate),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TEMPLATE_SET,   GNEInspectorFrame::TemplateEditor::onCmdSetTemplate)
};

// Object implementation
FXIMPLEMENT(GNEInspectorFrame,                              FXVerticalFrame,    GNEInspectorFrameMap,       ARRAYNUMBER(GNEInspectorFrameMap))
FXIMPLEMENT(GNEInspectorFrame::OverlappedInspection,        FXGroupBox,         OverlappedInspectionMap,    ARRAYNUMBER(OverlappedInspectionMap))
FXIMPLEMENT(GNEInspectorFrame::NeteditAttributesEditor,     FXGroupBox,         NeteditAttributesEditorMap, ARRAYNUMBER(NeteditAttributesEditorMap))
FXIMPLEMENT(GNEInspectorFrame::GEOAttributesEditor,         FXGroupBox,         GEOAttributesEditorMap,     ARRAYNUMBER(GEOAttributesEditorMap))
FXIMPLEMENT(GNEInspectorFrame::TemplateEditor,              FXGroupBox,         TemplateEditorMap,          ARRAYNUMBER(TemplateEditorMap))


// ===========================================================================
// method definitions
// ===========================================================================

GNEInspectorFrame::GNEInspectorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet):
    GNEFrame(horizontalFrameParent, viewNet, "Inspector"),
    myPreviousElementInspect(nullptr),
    myPreviousElementDelete(nullptr) {

    // Create back button
    myBackButton = new FXButton(myHeaderLeftFrame, "", GUIIconSubSys::getIcon(ICON_BIGARROWLEFT), this, MID_GNE_INSPECTORFRAME_GOBACK, GUIDesignButtonIconRectangular);
    myHeaderLeftFrame->hide();
    myBackButton->hide();

    // Create Overlapped Inspection modul
    myOverlappedInspection = new OverlappedInspection(this);

    // Create Attributes Editor modul
    myAttributesEditor = new GNEFrameAttributesModuls::AttributesEditor(this);

    // Create GEO Parameters Editor modul
    myGEOAttributesEditor = new GEOAttributesEditor(this);

    // create Generic parameters Editor modul
    myGenericParametersEditor = new GNEFrameAttributesModuls::GenericParametersEditor(this);

    // Create Netedit Attributes Editor modul
    myNeteditAttributesEditor = new NeteditAttributesEditor(this);

    // Create Template editor modul
    myTemplateEditor = new TemplateEditor(this);

    // Create AttributeCarrierHierarchy modul
    myAttributeCarrierHierarchy = new GNEFrameModuls::AttributeCarrierHierarchy(this);
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
    myViewNet->setDottedAC(nullptr);
    GNEFrame::hide();
}


bool
GNEInspectorFrame::processNetworkSupermodeClick(const Position& clickedPosition, GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
    // first check if we have clicked over an Attribute Carrier
    if (objectsUnderCursor.getAttributeCarrierFront()) {
        // change the selected attribute carrier if mySelectEdges is enabled and clicked element is a getLaneFront() and shift key isn't pressed
        if (!myViewNet->getKeyPressed().shiftKeyPressed() && myViewNet->getNetworkViewOptions().selectEdges() && (objectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_LANE)) {
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
GNEInspectorFrame::processDemandSupermodeClick(const Position& clickedPosition, GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor) {
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
    // Hide all elements
    myAttributesEditor->hideAttributesEditorModul();
    myNeteditAttributesEditor->hideNeteditAttributesEditor();
    myGEOAttributesEditor->hideGEOAttributesEditor();
    myGenericParametersEditor->hideGenericParametersEditor();
    myTemplateEditor->hideTemplateEditor();
    myAttributeCarrierHierarchy->hideAttributeCarrierHierarchy();
    myOverlappedInspection->hideOverlappedInspection();
    // If vector of attribute Carriers contain data
    if (ACs.size() > 0) {
        // Set header
        std::string headerString;
        if (ACs.front()->getTagProperty().isNetElement()) {
            headerString = "Net: ";
        } else if (ACs.front()->getTagProperty().isAdditional()) {
            headerString = "Additional: ";
        } else if (ACs.front()->getTagProperty().isShape()) {
            headerString = "Shape: ";
        }
        if (ACs.size() > 1) {
            headerString += toString(ACs.size()) + " ";
        }
        headerString += ACs.front()->getTagStr();
        if (ACs.size() > 1) {
            headerString += "s";
        }
        // Set headerString into header label
        getFrameHeaderLabel()->setText(headerString.c_str());

        // Show attributes editor
        myAttributesEditor->showAttributeEditorModul(ACs, true);

        // show netedit attributes editor if  we're inspecting elements with Netedit Attributes
        myNeteditAttributesEditor->showNeteditAttributesEditor();

        // Show GEO Attributes Editor if we're inspecting elements with GEO Attributes
        myGEOAttributesEditor->showGEOAttributesEditor();

        // show generic attributes editor
        if (ACs.size() == 1) {
            myGenericParametersEditor->showGenericParametersEditor(ACs.front());
        } else {
            myGenericParametersEditor->showGenericParametersEditor(ACs);
        }

        // If attributes correspond to an Edge and we aren't in demand mode, show template editor
        myTemplateEditor->showTemplateEditor();

        // if we inspect a single Attribute carrier vector, show their children
        if (ACs.size() == 1) {
            myAttributeCarrierHierarchy->showAttributeCarrierHierarchy(ACs.front());
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
GNEInspectorFrame::clearInspectedAC() {
    // Only remove if there is inspected ACs
    if (myAttributesEditor->getEditedACs().size() > 0) {
        myViewNet->setDottedAC(nullptr);
        // Inspect empty selection (to hide all Editors)
        inspectMultisection({});
    }
}


GNEFrameAttributesModuls::AttributesEditor*
GNEInspectorFrame::getAttributesEditor() const {
    return myAttributesEditor;
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


void
GNEInspectorFrame::updateFrameAfterUndoRedo() {
    // refresh Attribute Editor
    myAttributesEditor->refreshAttributeEditor(false, false);
    // refresh GenericParameters
    myGenericParametersEditor->refreshGenericParametersEditor();
    // refresh AC Hierarchy
    myAttributeCarrierHierarchy->refreshAttributeCarrierHierarchy();
}


void
GNEInspectorFrame::inspectClickedElement(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const Position& clickedPosition) {
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


void
GNEInspectorFrame::attributeUpdated() {
    myAttributesEditor->refreshAttributeEditor(false, false);
    myNeteditAttributesEditor->refreshNeteditAttributesEditor(true);
    myGEOAttributesEditor->refreshGEOAttributesEditor(true);
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
    myPreviousElement = new FXButton(frameButtons, "", GUIIconSubSys::getIcon(ICON_BIGARROWLEFT), this, MID_GNE_OVERLAPPED_PREVIOUS, GUIDesignButtonIconRectangular);
    // create current index button
    myCurrentIndexButton = new FXButton(frameButtons, "", nullptr, this, MID_GNE_OVERLAPPED_SHOWLIST, GUIDesignButton);
    // Create next Item Button
    myNextElement = new FXButton(frameButtons, "", GUIIconSubSys::getIcon(ICON_BIGARROWRIGHT), this, MID_GNE_OVERLAPPED_NEXT, GUIDesignButtonIconRectangular);
    // Create list of overlapped elements (by default hidden)
    myOverlappedElementList = new FXList(this, this, MID_GNE_OVERLAPPED_ITEMSELECTED, GUIDesignListSingleElement);
    // disable vertical scrolling
    myOverlappedElementList->setScrollStyle(VSCROLLING_OFF);
    // by default list of overlapped elements is hidden)
    myOverlappedElementList->hide();
    // Create help button
    myHelpButton = new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEInspectorFrame::OverlappedInspection::~OverlappedInspection() {}


void
GNEInspectorFrame::OverlappedInspection::showOverlappedInspection(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, const Position& clickedPosition) {
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
    if (AC->getTagProperty().getTag() == SUMO_TAG_LANE && myInspectorFrameParent->getViewNet()->getNetworkViewOptions().selectEdges()) {
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
    myCheckBoxBlockMovement = new FXCheckButton(myHorizontalFrameBlockMovement, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);

    // Create elements for block shape
    myHorizontalFrameBlockShape = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLabelBlockShape = new FXLabel(myHorizontalFrameBlockShape, "Block shape", nullptr, GUIDesignLabelAttribute);
    myCheckBoxBlockShape = new FXCheckButton(myHorizontalFrameBlockShape, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);

    // Create elements for close shape
    myHorizontalFrameCloseShape = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLabelCloseShape = new FXLabel(myHorizontalFrameCloseShape, "Close shape", nullptr, GUIDesignLabelAttribute);
    myCheckBoxCloseShape = new FXCheckButton(myHorizontalFrameCloseShape, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);

    // Create help button
    myHelpButton = new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEInspectorFrame::NeteditAttributesEditor::~NeteditAttributesEditor() {}


void
GNEInspectorFrame::NeteditAttributesEditor::showNeteditAttributesEditor() {
    if (myInspectorFrameParent->myAttributesEditor->getEditedACs().size() > 0) {
        // enable all editable elements
        myTextFieldAdditionalParent->enable();
        myCheckBoxBlockMovement->enable();
        myCheckBoxBlockShape->enable();
        myCheckBoxCloseShape->enable();
        // obtain tag property (only for improve code legibility)
        const auto& tagValue = myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getTagProperty();
        // Check if item can be moved
        if (tagValue.canBlockMovement()) {
            // show NeteditAttributesEditor
            show();
            // Iterate over AC to obtain values
            bool value = true;
            for (const auto& i : myInspectorFrameParent->myAttributesEditor->getEditedACs()) {
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
            for (const auto& i : myInspectorFrameParent->myAttributesEditor->getEditedACs()) {
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
            for (const auto& i : myInspectorFrameParent->myAttributesEditor->getEditedACs()) {
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
            for (const auto& i : myInspectorFrameParent->myAttributesEditor->getEditedACs()) {
                parents.insert(i->getAttribute(GNE_ATTR_PARENT));
            }
            // show additional parent frame
            myHorizontalFrameAdditionalParent->show();
            // set Label and TextField with the Tag and ID of parent
            myLabelAdditionalParent->setText((toString(myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getTagProperty().getParentTag()) + " parent").c_str());
            myTextFieldAdditionalParent->setText(toString(parents).c_str());
        }
        // disable all editable elements if we're in demand mode and inspected AC isn't a demand element
        if (((myInspectorFrameParent->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) && myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getTagProperty().isDemandElement()) ||
                ((myInspectorFrameParent->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && !myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getTagProperty().isDemandElement())) {
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
    if (myInspectorFrameParent->myAttributesEditor->getEditedACs().size() > 0) {
        // refresh block movement
        if (myHorizontalFrameBlockMovement->shown()) {
            // Iterate over AC to obtain values
            bool value = true;
            for (const auto& i : myInspectorFrameParent->myAttributesEditor->getEditedACs()) {
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
            for (const auto& i : myInspectorFrameParent->myAttributesEditor->getEditedACs()) {
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
            for (const auto& i : myInspectorFrameParent->myAttributesEditor->getEditedACs()) {
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
            myLabelAdditionalParent->setText((toString(myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getTagProperty().getParentTag()) + " parent").c_str());
            myTextFieldAdditionalParent->setText(myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getAttribute(GNE_ATTR_PARENT).c_str());
        }
    }
}


long
GNEInspectorFrame::NeteditAttributesEditor::onCmdSetNeteditAttribute(FXObject* obj, FXSelector, void*) {
    // make sure that ACs has elements
    if (myInspectorFrameParent->myAttributesEditor->getEditedACs().size() > 0) {
        // check if we're changing multiple attributes
        if (myInspectorFrameParent->myAttributesEditor->getEditedACs().size() > 1) {
            myInspectorFrameParent->getViewNet()->getUndoList()->p_begin("Change multiple attributes");
        }
        if (obj == myCheckBoxBlockMovement) {
            // set new values in all inspected Attribute Carriers
            for (const auto& i : myInspectorFrameParent->myAttributesEditor->getEditedACs()) {
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
            for (const auto& i : myInspectorFrameParent->myAttributesEditor->getEditedACs()) {
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
            for (const auto& i : myInspectorFrameParent->myAttributesEditor->getEditedACs()) {
                if (myCheckBoxCloseShape->getCheck() == 1) {
                    i->setAttribute(GNE_ATTR_CLOSE_SHAPE, "true", myInspectorFrameParent->getViewNet()->getUndoList());
                    myCheckBoxCloseShape->setText("true");
                } else {
                    i->setAttribute(GNE_ATTR_CLOSE_SHAPE, "false", myInspectorFrameParent->getViewNet()->getUndoList());
                    myCheckBoxCloseShape->setText("false");
                }
            }
        } else if (obj == myTextFieldAdditionalParent) {
            if (myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->isValid(GNE_ATTR_PARENT, myTextFieldAdditionalParent->getText().text())) {
                // change parent of all inspected elements
                for (const auto& i : myInspectorFrameParent->myAttributesEditor->getEditedACs()) {
                    i->setAttribute(GNE_ATTR_PARENT, myTextFieldAdditionalParent->getText().text(), myInspectorFrameParent->getViewNet()->getUndoList());
                }
                myTextFieldAdditionalParent->setTextColor(FXRGB(0, 0, 0));
            } else {
                myTextFieldAdditionalParent->setTextColor(FXRGB(255, 0, 0));
                myTextFieldAdditionalParent->killFocus();
            }
        }
        // finish change multiple attributes
        if (myInspectorFrameParent->myAttributesEditor->getEditedACs().size() > 1) {
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
    myUseGEOCheckButton = new FXCheckButton(myUseGEOFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);

    // Create help button
    myHelpButton = new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEInspectorFrame::GEOAttributesEditor::~GEOAttributesEditor() {}


void
GNEInspectorFrame::GEOAttributesEditor::showGEOAttributesEditor() {
    // make sure that ACs has elements
    if (myInspectorFrameParent->myAttributesEditor->getEditedACs().size() > 0) {
        // enable all editable elements
        myGEOAttributeTextField->enable();
        myUseGEOCheckButton->enable();
        // obtain tag property (only for improve code legibility)
        const auto& tagProperty = myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getTagProperty();
        // check if item can use a geo position
        if (tagProperty.hasGEOPosition() || tagProperty.hasGEOShape()) {
            // show GEOAttributesEditor
            show();
            // Iterate over AC to obtain values
            bool value = true;
            for (const auto& i : myInspectorFrameParent->myAttributesEditor->getEditedACs()) {
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
            if (tagProperty.hasGEOPosition() && myInspectorFrameParent->myAttributesEditor->getEditedACs().size() == 1) {
                myGEOAttributeFrame->show();
                myGEOAttributeLabel->setText(toString(SUMO_ATTR_GEOPOSITION).c_str());
                myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
                // only allow edit if geo conversion is defined
                if (GeoConvHelper::getFinal().getProjString() != "!") {
                    myGEOAttributeTextField->enable();
                    myGEOAttributeTextField->setText(myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getAttribute(SUMO_ATTR_GEOPOSITION).c_str());
                } else {
                    myGEOAttributeTextField->disable();
                    myGEOAttributeTextField->setText("No geo-conversion defined");
                }
            } else if (tagProperty.hasGEOShape() && myInspectorFrameParent->myAttributesEditor->getEditedACs().size() == 1) {
                myGEOAttributeFrame->show();
                myGEOAttributeLabel->setText(toString(SUMO_ATTR_GEOSHAPE).c_str());
                myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
                // only allow edit if geo conversion is defined
                if (GeoConvHelper::getFinal().getProjString() != "!") {
                    myGEOAttributeTextField->enable();
                    myGEOAttributeTextField->setText(myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getAttribute(SUMO_ATTR_GEOSHAPE).c_str());
                } else {
                    myGEOAttributeTextField->disable();
                    myGEOAttributeTextField->setText("No geo-conversion defined");
                }
            }
        }
        // disable all editable elements if we're in demand mode and inspected AC isn't a demand element
        if (((myInspectorFrameParent->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) && myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getTagProperty().isDemandElement()) ||
                ((myInspectorFrameParent->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_DEMAND) && !myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getTagProperty().isDemandElement())) {
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
    const auto& tagProperty = myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getTagProperty();
    // Check that myGEOAttributeFrame is shown
    if ((GeoConvHelper::getFinal().getProjString() != "!") && myGEOAttributeFrame->shown() && ((myGEOAttributeTextField->getTextColor() == FXRGB(0, 0, 0)) || forceRefresh)) {
        if (tagProperty.hasGEOPosition()) {
            myGEOAttributeTextField->setText(myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getAttribute(SUMO_ATTR_GEOPOSITION).c_str());
        } else if (tagProperty.hasGEOShape()) {
            myGEOAttributeTextField->setText(myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getAttribute(SUMO_ATTR_GEOSHAPE).c_str());
        }
        myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
    }
}


long
GNEInspectorFrame::GEOAttributesEditor::onCmdSetGEOAttribute(FXObject* obj, FXSelector, void*) {
    // make sure that ACs has elements
    if ((GeoConvHelper::getFinal().getProjString() != "!") && (myInspectorFrameParent->myAttributesEditor->getEditedACs().size() > 0)) {
        if (obj == myGEOAttributeTextField) {
            // obtain tag property (only for improve code legibility)
            const auto& tagProperty = myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getTagProperty();
            // Change GEO Attribute depending of type (Position or shape)
            if (tagProperty.hasGEOPosition()) {
                if (myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->isValid(SUMO_ATTR_GEOPOSITION, myGEOAttributeTextField->getText().text())) {
                    myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->setAttribute(SUMO_ATTR_GEOPOSITION, myGEOAttributeTextField->getText().text(), myInspectorFrameParent->getViewNet()->getUndoList());
                    myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
                } else {
                    myGEOAttributeTextField->setTextColor(FXRGB(255, 0, 0));
                    myGEOAttributeTextField->killFocus();
                }
            } else if (tagProperty.hasGEOShape()) {
                if (myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->isValid(SUMO_ATTR_GEOSHAPE, myGEOAttributeTextField->getText().text())) {
                    myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->setAttribute(SUMO_ATTR_GEOSHAPE, myGEOAttributeTextField->getText().text(), myInspectorFrameParent->getViewNet()->getUndoList());
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
            for (const auto& i : myInspectorFrameParent->myAttributesEditor->getEditedACs()) {
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
    myCopyTemplateButton = new FXButton(this, "", nullptr, this, MID_GNE_TEMPLATE_COPY, GUIDesignButton);
    myCopyTemplateButton->hide();

    // Create set template button
    mySetTemplateButton = new FXButton(this, "Set as Template\t\t", nullptr, this, MID_GNE_TEMPLATE_SET, GUIDesignButton);
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
    // show template editor only if we're editing an edge in Network mode
    if ((myInspectorFrameParent->getViewNet()->getEditModes().currentSupermode == GNE_SUPERMODE_NETWORK) &&
            (myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getTagProperty().getTag() == SUMO_TAG_EDGE)) {
        show();
        // show "Copy Template" (caption supplied via onUpdate)
        myCopyTemplateButton->show();
        // show "Set As Template"
        if (myInspectorFrameParent->myAttributesEditor->getEditedACs().size() == 1) {
            mySetTemplateButton->show();
            mySetTemplateButton->setText(("Set edge '" + myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getID() + "' as Template").c_str());
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
    for (const auto& it : myInspectorFrameParent->myAttributesEditor->getEditedACs()) {
        GNEEdge* edge = dynamic_cast<GNEEdge*>(it);
        assert(edge);
        edge->copyTemplate(myEdgeTemplate, myInspectorFrameParent->getViewNet()->getUndoList());
        myInspectorFrameParent->inspectMultisection(myInspectorFrameParent->myAttributesEditor->getEditedACs());
    }
    return 1;
}


long
GNEInspectorFrame::TemplateEditor::onCmdSetTemplate(FXObject*, FXSelector, void*) {
    assert(myInspectorFrameParent->myAttributesEditor->getEditedACs().size() == 1);
    GNEEdge* edge = dynamic_cast<GNEEdge*>(myInspectorFrameParent->myAttributesEditor->getEditedACs().front());
    assert(edge);
    setEdgeTemplate(edge);
    return 1;
}


long
GNEInspectorFrame::TemplateEditor::onUpdCopyTemplate(FXObject* sender, FXSelector, void*) {
    // declare caption for button
    FXString caption;
    if (myEdgeTemplate) {
        if (myInspectorFrameParent->myAttributesEditor->getEditedACs().size() == 1) {
            caption = ("Copy '" + myEdgeTemplate->getMicrosimID() + "' into edge '" + myInspectorFrameParent->myAttributesEditor->getEditedACs().front()->getID() + "'").c_str();
        } else {
            caption = ("Copy '" + myEdgeTemplate->getMicrosimID() + "' into " + toString(myInspectorFrameParent->myAttributesEditor->getEditedACs().size()) + " selected edges").c_str();
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
