/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEMoveFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2020
///
// The Widget for move elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>
#include <netedit/frames/common/GNEGroupBoxModule.h>

// ===========================================================================
// class definitions
// ===========================================================================

class GNEMoveFrame : public GNEFrame {

public:
    // ===========================================================================
    // class CommonMoveOptions
    // ===========================================================================

    class CommonMoveOptions : public GNEGroupBoxModule {

    public:
        /// @brief constructor
        CommonMoveOptions(GNEMoveFrame* moveFrameParent);

        /// @brief destructor
        ~CommonMoveOptions();

        /// @brief allow change lane
        bool getAllowChangeLane() const;

        /// @brief check if merge geometry points
        bool getMergeGeometryPoints() const;

    private:
        /// @brief checkbox for enable/disable change lanes
        FXCheckButton* myAllowChangeLanes;

        /// @brief checkbox for enable/disable merge geometry points
        FXCheckButton* myMergeGeometryPoints;
    };

    // ===========================================================================
    // class NetworkMoveOptions
    // ===========================================================================

    class NetworkMoveOptions : public GNEGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEMoveFrame::NetworkMoveOptions)

    public:
        /// @brief constructor
        NetworkMoveOptions(GNEMoveFrame* moveFrameParent);

        /// @brief destructor
        ~NetworkMoveOptions();

        /// @brief show NetworkMoveOptions
        void showNetworkMoveOptions();

        /// @brief hide NetworkMoveOptions
        void hideNetworkMoveOptions();

        /// @brief check if option "move whole polygons" is enabled
        bool getMoveWholePolygons() const;

        /// @brief check if option "force draw geometry points" is enabled
        bool getForceDrawGeometryPoints() const;

        /// @brief check if option "move only junction center" is enabled
        bool getMoveOnlyJunctionCenter() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called after change option
        long onCmdChangeOption(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(NetworkMoveOptions)

    private:
        /// @brief pointer to move frame parent
        GNEMoveFrame* myMoveFrameParent;

        /// @brief checkbox for enable/disable move whole polygons
        FXCheckButton* myMoveWholePolygons = nullptr;

        /// @brief checkbox for force draw geometry points
        FXCheckButton* myForceDrawGeometryPoints = nullptr;

        /// @brief checkbox for move only juntion center
        FXCheckButton* myMoveOnlyJunctionCenter = nullptr;
    };

    // ===========================================================================
    // class DemandMoveOptions
    // ===========================================================================

    class DemandMoveOptions : public GNEGroupBoxModule {

    public:
        /// @brief constructor
        DemandMoveOptions(GNEMoveFrame* moveFrameParent);

        /// @brief destructor
        ~DemandMoveOptions();

        /// @brief show DemandMoveOptions
        void showDemandMoveOptions();

        /// @brief hide DemandMoveOptions
        void hideDemandMoveOptions();

        /// @brief check if leave stopPersonConnected is enabled
        bool getLeaveStopPersonsConnected() const;

    private:
        /// @brief pointer to move frame parent
        GNEMoveFrame* myMoveFrameParent = nullptr;

        /// @brief checkbox for enable/disable leave stopPersons connected
        FXCheckButton* myLeaveStopPersonsConnected = nullptr;
    };

    // ===========================================================================
    // class ShiftEdgeSelectedGeometry
    // ===========================================================================

    class ShiftEdgeSelectedGeometry : public GNEGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEMoveFrame::ShiftEdgeSelectedGeometry)

    public:
        /// @brief constructor
        ShiftEdgeSelectedGeometry(GNEMoveFrame* moveFrameParent);

        /// @brief destructor
        ~ShiftEdgeSelectedGeometry();

        /// @brief enable shift edge geometry
        void enableShiftEdgeGeometry();

        /// @brief disable change Z in selection
        void disableShiftEdgeGeometry();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user changes Z value
        long onCmdChangeShiftValue(FXObject*, FXSelector, void*);

        /// @brief Called when user press the apply Z value button
        long onCmdShiftEdgeGeometry(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(ShiftEdgeSelectedGeometry)

    private:
        /// @brief pointer to move frame parent
        GNEMoveFrame* myMoveFrameParent;

        /// @brief textField for shift value
        FXTextField* myShiftValueTextField = nullptr;

        /// @brief button for apply Z value
        FXButton* myApplyZValue = nullptr;
    };

    // ===========================================================================
    // class ChangeZInSelection
    // ===========================================================================

    class ChangeZInSelection : public GNEGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEMoveFrame::ChangeZInSelection)

    public:
        /// @brief constructor
        ChangeZInSelection(GNEMoveFrame* moveFrameParent);

        /// @brief destructor
        ~ChangeZInSelection();

        /// @brief enabale change Z in selection
        void enableChangeZInSelection();

        /// @brief disable change Z in selection
        void disableChangeZInSelection();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user changes Z value
        long onCmdChangeZValue(FXObject*, FXSelector, void*);

        /// @brief Called when user changes Z mode
        long onCmdChangeZMode(FXObject*, FXSelector, void*);

        /// @brief Called when user press the apply Z value button
        long onCmdApplyZ(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(ChangeZInSelection)

        /// @brief update label
        void updateInfoLabel();

    private:
        /// @brief pointer to move frame parent
        GNEMoveFrame* myMoveFrameParent;

        /// @brief textField for Z value
        FXTextField* myZValueTextField = nullptr;

        /// @brief radio button for absolute value
        FXRadioButton* myAbsoluteValue = nullptr;

        /// @brief apply button
        FXButton* myApplyButton = nullptr;

        /// @brief radio button for relative value
        FXRadioButton* myRelativeValue = nullptr;

        /// @brief info label
        FXLabel* myInfoLabel = nullptr;
    };

    // ===========================================================================
    // class ShiftShapeGeometry
    // ===========================================================================

    class ShiftShapeGeometry : public GNEGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEMoveFrame::ShiftShapeGeometry)

    public:
        /// @brief constructor
        ShiftShapeGeometry(GNEMoveFrame* moveFrameParent);

        /// @brief destructor
        ~ShiftShapeGeometry();

        /// @brief show shift shape geometry
        void showShiftShapeGeometry();

        /// @brief hide change Z in selection
        void hideShiftShapeGeometry();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user changes Z value
        long onCmdChangeShiftValue(FXObject*, FXSelector, void*);

        /// @brief Called when user press the apply Z value button
        long onCmdShiftShapeGeometry(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(ShiftShapeGeometry)

    private:
        /// @brief pointer to move frame parent
        GNEMoveFrame* myMoveFrameParent;

        /// @brief textField for shiftX value
        FXTextField* myShiftValueXTextField = nullptr;

        /// @brief textField for shiftY value
        FXTextField* myShiftValueYTextField = nullptr;
    };

    // ===========================================================================
    // class Information
    // ===========================================================================

    class Information : public GNEGroupBoxModule {

    public:
        /// @brief constructor
        Information(GNEMoveFrame* moveFrameParent);

        /// @brief destructor
        ~Information();
    };

    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEMoveFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEMoveFrame();

    /**@brief handle processClick and set the relative coloring
     * @param[in] clickedPosition clicked position over ViewNet
     * @param viewObjects collection of objects under cursor after click over view
     * @param objectsUnderGrippedCursor collection of objects under gripped cursor after click over view
     */
    void processClick(const Position& clickedPosition,
                      const GNEViewNetHelper::ViewObjectsSelector& viewObjects,
                      const GNEViewNetHelper::ViewObjectsSelector& objectsUnderGrippedCursor);

    /// @brief show prohibition frame
    void show();

    /// @brief hide prohibition frame
    void hide();

    /// @brief get common mode options
    CommonMoveOptions* getCommonMoveOptions() const;

    /// @brief get network mode options
    NetworkMoveOptions* getNetworkMoveOptions() const;

    /// @brief get demand mode options
    DemandMoveOptions* getDemandMoveOptions() const;

private:
    /// @brief modul for CommonMove Options
    CommonMoveOptions* myCommonMoveOptions = nullptr;

    /// @brief modul for NetworkMove Options
    NetworkMoveOptions* myNetworkMoveOptions = nullptr;

    /// @brief modul for DemandMove Options
    DemandMoveOptions* myDemandMoveOptions = nullptr;

    /// @brief modul for shift edge selected geometry
    ShiftEdgeSelectedGeometry* myShiftEdgeSelectedGeometry = nullptr;

    /// @brief modul for change Z in selection
    ChangeZInSelection* myChangeZInSelection = nullptr;

    /// @brief modul for show information
    Information* myInformation = nullptr;

    /// @brief modul for shift shape geometry
    ShiftShapeGeometry* myShiftShapeGeometry = nullptr;
};
