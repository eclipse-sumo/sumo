/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEEdgeDataFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// The Widget for add edgeData elements
/****************************************************************************/
#pragma once


// ===========================================================================
// included modules
// ===========================================================================

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
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEEdgeDataFrame
     */
    GNEEdgeDataFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEEdgeDataFrame();

    /**@brief add additional element
     * @param objectsUnderCursor collection of objects under cursor after click over view
     * @return true if additional was sucesfully added
     */
    bool addEdgeData(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

protected:
    //

private:
    /// @brief Invalidated copy constructor.
    GNEEdgeDataFrame(const GNEEdgeDataFrame&) = delete;

    /// @brief Invalidated assignment operator.
    GNEEdgeDataFrame& operator=(const GNEEdgeDataFrame&) = delete;
};

/****************************************************************************/
