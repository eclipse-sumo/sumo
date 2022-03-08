/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GUIFrictionCoefficientTrigger.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 26.04.2004
/// @author  Thomas Weber
/// @date    Jan 2018
///
// Changes the friction on a set of lanes (gui version)
/****************************************************************************/
#include <config.h>

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/geom/PositionVector.h>
#include <utils/geom/Boundary.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/common/ToString.h>
#include <utils/common/Command.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include "GUIFrictionCoefficientTrigger.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <gui/GUIApplicationWindow.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <guisim/GUIFrictionCoefficientTrigger.h>
#include <utils/gui/globjects/GLIncludes.h>


// ===========================================================================
// FOX callback mapping
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIFrictionCoefficientTrigger::GUIFrictionCoefficientTriggerPopupMenu - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUIFrictionCoefficientTrigger::GUIFrictionCoefficientTriggerPopupMenu)
GUIFrictionCoefficientTriggerPopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_MANIP,         GUIFrictionCoefficientTrigger::GUIFrictionCoefficientTriggerPopupMenu::onCmdOpenManip),

};

// Object implementation
FXIMPLEMENT(GUIFrictionCoefficientTrigger::GUIFrictionCoefficientTriggerPopupMenu, GUIGLObjectPopupMenu, GUIFrictionCoefficientTriggerPopupMenuMap, ARRAYNUMBER(GUIFrictionCoefficientTriggerPopupMenuMap))


/* -------------------------------------------------------------------------
 * GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger) GUIManip_FrictionCoefficientTriggerMap[] = {
    FXMAPFUNC(SEL_COMMAND,  GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::MID_USER_DEF, GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::onCmdUserDef),
    FXMAPFUNC(SEL_UPDATE,   GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::MID_USER_DEF, GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::onUpdUserDef),
    FXMAPFUNC(SEL_COMMAND,  GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::MID_PRE_DEF,  GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::onCmdPreDef),
    FXMAPFUNC(SEL_UPDATE,   GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::MID_PRE_DEF,  GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::onUpdPreDef),
    FXMAPFUNC(SEL_COMMAND,  GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::MID_OPTION,   GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::onCmdChangeOption),
    FXMAPFUNC(SEL_COMMAND,  GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::MID_CLOSE,    GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::onCmdClose),
};

FXIMPLEMENT(GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger, GUIManipulator, GUIManip_FrictionCoefficientTriggerMap, ARRAYNUMBER(GUIManip_FrictionCoefficientTriggerMap))


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger - methods
 * ----------------------------------------------------------------------- */
GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger(
    GUIMainWindow& app,
    const std::string& name, GUIFrictionCoefficientTrigger& o,
    int /*xpos*/, int /*ypos*/)
    : GUIManipulator(app, name, 0, 0),
      myParent(&app), myChosenValue(0), myChosenTarget(myChosenValue, nullptr, MID_OPTION),
      myFriction(o.getDefaultFriction()), myFrictionTarget(myFriction),
      myObject(&o) {
    myChosenTarget.setTarget(this);
    FXVerticalFrame* f1 =
        new FXVerticalFrame(this, LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);

    FXGroupBox* gp = new FXGroupBox(f1, "Change Friction",
                                    GROUPBOX_TITLE_LEFT | FRAME_RIDGE,
                                    0, 0, 0, 0,  4, 4, 1, 1, 2, 0);
    {
        // default
        FXHorizontalFrame* gf1 =
            new FXHorizontalFrame(gp, LAYOUT_TOP | LAYOUT_LEFT, 0, 0, 0, 0, 10, 10, 5, 5);
        new FXRadioButton(gf1, "Default", &myChosenTarget, FXDataTarget::ID_OPTION + 0,
                          ICON_BEFORE_TEXT | LAYOUT_SIDE_TOP,
                          0, 0, 0, 0,   2, 2, 0, 0);
    }
    {
        // loaded
        FXHorizontalFrame* gf0 =
            new FXHorizontalFrame(gp, LAYOUT_TOP | LAYOUT_LEFT, 0, 0, 0, 0, 10, 10, 5, 5);
        new FXRadioButton(gf0, "Loaded", &myChosenTarget, FXDataTarget::ID_OPTION + 1,
                          ICON_BEFORE_TEXT | LAYOUT_SIDE_TOP,
                          0, 0, 0, 0,   2, 2, 0, 0);
    }
    {
        // predefined
        FXHorizontalFrame* gf2 =
            new FXHorizontalFrame(gp, LAYOUT_TOP | LAYOUT_LEFT, 0, 0, 0, 0, 10, 10, 5, 5);
        new FXRadioButton(gf2, "Predefined: ", &myChosenTarget, FXDataTarget::ID_OPTION + 2,
                          ICON_BEFORE_TEXT | LAYOUT_SIDE_TOP | LAYOUT_CENTER_Y,
                          0, 0, 0, 0,   2, 2, 0, 0);
        myPredefinedValues =
            new FXComboBox(gf2, 10, this, MID_PRE_DEF,
                           ICON_BEFORE_TEXT | LAYOUT_SIDE_TOP | LAYOUT_CENTER_Y | COMBOBOX_STATIC);
		myPredefinedValues->appendItem("10 %");
		myPredefinedValues->appendItem("20 %");
		myPredefinedValues->appendItem("30 %");
		myPredefinedValues->appendItem("40 %");
		myPredefinedValues->appendItem("50 %");
		myPredefinedValues->appendItem("60 %");
		myPredefinedValues->appendItem("70 %");
		myPredefinedValues->appendItem("80 %");
		myPredefinedValues->appendItem("90 %");
		myPredefinedValues->appendItem("100 %");
        myPredefinedValues->setNumVisible(5);
    }
    {
        // free
        FXHorizontalFrame* gf12 =
            new FXHorizontalFrame(gp, LAYOUT_TOP | LAYOUT_LEFT, 0, 0, 0, 0, 10, 10, 5, 5);
        new FXRadioButton(gf12, "Free Entry: ", &myChosenTarget, FXDataTarget::ID_OPTION + 3,
                          ICON_BEFORE_TEXT | LAYOUT_SIDE_TOP | LAYOUT_CENTER_Y,
                          0, 0, 0, 0,   2, 2, 0, 0);
        myUserDefinedFriction =
            new FXRealSpinner(gf12, 10, this, MID_USER_DEF,
                              LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        //myUserDefinedFriction->setFormatString("%.0f km/h");
        //myUserDefinedFriction->setIncrements(1, 10, 10);
        myUserDefinedFriction->setIncrement(10);
        myUserDefinedFriction->setRange(10, 100);
        myUserDefinedFriction->setValue(
            static_cast<GUIFrictionCoefficientTrigger*>(myObject)->getDefaultFriction() * 100);
    }
    new FXButton(f1, "Close", nullptr, this, MID_CLOSE,
                 BUTTON_INITIAL | BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_X, 0, 0, 0, 0, 30, 30, 4, 4);
    static_cast<GUIFrictionCoefficientTrigger*>(myObject)->setOverriding(true);
}


GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::~GUIManip_FrictionCoefficientTrigger() {}


long
GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::onCmdClose(FXObject*, FXSelector, void*) {
    destroy();
    return 1;
}


long
GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::onCmdUserDef(FXObject*, FXSelector, void*) {
    myFriction = (double)(myUserDefinedFriction->getValue() / 100); // Friction in %
    static_cast<GUIFrictionCoefficientTrigger*>(myObject)->setOverridingValue(myFriction);
    myParent->updateChildren();
    return 1;
}


long
GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::onUpdUserDef(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   myChosenValue != 3 ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    myParent->updateChildren();
    return 1;
}


long
GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::onCmdPreDef(FXObject*, FXSelector, void*) {
    myFriction = (double)(double)((myPredefinedValues->getCurrentItem() * 10 + 10) / 100.0);
    static_cast<GUIFrictionCoefficientTrigger*>(myObject)->setOverridingValue(myFriction);
    myParent->updateChildren();
    return 1;
}


long
GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::onUpdPreDef(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   myChosenValue != 2 ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    myParent->updateChildren();
    return 1;
}


long
GUIFrictionCoefficientTrigger::GUIManip_FrictionCoefficientTrigger::onCmdChangeOption(FXObject*, FXSelector, void*) {
    static_cast<GUIFrictionCoefficientTrigger*>(myObject)->setOverriding(true);
    switch (myChosenValue) {
        case 0:
            myFriction = (double) static_cast<GUIFrictionCoefficientTrigger*>(myObject)->getDefaultFriction();
            break;
        case 1:
            myFriction = (double) static_cast<GUIFrictionCoefficientTrigger*>(myObject)->getLoadedFriction();
            break;
        case 2:
            myFriction = (double)((myPredefinedValues->getCurrentItem() * 10 + 10) / 100.0);
            break;
        case 3:
            myFriction = (double)(myUserDefinedFriction->getValue() / 100);
            break;
        default:
            // hmmm, should not happen
            break;
    }
    static_cast<GUIFrictionCoefficientTrigger*>(myObject)->setOverridingValue(myFriction);
    myParent->updateChildren();
    if (myChosenValue == 1) {
        // !!! lock in between
        static_cast<GUIFrictionCoefficientTrigger*>(myObject)->setOverriding(false);
    }
    return 1;
}



/* -------------------------------------------------------------------------
 * GUIFrictionCoefficientTrigger::GUIFrictionCoefficientTriggerPopupMenu - methods
 * ----------------------------------------------------------------------- */
GUIFrictionCoefficientTrigger::GUIFrictionCoefficientTriggerPopupMenu::GUIFrictionCoefficientTriggerPopupMenu(
    GUIMainWindow& app, GUISUMOAbstractView& parent,
    GUIGlObject& o)
    : GUIGLObjectPopupMenu(app, parent, o) {}


GUIFrictionCoefficientTrigger::GUIFrictionCoefficientTriggerPopupMenu::~GUIFrictionCoefficientTriggerPopupMenu() {}


long
GUIFrictionCoefficientTrigger::GUIFrictionCoefficientTriggerPopupMenu::onCmdOpenManip(FXObject*,
        FXSelector,
        void*) {
    static_cast<GUIFrictionCoefficientTrigger*>(myObject)->openManipulator(
        *myApplication, *myParent);
    return 1;
}

// -------------------------------------------------------------------------
// GUIFrictionCoefficientTrigger - methods
// -------------------------------------------------------------------------

GUIFrictionCoefficientTrigger::GUIFrictionCoefficientTrigger(
    const std::string& id, const std::vector<MSLane*>& destLanes,
    const std::string& aXMLFilename) :
    MSFrictionCoefficientTrigger(id, destLanes, aXMLFilename),
    GUIGlObject_AbstractAdd(GLO_COF, id),
    myShowAsPercent(true), myLastValue(-1) {
    myFGPositions.reserve(destLanes.size());
    myFGRotations.reserve(destLanes.size());
    std::vector<MSLane*>::const_iterator i;
    for (i = destLanes.begin(); i != destLanes.end(); ++i) {
        const PositionVector& v = (*i)->getShape();
        myFGPositions.push_back(v.positionAtOffset(0));
        myBoundary.add(v.positionAtOffset(0));
        myFGRotations.push_back(-v.rotationDegreeAtOffset(0));
    }
}


GUIFrictionCoefficientTrigger::~GUIFrictionCoefficientTrigger() {}


GUIGLObjectPopupMenu*
GUIFrictionCoefficientTrigger::getPopUpMenu(GUIMainWindow& app,
                                  GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIFrictionCoefficientTriggerPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildShowManipulatorPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildShowParamsPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow*
GUIFrictionCoefficientTrigger::getParameterWindow(GUIMainWindow& app,
                                        GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // add items
    ret->mkItem("Friction [%]", true,
                new FunctionBinding<GUIFrictionCoefficientTrigger, double>(this, &GUIFrictionCoefficientTrigger::getCurrentFriction));
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUIFrictionCoefficientTrigger::drawGL(const GUIVisualizationSettings& s) const {
    GLHelper::pushName(getGlID());
    GLHelper::pushMatrix();
    glTranslated(0, 0, getType());
    const double exaggeration = getExaggeration(s);
    for (int i = 0; i < (int)myFGPositions.size(); ++i) {
        const Position& pos = myFGPositions[i];
        double rot = myFGRotations[i];
        GLHelper::pushMatrix();
        glTranslated(pos.x(), pos.y(), 0);
        glRotated(rot, 0, 0, 1);
        glTranslated(0, -1.5, 0);
        glScaled(exaggeration, exaggeration, 1);
        int noPoints = 9;
        if (s.scale > 25) {
            noPoints = (int)(9.0 + s.scale / 10.0);
            if (noPoints > 36) {
                noPoints = 36;
            }
        }
        glColor3d(1, 0, 0);
        GLHelper::drawFilledCircle((double) 1.3, noPoints);
        if (s.scale >= 5) {
            glTranslated(0, 0, .1);
            glColor3d(0, 0, 0);
            GLHelper::drawFilledCircle((double) 1.1, noPoints);
            // draw the Friction string
            // not if scale to low
            // compute
            double value = (double) getCurrentFriction();
            if (myShowAsPercent) {
                value *= 100.0;
                if (((int) value + 1) % 10 == 0) {
                    value = (double)(((int) value + 1) / 10 * 10); //TODO: check necessity
                }
            }
            if (value != myLastValue) {
                myLastValue = value;
                myLastValueString = toString<double>(myLastValue);
                std::string::size_type idx = myLastValueString.find('.');
                if (idx != std::string::npos) {
                    if (idx > myLastValueString.length()) {
                        idx = myLastValueString.length();
                    }
                    myLastValueString = myLastValueString.substr(0, idx);
                }
            }
            //draw
            glColor3d(1, 0, 0);
            glTranslated(0, 0, .1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            // draw last value string
            GLHelper::drawText(myLastValueString.c_str(), Position(0, 0), .1, 1.2, RGBColor(255, 255, 0), 180);
        }
        GLHelper::popMatrix();
    }
    GLHelper::popMatrix();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    GLHelper::popName();
}


double
GUIFrictionCoefficientTrigger::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
}


Boundary
GUIFrictionCoefficientTrigger::getCenteringBoundary() const {
    Boundary b(myBoundary);
    b.grow(20);
    return b;
}


GUIManipulator*
GUIFrictionCoefficientTrigger::openManipulator(GUIMainWindow& app,
                                     GUISUMOAbstractView&) {
    GUIManip_FrictionCoefficientTrigger* gui =
        new GUIManip_FrictionCoefficientTrigger(app, getFullName(), *this, 0, 0);
    gui->create();
    gui->show();
    return gui;
}


/****************************************************************************/
