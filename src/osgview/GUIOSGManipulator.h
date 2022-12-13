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
/// @file    GUIOSGManipulator.h
/// @author  Mirko Barthauer
/// @date    27.11.2022
///
// A custom camera manipulator to interact with the OSG view directly
/****************************************************************************/
#pragma once
#include <config.h>

#ifdef HAVE_OSG

#include "GUIOSGHeader.h"

enum ManipulatorMode {
    MODE_EGO = 0,
    MODE_WALK,
    MODE_TERRAIN
};

class GUIOSGManipulator : public osgGA::TerrainManipulator {
public:
    GUIOSGManipulator(ManipulatorMode initMode = MODE_TERRAIN, bool verticalFixed = true, double eyeHeight = 1.7);
    bool performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy);
    bool performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy);
    bool performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy);
    bool performMouseDeltaMovement(const double dx, const double dy);
    bool handleMouseMove(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    bool handleMouseDeltaMovement(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    bool handleKeyDown(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    bool handleKeyUp(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
    void rotateYawPitch(osg::Quat& rotation, const double yaw, const double pitch, const osg::Vec3d& localUp);
    osg::Camera* getHUD();
    /// @brief Set the position of the manipulator using a 4x4 matrix.
    void setByMatrix(const osg::Matrixd& matrix);
    /// @brief Get the position of the manipulator as 4x4 matrix.
    osg::Matrixd getMatrix() const;
    /// @brief Get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.
    osg::Matrixd getInverseMatrix() const;
    /// @brief inform HUD about the current window size to let it reposition
    void updateHUDPosition(int width, int height);

private:
#ifdef _DEBUG
    void updateHUD();
#endif

protected:
    osg::ref_ptr<osg::Geode> myTextNode;
    osg::ref_ptr<osg::Camera> myHUDCamera;
private:
    /// @brief remember which ManipulatorMode we use 
#ifdef _DEBUG
    osgText::FadeText* myText;
#endif
    ManipulatorMode myCurrentMode;
    double myWalkEyeHeight;
    double myMoveSpeed;
    double myMoveAccel;
    osg::Vec3d myMove;
};

#endif