/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GUIOSGBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Mirko Barthauer
/// @date    19.01.2012
///
// Builds OSG nodes from microsim objects
/****************************************************************************/
#include <config.h>

#ifdef HAVE_OSG

#include <guisim/GUIEdge.h>
#include <guisim/GUIJunctionWrapper.h>
#include <guisim/GUILane.h>
#include <guisim/GUINet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSJunction.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleType.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>

#include "GUIOSGView.h"
#include "GUIOSGBuilder.h"

//#define DEBUG_TESSEL

// ===========================================================================
// static member variables
// ===========================================================================

std::map<std::string, osg::ref_ptr<osg::Node> > GUIOSGBuilder::myCars;

// ===========================================================================
// member method definitions
// ===========================================================================

osg::Group*
GUIOSGBuilder::buildOSGScene(osg::Node* const tlg, osg::Node* const tly, osg::Node* const tlr, osg::Node* const tlu, osg::Node* const pole) {
    osgUtil::Tessellator tesselator;
    osg::Group* root = new osg::Group();
    GUINet* net = static_cast<GUINet*>(MSNet::getInstance());
    // build edges
    for (const MSEdge* e : net->getEdgeControl().getEdges()) {
        if (!e->isInternal()) {
            buildOSGEdgeGeometry(*e, *root, tesselator);
        }
    }
    // build junctions
    for (int index = 0; index < (int)net->myJunctionWrapper.size(); ++index) {
        buildOSGJunctionGeometry(*net->myJunctionWrapper[index], *root, tesselator);
    }
    // build traffic lights
    GUISUMOAbstractView::Decal d;
    const std::vector<std::string> tlids = net->getTLSControl().getAllTLIds();
    for (std::vector<std::string>::const_iterator i = tlids.begin(); i != tlids.end(); ++i) {
        MSTLLogicControl::TLSLogicVariants& vars = net->getTLSControl().get(*i);
        buildTrafficLightDetails(vars, tlg, tly, tlr, tlu, pole, *root);

        const MSTrafficLightLogic::LaneVectorVector& lanes = vars.getActive()->getLaneVectors();
        const MSLane* lastLane = 0;
        int idx = 0;
        for (MSTrafficLightLogic::LaneVectorVector::const_iterator j = lanes.begin(); j != lanes.end(); ++j, ++idx) {
            if ((*j).size() == 0) {
                continue;
            }
            const MSLane* const lane = (*j)[0];
            const Position pos = lane->getShape().back();
            const double angle = osg::DegreesToRadians(lane->getShape().rotationDegreeAtOffset(-1.) + 90.);
            d.centerZ = pos.z() + 4.;
            if (lane == lastLane) {
                d.centerX += 1.2 * sin(angle);
                d.centerY += 1.2 * cos(angle);
            } else {
                d.centerX = pos.x() - 1.5 * sin(angle);
                d.centerY = pos.y() - 1.5 * cos(angle);
            }
            osg::PositionAttitudeTransform* tlNode = getTrafficLight(d, vars, vars.getActive()->getLinksAt(idx)[0], nullptr, nullptr, nullptr, nullptr, nullptr, false, .25, -1, 1.);
            tlNode->setName("tlLogic:" + *i);
            root->addChild(tlNode);
            lastLane = lane;
        }
    }
    return root;
}


void
GUIOSGBuilder::buildLight(const GUISUMOAbstractView::Decal& d, osg::Group& addTo) {
    // each light must have a unique number
    osg::Light* light = new osg::Light(d.filename[5] - '0');
    // we set the light's position via a PositionAttitudeTransform object
    light->setPosition(osg::Vec4(0.0, 0.0, 0.0, 1.0));
    light->setDiffuse(osg::Vec4(1.0, 1.0, 1.0, 1.0));
    light->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0));
    light->setAmbient(osg::Vec4(1.0, 1.0, 1.0, 1.0));

    osg::LightSource* lightSource = new osg::LightSource();
    lightSource->setLight(light);
    lightSource->setLocalStateSetModes(osg::StateAttribute::ON);
    lightSource->setStateSetModes(*addTo.getOrCreateStateSet(), osg::StateAttribute::ON);

    osg::PositionAttitudeTransform* lightTransform = new osg::PositionAttitudeTransform();
    lightTransform->addChild(lightSource);
    lightTransform->setPosition(osg::Vec3d(d.centerX, d.centerY, d.centerZ));
    lightTransform->setScale(osg::Vec3d(0.1, 0.1, 0.1));
    addTo.addChild(lightTransform);
}


