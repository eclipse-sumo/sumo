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
/// @file    GUIOSGPerspectiveChanger.cpp
/// @author  Mirko Barthauer
/// @date    August 2022
///
// Implementation of GUIPerspectiveChanger for OSG 3D views
/****************************************************************************/
#include <config.h>

#include <fxkeys.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include "GUIOSGPerspectiveChanger.h"


// ===========================================================================
// method definitions
// ===========================================================================
GUIOSGPerspectiveChanger::GUIOSGPerspectiveChanger(
    GUIOSGView& callBack, const Boundary& viewPort) :
    GUIPerspectiveChanger(callBack, viewPort),
    myOrigWidth(viewPort.getWidth()),
    myOrigHeight(viewPort.getHeight()),
    myRotation(0) {
    myCameraManipulator = callBack.myCameraManipulator;
}


GUIOSGPerspectiveChanger::~GUIOSGPerspectiveChanger() {}


bool
GUIOSGPerspectiveChanger::onLeftBtnRelease(void* /* data */) {
    updateViewport();
    return false;
}


bool
GUIOSGPerspectiveChanger::onRightBtnRelease(void* /* data */) {
    updateViewport();
    return false;
}


bool
GUIOSGPerspectiveChanger::onMiddleBtnRelease(void* /* data */) {
    updateViewport();
    return false;
}


void GUIOSGPerspectiveChanger::onMouseMove(void* /* data */) {
    //updateViewport();
}


double
GUIOSGPerspectiveChanger::getRotation() const {
    return myRotation;
}


double
GUIOSGPerspectiveChanger::getXPos() const {
    osg::Vec3d lookFrom, lookAt, up;
    myCameraManipulator->getInverseMatrix().getLookAt(lookFrom, lookAt, up);
    return lookFrom.x();
}


double
GUIOSGPerspectiveChanger::getYPos() const {
    osg::Vec3d lookFrom, lookAt, up;
    myCameraManipulator->getInverseMatrix().getLookAt(lookFrom, lookAt, up);
    return lookFrom.y();
}


double
GUIOSGPerspectiveChanger::getZPos() const {
    osg::Vec3d lookFrom, lookAt, up;
    myCameraManipulator->getInverseMatrix().getLookAt(lookFrom, lookAt, up);
    return lookFrom.z();
}


double
GUIOSGPerspectiveChanger::getZoom() const {
    return 100.;
}


double
GUIOSGPerspectiveChanger::zPos2Zoom(double /* zPos */) const {
    return 100.;
}


double
GUIOSGPerspectiveChanger::zoom2ZPos(double /* zoom */) const {
    return getZPos();
}


void
GUIOSGPerspectiveChanger::setRotation(double rotation) {
    myRotation = rotation;
}


void
GUIOSGPerspectiveChanger::centerTo(const Position& pos, double radius, bool /* applyZoom */) {
    // maintain view direction if possible and scale so that the position and the
    // radius region around it are visible
    osg::Vec3d lookFrom, lookAt, up, dir, orthoDir;
    myCameraManipulator->getInverseMatrix().getLookAt(lookFrom, lookAt, up);
    dir = lookAt - lookFrom;
    // create helper vectors // check if parallel to z
    if ((dir ^ osg::Z_AXIS).length() > 0) {
        orthoDir[0] = -dir[1];
        orthoDir[1] = dir[0];
        up = osg::Z_AXIS;
    } else {
        orthoDir = -osg::X_AXIS;
        up = osg::Y_AXIS;
    }
    orthoDir.normalize();
    osg::Vec3d center(pos.x(), pos.y(), pos.z());
    osg::Vec3d leftBorder = center + orthoDir * radius;
    // construct new camera location which respects the fovy, resets the up vector
    double fovy, aspectRatio, zNear, zFar;
    dynamic_cast<GUIOSGView&>(myCallback).myViewer->getCamera()->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
    double halfFovy = DEG2RAD(.5 * fovy);
    osg::Vec3d outerFov = dir * cos(halfFovy) + orthoDir * sin(halfFovy);
    osg::Vec3d radiusVec = leftBorder - center;
    int sign = ((outerFov ^ radiusVec) * (outerFov ^ dir) > 0) ? 1 : -1;
    osg::Vec3d camUpdate = center + dir * sign * (outerFov ^ radiusVec).length() / (outerFov ^ dir).length();
    myCameraManipulator->setHomePosition(camUpdate, center, up);
    myRotation = 0.;
    dynamic_cast<GUIOSGView&>(myCallback).myViewer->home();
    updateViewport(camUpdate);
}


