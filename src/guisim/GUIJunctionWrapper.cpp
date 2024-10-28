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
/// @file    GUIJunctionWrapper.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Andreas Gaubatz
/// @date    Mon, 1 Jul 2003
///
// }
/****************************************************************************/
#include <config.h>

#include <string>
#include <utility>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSJunction.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeomHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSInternalJunction.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include "GUIJunctionWrapper.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include <osgview/GUIOSGHeader.h>

// ===========================================================================
// method definitions
// ===========================================================================
GUIJunctionWrapper::GUIJunctionWrapper(MSJunction& junction, const std::string& tllID):
    GUIGlObject(GLO_JUNCTION, junction.getID(), GUIIconSubSys::getIcon(GUIIcon::JUNCTION)),
    myJunction(junction),
    myTesselation(junction.getID(), "", RGBColor::MAGENTA, junction.getShape(), false, true, 0),
    myExaggeration(1),
    myTLLID(tllID) {
    if (myJunction.getShape().size() == 0) {
        Position pos = myJunction.getPosition();
        myBoundary = Boundary(pos.x() - 1., pos.y() - 1., pos.x() + 1., pos.y() + 1.);
    } else {
        myBoundary = myJunction.getShape().getBoxBoundary();
    }
    myMaxSize = MAX2(myBoundary.getWidth(), myBoundary.getHeight());
    myIsInternal = myJunction.getType() == SumoXMLNodeType::INTERNAL;
    myAmWaterway = myJunction.getIncoming().size() + myJunction.getOutgoing().size() > 0;
    myAmRailway = myJunction.getIncoming().size() + myJunction.getOutgoing().size() > 0;
    myAmAirway = myJunction.getIncoming().size() + myJunction.getOutgoing().size() > 0;
    for (auto it = myJunction.getIncoming().begin(); it != myJunction.getIncoming().end() && (myAmWaterway || myAmRailway); ++it) {
        if (!(*it)->isInternal()) {
            if (!isWaterway((*it)->getPermissions())) {
                myAmWaterway = false;
            }
            if (!isRailway((*it)->getPermissions())) {
                myAmRailway = false;
            }
            if (!isAirway((*it)->getPermissions())) {
                myAmAirway = false;
            }
        }
    }
    for (auto it = myJunction.getOutgoing().begin(); it != myJunction.getOutgoing().end() && (myAmWaterway || myAmRailway); ++it) {
        if (!(*it)->isInternal()) {
            if (!isWaterway((*it)->getPermissions())) {
                myAmWaterway = false;
            }
            if (!isRailway((*it)->getPermissions())) {
                myAmRailway = false;
            }
            if (!isAirway((*it)->getPermissions())) {
                myAmAirway = false;
            }
        }
    }
    myTesselation.getShapeRef().closePolygon();
}


GUIJunctionWrapper::~GUIJunctionWrapper() {}


GUIGLObjectPopupMenu*
GUIJunctionWrapper::getPopUpMenu(GUIMainWindow& app,
                                 GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildShowParamsPopupEntry(ret);
    buildPositionCopyEntry(ret, app);
    return ret;
}


GUIParameterTableWindow*
GUIJunctionWrapper::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // add items
    ret->mkItem(TL("type"), false, toString(myJunction.getType()));
    ret->mkItem(TL("name"), false, myJunction.getName());
    // close building
    ret->closeBuilding(&myJunction);
    return ret;
}


double
GUIJunctionWrapper::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.junctionSize.getExaggeration(s, this, 4);
}


Boundary
GUIJunctionWrapper::getCenteringBoundary() const {
    Boundary b = myBoundary;
    b.grow(1);
    return b;
}

const std::string
GUIJunctionWrapper::getOptionalName() const {
    return myJunction.getName();
}