void
GUIOSGBuilder::buildOSGEdgeGeometry(const MSEdge& edge,
                                    osg::Group& addTo,
                                    osgUtil::Tessellator& tessellator) {
    const std::vector<MSLane*>& lanes = edge.getLanes();
    for (std::vector<MSLane*>::const_iterator j = lanes.begin(); j != lanes.end(); ++j) {
        MSLane* l = (*j);
        const bool extrude = edge.isWalkingArea() || isSidewalk(l->getPermissions());
        const int geomFactor = (edge.isWalkingArea()) ? 1 : 2;
        const PositionVector& shape = l->getShape();
        const int originalSize = (int)shape.size();
        osg::Geode* geode = new osg::Geode();
        osg::Geometry* geom = new osg::Geometry();
        geode->addDrawable(geom);
        geode->setName("lane:" + l->getID());
        addTo.addChild(geode);
        dynamic_cast<GUIGlObject*>(l)->setNode(geode);
        const int upperShapeSize = originalSize * geomFactor;
        const int totalShapeSize = (extrude) ? originalSize * 2 * geomFactor : originalSize * geomFactor;
        const float zOffset = (extrude) ? (edge.isCrossing()) ? 0.01f : 0.1f : 0.f;
        osg::Vec4ubArray* osg_colors = new osg::Vec4ubArray(1);
        (*osg_colors)[0].set(128, 128, 128, 255);
        geom->setColorArray(osg_colors, osg::Array::BIND_OVERALL);
        osg::Vec3Array* osg_coords = new osg::Vec3Array(totalShapeSize);
        geom->setVertexArray(osg_coords);
        int sizeDiff = 0;
        if (edge.isWalkingArea()) {
            int index = upperShapeSize - 1;
            for (int k = 0; k < upperShapeSize; ++k, --index) {
                (*osg_coords)[index].set((float)shape[k].x(), (float)shape[k].y(), (float)shape[k].z() + zOffset);
            }
            geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, upperShapeSize));
        } else {
            int index = 0;
            PositionVector rshape = shape;
            rshape.move2side(l->getWidth() / 2);
            for (int k = (int)rshape.size() - 1; k >= 0; --k, ++index) {
                (*osg_coords)[index].set((float)rshape[k].x(), (float)rshape[k].y(), (float)rshape[k].z() + zOffset);
            }
            PositionVector lshape = shape;
            lshape.move2side(-l->getWidth() / 2);
            for (int k = 0; k < (int)lshape.size(); ++k, ++index) {
                (*osg_coords)[index].set((float)lshape[k].x(), (float)lshape[k].y(), (float)lshape[k].z() + zOffset);
            }
            sizeDiff = (int)rshape.size() + (int)lshape.size() - upperShapeSize;
            int minSize = MIN2((int)rshape.size(), (int)lshape.size());
            osg::DrawElementsUInt* surface = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLE_STRIP, 0);
            for (int i = 0; i < minSize; ++i) {
                surface->push_back(i);
                surface->push_back(upperShapeSize + sizeDiff - 1 - i);
            }
            geom->addPrimitiveSet(surface);
        }
        if (extrude) {
            int index = upperShapeSize;
            for (int k = 0; k < upperShapeSize + sizeDiff; ++k, ++index) {
                (*osg_coords)[index].set((*osg_coords)[k].x(), (*osg_coords)[k].y(), 0.);
            }
            // extrude edge to create the kerb
            for (int i = 0; i < upperShapeSize + sizeDiff; ++i) {
                osg::Vec3 surfaceVec = (*osg_coords)[i] - (*osg_coords)[(i + 1) % (upperShapeSize + sizeDiff)];
                if (surfaceVec.length() > 0.) {
                    osg::DrawElementsUInt* kerb = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);
                    kerb->push_back(i);
                    kerb->push_back(upperShapeSize + i);
                    kerb->push_back(upperShapeSize + (i + 1) % (upperShapeSize + sizeDiff));
                    kerb->push_back((i + 1) % (upperShapeSize + sizeDiff));
                    geom->addPrimitiveSet(kerb);
                }
            }
        }

        osg::ref_ptr<osg::StateSet> ss = geode->getOrCreateStateSet();
        ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        ss->setMode(GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);

        if (shape.size() > 2) {
            tessellator.retessellatePolygons(*geom);

#ifdef DEBUG_TESSEL
            std::cout << "l=" << l->getID() << " origPoints=" << shape.size() << " geomSize=" << geom->getVertexArray()->getNumElements() << " points=";
            for (int i = 0; i < (int)geom->getVertexArray()->getNumElements(); i++) {
                const osg::Vec3& p = (*((osg::Vec3Array*)geom->getVertexArray()))[i];
                std::cout << p.x() << "," << p.y() << "," << p.z() << " ";
            }
            std::cout << "\n";
#endif
        }
        osgUtil::SmoothingVisitor sv;
#if OSG_MIN_VERSION_REQUIRED(3,5,4)
        sv.setCreaseAngle(0.6 * osg::PI);
#endif
        geom->accept(sv);
        static_cast<GUILane*>(l)->setGeometry(geom);
    }
}


