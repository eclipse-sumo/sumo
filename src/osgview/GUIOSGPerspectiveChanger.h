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
/// @file    GUIOSGPerspectiveChanger.h
/// @author  Mirko Barthauer
/// @date    August 2022
///
// Implementation of GUIPerspectiveChanger for OSG 3D views
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/geom/Position.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include "GUIOSGHeader.h"
#include "GUIOSGView.h"


// ===========================================================================
// class declarations
// ===========================================================================
class Boundary;

class GUIOSGPerspectiveChanger : public GUIPerspectiveChanger {
public:

    /* Constructor
     * @param[in] callBack The view to be udpated upon changes
     */
    GUIOSGPerspectiveChanger(GUIOSGView& callBack, const Boundary& viewPort);

    /// Destructor
    ~GUIOSGPerspectiveChanger();

    /// @brief mouse functions
    //@{
    /// @brief called when user releases left button
    bool onLeftBtnRelease(void* data);

    /// @brief called when user releases right button
    bool onRightBtnRelease(void* data);

    /// @brief called when user releases middle button
    bool onMiddleBtnRelease(void* data);

    /// @brief called when user moves mouse
    void onMouseMove(void* data);
    //@}

    /// @brief Returns the rotation of the canvas stored in this changer
    double getRotation() const;

    /// @brief Returns the x-offset of the field to show stored in this changer
    double getXPos() const;

    /// @brief Returns the y-offset of the field to show stored in this changer
    double getYPos() const;

    /// @brief Returns the zoom factor computed stored in this changer
    double getZoom() const;

    /// @brief Returns the camera height corresponding to the current zoom factor
    double getZPos() const;

    /// @brief Returns the camera height at which the given zoom level is reached
    double zoom2ZPos(double zoom) const;

    /// @brief Returns the zoom level that is achieved at a given camera height
    double zPos2Zoom(double zPos) const;

    /// @brief Centers the view to the given position, setting it to a size that covers the radius. Used for: Centering of vehicles and junctions */
    void centerTo(const Position& pos, double radius, bool applyZoom = true);

    /// @brief Sets the viewport Used for: Adapting a new viewport
    void setViewport(double zoom, double xPos, double yPos);

    /// @brief Alternative method for setting the viewport
    void setViewportFrom(double xPos, double yPos, double zPos);

    /// @brief Sets the rotation
    void setRotation(double rotation);

    /* @brief Adapts the viewport so that a change in canvass size keeps most of the
     * view intact (by showing more / less instead of zooming)
     * The canvass is clipped/enlarged on the left side of the screen
     *
     * @param[in] change The horizontal change in canvas size in pixels
     */
    void changeCanvasSizeLeft(int change);

    /// @brief set viewport
    void setViewport(const Boundary& viewPort);

    /// @brief update the min/max coordinates of the view on the ground plane after changing the camera pos / direction
    void updateViewport();

private:
    /// @brief update the min/max coordinates of the view on the ground plane after changing the camera pos / direction
    void updateViewport(osg::Vec3d& lookFrom);

    /// @brief calculate the position on the ground (in world coordinates) given a normalised view coordinate (x,y ~ [-1;1])
    osg::Vec3d getPositionOnGround(double x, double y);
private:
    /// the original viewport dimensions in m which serve as the reference point for 100% zoom
    double myOrigWidth, myOrigHeight;

    /// the current rotation
    double myRotation;

    /// the OSG camera structure to gather the view coordinates
    osg::ref_ptr<osgGA::TerrainManipulator> myCameraManipulator;

    /// @brief Invalidated copy constructor.
    GUIOSGPerspectiveChanger(const GUIOSGPerspectiveChanger&);

    /// @brief Invalidated assignment operator.
    GUIOSGPerspectiveChanger& operator=(const GUIOSGPerspectiveChanger&);
};
