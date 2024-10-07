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

class GUIOSGView;

enum ManipulatorMode {
    MODE_EGO = 0,
    MODE_WALK,
    MODE_TERRAIN
};

class GUIOSGManipulator : public osgGA::TerrainManipulator {
public:
    GUIOSGManipulator(GUIOSGView* parent, ManipulatorMode initMode = MODE_TERRAIN, bool verticalFixed = true, double eyeHeight = 1.7);
    ~GUIOSGManipulator();
    bool performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy) override;
    bool performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy) override;
    bool performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy) override;
    bool performMouseDeltaMovement(const float dx, const float dy) override;
    bool handleMouseMove(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;
    bool handleMouseDrag(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;
    bool handleMousePush(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;
    bool handleMouseRelease(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;
    bool handleMouseDeltaMovement(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;
    bool handleKeyDown(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;
    bool handleKeyUp(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;
    void rotateYawPitch(osg::Quat& rotation, const double yaw, const double pitch, const osg::Vec3d& localUp);
    /// @brief Set the position of the manipulator using a 4x4 matrix.
    void setByMatrix(const osg::Matrixd& matrix) override;
    /// @brief Get the position of the manipulator as 4x4 matrix.
    osg::Matrixd getMatrix() const override;
    /// @brief Get the position of the manipulator as a inverse matrix of the manipulator, typically used as a model view matrix.
    osg::Matrixd getInverseMatrix() const override;
    /// @brief Note the current manipulator mode in the HUD
    void updateHUDText();
protected:
    void centerMousePointer(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override;
private:
    /// @brief invalidated standard constructor
    GUIOSGManipulator() = delete;
    GUIOSGManipulator(const GUIOSGManipulator& g);
private:
    /// @brief ref to parent view for callback
    GUIOSGView* myParent;
    /// @brief remember which ManipulatorMode we use
    ManipulatorMode myCurrentMode;
    //double myWalkEyeHeight;
    double myMoveSpeed;
    osg::Vec3d myMove;
};

#endif