void
GUIOSGBuilder::buildOSGJunctionGeometry(GUIJunctionWrapper& junction,
                                        osg::Group& addTo,
                                        osgUtil::Tessellator& tessellator) {
    const PositionVector& shape = junction.getJunction().getShape();
    osg::Geode* geode = new osg::Geode();
    osg::Geometry* geom = new osg::Geometry();
    geode->addDrawable(geom);
    geode->setName("junction:" + junction.getMicrosimID());
    addTo.addChild(geode);
    dynamic_cast<GUIGlObject&>(junction).setNode(geode);
    osg::Vec3Array* osg_coords = new osg::Vec3Array((int)shape.size()); // OSG needs float coordinates here
    geom->setVertexArray(osg_coords);
    for (int k = 0; k < (int)shape.size(); ++k) {
        (*osg_coords)[k].set((float)shape[k].x(), (float)shape[k].y(), (float)shape[k].z());
    }
    osg::Vec3Array* osg_normals = new osg::Vec3Array(1);
    (*osg_normals)[0] = osg::Vec3(0, 0, 1); // OSG needs float coordinates here
    geom->setNormalArray(osg_normals, osg::Array::BIND_PER_PRIMITIVE_SET);
    osg::Vec4ubArray* osg_colors = new osg::Vec4ubArray(1);
    (*osg_colors)[0].set(128, 128, 128, 255);
    geom->setColorArray(osg_colors, osg::Array::BIND_OVERALL);
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, (int)shape.size()));

    osg::ref_ptr<osg::StateSet> ss = geode->getOrCreateStateSet();
    ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    ss->setMode(GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);

    if (shape.size() > 4) {
        tessellator.retessellatePolygons(*geom);
    }
    junction.setGeometry(geom);
}


