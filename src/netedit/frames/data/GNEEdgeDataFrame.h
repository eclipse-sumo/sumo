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
/// @file    GNEEdgeDataFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// The Widget for add edgeData elements
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEGenericDataFrame.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEEdgeDataFrame
 * The Widget for setting internal attributes of additional elements
 */
class GNEEdgeDataFrame : public GNEGenericDataFrame {

public:
    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEEdgeDataFrame
     */
    GNEEdgeDataFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEEdgeDataFrame();

    /**@brief add additional element
     * @param viewObjects collection of objects under cursor after click over view
     * @param mouseButtonKeyPressed key pressed during click
     * @return true if additional was successfully added
     */
    bool addEdgeData(const GNEViewNetHelper::ViewObjectsSelector& viewObjects, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed);

private:
    /// @brief Invalidated copy constructor.
    GNEEdgeDataFrame(const GNEEdgeDataFrame&) = delete;

    /// @brief Invalidated assignment operator.
    GNEEdgeDataFrame& operator=(const GNEEdgeDataFrame&) = delete;
};

/****************************************************************************/
