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
/// @file    GNEAdditionalFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
///
// The Widget for add additional elements
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNECommonNetworkModules.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEAdditionalFrame
 * The Widget for setting internal attributes of additional elements
 */
class GNEAdditionalFrame : public GNEFrame {

public:
    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEAdditionalFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEAdditionalFrame();

    /// @brief show Frame
    void show();

    /**@brief add additional element
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @return true if additional was sucesfully added
     */
    bool addAdditional(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

    /// @brief get consecutive lane selector
    GNECommonNetworkModules::ConsecutiveLaneSelector* getConsecutiveLaneSelector() const;

    /// @brief create path
    void createPath();

protected:
    /// @brief SumoBaseObject used for create additional
    CommonXMLStructure::SumoBaseObject* myBaseAdditional;

    /// @brief Tag selected in TagSelector
    void tagSelected();

private:
    // @brief create baseAdditionalObject
    bool createBaseAdditionalObject(const GNETagProperties& tagProperty);

    /// @brief build common additional attributes
    bool buildAdditionalCommonAttributes(const GNETagProperties& tagValues);

    /// @brief build additional over an edge (parent of lane)
    bool buildAdditionalOverEdge(GNELane* lane, const GNETagProperties& tagValues);

    /// @brief build additional over a single lane
    bool buildAdditionalOverLane(GNELane* lane, const GNETagProperties& tagValues);

    /// @brief build additional over view
    bool buildAdditionalOverView(const GNETagProperties& tagValues);

    /// @brief item selector
    GNEFrameModules::TagSelector* myAdditionalTagSelector;

    /// @brief internal additional attributes
    GNEFrameAttributeModules::AttributesCreator* myAdditionalAttributes;

    /// @brief Netedit parameter
    GNEFrameAttributeModules::NeteditAttributes* myNeteditAttributes;

    /// @brief Module for select a single parent additional
    GNEFrameModules::SelectorParent* mySelectorAdditionalParent;

    /// @brief Module for select edges
    GNECommonNetworkModules::EdgesSelector* myEdgesSelector;

    /// @brief Module for select lanes
    GNECommonNetworkModules::LanesSelector* myLanesSelector;

    /// @brief Module for select consecutive lanes
    GNECommonNetworkModules::ConsecutiveLaneSelector* myConsecutiveLaneSelector;
};
