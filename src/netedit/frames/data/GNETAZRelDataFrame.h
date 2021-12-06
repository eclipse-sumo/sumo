/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNETAZRelDataFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    May 2020
///
// The Widget for add TAZRelationData elements
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNEGenericDataFrame.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNETAZRelDataFrame
 * The Widget for setting internal attributes of additional elements
 */
class GNETAZRelDataFrame : public GNEGenericDataFrame {

public:
    // ===========================================================================
    // class Legend
    // ===========================================================================

    class Legend : public FXGroupBoxModul {

    public:
        /// @brief constructor
        Legend(GNETAZRelDataFrame* TAZRelDataFrame);

        /// @brief destructor
        ~Legend();

        /// @brief set labels
        void setLabels(const GNETAZElement* fromTAZ, const GNETAZElement* toTAZ);

    private:
        /// @brief from TAZ label
        FXLabel* myFromTAZLabel;

        /// @brief to TAZ Label
        FXLabel* myToTAZLabel;
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNETAZRelDataFrame
     */
    GNETAZRelDataFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNETAZRelDataFrame();

    /// @brief set clicked TAZ
    bool setTAZ(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

    /// @brief build TAZRelation data
    void buildTAZRelationData();

    /// @brief get first selected TAZ Element
    GNETAZElement* getFirstTAZ() const;

    /// @brief get first selected TAZ Element
    GNETAZElement* getSecondTAZ() const;

    /// @brief clear TAZ selection
    void clearTAZSelection();

protected:
    /// @brief first selected TAZ Element
    GNETAZElement* myFirstTAZ;

    /// @brief first selected TAZ Element
    GNETAZElement* mySecondTAZ;

    /// @brief TAZRel legend
    GNETAZRelDataFrame::Legend* myLegend;

private:
    /// @brief Invalidated copy constructor.
    GNETAZRelDataFrame(const GNETAZRelDataFrame&) = delete;

    /// @brief Invalidated assignment operator.
    GNETAZRelDataFrame& operator=(const GNETAZRelDataFrame&) = delete;
};

/****************************************************************************/