void
GUIOSGBuilder::buildTrafficLightDetails(MSTLLogicControl::TLSLogicVariants& vars, osg::Node* const tlg, osg::Node* const tly, osg::Node* const tlr, osg::Node* const tlu, osg::Node* poleBase, osg::Group& addTo) {
    // get the poleBase diameter for later repositioning
    osg::ComputeBoundsVisitor bboxCalc;
    poleBase->accept(bboxCalc);
    const double poleDiameter = bboxCalc.getBoundingBox().yMax() - bboxCalc.getBoundingBox().yMin();
    tlg->accept(bboxCalc);
    const double tlWidth = bboxCalc.getBoundingBox().yMax() - bboxCalc.getBoundingBox().yMin();

    // loop through lanes, collect edges, skip ped and bike infra for the time being
    MSTrafficLightLogic* tlLogic = vars.getActive();
    const MSTrafficLightLogic::LinkVectorVector& allLinks = tlLogic->getLinks();
    std::set<const MSEdge*> seenEdges;

    for (const MSTrafficLightLogic::LinkVector& lv : allLinks) {
        for (const MSLink* tlLink : lv) {
            // if not in seenEdges, create pole and reference it in the maps above
            const MSEdge* approach = &tlLink->getLaneBefore()->getEdge();
            if (!approach->isWalkingArea() && seenEdges.find(approach) != seenEdges.end()) {
                continue;
            }
            const std::vector<MSLane*> appLanes = approach->getLanes();
            // ref pos
            const double poleMinHeight = 5.;
            const double poleOffset = .5;
            double angle = 90. - appLanes[0]->getShape().rotationDegreeAtOffset(-1.);
            bool onlyPedCycle = isBikepath(approach->getPermissions()) || isSidewalk(approach->getPermissions());
            Position pos = appLanes[0]->getShape().back();
            double skipWidth = 0.;
            int firstSignalLaneIx = 0;
            std::vector<std::pair<osg::Group*, osg::Vec3d>> repeaters;
            // start with local coordinate system
            osg::PositionAttitudeTransform* appBase = new osg::PositionAttitudeTransform();
            osg::PositionAttitudeTransform* rightPoleBase = new osg::PositionAttitudeTransform();
            osg::PositionAttitudeTransform* rightPoleScaleNode = new osg::PositionAttitudeTransform();
            rightPoleScaleNode->addChild(poleBase);
            rightPoleBase->addChild(rightPoleScaleNode);
            appBase->addChild(rightPoleBase);
            rightPoleBase->setPosition(osg::Vec3d(pos.x(), pos.y(), pos.z()));
            rightPoleBase->setAttitude(osg::Quat(0., osg::Vec3d(1, 0, 0),
                                                 0., osg::Vec3d(0, 1, 0),
                                                 DEG2RAD(angle), osg::Vec3d(0, 0, 1)));
            if (onlyPedCycle) { // pedestrian / cyclist signal only
                rightPoleScaleNode->setScale(osg::Vec3d(.12 / poleDiameter, .12 / poleDiameter, 2.8));
                if (approach->isCrossing()) { // center VRU signal pole at crossings
                    // move pole to the other side of the road
                    osg::Vec3d offset(cos(DEG2RAD(angle)), sin(DEG2RAD(angle)), 0.);
                    appBase->setPosition(appBase->getPosition() + offset * (poleOffset + approach->getLength()));
                    appBase->setAttitude(osg::Quat(0., osg::Vec3d(1, 0, 0),
                                                   0., osg::Vec3d(0, 1, 0),
                                                   DEG2RAD(angle + 180), osg::Vec3d(0, 0, 1)));
                } else if (approach->isWalkingArea()) { // pole for other direction > get position from crossing
                    pos = tlLink->getLane()->getShape().back();
                    angle = 90. - tlLink->getLane()->getShape().rotationDegreeAtOffset(-1.);
                    rightPoleBase->setPosition(osg::Vec3d(pos.x(), pos.y(), pos.z()) - osg::Vec3d(poleOffset * cos(DEG2RAD(angle)), poleOffset * sin(DEG2RAD(angle)), 0.));
                    rightPoleBase->setAttitude(osg::Quat(0., osg::Vec3d(1, 0, 0),
                                                         0., osg::Vec3d(0, 1, 0),
                                                         DEG2RAD(angle), osg::Vec3d(0, 0, 1)));
                    if (tlLink->getLane()->getLinkCont()[0]->getTLIndex() < 0) { // check whether the other side is not specified explicitly
                        osg::PositionAttitudeTransform* leftPoleBase = new osg::PositionAttitudeTransform();
                        osg::PositionAttitudeTransform* leftPoleScaleNode = new osg::PositionAttitudeTransform();
                        appBase->addChild(leftPoleBase);
                        leftPoleScaleNode->addChild(poleBase);
                        leftPoleScaleNode->setScale(osg::Vec3d(.12 / poleDiameter, .12 / poleDiameter, 2.8));
                        leftPoleBase->addChild(leftPoleScaleNode);
                        double otherAngle = 90. - tlLink->getLane()->getShape().rotationDegreeAtOffset(1.);
                        Position otherPosRel = tlLink->getLane()->getShape().front();
                        osg::Vec3d leftPolePos(otherPosRel.x(), otherPosRel.y(), otherPosRel.z());
                        leftPoleBase->setPosition(leftPolePos + osg::Vec3d(poleOffset * cos(DEG2RAD(otherAngle)), poleOffset * sin(DEG2RAD(otherAngle)), 0.));
                        leftPoleBase->setAttitude(osg::Quat(0., osg::Vec3d(1., 0., 0.),
                                                            0., osg::Vec3d(0., 1., 0.),
                                                            DEG2RAD(angle + 180.), osg::Vec3d(0., 0., 1.)));
                        repeaters.push_back({ leftPoleBase, osg::Vec3d(0., 0., leftPoleBase->getPosition().z())});
                    }
                } else {
                    double laneWidth = appLanes[0]->getWidth();
                    osg::Vec3d offset(-poleOffset * cos(DEG2RAD(angle)) - (.5 * laneWidth - skipWidth + poleOffset) * sin(DEG2RAD(angle)), poleOffset * sin(DEG2RAD(angle)) + (.5 * laneWidth - skipWidth + poleOffset) * cos(DEG2RAD(angle)), 0.);
                    rightPoleBase->setPosition(rightPoleBase->getPosition() + offset);
                }
            } else {
                // skip sidewalk and bike lane if leftmost lane is for cars
                if (!noVehicles(appLanes.back()->getPermissions())) {
                    for (MSLane* appLane : appLanes) {
                        SVCPermissions permissions = appLane->getPermissions();
                        if (isSidewalk(permissions) || isForbidden(permissions)) {
                            skipWidth += appLane->getWidth();
                        } else {
                            break;
                        }
                        firstSignalLaneIx++;
                    }
                }
                const double laneWidth = appLanes[0]->getWidth();
                const double horizontalWidth = approach->getWidth() - skipWidth;
                const int laneCount = (int)appLanes.size() - firstSignalLaneIx;
                osg::Vec3d offset(-poleOffset * cos(DEG2RAD(angle)) - (.5 * laneWidth - skipWidth + poleOffset) * sin(DEG2RAD(angle)), -poleOffset * sin(DEG2RAD(angle)) + (.5 * laneWidth - skipWidth + poleOffset) * cos(DEG2RAD(angle)), 0.);
                rightPoleBase->setPosition(rightPoleBase->getPosition() + offset);

                if (laneCount < 3) { // cantilever
                    const double cantiWidth = horizontalWidth - .1 * appLanes.back()->getWidth() + poleOffset;
                    const double holderWidth = cantiWidth - .4 * appLanes.back()->getWidth();
                    const double holderAngle = 7.5; // degrees
                    const double extraHeight = sin(DEG2RAD(holderAngle)) * holderWidth;
                    rightPoleScaleNode->setScale(osg::Vec3d(.25 / poleDiameter, .25 / poleDiameter, poleMinHeight + extraHeight));
                    osg::PositionAttitudeTransform* cantileverBase = new osg::PositionAttitudeTransform();
                    cantileverBase->setPosition(osg::Vec3d(0., 0., poleMinHeight));
                    cantileverBase->setAttitude(osg::Quat(DEG2RAD(90.), osg::Vec3d(1, 0, 0),
                                                          0., osg::Vec3d(0, 1, 0),
                                                          0., osg::Vec3d(0, 0, 1)));
                    cantileverBase->setScale(osg::Vec3d(1., 1., cantiWidth));
                    cantileverBase->addChild(poleBase);
                    rightPoleBase->addChild(cantileverBase);
                    osg::PositionAttitudeTransform* cantileverHolderBase = new osg::PositionAttitudeTransform();
                    cantileverHolderBase->setPosition(osg::Vec3d(0., 0., poleMinHeight + extraHeight - .02));
                    cantileverHolderBase->setAttitude(osg::Quat(DEG2RAD(90. + holderAngle), osg::Vec3d(1, 0, 0),
                                                      0., osg::Vec3d(0, 1, 0),
                                                      0., osg::Vec3d(0, 0, 1)));
                    cantileverHolderBase->setScale(osg::Vec3d(.04 / poleDiameter, .04 / poleDiameter, sqrt(pow(holderWidth, 2.) + pow(extraHeight, 2.))));
                    cantileverHolderBase->addChild(poleBase);
                    rightPoleBase->addChild(cantileverHolderBase);
                } else { // signal bridge
                    rightPoleScaleNode->setScale(osg::Vec3d(.25 / poleDiameter, .25 / poleDiameter, poleMinHeight));
                    osg::PositionAttitudeTransform* leftPoleBase = new osg::PositionAttitudeTransform();
                    leftPoleBase->addChild(poleBase);
                    leftPoleBase->setScale(osg::Vec3d(.25 / poleDiameter, .25 / poleDiameter, poleMinHeight));
                    osg::Vec3d leftPolePos = osg::Vec3d(0, -(horizontalWidth + 2. * poleOffset), 0.);
                    leftPoleBase->setPosition(leftPolePos);
                    rightPoleBase->addChild(leftPoleBase);
                    osg::PositionAttitudeTransform* bridgeBase = new osg::PositionAttitudeTransform();
                    bridgeBase->setPosition(osg::Vec3d(0., 0., poleMinHeight - .125));
                    bridgeBase->setAttitude(osg::Quat(DEG2RAD(90.), osg::Vec3d(1, 0, 0),
                                                      0., osg::Vec3d(0, 1, 0),
                                                      0., osg::Vec3d(0, 0, 1)));
                    bridgeBase->setScale(osg::Vec3d(.25 / poleDiameter, .25 / poleDiameter, leftPolePos.length()));
                    bridgeBase->addChild(poleBase);
                    rightPoleBase->addChild(bridgeBase);
                }
            }
            seenEdges.insert(approach);

            // Add signals and position them along the cantilever/bridge
            double refPos = poleOffset /*- skipWidth*/;
            std::vector<MSLane*>::const_iterator it = appLanes.begin();
            for (std::advance(it, firstSignalLaneIx); it != appLanes.end(); it++) {
                // get tlLinkIndices
                const std::vector<MSLink*>& links = (*it)->getLinkCont();
                std::set<int> tlIndices;
                for (MSLink* link : links) {
                    if (link->getTLIndex() > -1) {
                        tlIndices.insert(link->getTLIndex());
                    }
                }
                std::set<int> seenTlIndices;
                bool placeRepeaters = true;
                for (MSLink* link : links) {
                    std::vector<std::pair<osg::Group*, osg::Vec3d>> signalTransforms = { {rightPoleBase, osg::Vec3d(0., 0., 0.)} };
                    if (placeRepeaters) {
                        signalTransforms.insert(signalTransforms.end(), repeaters.begin(), repeaters.end());
                        repeaters.clear();
                        placeRepeaters = false;
                    }
                    int tlIndex = link->getTLIndex();
                    if (tlIndex < 0 || seenTlIndices.find(tlIndex) != seenTlIndices.end()) {
                        continue;
                    }
                    for (const std::pair<osg::Group*, osg::Vec3d>& transform : signalTransforms) {
                        GUISUMOAbstractView::Decal d;
                        d.centerX = transform.second.x() + 0.15;
                        d.centerY = (onlyPedCycle) ? 0. : -(refPos + .5 * (*it)->getWidth() - ((double)tlIndices.size() / 2. - 1. + (double)seenTlIndices.size()) * 1.5 * tlWidth);
                        d.centerY += transform.second.y();
                        d.centerZ = (onlyPedCycle) ? 2.2 : 3.8;
                        d.centerZ += transform.second.z();
                        d.altitude = (onlyPedCycle) ? 0.6 : -1;
                        osg::PositionAttitudeTransform* tlNode = getTrafficLight(d, vars, links[0], tlg, tly, tlr, tlu, poleBase, false);
                        tlNode->setAttitude(osg::Quat(0., osg::Vec3d(1, 0, 0),
                                                      0., osg::Vec3d(0, 1, 0),
                                                      DEG2RAD(180.0), osg::Vec3d(0, 0, 1)));
                        transform.first->addChild(tlNode);
                    }
                    seenTlIndices.insert(tlIndex);
                }
                // only one signal for bike/pedestrian only edges
                if (onlyPedCycle) {
                    break;
                }
                refPos += (*it)->getWidth();
            }
            // interaction
            appBase->setNodeMask(1 << GUIOSGView::NODESET_TLSMODELS);
            appBase->setName("tlLogic:" + tlLogic->getID());
            addTo.addChild(appBase);
        }
    }
}


