/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2021-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEWireFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2021
///
// The Widget for editing wires
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNETagSelector;
class GNEAttributesEditor;
class GNESelectorParent;
class GNEConsecutiveSelector;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEWireFrame : public GNEFrame {

public:
    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEWireFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEWireFrame();

    /// @brief show wire frame
    void show();

    /**@brief add wire element
     * @param viewObjects collection of objects under cursor after click over view
     * @return true if wire was successfully added
     */
    bool addWire(const GNEViewNetHelper::ViewObjectsSelector& viewObjects);

    /// @brief get consecutive lane selector
    GNEConsecutiveSelector* getConsecutiveLaneSelector() const;

    /// @brief create path
    bool createPath(const bool useLastRoute);

protected:
    /// @brief SumoBaseObject used for create wire
    CommonXMLStructure::SumoBaseObject* myBaseWire = nullptr;

    /// @brief Tag selected in GNETagSelector
    void tagSelected();

private:
    // @brief create baseWireObject
    bool createBaseWireObject(const GNETagProperties* tagProperty);

    /// @brief build wire over view
    bool buildWireOverView(const GNETagProperties* tagProperty);

    /// @brief item selector
    GNETagSelector* myWireTagSelector = nullptr;

    /// @brief internal wire attributes
    GNEAttributesEditor* myWireAttributesEditor = nullptr;

    /// @brief Select wire parent
    GNESelectorParent* mySelectorWireParent = nullptr;

    /// @brief Module for select consecutive lanes
    GNEConsecutiveSelector* myConsecutiveLaneSelector = nullptr;

    /// @brief Warn about experimental state
    bool myWarnedExperimental = false;
};
