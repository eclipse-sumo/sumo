/****************************************************************************/
/// @file    GNEInternalLane.cpp
/// @author  Jakob Erdmann
/// @date    June 2011
/// @version $Id$
///
// A class for visualizing Inner Lanes (used when editing traffic lights)
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

#include <string>
#include <iostream>
#include <utility>
#include <time.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/common/RandHelper.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/gui/images/GUITexturesHelper.h>

#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNEViewNet.h"
#include "GNEInternalLane.h"
#include "GNETLSEditorFrame.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
/// @note: msvc10 does not approve of allocating empty arrays
/*
FXDEFMAP(GNEInternalLane) GNEInternalLaneMap[]= {
    //FXMAPFUNC(SEL_COMMAND,  MID_GNE_PHASE_DURATION,     GNETLSEditorFrame::onDefault),
};
*/

// Object implementation
//FXIMPLEMENT(GNEInternalLane, FXDelegator, GNEInternalLaneMap, ARRAYNUMBER(GNEInternalLaneMap))
FXIMPLEMENT(GNEInternalLane, FXDelegator, 0, 0)

// ===========================================================================
// static member definitions
// ===========================================================================

StringBijection<FXuint>::Entry GNEInternalLane::linkStateNamesValues[] = {
    { "Green-Major", LINKSTATE_TL_GREEN_MAJOR },
    { "Green-Minor", LINKSTATE_TL_GREEN_MINOR },
    { "Yellow-Major", LINKSTATE_TL_YELLOW_MAJOR },
    { "Yellow-Minor", LINKSTATE_TL_YELLOW_MINOR },
    { "Red", LINKSTATE_TL_RED },
    { "Red-Yellow", LINKSTATE_TL_REDYELLOW },
    { "Stop", LINKSTATE_STOP },
    { "Off", LINKSTATE_TL_OFF_NOSIGNAL },
    { "Off-Blinking", LINKSTATE_TL_OFF_BLINKING },
};

const StringBijection<FXuint> GNEInternalLane::LinkStateNames(
    GNEInternalLane::linkStateNamesValues, LINKSTATE_TL_OFF_BLINKING);

// ===========================================================================
// method definitions
// ===========================================================================
GNEInternalLane::GNEInternalLane(GNETLSEditorFrame* editor, const std::string& id, const PositionVector& shape, int tlIndex, LinkState state) :
    GUIGlObject(editor == 0 ? GLO_JUNCTION : GLO_TLLOGIC, id),
    myShape(shape),
    myState(state),
    myStateTarget(myState),
    myEditor(editor),
    myTlIndex(tlIndex),
    myPopup(0) {
    int segments = (int) myShape.size() - 1;
    if (segments >= 0) {
        myShapeRotations.reserve(segments);
        myShapeLengths.reserve(segments);
        for (int i = 0; i < segments; ++i) {
            const Position& f = myShape[i];
            const Position& s = myShape[i + 1];
            myShapeLengths.push_back(f.distanceTo2D(s));
            myShapeRotations.push_back((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI);
        }
    }
}


GNEInternalLane::GNEInternalLane() :
    GUIGlObject(GLO_TLLOGIC, "dummyInternalLane") {
    assert(false);
}


GNEInternalLane::~GNEInternalLane() {}


long
GNEInternalLane::onDefault(FXObject* obj, FXSelector sel, void* data) {
    if (myEditor != 0) {
        FXuint before = myState;
        myStateTarget.handle(obj, sel, data);
        if (myState != before) {
            myEditor->handleChange(this);
        }
        // let GUISUMOAbstractView know about clicks so that the popup is properly destroyed
        if (FXSELTYPE(sel) == SEL_COMMAND) {
            if (myPopup != 0) {
                myPopup->getParentView()->destroyPopup();
                myPopup = 0;
            }
        }
    }
    return 1;
}


void
GNEInternalLane::drawGL(const GUIVisualizationSettings& s) const {
    glPushMatrix();
    glPushName(getGlID());
    glTranslated(0, 0, GLO_JUNCTION + 0.1); // must draw on top of junction
    GLHelper::setColor(colorForLinksState(myState));
    // draw lane
    // check whether it is not too small
    if (s.scale < 1.) {
        GLHelper::drawLine(myShape);
    } else {
        GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, 0.2);
    }
    glPopName();
    glPopMatrix();
}


void
GNEInternalLane::setLinkState(LinkState state) {
    myState = state;
    myOrigState = state;
}


LinkState
GNEInternalLane::getLinkState() const {
    return (LinkState)myState;
}


int
GNEInternalLane::getTLIndex() const {
    return myTlIndex;
}


GUIGLObjectPopupMenu*
GNEInternalLane::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    myPopup = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(myPopup, app);
    if (myEditor != 0) {
        const std::vector<std::string> names = LinkStateNames.getStrings();
        for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); it++) {
            FXuint state = LinkStateNames.get(*it);
            std::string origHint = ((LinkState)state == myOrigState ? " (original)" : "");
            FXMenuRadio* mc = new FXMenuRadio(myPopup, (*it + origHint).c_str(), this, FXDataTarget::ID_OPTION + state);
            mc->setSelBackColor(MFXUtils::getFXColor(colorForLinksState(state)));
            mc->setBackColor(MFXUtils::getFXColor(colorForLinksState(state)));
        }
    }
    return myPopup;
}


GUIParameterTableWindow*
GNEInternalLane::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 2);
    // add items
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GNEInternalLane::getCenteringBoundary() const {
    Boundary b = myShape.getBoxBoundary();
    b.grow(10);
    return b;
}


RGBColor
GNEInternalLane::colorForLinksState(FXuint state) {
    if (state == LINKSTATE_TL_YELLOW_MINOR) {
        // special case (default gui does not distinguish between yellow major/minor
        return RGBColor(179, 179, 0, 255);
    } else {
        return GUIVisualizationSettings::getLinkColor((LinkState)state);
    }
}

/****************************************************************************/