void
GUIOSGBuilder::buildDecal(const GUISUMOAbstractView::Decal& d, osg::Group& addTo) {
    osg::Node* pLoadedModel = osgDB::readNodeFile(d.filename);
    osg::PositionAttitudeTransform* base = new osg::PositionAttitudeTransform();
    double zOffset = 0.;
    if (pLoadedModel == nullptr) {
        // check for 2D image
        osg::Image* pImage = osgDB::readImageFile(d.filename);
        if (pImage == nullptr) {
            base = nullptr;
            WRITE_ERRORF(TL("Could not load '%'."), d.filename);
            return;
        }
        osg::Texture2D* texture = new osg::Texture2D();
        texture->setImage(pImage);
        osg::Geometry* quad = osg::createTexturedQuadGeometry(osg::Vec3d(-0.5 * d.width, -0.5 * d.height, 0.), osg::Vec3d(d.width, 0., 0.), osg::Vec3d(0., d.height, 0.));
        quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
        osg::Geode* const pModel = new osg::Geode();
        pModel->addDrawable(quad);
        base->addChild(pModel);
        zOffset = d.layer;
    } else {
        osg::ShadeModel* sm = new osg::ShadeModel();
        sm->setMode(osg::ShadeModel::FLAT);
        pLoadedModel->getOrCreateStateSet()->setAttribute(sm);
        base->addChild(pLoadedModel);
    }
    osg::ComputeBoundsVisitor bboxCalc;
    base->accept(bboxCalc);
    const osg::BoundingBox& bbox = bboxCalc.getBoundingBox();
    WRITE_MESSAGEF(TL("Loaded decal '%' with bounding box % %."), d.filename, toString(Position(bbox.xMin(), bbox.yMin(), bbox.zMin())), toString(Position(bbox.xMax(), bbox.yMax(), bbox.zMax())));
    double xScale = d.width > 0 ? d.width / (bbox.xMax() - bbox.xMin()) : 1.;
    double yScale = d.height > 0 ? d.height / (bbox.yMax() - bbox.yMin()) : 1.;
    const double zScale = d.altitude > 0 ? d.altitude / (bbox.zMax() - bbox.zMin()) : 1.;
    if (d.width < 0 && d.height < 0 && d.altitude > 0) {
        xScale = yScale = zScale;
    }
    base->setScale(osg::Vec3d(xScale, yScale, zScale));
    base->setPosition(osg::Vec3d(d.centerX, d.centerY, d.centerZ + zOffset));
    base->setAttitude(osg::Quat(osg::DegreesToRadians(d.roll), osg::Vec3d(1, 0, 0),
                                osg::DegreesToRadians(d.tilt), osg::Vec3d(0, 1, 0),
                                osg::DegreesToRadians(d.rot), osg::Vec3d(0, 0, 1)));
    addTo.addChild(base);
}


