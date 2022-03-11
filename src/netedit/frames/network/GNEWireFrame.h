/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2021-2022 German Aerospace Center (DLR) and others.
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

#include "GNECommonNetworkModules.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEWireFrame
 * The Widget for editing connection foes
 */
class GNEWireFrame : public GNEFrame {

public:
    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEWireFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEWireFrame();

    /// @brief show wire frame
    void show();

    /**@brief add wire element
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @return true if wire was sucesfully added
     */
    bool addWire(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

    /// @brief get consecutive lane selector
    GNECommonNetworkModules::ConsecutiveLaneSelector* getConsecutiveLaneSelector() const;

    /// @brief create path
    void createPath();

protected:
    /// @brief SumoBaseObject used for create wire
    CommonXMLStructure::SumoBaseObject* myBaseWire = nullptr;

    /// @brief Tag selected in TagSelector
    void tagSelected();

private:
    // @brief create baseWireObject
    bool createBaseWireObject(const GNETagProperties& tagProperty);

    /// @brief build wire over view
    bool buildWireOverView(const GNETagProperties& tagValues);

    /// @brief item selector
    GNEFrameModules::TagSelector* myWireTagSelector = nullptr;

    /// @brief internal wire attributes
    GNEFrameAttributeModules::AttributesCreator* myWireAttributes = nullptr;

    /// @brief Netedit parameter
    GNEFrameAttributeModules::NeteditAttributes* myNeteditAttributes = nullptr;

    /// @brief Module for select a single parent wire
    GNEFrameModules::SelectorParent* mySelectorWireParent = nullptr;

    /// @brief Module for select consecutive lanes
    GNECommonNetworkModules::ConsecutiveLaneSelector* myConsecutiveLaneSelector;
};
