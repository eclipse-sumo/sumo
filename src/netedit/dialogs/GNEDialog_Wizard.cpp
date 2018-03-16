/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDialog_Wizard.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// The "About" - dialog for NETEDIT, (adapted from GUIDialog_AboutSUMO)
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/foxtools/FXLinkLabel.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/common/ToString.h>
#include <utils/gui/div/GUIDesigns.h>
#include "GNEDialog_Wizard.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEDialog_Wizard::InputString) InputStringMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE, GNEDialog_Wizard::InputString::onCmdSetOption),
};
FXDEFMAP(GNEDialog_Wizard::InputBool) InputBoolMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE, GNEDialog_Wizard::InputBool::onCmdSetOption),
};
FXDEFMAP(GNEDialog_Wizard::InputInt) InputIntMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE, GNEDialog_Wizard::InputInt::onCmdSetOption),
};
FXDEFMAP(GNEDialog_Wizard::InputFloat) InputFloatMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE, GNEDialog_Wizard::InputFloat::onCmdSetOption),
};

// Object implementation
FXIMPLEMENT(GNEDialog_Wizard::InputString, FXHorizontalFrame, InputStringMap, ARRAYNUMBER(InputStringMap))
FXIMPLEMENT(GNEDialog_Wizard::InputBool, FXHorizontalFrame, InputBoolMap, ARRAYNUMBER(InputBoolMap))
FXIMPLEMENT(GNEDialog_Wizard::InputInt, FXHorizontalFrame, InputIntMap, ARRAYNUMBER(InputIntMap))
FXIMPLEMENT(GNEDialog_Wizard::InputFloat, FXHorizontalFrame, InputFloatMap, ARRAYNUMBER(InputFloatMap))

// ===========================================================================
// method definitions
// ===========================================================================
GNEDialog_Wizard::GNEDialog_Wizard(FXWindow* parent,  const char* titleName, int width, int height) :
    FXDialogBox(parent, titleName, GUIDesignDialogBox, 0, 0, width, height) {
    OptionsCont& oc = OptionsCont::getOptions();
    FXVerticalFrame* contentFrame = new FXVerticalFrame(this, GUIDesignContentsFrame);

    FXTabBook* tabbook = new FXTabBook(contentFrame, 0, 0, GUIDesignTabBook);

    for (auto it_topic : oc.getSubTopics()) {
        if (it_topic == "Configuration") {
            continue;
        }
        new FXTabItem(tabbook, it_topic.c_str(), NULL, TAB_LEFT_NORMAL);
        FXScrollWindow* scrollTab = new FXScrollWindow(tabbook, LAYOUT_FILL_X | LAYOUT_FILL_Y);
        FXVerticalFrame* tabContent = new FXVerticalFrame(scrollTab, FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_X | LAYOUT_FILL_Y);
        const std::vector<std::string> entries = oc.getSubTopicsEntries(it_topic);
        for (auto it_opt : entries) {
            if (it_opt != "geometry.remove" && it_opt != "edges.join" && it_opt != "geometry.split" && it_opt != "ramps.guess" && it_opt != "ramps.set") {
                std::string type = oc.getTypeName(it_opt);
                if (type == "STR" || type == "FILE") {
                    new InputString(tabContent, it_opt);
                } else if (type == "BOOL") {
                    new InputBool(tabContent, it_opt);
                } else if (type == "INT") {
                    new InputInt(tabContent, it_opt);
                } else if (type == "FLOAT") {
                    new InputFloat(tabContent, it_opt);
                }
                // @todo missing types (type INT[] is only used in microsim)
            }
        }
    }

    // ok-button
    new FXButton(contentFrame, "OK\t\tContine with the import.", GUIIconSubSys::getIcon(ICON_ACCEPT), this, ID_ACCEPT, GUIDesignButtonOK);
}


GNEDialog_Wizard::~GNEDialog_Wizard() { }

// ===========================================================================
// Option input classes method definitions
// ===========================================================================
GNEDialog_Wizard::InputString::InputString(FXComposite* parent, const std::string& name) :
    FXHorizontalFrame(parent, LAYOUT_FILL_X),
    myName(name) {
    OptionsCont& oc = OptionsCont::getOptions();
    new FXLabel(this, name.c_str());
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(oc.getString(name).c_str());
}


long
GNEDialog_Wizard::InputString::onCmdSetOption(FXObject*, FXSelector, void*) {
    OptionsCont& oc = OptionsCont::getOptions();
    oc.resetWritable();
    oc.set(myName, myTextField->getText().text());
    return 1;
}


GNEDialog_Wizard::InputBool::InputBool(FXComposite* parent, const std::string& name) :
    FXHorizontalFrame(parent, LAYOUT_FILL_X),
    myName(name) {
    OptionsCont& oc = OptionsCont::getOptions();
    new FXLabel(this, name.c_str());
    myCheck = new FXMenuCheck(this, "", this, MID_GNE_SET_ATTRIBUTE);
    myCheck->setCheck(oc.getBool(name));
}


long
GNEDialog_Wizard::InputBool::onCmdSetOption(FXObject*, FXSelector, void*) {
    OptionsCont& oc = OptionsCont::getOptions();
    oc.resetWritable();
    oc.set(myName, myCheck->getCheck() ? "true" : "false");
    return 1;
}


GNEDialog_Wizard::InputInt::InputInt(FXComposite* parent, const std::string& name) :
    FXHorizontalFrame(parent, LAYOUT_FILL_X),
    myName(name) {
    OptionsCont& oc = OptionsCont::getOptions();
    new FXLabel(this, name.c_str());
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_INTEGER | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(toString(oc.getInt(name)).c_str());
}


long
GNEDialog_Wizard::InputInt::onCmdSetOption(FXObject*, FXSelector, void*) {
    OptionsCont& oc = OptionsCont::getOptions();
    oc.resetWritable();
    oc.set(myName, myTextField->getText().text());
    return 1;
}


GNEDialog_Wizard::InputFloat::InputFloat(FXComposite* parent, const std::string& name) :
    FXHorizontalFrame(parent, LAYOUT_FILL_X),
    myName(name) {
    OptionsCont& oc = OptionsCont::getOptions();
    new FXLabel(this, name.c_str());
    myTextField = new FXTextField(this, 100, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_REAL | LAYOUT_RIGHT, 0, 0, 0, 0, 4, 2, 0, 2);
    myTextField->setText(toString(oc.getFloat(name)).c_str());
}


long
GNEDialog_Wizard::InputFloat::onCmdSetOption(FXObject*, FXSelector, void*) {
    OptionsCont& oc = OptionsCont::getOptions();
    oc.resetWritable();
    oc.set(myName, myTextField->getText().text());
    return 1;
}


/****************************************************************************/
