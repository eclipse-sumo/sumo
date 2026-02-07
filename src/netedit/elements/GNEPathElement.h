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
/// @file    GNEPathElement.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2024
///
// Class used for path elements (routes, trips, flows...)
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/gui/settings/GUIVisualizationSettings.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNELane;
class GNESegment;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEPathElement {

public:
    enum class Options : int {
        NETWORK_ELEMENT =       1 << 0, // Network element
        ADDITIONAL_ELEMENT =    1 << 1, // Additional element
        DEMAND_ELEMENT =        1 << 2, // Demand element
        DATA_ELEMENT =          1 << 3, // Data element
        ROUTE =                 1 << 4, // Route (needed for overlapping labels)
    };

    /// @brief constructor
    GNEPathElement(const GNEPathElement::Options options);

    /// @brief destructor
    virtual ~GNEPathElement();

    /// @brief get path element option
    GNEPathElement::Options getPathElementOptions() const;

    /// @brief check if pathElement is a network element
    bool isNetworkElement() const;

    /// @brief check if pathElement is an additional element
    bool isAdditionalElement() const;

    /// @brief check if pathElement is a demand element
    bool isDemandElement() const;

    /// @brief check if pathElement is a data element
    bool isDataElement() const;

    /// @brief check if pathElement is a route
    bool isRoute() const;

    /// @brief implement in children+
    /// @{

    /// @brief compute pathElement
    virtual void computePathElement() = 0;

    /// @brief check if path element is selected
    virtual bool isPathElementSelected() const = 0;

    /**@brief Draws partial object over lane
        * @param[in] s The settings for the current view (may influence drawing)
        * @param[in] segment lane segment
        * @param[in] offsetFront front offset
        */
    virtual void drawLanePartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const = 0;

    /**@brief Draws partial object over junction
        * @param[in] s The settings for the current view (may influence drawing)
        * @param[in] segment junction segment
        * @param[in] offsetFront front offset
        */
    virtual void drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const = 0;

    /// @brief get first path lane
    virtual GNELane* getFirstPathLane() const = 0;

    /// @brief get last path lane
    virtual GNELane* getLastPathLane() const = 0;

    /// @}

private:
    /// @brief pathElement option
    const GNEPathElement::Options myOptions = GNEPathElement::Options::NETWORK_ELEMENT;

    /// @brief invalidate default constructor
    GNEPathElement() = delete;

    /// @brief Invalidated copy constructor.
    GNEPathElement(const GNEPathElement&) = delete;

    /// @brief Invalidated assignment operator.
    GNEPathElement& operator=(const GNEPathElement&) = delete;
};

/// @brief override tag parent bit operator
constexpr GNEPathElement::Options operator|(GNEPathElement::Options a, GNEPathElement::Options b) {
    return static_cast<GNEPathElement::Options>(static_cast<int>(a) | static_cast<int>(b));
}

/// @brief override tag parent bit operator
constexpr bool operator&(GNEPathElement::Options a, GNEPathElement::Options b) {
    return (static_cast<int>(a) & static_cast<int>(b)) != 0;
}