void
GUIJunctionWrapper::drawGL(const GUIVisualizationSettings& s) const {
    const bool s2 = s.secondaryShape;
    if (!myIsInternal && s.drawJunctionShape && !s2) {
        // check whether it is not too small
        const double exaggeration = getExaggeration(s);
        if (s.scale * exaggeration >= s.junctionSize.minSize) {
            GLHelper::pushMatrix();
            GLHelper::pushName(getGlID());
            const double colorValue = getColorValue(s, s.junctionColorer.getActive());
            const RGBColor color = s.junctionColorer.getScheme().getColor(colorValue);
            GLHelper::setColor(color);

            // recognize full transparency and simply don't draw
            if (color.alpha() != 0) {
                if ((exaggeration > 1 || myExaggeration > 1) && exaggeration != myExaggeration) {
                    myExaggeration = exaggeration;
                    myTesselation.setShape(myJunction.getShape());
                    myTesselation.getShapeRef().closePolygon();
                    myTesselation.getShapeRef().scaleRelative(exaggeration);
                    myTesselation.myTesselation.clear();
                }
                glTranslated(0, 0, getType());
                if (s.scale * myMaxSize < 40.) {
                    GLHelper::drawFilledPoly(myTesselation.getShape(), true);
                } else {
                    myTesselation.drawTesselation(myTesselation.getShape());
                }
                // make small junctions more visible when coloring by type
                if (myJunction.getType() == SumoXMLNodeType::RAIL_SIGNAL && s.junctionColorer.getActive() == 2) {
                    glTranslated(myJunction.getPosition(s2).x(), myJunction.getPosition(s2).y(), getType() + 0.05);
                    GLHelper::drawFilledCircle(2 * exaggeration, 12);
                }
            }
            GLHelper::popName();
            GLHelper::popMatrix();
            if (s.geometryIndices.show(this)) {
                GLHelper::debugVertices(myJunction.getShape(), s.geometryIndices, s.scale);
            }
        }
    }
    if (myIsInternal) {
        drawName(myJunction.getPosition(s2), s.scale, s.internalJunctionName, s.angle);
    } else {
        drawName(myJunction.getPosition(s2), s.scale, s.junctionID, s.angle);
        if (s.junctionName.show(this) && myJunction.getName() != "") {
            GLHelper::drawTextSettings(s.junctionName, myJunction.getName(), myJunction.getPosition(s2), s.scale, s.angle);
        }
        if ((s.tlsPhaseIndex.show(this) || s.tlsPhaseName.show(this)) && myTLLID != "") {
            const MSTrafficLightLogic* active = MSNet::getInstance()->getTLSControl().getActive(myTLLID);
            if (s.tlsPhaseIndex.show(this)) {
                const int index = active->getCurrentPhaseIndex();
                GLHelper::drawTextSettings(s.tlsPhaseIndex, toString(index), myJunction.getPosition(s2), s.scale, s.angle);
            }
            if (s.tlsPhaseName.show(this)) {
                const std::string& name = active->getCurrentPhaseDef().getName();
                if (name != "") {
                    const Position offset = (s.tlsPhaseIndex.show(this) ?
                                             Position(0, 0.8 * s.tlsPhaseIndex.scaledSize(s.scale)).rotateAround2D(DEG2RAD(-s.angle), Position(0, 0))
                                             : Position(0, 0));
                    GLHelper::drawTextSettings(s.tlsPhaseName, name, myJunction.getPosition(s2) - offset, s.scale, s.angle);
                }
            }
        }
    }
}


double
GUIJunctionWrapper::getColorValue(const GUIVisualizationSettings& /* s */, int activeScheme) const {
    switch (activeScheme) {
        case 0:
            if (myAmWaterway) {
                return 1;
            } else if (myAmRailway && MSNet::getInstance()->hasInternalLinks()) {
                return 2;
            } else if (myAmAirway) {
                return 3;
            } else {
                return 0;
            }
        case 1:
            return gSelected.isSelected(getType(), getGlID()) ? 1 : 0;
        case 2:
            switch (myJunction.getType()) {
                case SumoXMLNodeType::TRAFFIC_LIGHT:
                    return 0;
                case SumoXMLNodeType::TRAFFIC_LIGHT_NOJUNCTION:
                    return 1;
                case SumoXMLNodeType::PRIORITY:
                    return 2;
                case SumoXMLNodeType::PRIORITY_STOP:
                    return 3;
                case SumoXMLNodeType::RIGHT_BEFORE_LEFT:
                    return 4;
                case SumoXMLNodeType::ALLWAY_STOP:
                    return 5;
                case SumoXMLNodeType::DISTRICT:
                    return 6;
                case SumoXMLNodeType::NOJUNCTION:
                    return 7;
                case SumoXMLNodeType::DEAD_END:
                case SumoXMLNodeType::DEAD_END_DEPRECATED:
                    return 8;
                case SumoXMLNodeType::UNKNOWN:
                case SumoXMLNodeType::INTERNAL:
                    assert(false);
                    return 8;
                case SumoXMLNodeType::RAIL_SIGNAL:
                    return 9;
                case SumoXMLNodeType::ZIPPER:
                    return 10;
                case SumoXMLNodeType::TRAFFIC_LIGHT_RIGHT_ON_RED:
                    return 11;
                case SumoXMLNodeType::RAIL_CROSSING:
                    return 12;
                case SumoXMLNodeType::LEFT_BEFORE_RIGHT:
                    return 13;
                default:
                    assert(false);
                    return 0;
            }
        case 3:
            return myJunction.getPosition().z();
        default:
            assert(false);
            return 0;
    }
}

#ifdef HAVE_OSG
void
GUIJunctionWrapper::updateColor(const GUIVisualizationSettings& s) {
    const double colorValue = getColorValue(s, s.junctionColorer.getActive());
    const RGBColor& col = s.junctionColorer.getScheme().getColor(colorValue);
    osg::Vec4ubArray* colors = dynamic_cast<osg::Vec4ubArray*>(myGeom->getColorArray());
    (*colors)[0].set(col.red(), col.green(), col.blue(), col.alpha());
    myGeom->setColorArray(colors);
}
#endif


/****************************************************************************/
