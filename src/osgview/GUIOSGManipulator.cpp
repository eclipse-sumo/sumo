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
/// @file    GUIOSGManipulator.cpp
/// @author  Mirko Barthauer
/// @date    27.11.2022
///
// A custom camera manipulator to interact with the OSG view directly
/****************************************************************************/
#include <config.h>

#include <utils/geom/GeomHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringBijection.h>

#include "GUIOSGManipulator.h"
#include "GUIOSGView.h"

#ifdef HAVE_OSG

#define DEFAULT_MOVESPEED_MIN 1.0
//#define DEFAULT_MOVESPEED_MAX 30.0
//#define DEFAULT_MOVEACCEL 2.0

static StringBijection<ManipulatorMode>::Entry ModeTextInitializer[] = {
    {"Ego",        MODE_EGO},
    {"Walk",       MODE_WALK},
    {"Terrain",    MODE_TERRAIN}
};

StringBijection<ManipulatorMode> ModeText(ModeTextInitializer, MODE_TERRAIN, false);


// ===========================================================================
// method definitions
// ===========================================================================
GUIOSGManipulator::GUIOSGManipulator(GUIOSGView* parent, ManipulatorMode initMode, bool verticalFixed, double /* eyeHeight */) :
    myParent(parent), myCurrentMode(initMode), /*myWalkEyeHeight(eyeHeight),*/ myMoveSpeed(DEFAULT_MOVESPEED_MIN) {
    setAllowThrow(false);
    setVerticalAxisFixed(verticalFixed);
}


GUIOSGManipulator::~GUIOSGManipulator() {
}


bool
GUIOSGManipulator::performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy) {
    if (myCurrentMode == MODE_TERRAIN) {
        return osgGA::TerrainManipulator::performMovementMiddleMouseButton(eventTimeDelta, dx, dy);
    }
    return false;
}


bool
GUIOSGManipulator::performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy) {
    if (myCurrentMode == MODE_TERRAIN) {
        return osgGA::TerrainManipulator::performMovementLeftMouseButton(eventTimeDelta, dx, dy);
    }
    return false;
}


bool
GUIOSGManipulator::performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy) {
    if (myCurrentMode == MODE_TERRAIN) {
        return osgGA::TerrainManipulator::performMovementRightMouseButton(eventTimeDelta, dx, -dy);
    }
    return false;
}


bool
GUIOSGManipulator::handleMouseMove(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    if (myCurrentMode == MODE_EGO || myCurrentMode == MODE_WALK) {
        return handleMouseDeltaMovement(ea, aa);
    }
    return false;
}


bool
GUIOSGManipulator::handleMouseDrag(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    if (myCurrentMode == MODE_TERRAIN) {
        return osgGA::TerrainManipulator::handleMouseDrag(ea, aa);
    }
    return handleMouseDeltaMovement(ea, aa);
}


bool
GUIOSGManipulator::handleMousePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    if (myCurrentMode == MODE_TERRAIN) {
        return osgGA::TerrainManipulator::handleMousePush(ea, aa);
    }
    return handleMouseDeltaMovement(ea, aa);
}


bool
GUIOSGManipulator::handleMouseRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    if (myCurrentMode == MODE_TERRAIN) {
        return osgGA::TerrainManipulator::handleMouseRelease(ea, aa);
    }
    return false;
}


bool
GUIOSGManipulator::handleMouseDeltaMovement(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    addMouseEvent(ea);
    if (_ga_t0.get() == NULL || _ga_t1.get() == NULL) {
        return false;
    }
    float dt = static_cast<float>(_ga_t0->getTime() - _ga_t1->getTime());
    if (dt > 0.1) { // wait until the mouse movement is sufficiently smooth
        return false;
    }
    float intensity = 50.;
    float dx = _ga_t0->getXnormalized() * intensity * dt;
    float dy = _ga_t0->getYnormalized() * intensity * dt;
    if (dx == 0. && dy == 0.) {
        return false;
    }
    centerMousePointer(ea, aa);
    // calculate delta angles from dx and dy movements
    if (performMouseDeltaMovement(dx, dy)) {
        aa.requestRedraw();
    }
    return true;
}


bool
GUIOSGManipulator::performMouseDeltaMovement(const float dx, const float dy) {
    rotateYawPitch(_rotation, dx, dy, osg::Z_AXIS);
    return true;
}


void GUIOSGManipulator::centerMousePointer(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    _mouseCenterX = (ea.getXmin() + ea.getXmax()) / 2.0f;
    _mouseCenterY = (ea.getYmin() + ea.getYmax()) / 2.0f;
    aa.requestWarpPointer(_mouseCenterX, _mouseCenterY);
}