void
GUIOSGPerspectiveChanger::setViewport(double /* zoom */, double xPos, double yPos) {
    setViewportFrom(xPos, yPos, 0.);
}


void
GUIOSGPerspectiveChanger::setViewportFrom(double xPos, double yPos, double /* zPos */) {
    // Keep camera orientation if possible and point it to point to (x,y,0) if possible.
    // get current camera orientation
    osg::Vec3d lookFrom, lookAt, up, dir;
    myCameraManipulator->getInverseMatrix().getLookAt(lookFrom, lookAt, up);
    dir = lookAt - lookFrom;
    if ((dir.z() > 0. && lookFrom.z() >= 0.) || dir.z() == 0.) {         // create bird view
        lookFrom[0] = xPos;
        lookFrom[1] = yPos;
        lookAt = lookFrom - osg::Vec3d(0., 0., 1.);
    } else { // shift current view to reach (x,y,0)
        osg::Vec3d shift;
        // compute the point on the ground which is in line with the camera direction (solve for z=0)
        double factor = -lookFrom.z() / dir.z();
        osg::Vec3d groundTarget = lookFrom + dir * factor;
        shift[0] = xPos - groundTarget.x();
        shift[1] = yPos - groundTarget.y();
        lookFrom += shift;
        lookAt += shift;
    }
    osg::Matrix m;
    m.makeLookAt(lookFrom, lookAt, up);
    myCameraManipulator->setByInverseMatrix(m);
    updateViewport(lookFrom);
}


void
GUIOSGPerspectiveChanger::updateViewport() {
    osg::Vec3d lookFrom, lookAt, up, dir;
    myCameraManipulator->getInverseMatrix().getLookAt(lookFrom, lookAt, up);
    updateViewport(lookFrom);
}


void
GUIOSGPerspectiveChanger::updateViewport(osg::Vec3d& /* lookFrom */) {
    osg::Vec3d bottomLeft = getPositionOnGround(-1., -1.);
    osg::Vec3d bottomRight = getPositionOnGround(1., -1.);
    osg::Vec3d topLeft = getPositionOnGround(1., -1.);
    osg::Vec3d topRight = getPositionOnGround(1., 1.);
    double xMin, xMax, yMin, yMax;
    xMin = MIN4(bottomLeft.x(), bottomRight.x(), topLeft.x(), topRight.x());
    xMax = MAX4(bottomLeft.x(), bottomRight.x(), topLeft.x(), topRight.x());
    yMin = MIN4(bottomLeft.y(), bottomRight.y(), topLeft.y(), topRight.y());
    yMax = MAX4(bottomLeft.y(), bottomRight.y(), topLeft.y(), topRight.y());
    myViewPort.set(xMin, yMin, xMax, yMax);
}


osg::Vec3d
GUIOSGPerspectiveChanger::getPositionOnGround(double x, double y) {
    osg::Matrix VP = dynamic_cast<GUIOSGView&>(myCallback).myViewer->getCamera()->getViewMatrix() * dynamic_cast<GUIOSGView&>(myCallback).myViewer->getCamera()->getProjectionMatrix();
    osg::Matrix inverseVP;
    inverseVP.invert(VP);

    // compute world near far
    osg::Vec3d nearPoint(x, y, -1.);
    osg::Vec3d farPoint(x, y, 1.);
    osg::Vec3d nearPointWorld = nearPoint * inverseVP;
    osg::Vec3d farPointWorld = farPoint * inverseVP;

    // compute crossing with ground plane
    osg::Vec3d ray = farPointWorld - nearPointWorld;
    if (abs(ray.z()) > 0) {
        return nearPointWorld + ray * (-nearPointWorld.z() / ray.z());
    }
    return osg::Vec3d(0., 0., 0.);
}


void
GUIOSGPerspectiveChanger::changeCanvasSizeLeft(int /* change */) {
}


void
GUIOSGPerspectiveChanger::setViewport(const Boundary& viewPort) {
    setViewport(100., viewPort.getCenter().x(), viewPort.getCenter().y());
}