osg::PositionAttitudeTransform*
GUIOSGBuilder::getTrafficLight(const GUISUMOAbstractView::Decal& d, MSTLLogicControl::TLSLogicVariants& vars, const MSLink* link, osg::Node* const tlg, osg::Node* const tly, osg::Node* const tlr, osg::Node* const tlu, osg::Node* const pole, const bool withPole, const double size, double poleHeight, double transparency) {
    osg::PositionAttitudeTransform* ret = new osg::PositionAttitudeTransform();
    double xScale = 1., yScale = 1., zScale = 1.;
    if (tlg != nullptr) {
        osg::ComputeBoundsVisitor bboxCalc;
        tlg->accept(bboxCalc);
        const osg::BoundingBox& bbox = bboxCalc.getBoundingBox();
        xScale = d.width > 0 ? d.width / (bbox.xMax() - bbox.xMin()) : 1.;
        yScale = d.height > 0 ? d.height / (bbox.yMax() - bbox.yMin()) : 1.;
        double addHeight = (withPole) ? poleHeight : 0.;
        zScale = d.altitude > 0 ? d.altitude / (addHeight + bbox.zMax() - bbox.zMin()) : 1.;
    }
    if (d.width < 0 && d.height < 0 && d.altitude > 0) {
        xScale = yScale = zScale;
    }
    osg::PositionAttitudeTransform* base = new osg::PositionAttitudeTransform();
    osg::Switch* switchNode = new osg::Switch();
    switchNode->addChild(createTrafficLightState(d, tlg, withPole, size, osg::Vec4d(0., 1., 0., transparency)));
    switchNode->addChild(createTrafficLightState(d, tly, withPole, size, osg::Vec4d(1., 1., 0., transparency)));
    switchNode->addChild(createTrafficLightState(d, tlr, withPole, size, osg::Vec4d(1., 0., 0., transparency)));
    switchNode->addChild(createTrafficLightState(d, tlu, withPole, size, osg::Vec4d(1., .5, 0., transparency)));
    base->addChild(switchNode);
    vars.addSwitchCommand(new GUIOSGView::Command_TLSChange(link, switchNode));
    if (withPole) {
        base->setPosition(osg::Vec3d(0., 0., poleHeight));
        osg::PositionAttitudeTransform* poleBase = new osg::PositionAttitudeTransform();
        poleBase->addChild(pole);
        poleBase->setScale(osg::Vec3d(1., 1., poleHeight));
        ret->addChild(poleBase);
    }
    ret->setAttitude(osg::Quat(osg::DegreesToRadians(d.roll), osg::Vec3(1, 0, 0),
                               osg::DegreesToRadians(d.tilt), osg::Vec3(0, 1, 0),
                               osg::DegreesToRadians(d.rot), osg::Vec3(0, 0, 1)));
    ret->setPosition(osg::Vec3d(d.centerX, d.centerY, d.centerZ));
    ret->setScale(osg::Vec3d(xScale, yScale, zScale));
    ret->addChild(base);
    return ret;
}


