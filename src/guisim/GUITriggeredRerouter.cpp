/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUITriggeredRerouter.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 25.07.2005
/// @version $Id$
///
// Reroutes vehicles passing an edge (gui version)
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
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
#include "GUITriggeredRerouter.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <gui/GUIApplicationWindow.h>
#include <microsim/logging/FunctionBinding.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GLIncludes.h>


// ===========================================================================
// FOX callback mapping
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUITriggeredRerouter::GUITriggeredRerouterPopupMenu - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUITriggeredRerouter::GUITriggeredRerouterPopupMenu)
GUITriggeredRerouterPopupMenuMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_MANIP,         GUITriggeredRerouter::GUITriggeredRerouterPopupMenu::onCmdOpenManip),

};

// Object implementation
FXIMPLEMENT(GUITriggeredRerouter::GUITriggeredRerouterPopupMenu, GUIGLObjectPopupMenu, GUITriggeredRerouterPopupMenuMap, ARRAYNUMBER(GUITriggeredRerouterPopupMenuMap))


/* -------------------------------------------------------------------------
 * GUITriggeredRerouter::GUIManip_TriggeredRerouter - mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUITriggeredRerouter::GUIManip_TriggeredRerouter) GUIManip_TriggeredRerouterMap[] = {
    FXMAPFUNC(SEL_COMMAND,  GUITriggeredRerouter::GUIManip_TriggeredRerouter::MID_USER_DEF, GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdUserDef),
    FXMAPFUNC(SEL_UPDATE,   GUITriggeredRerouter::GUIManip_TriggeredRerouter::MID_USER_DEF, GUITriggeredRerouter::GUIManip_TriggeredRerouter::onUpdUserDef),
    FXMAPFUNC(SEL_COMMAND,  GUITriggeredRerouter::GUIManip_TriggeredRerouter::MID_OPTION,   GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdChangeOption),
    FXMAPFUNC(SEL_COMMAND,  GUITriggeredRerouter::GUIManip_TriggeredRerouter::MID_CLOSE,    GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdClose),
};

FXIMPLEMENT(GUITriggeredRerouter::GUIManip_TriggeredRerouter, GUIManipulator, GUIManip_TriggeredRerouterMap, ARRAYNUMBER(GUIManip_TriggeredRerouterMap))


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUITriggeredRerouter::GUIManip_TriggeredRerouter - methods
 * ----------------------------------------------------------------------- */
GUITriggeredRerouter::GUIManip_TriggeredRerouter::GUIManip_TriggeredRerouter(
    GUIMainWindow& app,
    const std::string& name, GUITriggeredRerouter& o,
    int /*xpos*/, int /*ypos*/)
    : GUIManipulator(app, name, 0, 0), myParent(&app),
      myChosenValue(0), myChosenTarget(myChosenValue, nullptr, MID_OPTION),
      myUsageProbability(o.getProbability()), myUsageProbabilityTarget(myUsageProbability),
      myObject(&o) {
    myChosenTarget.setTarget(this);
    FXVerticalFrame* f1 =
        new FXVerticalFrame(this, LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0, 0, 0, 0, 0);

    FXGroupBox* gp = new FXGroupBox(f1, "Change Probability",
                                    GROUPBOX_TITLE_LEFT | FRAME_SUNKEN | FRAME_RIDGE,
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
        // free
        FXHorizontalFrame* gf12 =
            new FXHorizontalFrame(gp, LAYOUT_TOP | LAYOUT_LEFT, 0, 0, 0, 0, 10, 10, 5, 5);
        new FXRadioButton(gf12, "User Given: ", &myChosenTarget, FXDataTarget::ID_OPTION + 1,
                          ICON_BEFORE_TEXT | LAYOUT_SIDE_TOP | LAYOUT_CENTER_Y,
                          0, 0, 0, 0,   2, 2, 0, 0);
        myUsageProbabilityDial =
            new FXRealSpinner(gf12, 10, this, MID_USER_DEF,
                              LAYOUT_TOP | FRAME_SUNKEN | FRAME_THICK);
        //myUsageProbabilityDial->setFormatString("%.2f");
        //myUsageProbabilityDial->setIncrements(.1, .1, .1);
        myUsageProbabilityDial->setIncrement(.1);
        myUsageProbabilityDial->setRange(0, 1);
        myUsageProbabilityDial->setValue(myObject->getUserProbability());
    }
    {
        // off
        FXHorizontalFrame* gf13 =
            new FXHorizontalFrame(gp, LAYOUT_TOP | LAYOUT_LEFT, 0, 0, 0, 0, 10, 10, 5, 5);
        new FXRadioButton(gf13, "Off", &myChosenTarget, FXDataTarget::ID_OPTION + 2,
                          ICON_BEFORE_TEXT | LAYOUT_SIDE_TOP,
                          0, 0, 0, 0,   2, 2, 0, 0);
    }
    myChosenValue = myObject->inUserMode()
                    ? myObject->getUserProbability() > 0
                    ? 1 : 2
                    : 0;
    new FXButton(f1, "Close", nullptr, this, MID_CLOSE,
                 BUTTON_INITIAL | BUTTON_DEFAULT | FRAME_RAISED | FRAME_THICK | LAYOUT_TOP | LAYOUT_LEFT | LAYOUT_CENTER_X, 0, 0, 0, 0, 30, 30, 4, 4);
}