void
GUIOSGManipulator::rotateYawPitch(osg::Quat& rotation, const double yaw, const double pitch, const osg::Vec3d& localUp) {
    bool verticalAxisFixed = (localUp != osg::Vec3d(0., 0., 0.));

    // fix current rotation
    if (verticalAxisFixed) {
        fixVerticalAxis(rotation, localUp, true);
    }

    // rotations
    osg::Quat rotateYaw(-yaw, verticalAxisFixed ? localUp : rotation * osg::Vec3d(0., 1., 0.));
    osg::Quat rotatePitch;
    osg::Quat newRotation;
    osg::Vec3d cameraRight(rotation * osg::Vec3d(1., 0., 0.));

    double my_dy = pitch;
    int i = 0;
    osg::Vec3f eye = _center - _rotation * osg::Vec3d(0., 0., -_distance);
    do {
        // rotations
        rotatePitch.makeRotate(my_dy, cameraRight);
        newRotation = rotation * rotateYaw * rotatePitch;

        // update vertical axis
        if (verticalAxisFixed) {
            fixVerticalAxis(newRotation, localUp, false);
        }

        // check for viewer's up vector to be more than 90 degrees from "up" axis
        osg::Vec3d newCameraUp = newRotation * osg::Vec3d(0., 1., 0.);
        if (newCameraUp * localUp > 0.) {
            // apply new rotation
            setByMatrix(osg::Matrixd::rotate(newRotation) * osg::Matrixd::translate(eye));
            return;
        }
        my_dy /= 2.;
        if (++i == 20) {
            setByMatrix(osg::Matrixd::rotate(rotation) * osg::Matrixd::rotate(rotateYaw) * osg::Matrixd::translate(eye));
            return;
        }
    } while (true);
}


bool
GUIOSGManipulator::handleKeyDown(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /* aa */) {
    bool result = false;
    switch (ea.getKey()) {
        case osgGA::GUIEventAdapter::KEY_Up:
            myMove.z() -= myMoveSpeed;
            result = true;
            break;
        case osgGA::GUIEventAdapter::KEY_Down:
            myMove.z() += myMoveSpeed;
            result = true;
            break;
        case osgGA::GUIEventAdapter::KEY_Right:
            myMove.x() += myMoveSpeed;
            result = true;
            break;
        case osgGA::GUIEventAdapter::KEY_Left:
            myMove.x() -= myMoveSpeed;
            result = true;
            break;
    }
    _center += getMatrix().getRotate() * myMove;
    return result;
}


bool
GUIOSGManipulator::handleKeyUp(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /* aa */) {
    bool result = false;
    switch (ea.getKey()) {
        case osgGA::GUIEventAdapter::KEY_Up:
        case osgGA::GUIEventAdapter::KEY_Down:
        case osgGA::GUIEventAdapter::KEY_Right:
        case osgGA::GUIEventAdapter::KEY_Left:
            myMove.set(0, 0, 0);
            return true;
        case osgGA::GUIEventAdapter::KEY_F:
            if (myCurrentMode == MODE_EGO) {
                myCurrentMode = MODE_TERRAIN; // MODE_WALK disabled until it's fully implemented
            } else if (myCurrentMode == MODE_WALK) {
                myCurrentMode = MODE_TERRAIN;
            } else {
                myCurrentMode = MODE_EGO;
            }
            updateHUDText();
            return true;
    }
    return result;
}


void
GUIOSGManipulator::updateHUDText() {
    myParent->updateHUDText(TLF("Currently in % camera mode. Press [F] to switch.", ModeText.getString(myCurrentMode)));
}


void
GUIOSGManipulator::setByMatrix(const osg::Matrixd& matrix) {
    _center = osg::Vec3d(0., 0., -_distance) * matrix;
    _rotation = matrix.getRotate();

    // fix current rotation
    if (getVerticalAxisFixed()) {
        fixVerticalAxis(_center, _rotation, true);
    }
}


osg::Matrixd GUIOSGManipulator::getMatrix() const {
    if (myCurrentMode == MODE_TERRAIN) {
        return osg::Matrixd::translate(0., 0., _distance) *
               osg::Matrixd::rotate(_rotation) *
               osg::Matrixd::translate(_center);
    } else {
        osg::Vec3f eye = _center - _rotation * osg::Vec3d(0., 0., -_distance);
        return osg::Matrixd::rotate(_rotation) * osg::Matrixd::translate(eye);
    }
}


osg::Matrixd GUIOSGManipulator::getInverseMatrix() const {
    if (myCurrentMode == MODE_TERRAIN) {
        return osg::Matrixd::translate(-_center) *
               osg::Matrixd::rotate(_rotation.inverse()) *
               osg::Matrixd::translate(0.0, 0.0, -_distance);
    } else {
        osg::Vec3f eye = _center - _rotation * osg::Vec3d(0., 0., -_distance);
        return osg::Matrixd::translate(-eye) * osg::Matrixd::rotate(_rotation.inverse());
    }
}

#endif
