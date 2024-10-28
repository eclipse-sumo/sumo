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
/// @file    GNETypeDistributionFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2023
///
// The Widget for edit type distribution elements
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEDistributionFrame.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNETypeDistributionFrame
 */
class GNETypeDistributionFrame : public GNEFrame {

public:
    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNETypeDistributionFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNETypeDistributionFrame();

    /// @brief show Frame
    void show();

    /// @brief get type distribution selector
    GNEDistributionFrame::DistributionSelector* getDistributionSelector() const;

protected:
    /// @brief function called after set a valid attribute in AttributeCreator/AttributeEditor/ParametersEditor/...
    void attributeUpdated(SumoXMLAttr attribute);

private:
    /// @brief type editor
    GNEDistributionFrame::DistributionEditor* myDistributionEditor = nullptr;

    /// @brief type distribution selector
    GNEDistributionFrame::DistributionSelector* myDistributionSelector = nullptr;

    /// @brief distribution attributes editor
    GNEFrameAttributeModules::AttributesEditor* myAttributesEditor = nullptr;

    /// @brief editor for distribution values
    GNEDistributionFrame::DistributionValuesEditor* myDistributionValuesEditor = nullptr;
};