GUITriggeredRerouter::GUIManip_TriggeredRerouter::~GUIManip_TriggeredRerouter() {}


long
GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdClose(FXObject*, FXSelector, void*) {
    destroy();
    return 1;
}


long
GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdUserDef(FXObject*, FXSelector, void*) {
    myUsageProbability = (double)(myUsageProbabilityDial->getValue());
    static_cast<GUITriggeredRerouter*>(myObject)->setUserUsageProbability(myUsageProbability);
    static_cast<GUITriggeredRerouter*>(myObject)->setUserMode(true);
    myParent->updateChildren();
    return 1;
}


long
GUITriggeredRerouter::GUIManip_TriggeredRerouter::onUpdUserDef(FXObject* sender, FXSelector, void* ptr) {
    sender->handle(this,
                   myChosenValue != 1 ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE),
                   ptr);
    myParent->updateChildren();
    return 1;
}


long
GUITriggeredRerouter::GUIManip_TriggeredRerouter::onCmdChangeOption(FXObject*, FXSelector, void*) {
    static_cast<GUITriggeredRerouter*>(myObject)->setUserUsageProbability(myUsageProbability);
    switch (myChosenValue) {
        case 0:
            static_cast<GUITriggeredRerouter*>(myObject)->setUserMode(false);
            break;
        case 1:
            static_cast<GUITriggeredRerouter*>(myObject)->setUserMode(true);
            break;
        case 2:
            static_cast<GUITriggeredRerouter*>(myObject)->setUserUsageProbability(0);
            static_cast<GUITriggeredRerouter*>(myObject)->setUserMode(true);
            break;
        default:
            throw 1;
    }
    myParent->updateChildren();
    return 1;
}


/* -------------------------------------------------------------------------
 * GUITriggeredRerouter::GUITriggeredRerouterPopupMenu - methods
 * ----------------------------------------------------------------------- */
GUITriggeredRerouter::GUITriggeredRerouterPopupMenu::GUITriggeredRerouterPopupMenu(
    GUIMainWindow& app, GUISUMOAbstractView& parent,
    GUIGlObject& o)
    : GUIGLObjectPopupMenu(app, parent, o) {}


GUITriggeredRerouter::GUITriggeredRerouterPopupMenu::~GUITriggeredRerouterPopupMenu() {}


long
GUITriggeredRerouter::GUITriggeredRerouterPopupMenu::onCmdOpenManip(FXObject*,
        FXSelector,
        void*) {
    static_cast<GUITriggeredRerouter*>(myObject)->openManipulator(
        *myApplication, *myParent);
    return 1;
}

// -------------------------------------------------------------------------
// GUITriggeredRerouter - methods
// -------------------------------------------------------------------------

