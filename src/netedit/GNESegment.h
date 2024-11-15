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
/// @file    GNESegment.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2024
///
// Segment used in Path Manager
/****************************************************************************/
#pragma once
#include <config.h>

#include <netbuild/NBEdge.h>
#include <netbuild/NBVehicle.h>
#include <netedit/elements/GNEContour.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>


// ===========================================================================
// class definitions
// ===========================================================================

class GNELane;
class GNEJunction;
class GNEPathElement;
class GNEPathManager;


/// @brief segment
class Segment {

public:
    /// @brief constructor for lanes
    Segment(GNEPathManager* pathManager, GNEPathElement* element, const GNELane* lane, std::vector<Segment*>& segments);

    /// @brief constructor for junctions
    Segment(GNEPathManager* pathManager, GNEPathElement* element, const GNEJunction* junction, std::vector<Segment*>& segments);

    /// @brief destructor
    ~Segment();

    /// @name functions related with segment contour
    /// @{
    /// @brief get contour associated with segment
    GNEContour* getContour() const;

    /// @brief get from contour associated with segment (only if this is the first path segment)
    GNEContour* getFromContour() const;

    /// @brief get to contour associated with segment (only if this is the last path segment)
    GNEContour* getToContour() const;

    /// @}

    /// @brief functions related with the other paht segments
    /// @{
    /// @brief get next segment
    Segment* getNextSegment() const;

    /// @brief get previous segment
    Segment* getPreviousSegment() const;

    /// @brief check if segment is the first path's segment
    bool isFirstSegment() const;

    /// @brief check if segment is the last path's segment
    bool isLastSegment() const;

    /// @}

    /// @name functions related with GNE elements related with this segment
    /// @{
    /// @brief get path element
    GNEPathElement* getPathElement() const;

    /// @brief get lane associated with this segment
    const GNELane* getLane() const;

    /// @brief get previous lane
    const GNELane* getPreviousLane() const;

    /// @brief get next lane
    const GNELane* getNextLane() const;

    /// @brief get lane index
    int getLaneIndex() const;

    /// @brief get junction associated with this segment
    const GNEJunction* getJunction() const;

    /// @brief get lane index
    int getJunctionIndex() const;

    /// @}

    /// @brief check if segment is label segment
    bool isLabelSegment() const;

    /// @brief mark segment as middle segment (used for certain elements as E2 multilane detectors)
    void markSegmentLabel();

protected:
    /// @brief path manager
    GNEPathManager* myPathManager;

    /// @brief path element
    GNEPathElement* myPathElement;

    /// @brief lane associated with this segment
    const GNELane* myLane;

    /// @brief junction associated with this segment
    const GNEJunction* myJunction;

    /// @brief lane index
    int myLaneIndex = 0;

    /// @brief junction index
    int myJunctionIndex = 0;

    /// @brief pointer to next segment (use for draw red line)
    Segment* myNextSegment;

    /// @brief pointer to previous segment (use for draw red line)
    Segment* myPreviousSegment;

    /// @brief flag for check if this segment is a label segment
    bool myLabelSegment;

    /// @brief contour associated with segment
    GNEContour* myContour;

    /// @brief from contour, used for moving elements (only in the first segment)
    GNEContour* myFromContour;

    /// @brief to contour, used for moving elements (only in the last segment)
    GNEContour* myToContour;

private:
    /// @brief default constructor
    Segment();

    /// @brief Invalidated copy constructor.
    Segment(const Segment&) = delete;

    /// @brief Invalidated assignment operator.
    Segment& operator=(const Segment&) = delete;
};
