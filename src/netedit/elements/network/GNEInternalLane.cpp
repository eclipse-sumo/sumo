/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEInternalLane.cpp
/// @author  Jakob Erdmann
/// @date    June 2011
///
// A class for visualizing Inner Lanes (used when editing traffic lights)
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/frames/network/GNETLSEditorFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>

#include "GNEInternalLane.h"
#include "GNEJunction.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXIMPLEMENT(GNEInternalLane, FXDelegator, 0, 0)

// ===========================================================================
// static member definitions
// ===========================================================================

StringBijection<FXuint>::Entry GNEInternalLane::linkStateNamesValues[] = {
    { "Green-Major",    LINKSTATE_TL_GREEN_MAJOR },
    { "Green-Minor",    LINKSTATE_TL_GREEN_MINOR },
    //{ "Yellow-Major", LINKSTATE_TL_YELLOW_MAJOR }, (should not be used)
    { "Yellow",         LINKSTATE_TL_YELLOW_MINOR },
    { "Red",            LINKSTATE_TL_RED },
    { "Red-Yellow",     LINKSTATE_TL_REDYELLOW },
    { "Stop",           LINKSTATE_STOP },
    { "Off",            LINKSTATE_TL_OFF_NOSIGNAL },
    { "Off-Blinking",   LINKSTATE_TL_OFF_BLINKING },
};

const StringBijection<FXuint> GNEInternalLane::LinkStateNames(
    GNEInternalLane::linkStateNamesValues, LINKSTATE_TL_OFF_BLINKING);

// ===========================================================================
// method definitions
// ===========================================================================

GNEInternalLane::GNEInternalLane(GNETLSEditorFrame* editor, GNEJunction* junctionParent,
                                 const std::string& id, const PositionVector& shape, int tlIndex, LinkState state) :
    GNENetworkElement(junctionParent->getNet(), id, GLO_TLLOGIC, GNE_TAG_INTERNAL_LANE,
                      GUIIconSubSys::getIcon(GUIIcon::LANE), {}, {}, {}, {}, {}, {}),
                                myJunctionParent(junctionParent),
                                myState(state),
                                myStateTarget(myState),
                                myEditor(editor),
                                myTlIndex(tlIndex),
myPopup(nullptr) {
    // calculate internal lane geometry
    myInternalLaneGeometry.updateGeometry(shape);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
    // vinculate this internal lane with their junction parent
    myJunctionParent->addInternalLane(this);
}


GNEInternalLane::GNEInternalLane() :
    GNENetworkElement(nullptr, "dummyInternalLane", GLO_TLLOGIC, GNE_TAG_INTERNAL_LANE,
                      GUIIconSubSys::getIcon(GUIIcon::LANE), {}, {}, {}, {}, {}, {}),
myJunctionParent(nullptr),
myState(0),
myEditor(0),
myTlIndex(0),
myPopup(nullptr) {
}


GNEInternalLane::~GNEInternalLane() {
    // remove this internal lane from junction parent
    myJunctionParent->removeInternalLane(this);
}


void
GNEInternalLane::updateGeometry() {
    // nothing to update
}


Position
GNEInternalLane::getPositionInView() const {
    return myJunctionParent->getPositionInView();
}


bool
GNEInternalLane::checkDrawFromContour() const {
    return false;
}


bool
GNEInternalLane::checkDrawToContour() const {
    return false;
}


bool
GNEInternalLane::checkDrawRelatedContour() const {
    return false;
}


bool
GNEInternalLane::checkDrawOverContour() const {
    return myNet->getViewNet()->getViewObjectsSelector().getGUIGlObjectFront() == this;
}


bool
GNEInternalLane::checkDrawDeleteContour() const {
    return false;
}


bool
GNEInternalLane::checkDrawSelectContour() const {
    return false;
}


bool
GNEInternalLane::checkDrawMoveContour() const {
    return false;
}


GNEMoveOperation*
GNEInternalLane::getMoveOperation() {
    // internal lanes cannot be moved
    return nullptr;
}