GUITriggeredRerouter::GUITriggeredRerouter(const std::string& id, const MSEdgeVector& edges, double prob,
        const std::string& aXMLFilename, bool off, SUMOTime timeThreshold, const std::string& vTypes, SUMORTree& rtree) :
    MSTriggeredRerouter(id, edges, prob, aXMLFilename, off, timeThreshold, vTypes),
    GUIGlObject_AbstractAdd(GLO_REROUTER, id) {
    // add visualisation objects for edges which trigger the rerouter
    for (MSEdgeVector::const_iterator it = edges.begin(); it != edges.end(); ++it) {
        myEdgeVisualizations.push_back(new GUITriggeredRerouterEdge(dynamic_cast<GUIEdge*>(*it), this, false));
        rtree.addAdditionalGLObject(myEdgeVisualizations.back());
        myBoundary.add(myEdgeVisualizations.back()->getCenteringBoundary());
    }
}


GUITriggeredRerouter::~GUITriggeredRerouter() {
    for (std::vector<GUITriggeredRerouterEdge*>::iterator it = myEdgeVisualizations.begin(); it != myEdgeVisualizations.end(); ++it) {
        delete *it;
    }
    myEdgeVisualizations.clear();
}


void
GUITriggeredRerouter::myEndElement(int element) {
    MSTriggeredRerouter::myEndElement(element);
    if (element == SUMO_TAG_INTERVAL) {
        // add visualisation objects for closed edges
        const RerouteInterval& ri = myIntervals.back();
        for (MSEdgeVector::const_iterator it = ri.closed.begin(); it != ri.closed.end(); ++it) {
            myEdgeVisualizations.push_back(new GUITriggeredRerouterEdge(dynamic_cast<GUIEdge*>(*it), this, true));
            dynamic_cast<GUINet*>(GUINet::getInstance())->getVisualisationSpeedUp().addAdditionalGLObject(myEdgeVisualizations.back());
            myBoundary.add(myEdgeVisualizations.back()->getCenteringBoundary());
        }
    }
}


