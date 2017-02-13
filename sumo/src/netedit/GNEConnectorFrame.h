/****************************************************************************/
/// @file    GNEConnectorFrame.h
/// @author  Jakob Erdmann
/// @date    May 2011
/// @version $Id$
///
// The Widget for modifying lane-to-lane connections
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEConnectorFrame_h
#define GNEConnectorFrame_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEFrame.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNELane;
class GNEInternalLane;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEConnectorFrame
 * The Widget for modifying selections of network-elements
 */
class GNEConnectorFrame : public GNEFrame {
    /// @brief FOX-declaration
    FXDECLARE(GNEConnectorFrame)

public:
    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEConnectorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEConnectorFrame();

    /**@brief either sets the current lane or toggles the connection of the
     * current lane to this lane (if they share a junction)
     * @param[in] lane Either the lane to set as current lane, or the destination from current lane
     * @param[in] mayDefinitelyPass Whether new connections shall have the pass attribute set
     * @param[in] toggle Whether non-existing connections shall be created
     */
    void handleLaneClick(GNELane* lane, bool mayDefinitelyPass, bool allowConflict, bool toggle);

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user presses the OK-Button saves any connection modifications
    long onCmdOK(FXObject*, FXSelector, void*);

    /// @brief Called when the user presses the Cancel-button discards any connection modifications
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief Called when the user presses the Corresponding-button
    long onCmdSelectDeadEnds(FXObject*, FXSelector, void*);
    long onCmdSelectDeadStarts(FXObject*, FXSelector, void*);
    long onCmdSelectConflicts(FXObject*, FXSelector, void*);
    long onCmdSelectPass(FXObject*, FXSelector, void*);
    long onCmdClearSelectedConnections(FXObject*, FXSelector, void*);
    long onCmdResetSelectedConnections(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX needs this
    GNEConnectorFrame() {}

private:

    /// @brief the status of a target lane
    enum LaneStatus {
        UNCONNECTED,
        CONNECTED,
        CONNECTED_PASS,
        CONFLICTED
    };

    /// @brief Groupbox for description
    FXGroupBox* myGroupBoxDescription;

    /// @brief the label that shows the current editing state
    FXLabel* myLaneDescriptionLabel;

    /// @brief GroupBox for Buttons
    FXGroupBox* myGroupBoxModifications;

    /// @brief "Cancel" button
    FXButton* myCancelButton;

    /// @brief "OK" button
    FXButton* mySaveButton;

    /// @brief groupbox for operations
    FXGroupBox* myGroupBoxOperations;

    /// @brief "Select Dead Ends" button
    FXButton* mySelectDeadEndsButton;

    /// @brief "Select Dead Starts" button
    FXButton* mySelectDeadStartsButton;

    /// @brief "Select Conflicts" button
    FXButton* mySelectConflictsButton;

    /// @brief "Select Edges which may always pass"
    FXButton* mySelectPassingButton;

    /// @brief "Clear Selected"
    FXButton* myClearSelectedButton;

    /// @brief "Reset Selected"
    FXButton* myResetSelectedButton;

    /// @brief groupbox for selection hints
    FXGroupBox* myGroupBoxSelection;

    /// @brief Selection Hint
    FXLabel* myHoldShiftLabel;

    /// @brief hold control label
    FXLabel* myHoldControlLabel;

    /// @brief group box for legend
    FXGroupBox* myGroupBoxLegend;

    /// @brief source label
    FXLabel* mySourceLabel;

    /// @brief target label
    FXLabel* myTargetLabel;

    /// @brief possible target label
    FXLabel* myPossibleTargetLabel;

    /// @brief target pass label
    FXLabel* myTargetPassLabel;

    /// @brief conflict label
    FXLabel* myConflictLabel;

    /// @brief the lane of which connections are to be modified
    GNELane* myCurrentLane;

    /// @brief the set of lanes to which the current lane may be connected
    std::set<GNELane*> myPotentialTargets;

    /// @brief number of changes
    int myNumChanges;

    /// @brief the internal lanes belonging the the current junction indexed by their tl-index
    std::map<int, GNEInternalLane*> myInternalLanes;

    /// @brief color for the from-lane of a connection
    static RGBColor sourceColor;

    /// @brief color for the to-lane of a connection
    static RGBColor targetColor;

    /// @brief color for a to-lane that cannot be used because another connection conflicts
    static RGBColor conflictColor;

    /// @brief color for the to-lane of a connection with pass attribute
    static RGBColor targetPassColor;

    /// @brief color for potential to-lane targets (currently unconnected)
    static RGBColor potentialTargetColor;

private:
    /// @brief update description
    void updateDescription() const;

    /// @brief init targets
    void initTargets();

    /// @brief clean up when deselecting current lane
    void cleanup();

    /// @brief remove connections
    void removeConnections(GNELane* lane);

    /// @brief return the status of toLane
    LaneStatus getLaneStatus(const std::vector<NBEdge::Connection>& connections, GNELane* targetLane);

};


#endif

/****************************************************************************/

