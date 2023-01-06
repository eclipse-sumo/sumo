/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
    // Confirm TAZ relation
    // ===========================================================================

    class ConfirmTAZRelation : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNETAZRelDataFrame::ConfirmTAZRelation)

    public:
        /// @brief constructor
        ConfirmTAZRelation(GNETAZRelDataFrame* TAZRelDataFrame);

        /// @brief destructor
        ~ConfirmTAZRelation();

        /// @brief called when user press confirm TAZ Relation button
        long onCmdConfirmTAZRelation(FXObject*, FXSelector, void*);

        /// @brief called when TAZ Relation button is updated
        long onUpdConfirmTAZRelation(FXObject*, FXSelector, void*);

        /// @brief called when user press clear TAZ Relation button
        long onCmdClearSelection(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(ConfirmTAZRelation)

    private:
        /// @brief pointer to TAZRelDataFrame parent
        GNETAZRelDataFrame* myTAZRelDataFrame = nullptr;

        /// @brief confirm TAZ Button
        FXButton* myConfirmTAZButton = nullptr;

        /// @brief clear TAZ Button
        FXButton* myClearTAZButton = nullptr;
    };

    // ===========================================================================
    // class Legend
    // ===========================================================================

    class Legend : public MFXGroupBoxModule {

    public:
        /// @brief constructor
        Legend(GNETAZRelDataFrame* TAZRelDataFrame);

        /// @brief destructor
        ~Legend();

        /// @brief set labels
        void setLabels(const GNETAZ* fromTAZ, const GNETAZ* toTAZ);

    private:
        /// @brief from TAZ label
        FXLabel* myFromTAZLabel;

        /// @brief to TAZ Label
        FXLabel* myToTAZLabel;
    };

    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNETAZRelDataFrame
     */
    GNETAZRelDataFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNETAZRelDataFrame();

    /// @brief set clicked TAZ
    bool setTAZ(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

    /// @brief build TAZRelation data
    void buildTAZRelationData();

    /// @brief get first selected TAZ Element
    GNEAdditional* getFirstTAZ() const;

    /// @brief get first selected TAZ Element
    GNEAdditional* getSecondTAZ() const;

    /// @brief clear TAZ selection
    void clearTAZSelection();

protected:
    /// @brief first selected TAZ Element
    GNETAZ* myFirstTAZ = nullptr;

    /// @brief first selected TAZ Element
    GNETAZ* mySecondTAZ = nullptr;

    /// @brief confirm TAZ Relation
    GNETAZRelDataFrame::ConfirmTAZRelation* myConfirmTAZRelation = nullptr;

    /// @brief TAZRel legend
    GNETAZRelDataFrame::Legend* myLegend = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNETAZRelDataFrame(const GNETAZRelDataFrame&) = delete;

    /// @brief Invalidated assignment operator.
    GNETAZRelDataFrame& operator=(const GNETAZRelDataFrame&) = delete;
};

/****************************************************************************/
