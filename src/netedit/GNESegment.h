/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
// GNESegment used in Path Manager
/****************************************************************************/
#pragma once
#include <config.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNELane;
class GNEJunction;
class GNEPathElement;
class GNEPathManager;

// ===========================================================================
// class definitions
// ===========================================================================

class GNESegment {

public:
    /// @brief constructor for lanes
    GNESegment(GNEPathManager* pathManager, GNEPathElement* element, const GNELane* lane, std::vector<GNESegment*>& segments);

    /// @brief constructor for junctions
    GNESegment(GNEPathManager* pathManager, GNEPathElement* element, const GNEJunction* junction, std::vector<GNESegment*>& segments);

    /// @brief destructor
    ~GNESegment();

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
    GNESegment* getNextSegment() const;

    /// @brief get previous segment
    GNESegment* getPreviousSegment() const;

    /// @brief check if segment is the first path's segment
    bool isFirstSegment() const;

    /// @brief check if segment is the last path's segment
    bool isLastSegment() const;

    /// @}

    /// @name functions related with GNE elements associated with this segment
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

    /// @name functions related with labeling segments (used for certain elements as E2 multilane detectors)
    /// @{

    /// @brief check if segment is label segment
    bool isLabelSegment() const;

    /// @brief mark segment as middle segment
    void markSegmentLabel();

    /// @}

protected:
    /// @brief pointer to path manager
    GNEPathManager* myPathManager;

    /// @brief path element associated with this segment
    GNEPathElement* myPathElement;

    /// @brief lane associated with this segment
    const GNELane* myLane;

    /// @brief junction associated with this segment
    const GNEJunction* myJunction;

    /// @brief lane index
    int myLaneIndex = 0;

    /// @brief junction index
    int myJunctionIndex = 0;

    /// @brief pointer to next segment
    GNESegment* myNextSegment;

    /// @brief pointer to previous segment
    GNESegment* myPreviousSegment;

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
    GNESegment();

    /// @brief Invalidated copy constructor.
    GNESegment(const GNESegment&) = delete;

    /// @brief Invalidated assignment operator.
    GNESegment& operator=(const GNESegment&) = delete;
};
