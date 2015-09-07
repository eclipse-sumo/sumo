/****************************************************************************/
/// @file    GNEInspector.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id: GNEInspector.cpp 4419 2015-03-17 09:55:18Z erdm_ja $
///
// The Widget for modifying network-element attributes (i.e. lane speed)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <cmath>
#include <cassert>
#include <iostream>
#include <utils/foxtools/MFXUtils.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include "GNEInspector.h"
#include "GNEUndoList.h"
#include "GNEEdge.h"
#include "GNEAttributeCarrier.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEInspector) GNEInspectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_COPY_TEMPLATE, GNEInspector::onCmdCopyTemplate),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TEMPLATE,  GNEInspector::onCmdSetTemplate),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_COPY_TEMPLATE, GNEInspector::onUpdCopyTemplate)
};

/*
FXDEFMAP(GNEInspector::AttrPanel) AttrPanelMap[]= {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_COPY_TEMPLATE, GNEInspector::AttrPanelMap::onCmdCopyTemplate),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TEMPLATE,  GNEInspector::AttrPanelMap::onCmdSetTemplate)
};
*/

FXDEFMAP(GNEInspector::AttrInput) AttrInputMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE, GNEInspector::AttrInput::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OPEN_ATTRIBUTE_EDITOR, GNEInspector::AttrInput::onCmdOpenAttributeEditor)
};

// Object implementation
FXIMPLEMENT(GNEInspector, FXScrollWindow, GNEInspectorMap, ARRAYNUMBER(GNEInspectorMap))
// FXIMPLEMENT(GNEInspector::AttrPanel, FXVerticalFrame, AttrPanelMap, ARRAYNUMBER(AttrPanelMap))
FXIMPLEMENT(GNEInspector::AttrInput, FXHorizontalFrame, AttrInputMap, ARRAYNUMBER(AttrInputMap))

// ===========================================================================
// static members
// ===========================================================================
const int GNEInspector::WIDTH = 140;

// ===========================================================================
// method definitions
// ===========================================================================
GNEInspector::GNEInspector(FXComposite* parent, GNEUndoList* undoList):
    FXScrollWindow(parent, LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0, 0, WIDTH, 0),
    myUndoList(undoList),
    myHeaderFont(new FXFont(getApp(), "Arial", 18, FXFont::Bold)),
    myPanel(0),
    myEdgeTemplate(0) {
    myPanel = new AttrPanel(this, myACs, undoList);
}


void
GNEInspector::create() {
    FXScrollWindow::create();
    myHeaderFont->create();
    myPanel->create();
}


GNEInspector::~GNEInspector() {
    delete myPanel;
    delete myHeaderFont;
    if (myEdgeTemplate) {
        myEdgeTemplate->decRef("GNEInspector::~GNEInspector");
        if (myEdgeTemplate->unreferenced()) {
            delete myEdgeTemplate;
        }
    }
}


void
GNEInspector::inspect(const std::vector<GNEAttributeCarrier*>& ACs) {
    delete myPanel;
    myACs = ACs;
    myPanel = new AttrPanel(this, myACs, myUndoList);
    myPanel->create();
    recalc();
}


void
GNEInspector::update() {
    inspect(myACs);
}

void
GNEInspector::setEdgeTemplate(GNEEdge* tpl) {
    if (myEdgeTemplate) {
        myEdgeTemplate->decRef("GNEInspector::setEdgeTemplate");
        if (myEdgeTemplate->unreferenced()) {
            delete myEdgeTemplate;
        }
    }
    myEdgeTemplate = tpl;
    myEdgeTemplate->incRef("GNEInspector::setEdgeTemplate");
}


long
GNEInspector::onCmdCopyTemplate(FXObject*, FXSelector, void*) {
    for (std::vector<GNEAttributeCarrier*>::iterator it = myACs.begin(); it != myACs.end(); it++) {
        GNEEdge* edge = dynamic_cast<GNEEdge*>(*it);
        assert(edge);
        edge->copyTemplate(myEdgeTemplate, myUndoList);
    }
    return 1;
}


long
GNEInspector::onCmdSetTemplate(FXObject*, FXSelector, void*) {
    assert(myACs.size() == 1);
    GNEEdge* edge = dynamic_cast<GNEEdge*>(myACs[0]);
    assert(edge);
    setEdgeTemplate(edge);
    myPanel->update();
    return 1;
}


long
GNEInspector::onUpdCopyTemplate(FXObject* sender, FXSelector, void*) {
    FXString caption;
    if (myEdgeTemplate) {
        caption = ("Copy '" + myEdgeTemplate->getMicrosimID() + "'").c_str();
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), NULL);
    } else {
        caption = "No Template Set";
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    }
    sender->handle(this, FXSEL(SEL_COMMAND, FXLabel::ID_SETSTRINGVALUE), (void*)&caption);
    return 1;
}


// ===========================================================================
// AttrPanel method definitions
// ===========================================================================

