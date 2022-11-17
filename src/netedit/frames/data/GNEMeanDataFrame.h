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
/// @file    GNEMeanDataFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// The Widget for edit meanData elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>
#include <netedit/frames/GNEPathCreator.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEMeanDataFrame
 * The Widget for setting internal attributes of additional elements
 */
class GNEMeanDataFrame : public GNEFrame {

public:
    // ===========================================================================
    // class MeanDataTypeSelector
    // ===========================================================================

    class MeanDataTypeSelector : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEMeanDataFrame::MeanDataTypeSelector)

    public:
        /// @brief constructor
        MeanDataTypeSelector(GNEMeanDataFrame* meanDataFrameParent);

        /// @brief destructor
        ~MeanDataTypeSelector();

        /// @brief get current MeanData
        const GNETagProperties &getCurrentMeanData() const;

        /// @brief refresh vehicle type selector
        void refreshMeanDataTypeSelector();

        /// @brief refresh vehicle type selector (only IDs, without refreshing attributes)
        void refreshMeanDataTypeSelectorIDs();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select another element in ComboBox
        long onCmdSelectItem(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(MeanDataTypeSelector)

    private:
        /// @brief pointer to Frame Parent
        GNEMeanDataFrame* myMeanDataFrameParent;

        /// @brief CurrentMeanData selected
        GNETagProperties myCurrentMeanData;

        /// @brief comboBox with the list of vTypes
        MFXIconComboBox* myTypeComboBox;

        /// @brief invalidMeanData
        GNETagProperties myInvalidMeanData;
    };

    /**@brief Constructor (protected due GNEMeanDataFrame is abtract)
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet GNEViewNet that uses this GNEFrame
     */
    GNEMeanDataFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEMeanDataFrame();

    /// @brief show Frame
    void show();

    /// @brief hide Frame
    void hide();

    /// @brief function called after undo/redo in the current frame
    void updateFrameAfterUndoRedo();

protected:
    /// @brief meanData type selector
    MeanDataTypeSelector* myMeanDataTypeSelector = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNEMeanDataFrame(const GNEMeanDataFrame&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMeanDataFrame& operator=(const GNEMeanDataFrame&) = delete;
};