GUIGLObjectPopupMenu*
GUITriggeredRerouter::getPopUpMenu(GUIMainWindow& app,
                                   GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUITriggeredRerouterPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildShowManipulatorPopupEntry(ret, false);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow*
GUITriggeredRerouter::getParameterWindow(GUIMainWindow&,
        GUISUMOAbstractView&) {
    return nullptr;
}


void
GUITriggeredRerouter::drawGL(const GUIVisualizationSettings& s) const {
    UNUSED_PARAMETER(s);
}


Boundary
GUITriggeredRerouter::getCenteringBoundary() const {
    Boundary b(myBoundary);
    b.grow(20);
    return b;
}



GUIManipulator*
GUITriggeredRerouter::openManipulator(GUIMainWindow& app,
                                      GUISUMOAbstractView&) {
    GUIManip_TriggeredRerouter* gui =
        new GUIManip_TriggeredRerouter(app, getFullName(), *this, 0, 0);
    gui->create();
    gui->show();
    return gui;
}


/* -------------------------------------------------------------------------
 * GUITriggeredRerouterEdge - methods
 * ----------------------------------------------------------------------- */
GUITriggeredRerouter::GUITriggeredRerouterEdge::GUITriggeredRerouterEdge(GUIEdge* edge, GUITriggeredRerouter* parent, bool closed) :
    GUIGlObject(GLO_REROUTER_EDGE, parent->getID() + ":" + edge->getID()),
    myParent(parent),
    myEdge(edge),
    myAmClosedEdge(closed) {
    const std::vector<MSLane*>& lanes = edge->getLanes();
    myFGPositions.reserve(lanes.size());
    myFGRotations.reserve(lanes.size());
    for (std::vector<MSLane*>::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
        const PositionVector& v = (*i)->getShape();
        const double pos = closed ? 3 : v.length() - (double) 6.;
        myFGPositions.push_back((*i)->geometryPositionAtOffset(pos));
        myFGRotations.push_back(-v.rotationDegreeAtOffset(pos));
        myBoundary.add(myFGPositions.back());
    }
}


GUITriggeredRerouter::GUITriggeredRerouterEdge::~GUITriggeredRerouterEdge() {}


GUIGLObjectPopupMenu*
GUITriggeredRerouter::GUITriggeredRerouterEdge::getPopUpMenu(GUIMainWindow& app,
        GUISUMOAbstractView& parent) {
    return myParent->getPopUpMenu(app, parent);
}


GUIParameterTableWindow*
GUITriggeredRerouter::GUITriggeredRerouterEdge::getParameterWindow(GUIMainWindow&,
        GUISUMOAbstractView&) {
    return nullptr;
}


void
GUITriggeredRerouter::GUITriggeredRerouterEdge::drawGL(const GUIVisualizationSettings& s) const {
    const double exaggeration = s.addSize.getExaggeration(s, this);
    if (s.scale * exaggeration >= 3) {
        glPushName(getGlID());
        const double prob = myParent->getProbability();
        if (myAmClosedEdge) {
            // draw closing symbol onto all lanes
            const RerouteInterval* const ri =
                myParent->getCurrentReroute(MSNet::getInstance()->getCurrentTimeStep());
            if (ri != nullptr && prob > 0) {
                // draw only if the edge is closed at this time
                if (std::find(ri->closed.begin(), ri->closed.end(), myEdge) != ri->closed.end()) {
                    const int noLanes = (int)myFGPositions.size();
                    for (int j = 0; j < noLanes; ++j) {
                        Position pos = myFGPositions[j];
                        double rot = myFGRotations[j];
                        glPushMatrix();
                        glTranslated(pos.x(), pos.y(), 0);
                        glRotated(rot, 0, 0, 1);
                        glTranslated(0, -1.5, 0);
                        int noPoints = 9;
                        if (s.scale > 25) {
                            noPoints = (int)(9.0 + s.scale / 10.0);
                            if (noPoints > 36) {
                                noPoints = 36;
                            }
                        }
                        glTranslated(0, 0, getType());
                        //glScaled(exaggeration, exaggeration, 1);
                        glColor3d(0.7, 0, 0);
                        GLHelper::drawFilledCircle((double) 1.3, noPoints);
                        glTranslated(0, 0, .1);
                        glColor3d(1, 0, 0);
                        GLHelper::drawFilledCircle((double) 1.3, noPoints, 0, prob * 360);
                        glTranslated(0, 0, .1);
                        glColor3d(1, 1, 1);
                        glRotated(-90, 0, 0, 1);
                        glBegin(GL_TRIANGLES);
                        glVertex2d(0 - .3, -1.);
                        glVertex2d(0 - .3, 1.);
                        glVertex2d(0 + .3, 1.);
                        glVertex2d(0 + .3, -1.);
                        glVertex2d(0 - .3, -1.);
                        glVertex2d(0 + .3, 1.);
                        glEnd();
                        glPopMatrix();
                    }
                }
            }

        } else {
            // draw rerouter symbol onto all lanes
            for (int i = 0; i < (int)myFGPositions.size(); ++i) {
                const Position& pos = myFGPositions[i];
                double rot = myFGRotations[i];
                glPushMatrix();
                glTranslated(pos.x(), pos.y(), 0);
                glRotated(rot, 0, 0, 1);
                glTranslated(0, 0, getType());
                glScaled(exaggeration, exaggeration, 1);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

                glBegin(GL_TRIANGLES);
                glColor3d(1, .8f, 0);
                // base
                glVertex2d(0 - 1.4, 0);
                glVertex2d(0 - 1.4, 6);
                glVertex2d(0 + 1.4, 6);
                glVertex2d(0 + 1.4, 0);
                glVertex2d(0 - 1.4, 0);
                glVertex2d(0 + 1.4, 6);
                glEnd();

                // draw "U"
                GLHelper::drawText("U", Position(0, 2), .1, 3, RGBColor::BLACK, 180);

                // draw Probability
                GLHelper::drawText((toString((int)(prob * 100)) + "%").c_str(), Position(0, 4), .1, 0.7, RGBColor::BLACK, 180);

                glPopMatrix();
            }
        }
        glPopName();
    }
}


Boundary
GUITriggeredRerouter::GUITriggeredRerouterEdge::getCenteringBoundary() const {
    Boundary b(myBoundary);
    b.grow(20);
    return b;
}


/****************************************************************************/

