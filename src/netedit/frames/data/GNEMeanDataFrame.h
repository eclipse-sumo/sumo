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
/// @file    GNEMeanDataFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2022
///
// The Widget for edit meanData elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>


// ===========================================================================
// class declaration
// ===========================================================================

class GNEMeanData;

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
        const GNETagProperties& getCurrentMeanData() const;

        /// @brief refresh meanData selector
        void refreshMeanDataTypeSelector();

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
        MFXComboBoxIcon* myTypeComboBox;

        /// @brief invalidMeanData
        GNETagProperties myInvalidMeanData;
    };

    // ===========================================================================
    // class MeanDataEditor
    // ===========================================================================

    class MeanDataEditor : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEMeanDataFrame::MeanDataEditor)

    public:
        /// @brief constructor
        MeanDataEditor(GNEMeanDataFrame* meanDataFrameParent);

        /// @brief destructor
        ~MeanDataEditor();

        /// @brief show MeanDataEditor modul
        void showMeanDataEditorModule();

        /// @brief hide MeanDataEditor box
        void hideMeanDataEditorModule();

        /// @brief update MeanDataEditor modul
        void refreshMeanDataEditorModule();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when "Vreate Vehicle MeanData" button is clicked
        long onCmdCreateMeanData(FXObject*, FXSelector, void*);

        /// @brief Called when "Delete Vehicle MeanData" button is clicked
        long onCmdDeletetMeanData(FXObject*, FXSelector, void*);

        /// @brief Called when "Copy Vehicle MeanData" button is clicked
        long onCmdCopyMeanData(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(MeanDataEditor)

    private:
        /// @brief pointer to meanData Frame Parent
        GNEMeanDataFrame* myMeanDataFrameParent;

        /// @brief "create meanData" button
        FXButton* myCreateMeanDataButton;

        /// @brief "delete meanData" button
        FXButton* myDeleteMeanDataButton;

        /// @brief "copy meanData"
        FXButton* myCopyMeanDataButton;
    };

    // ===========================================================================
    // class MeanDataSelector
    // ===========================================================================

    class MeanDataSelector : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEMeanDataFrame::MeanDataSelector)

    public:
        /// @brief constructor
        MeanDataSelector(GNEMeanDataFrame* typeFrameParent);

        /// @brief destructor
        ~MeanDataSelector();

        /// @brief show MeanDataSelector
        void showMeanDataSelector();

        /// @brief hide MeanDataSelector
        void hideMeanDataSelector();

        /// @brief get current Vehicle MeanData
        GNEMeanData* getCurrentMeanData() const;

        /// @brief set current Vehicle MeanData
        void setCurrentMeanData(GNEMeanData* vMeanData);

        /// @brief refresh meanData selector
        void refreshMeanDataSelector(bool afterChangingID);

        /// @brief refresh meanData selector (only IDs, without refreshing attributes)
        void refreshMeanDataSelectorIDs();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select another element in ComboBox
        long onCmdSelectItem(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(MeanDataSelector)

    private:
        /// @brief pointer to Frame Parent
        GNEMeanDataFrame* myMeanDataFrameParent;

        /// @brief pointer to current meanData
        GNEMeanData* myCurrentMeanData;

        /// @brief comboBox with the list of meanDatas
        MFXComboBoxIcon* myMeanDataComboBox;
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

    /// @brief function called after set a valid attribute in AttributeCreator/AttributeEditor/ParametersEditor/...
    void attributeUpdated(SumoXMLAttr attribute);

    /// @brief function called after undo/redo in the current frame
    void updateFrameAfterUndoRedo();

protected:
    /// @brief meanData type selector
    MeanDataTypeSelector* myMeanDataTypeSelector = nullptr;

    /// @brief meanData editor
    MeanDataEditor* myMeanDataEditor = nullptr;

    /// @brief meanData selector
    MeanDataSelector* myMeanDataSelector = nullptr;

    /// @brief meanData attributes editor
    GNEFrameAttributeModules::AttributesEditor* myMeanDataAttributesEditor = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNEMeanDataFrame(const GNEMeanDataFrame&) = delete;

    /// @brief Invalidated assignment operator.
    GNEMeanDataFrame& operator=(const GNEMeanDataFrame&) = delete;
};
