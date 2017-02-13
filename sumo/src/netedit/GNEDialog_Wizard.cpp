/****************************************************************************/
/// @file    GNEDialog_Wizard.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// The "About" - dialog for NETEDIT, (adapted from GUIDialog_AboutSUMO)
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

#include <utils/foxtools/FXLinkLabel.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/common/ToString.h>
#include <utils/gui/div/GUIDesigns.h>
#include "GNEDialog_Wizard.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


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
GNEDialog_Wizard::GNEDialog_Wizard(FXWindow* parent,  const char* name, int width, int height) :
    FXDialogBox(parent, name, GUIDesignDialogBox, 0, 0, width, height) {
    OptionsCont& oc = OptionsCont::getOptions();
    FXVerticalFrame* contentFrame = new FXVerticalFrame(this, GUIDesignContentsFrame);

    FXTabBook* tabbook = new FXTabBook(contentFrame, 0, 0, GUIDesignTabBook);

    const std::vector<std::string>& topics = oc.getSubTopics();
    for (std::vector<std::string>::const_iterator it_topic = topics.begin(); it_topic != topics.end(); it_topic++) {
        std::string topic = *it_topic;
        if (topic == "Configuration") {
            continue;
        }
        new FXTabItem(tabbook, topic.c_str(), NULL, TAB_LEFT_NORMAL);
        FXScrollWindow* scrollTab = new FXScrollWindow(tabbook, LAYOUT_FILL_X | LAYOUT_FILL_Y);
        FXVerticalFrame* tabContent = new FXVerticalFrame(scrollTab, FRAME_THICK | FRAME_RAISED | LAYOUT_FILL_X | LAYOUT_FILL_Y);
        const std::vector<std::string> entries = oc.getSubTopicsEntries(topic);
        for (std::vector<std::string>::const_iterator it_opt = entries.begin(); it_opt != entries.end(); it_opt++) {
            std::string name = *it_opt;
            if (name != "geometry.remove" && name != "edges.join" && name != "geometry.split" && name != "ramps.guess" && name != "ramps.set") {
                std::string type = oc.getTypeName(name);
                if (type == "STR" || type == "FILE") {
                    new InputString(tabContent, name);
                } else if (type == "BOOL") {
                    new InputBool(tabContent, name);
                } else if (type == "INT") {
                    new InputInt(tabContent, name);
                } else if (type == "FLOAT") {
                    new InputFloat(tabContent, name);
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
