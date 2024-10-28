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
/// @file    GNEEdgeRelDataFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2020
///
// The Widget for add EdgeRelationData elements
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEGenericDataFrame.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEEdgeRelDataFrame
 * The Widget for setting internal attributes of additional elements
 */
class GNEEdgeRelDataFrame : public GNEGenericDataFrame {

public:
    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEEdgeRelDataFrame
     */
    GNEEdgeRelDataFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEEdgeRelDataFrame();

    /**@brief add additional element
     * @param viewObjects collection of objects under cursor after click over view
     * @return true if additional was successfully added
     */
    bool addEdgeRelationData(const GNEViewNetHelper::ViewObjectsSelector& viewObjects, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed);

protected:
    /// @brief create path
    bool createPath(const bool useLastRoute);

private:
    /// @brief Invalidated copy constructor.
    GNEEdgeRelDataFrame(const GNEEdgeRelDataFrame&) = delete;

    /// @brief Invalidated assignment operator.
    GNEEdgeRelDataFrame& operator=(const GNEEdgeRelDataFrame&) = delete;
};

/****************************************************************************/