osg::PositionAttitudeTransform*
GUIOSGBuilder::createTrafficLightState(const GUISUMOAbstractView::Decal& d, osg::Node* tl, const double withPole, const double size, osg::Vec4d color) {
    osg::PositionAttitudeTransform* ret = new osg::PositionAttitudeTransform();
    if (tl != nullptr) {
        ret->addChild(tl);
    }
    if (size > 0.) {
        unsigned int nodeMask = (withPole) ? 1 << GUIOSGView::NodeSetGroup::NODESET_TLSDOMES : 1 << GUIOSGView::NodeSetGroup::NODESET_TLSLINKMARKERS;
        osg::Geode* geode = new osg::Geode();
        osg::Vec3d center = osg::Vec3d(0., 0., (withPole) ? -1.8 : 0.);
        osg::ShapeDrawable* shape = new osg::ShapeDrawable(new osg::Sphere(center, (float)size));
        geode->addDrawable(shape);
        osg::ref_ptr<osg::StateSet> ss = shape->getOrCreateStateSet();
        ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        ss->setMode(GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
        shape->setColor(color);
        osg::PositionAttitudeTransform* ellipse = new osg::PositionAttitudeTransform();
        ellipse->addChild(geode);
        ellipse->setPosition(center);
        ellipse->setPivotPoint(center);
        if (withPole) {
            ellipse->setScale(osg::Vec3d(4., 4., 2.5 * d.altitude + 1.1));
        } else {
            ellipse->setScale(osg::Vec3d(4., 4., 1.1));
        }
        ellipse->setNodeMask(nodeMask);
        ret->addChild(ellipse);
    }
    return ret;
}


void
GUIOSGBuilder::setShapeState(osg::ref_ptr<osg::ShapeDrawable> shape) {
    osg::ref_ptr<osg::StateSet> ss = shape->getOrCreateStateSet();
    ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    ss->setMode(GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
}


GUIOSGView::OSGMovable
GUIOSGBuilder::buildMovable(const MSVehicleType& type) {
    GUIOSGView::OSGMovable m;
    m.pos = new osg::PositionAttitudeTransform();
    double enlarge = 0.05;
    const std::string& osgFile = type.getOSGFile();
    if (myCars.find(osgFile) == myCars.end()) {
        myCars[osgFile] = osgDB::readNodeFile(osgFile);
        if (myCars[osgFile] == 0) {
            WRITE_ERRORF(TL("Could not load '%'. The model is replaced by a cone shape."), osgFile);
            osg::PositionAttitudeTransform* rot = new osg::PositionAttitudeTransform();
            rot->addChild(new osg::ShapeDrawable(new osg::Cone(osg::Vec3d(0, 0, 0), 1.0f, 1.0f)));
            rot->setAttitude(osg::Quat(osg::DegreesToRadians(90.), osg::Vec3(1, 0, 0),
                                       0., osg::Vec3(0, 1, 0),
                                       0., osg::Vec3(0, 0, 1)));
            myCars[osgFile] = rot;
        }
    }
    osg::Node* carNode = myCars[osgFile];
    if (carNode != nullptr) {
        osg::ComputeBoundsVisitor bboxCalc;
        carNode->accept(bboxCalc);
        const osg::BoundingBox& bbox = bboxCalc.getBoundingBox();
        osg::PositionAttitudeTransform* base = new osg::PositionAttitudeTransform();
        base->addChild(carNode);
        base->setPivotPoint(osg::Vec3d((bbox.xMin() + bbox.xMax()) / 2., bbox.yMin(), bbox.zMin()));
        base->setScale(osg::Vec3d(type.getWidth() / (bbox.xMax() - bbox.xMin()),
                                  type.getLength() / (bbox.yMax() - bbox.yMin()),
                                  type.getHeight() / (bbox.zMax() - bbox.zMin())));
        m.pos->addChild(base);

        // material for coloring the person or vehicle body
        m.mat = new osg::Material();
        osg::ref_ptr<osg::StateSet> ss = base->getOrCreateStateSet();
        ss->setAttribute(m.mat, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);
        ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        ss->setMode(GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
    }
    if (type.getVehicleClass() != SVC_PEDESTRIAN) {
        m.lights = new osg::Switch();
        for (double sideFactor = -1.; sideFactor < 2.5; sideFactor += 2.) {
            osg::Geode* geode = new osg::Geode();
            osg::ShapeDrawable* right = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3d((type.getWidth() / 2. + enlarge)*sideFactor, 0., type.getHeight() / 2.), 0.2f));
            geode->addDrawable(right);
            //pat->addChild(geode);
            setShapeState(right);
            right->setColor(osg::Vec4(1.f, .5f, 0.f, .8f));
            osg::Sequence* seq = new osg::Sequence();
            // Wikipedia says about 1.5Hz
            seq->addChild(geode, .33);
            seq->addChild(new osg::Geode(), .33);
            // loop through all children
            seq->setInterval(osg::Sequence::LOOP, 0, -1);
            // real-time playback, repeat indefinitely
            seq->setDuration(1.0f, -1);
            // must be started explicitly
            seq->setMode(osg::Sequence::START);
            m.lights->addChild(seq);
        }
        osg::Geode* geode = new osg::Geode();
        osg::CompositeShape* comp = new osg::CompositeShape();
        comp->addChild(new osg::Sphere(osg::Vec3d(-(type.getWidth() / 2. + enlarge), type.getLength() + enlarge, type.getHeight() / 2.), .2f));
        comp->addChild(new osg::Sphere(osg::Vec3d(type.getWidth() / 2. + enlarge, type.getLength() + enlarge, type.getHeight() / 2.), .2f));
        osg::ShapeDrawable* brake = new osg::ShapeDrawable(comp);
        brake->setColor(osg::Vec4(1.f, 0.f, 0.f, .8f));
        geode->addDrawable(brake);
        setShapeState(brake);
        m.lights->addChild(geode);

        osg::Vec3d center(0, -type.getLength() / 2., 0.);
        osg::PositionAttitudeTransform* ellipse = new osg::PositionAttitudeTransform();
        ellipse->addChild(geode);
        ellipse->addChild(m.lights);
        ellipse->setPivotPoint(center);
        ellipse->setPosition(center);
        m.pos->addChild(ellipse);
    }
    m.active = true;
    return m;
}


osg::Node*
GUIOSGBuilder::buildPlane(const float length) {
    osg::Geode* geode = new osg::Geode();
    osg::Geometry* geom = new osg::Geometry;
    geode->addDrawable(geom);
    osg::Vec3Array* coords = new osg::Vec3Array(4); // OSG needs float coordinates here
    geom->setVertexArray(coords);
    (*coords)[0].set(.5f * length, .5f * length, -0.1f);
    (*coords)[1].set(.5f * length, -.5f * length, -0.1f);
    (*coords)[2].set(-.5f * length, -.5f * length, -0.1f);
    (*coords)[3].set(-.5f * length, .5f * length, -0.1f);
    osg::Vec3Array* normals = new osg::Vec3Array(1); // OSG needs float coordinates here
    (*normals)[0].set(0, 0, 1);
    geom->setNormalArray(normals, osg::Array::BIND_PER_PRIMITIVE_SET);
    osg::Vec4ubArray* colors = new osg::Vec4ubArray(1);
    (*colors)[0].set(0, 255, 0, 255);
    geom->setColorArray(colors, osg::Array::BIND_OVERALL);
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON, 0, 4));

    osg::ref_ptr<osg::StateSet> ss = geode->getOrCreateStateSet();
    ss->setRenderingHint(osg::StateSet::OPAQUE_BIN);
    ss->setMode(GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);

    return geode;
}


#endif

/****************************************************************************/