void
GNEInternalLane::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undolist*/) {
    // geometry points of internal lanes cannot be removed
}


long
GNEInternalLane::onDefault(FXObject* obj, FXSelector sel, void* data) {
    if (myEditor != nullptr) {
        FXuint before = myState;
        myStateTarget.handle(obj, sel, data);
        if (myState != before) {
            myEditor->handleChange(this);
        }
        // let GUISUMOAbstractView know about clicks so that the popup is properly destroyed
        if (FXSELTYPE(sel) == SEL_COMMAND) {
            if (myPopup != nullptr) {
                myPopup->getParentView()->destroyPopup();
                myPopup = nullptr;
            }
        }
    }
    return 1;
}


void
GNEInternalLane::drawGL(const GUIVisualizationSettings& s) const {
    // only draw if we're not selecting E1 detectors in TLS Mode
    if (!myNet->getViewNet()->selectingDetectorsTLSMode()) {
        // get detail level
        const auto d = s.getDetailLevel(1);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (!s.drawForViewObjectsHandler) {
            // get link state color
            const auto linkStateColor = colorForLinksState(myState);
            // push layer matrix
            GLHelper::pushMatrix();
            // translate to front
            myEditor->getViewNet()->drawTranslateFrontAttributeCarrier(myJunctionParent, GLO_TLLOGIC);
            // move front again
            glTranslated(0, 0, 0.5);
            // set color
            GLHelper::setColor(linkStateColor);
            // draw geometry
            GUIGeometry::drawGeometry(d, myInternalLaneGeometry,
                                      s.connectionSettings.connectionWidth);
            // pop layer matrix
            GLHelper::popMatrix();
            // draw edge name
            if (s.internalEdgeName.show(this)) {
                GLHelper::drawTextSettings(s.internalEdgeName, getMicrosimID(), myInternalLaneGeometry.getShape().getLineCenter(),
                                           s.scale, s.angle);
            }
            // draw dotted contour
            myNetworkElementContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        }
        // calculate contour
        myNetworkElementContour.calculateContourExtrudedShape(s, d, this, myInternalLaneGeometry.getShape(), getType(), 
                s.connectionSettings.connectionWidth, 1, true, true, 0);
    }
}


void
GNEInternalLane::deleteGLObject() {
    // Internal lanes cannot be removed
}


void
GNEInternalLane::updateGLObject() {
    updateGeometry();
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
    if ((myEditor != nullptr) && (myEditor->getViewNet()->getEditModes().isCurrentSupermodeNetwork())) {
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
    // internal lanes don't have attributes
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GNEInternalLane::getCenteringBoundary() const {
    return myNetworkElementContour.getContourBoundary();
}


void
GNEInternalLane::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to update
}


RGBColor
GNEInternalLane::colorForLinksState(FXuint state) {
    try {
        return GUIVisualizationSettings::getLinkColor((LinkState)state);
    } catch (ProcessError&) {
        WRITE_WARNINGF(TL("invalid link state='%'"), toString(state));
        return RGBColor::BLACK;
    }
}


std::string
GNEInternalLane::getAttribute(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
}


PositionVector
GNEInternalLane::getAttributePositionVector(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
}


void
GNEInternalLane::setAttribute(SumoXMLAttr key, const std::string& /*value*/, GNEUndoList* /*undoList*/) {
    throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
}


bool
GNEInternalLane::isValid(SumoXMLAttr key, const std::string& /*value*/) {
    throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
}


bool
GNEInternalLane::isAttributeEnabled(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
}


const Parameterised::Map&
GNEInternalLane::getACParametersMap() const {
    throw InvalidArgument(getTagStr() + " doesn't have parameters");
}


void
GNEInternalLane::setAttribute(SumoXMLAttr key, const std::string& /*value*/) {
    throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
}


void
GNEInternalLane::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // internal lanes cannot be moved
}


void
GNEInternalLane::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // internal lanes cannot be moved
}

/****************************************************************************/