GNEInspector::AttrPanel::AttrPanel(GNEInspector* parent, const std::vector<GNEAttributeCarrier*>& ACs, GNEUndoList* undoList) :
    FXVerticalFrame(parent, LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0, 0, WIDTH, 0, 2, 0, 0, 0, 0, 0) {
    FXLabel* header;
    if (ACs.size() > 0) {
        std::string headerString = toString(ACs[0]->getTag());
        if (ACs.size() > 1) {
            headerString = toString(ACs.size()) + " " + headerString + "s";
        }
        header = new FXLabel(this, headerString.c_str());
        new FXHorizontalSeparator(this, SEPARATOR_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 2, 2, 2, 4, 4);

        const std::vector<SumoXMLAttr>& attrs = ACs[0]->getAttrs();
        for (std::vector<SumoXMLAttr>::const_iterator it = attrs.begin(); it != attrs.end(); it++) {
            if (ACs.size() > 1 && GNEAttributeCarrier::isUnique(*it)) {
                // disable editing for some attributes in case of multi-selection
                // even displaying is problematic because of string rendering restrictions
                continue;
            }
            std::set<std::string> occuringValues;
            for (std::vector<GNEAttributeCarrier*>::const_iterator it_ac = ACs.begin(); it_ac != ACs.end(); it_ac++) {
                occuringValues.insert((*it_ac)->getAttribute(*it));
            }
            std::ostringstream oss;
            for (std::set<std::string>::iterator it_val = occuringValues.begin(); it_val != occuringValues.end(); it_val++) {
                if (it_val != occuringValues.begin()) {
                    oss << " ";
                }
                oss << *it_val;
            }
            new AttrInput(this, ACs, *it, oss.str(), undoList);
        }

        if (dynamic_cast<GNEEdge*>(ACs[0])) {
            new FXHorizontalSeparator(this, SEPARATOR_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 2, 2, 2, 4, 4);
            // "Copy Template" (caption supplied via onUpdate)
            new FXButton(this, "", 0, parent, MID_GNE_COPY_TEMPLATE,
                         ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                         0, 0, 0, 0, 4, 4, 3, 3);

            if (ACs.size() == 1) {
                // "Set As Template"
                new FXButton(this, "Set as Template\t\t", 0, parent, MID_GNE_SET_TEMPLATE,
                             ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                             0, 0, 0, 0, 4, 4, 3, 3);
            }
        };
    } else {
        header = new FXLabel(this, "No Object\nselected", 0, JUSTIFY_LEFT);
    }
    header->setFont(parent->getHeaderFont());

}


// ===========================================================================
// AttrInput method definitions
//
// ===========================================================================
GNEInspector::AttrInput::AttrInput(
    FXComposite* parent,
    const std::vector<GNEAttributeCarrier*>& ACs, SumoXMLAttr attr, std::string initialValue,
    GNEUndoList* undoList) :
    FXHorizontalFrame(parent, LAYOUT_FILL_X, 0, 0, WIDTH, 0, 0, 0, 0, 2),
    myAttr(attr),
    myACs(&ACs),
    myUndoList(undoList),
    myTextField(0),
    myChoicesCombo(0) {
    //std::string label = toString(attr) + "\t\tOpen edit dialog for attribute '" + toString(attr) + "'";
    std::string label = toString(attr);
    FXButton* but = new FXButton(this, label.c_str(), 0, this, MID_GNE_OPEN_ATTRIBUTE_EDITOR,
                                 0, 0, 0, 0, 0, DEFAULT_PAD, DEFAULT_PAD, 1, 1);
    //BUTTON_NORMAL, 0,0,0,0, DEFAULT_PAD,DEFAULT_PAD, 1,1);
    int cols = (WIDTH - but->getDefaultWidth() - 6) / 9;
    const std::vector<std::string>& choices = GNEAttributeCarrier::discreteChoices(ACs[0]->getTag(), attr);
    if (choices.size() == 0) {
        // rudimentary input restriction
        unsigned int numerical = GNEAttributeCarrier::isNumerical(attr) ? TEXTFIELD_REAL : 0;
        myTextField = new FXTextField(this, cols,
                                      this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_RIGHT | numerical, 0, 0, 0, 0, 4, 2, 0, 2);
        myTextField->setText(initialValue.c_str());
    } else {
        myChoicesCombo = new FXComboBox(this, 12, this, MID_GNE_SET_ATTRIBUTE,
                                        FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC | LAYOUT_CENTER_Y);
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            myChoicesCombo->appendItem(it->c_str());
        }
        myChoicesCombo->setNumVisible((int)choices.size());
        myChoicesCombo->setCurrentItem(myChoicesCombo->findItem(initialValue.c_str()));
    }
}


long
GNEInspector::AttrInput::onCmdOpenAttributeEditor(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEInspector::AttrInput::onCmdSetAttribute(FXObject*, FXSelector, void* data) {
    std::string newVal(myTextField != 0 ? myTextField->getText().text() : (char*) data);
    const std::vector<GNEAttributeCarrier*>& ACs = *myACs;
    if (ACs[0]->isValid(myAttr, newVal)) {
        // if its valid for the first AC than its valid for all (of the same type)
        if (ACs.size() > 1) {
            myUndoList->p_begin("Change multiple attributes");
        }
        for (std::vector<GNEAttributeCarrier*>::const_iterator it_ac = ACs.begin(); it_ac != ACs.end(); it_ac++) {
            (*it_ac)->setAttribute(myAttr, newVal, myUndoList);
        }
        if (ACs.size() > 1) {
            myUndoList->p_end();
        }
        if (myTextField != 0) {
            myTextField->setTextColor(FXRGB(0, 0, 0));
            myTextField->killFocus();
        }
    } else {
        if (myTextField != 0) {
            myTextField->setTextColor(FXRGB(255, 0, 0));
        }
    }
    return 1;
}
/****************************************************************************/
