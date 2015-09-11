/****************************************************************************/
/// @file    GNEConnector.h
/// @author  Jakob Erdmann
/// @date    May 2011
/// @version $Id$
///
// The Widget for modifying lane-to-lane connections
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEConnector_h
#define GNEConnector_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <netbuild/NBEdge.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEViewNet;
class GNELane;
class GNEInternalLane;
class GNEUndoList;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEConnector
 * The Widget for modifying selections of network-elements
 */
class GNEConnector : public FXScrollWindow {
    // FOX-declarations
    FXDECLARE(GNEConnector)

public:
    /** @brief Constructor
     * @param[in] parent The parent window
     */
    GNEConnector(FXComposite* parent, GNEViewNet* updateTarget, GNEUndoList* undoList);


    /// @brief Destructor
    ~GNEConnector();


    FXFont* getHeaderFont() {
        return myHeaderFont;
    }

    /** @brief either sets the current lane or toggles the connection of the
     * current lane to this lane (if they share a junction)
     * @param[in] lane Either the lane to set as current lane, or the destination from current lane
     * @param[in] mayDefinitelyPass Whether new connections shall have the pass attribute set
     * @param[in] toggle Whether non-existing connections shall be created
     */
    void handleLaneClick(GNELane* lane, bool mayDefinitelyPass = false, bool toggle = true);

    /// @name FOX-callbacks
    /// @{
    /** @brief Called when the user presses the OK-Button
     * saves any connection modifications
     */
    long onCmdOK(FXObject*, FXSelector, void*);

    /** @brief Called when the user presses the Cancel-button
     * discards any connection modifications
     */
    long onCmdCancel(FXObject*, FXSelector, void*);

    /** @brief Called when the user presses the Corresponding-button */
    long onCmdSelectDeadEnds(FXObject*, FXSelector, void*);
    long onCmdSelectDeadStarts(FXObject*, FXSelector, void*);
    long onCmdSelectConflicts(FXObject*, FXSelector, void*);
    long onCmdSelectPass(FXObject*, FXSelector, void*);
    long onCmdClearSelectedConnections(FXObject*, FXSelector, void*);
    long onCmdResetSelectedConnections(FXObject*, FXSelector, void*);
    /// @}

protected:
    /// @brief FOX needs this
    GNEConnector() {}

private:

    /// @brief the status of a target lane
    enum LaneStatus {
        UNCONNECTED,
        CONNECTED,
        CONNECTED_PASS,
        CONFLICTED
    };

    /// @brief the panel to hold all member widgets
    FXVerticalFrame* myContentFrame;

    /// @brief Font for the widget
    FXFont* myHeaderFont;

    /// @brief the label that shows the current editing state
    FXLabel* myDescription;

    /// @brief the lane of which connections are to be modified
    GNELane* myCurrentLane;

    /// @brief the set of lanes to which the current lane may be connected
    std::set<GNELane*> myPotentialTargets;

    GNEUndoList* myUndoList;

    unsigned int myNumChanges;

    /* @brief the window to inform when connections are modified */
    GNEViewNet* myUpdateTarget;

    /// @brief the internal lanes belonging the the current junction indexed by their tl-index
    std::map<int, GNEInternalLane*> myInternalLanes;

    static const int WIDTH;

    /// @brief color for the from-lane of a connection
    static const RGBColor sourceColor;
    /// @brief color for the to-lane of a connection
    static const RGBColor targetColor;
    /// @brief color for a to-lane that cannot be used because another connection conflicts
    static const RGBColor conflictColor;
    /// @brief color for the to-lane of a connection with pass attribute
    static const RGBColor targetPassColor;
    /// @brief color for potential to-lane targets (currently unconnected)
    static const RGBColor potentialTargetColor;

private:
    void updateDescription() const;

    void initTargets();

    // @brief clean up when deselecting current lane
    void cleanup();

    // @brief remove connections
    void removeConnections(GNELane* lane);

    // @brief return the status of toLane
    LaneStatus getLaneStatus(const std::vector<NBEdge::Connection>& connections, GNELane* targetLane);

    /* @brief return the link number (tlLinkNo) of an existing connection
     * @param[in] connections All connections of the current edge from the given lane
     * @param[in] targetLane The target lane of the connection
     */
    unsigned int getTLLLinkNumber(const std::vector<NBEdge::Connection>& connections, GNELane* targetLane);

    /* @brief builds internal lanes for the given node */
    void buildIinternalLanes(NBNode* node);

};


#endif

/****************************************************************************/

