/****************************************************************************/
/// @file    GNETLSEditorFrame.h
/// @author  Jakob Erdmann
/// @date    May 2011
/// @version $Id$
///
// The Widget for modifying traffic lights
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
#ifndef GNETLSEditorFrame_h
#define GNETLSEditorFrame_h

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
class NBTrafficLightDefinition;
class NBLoadedSUMOTLDef;
class GNEEdge;
class GNELane;
class GNEInternalLane;
class GNEJunction;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNETLSEditorFrame
 * The Widget for modifying selections of network-elements
 */
class GNETLSEditorFrame : public GNEFrame {
    /// @brief FOX-declaration
    FXDECLARE(GNETLSEditorFrame)

public:
    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNETLSEditorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNETLSEditorFrame();

    /// @brief edits the traffic light for the given junction
    /// @param[in] junction The junction of which the traffic light shall be edited
    void editJunction(GNEJunction* junction);

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user presses the OK-Button
    /// @note saves any modifications
    long onCmdOK(FXObject*, FXSelector, void*);

    /// @brief Called when the user presses the Cancel-button
    /// @note discards any modifications
    long onCmdCancel(FXObject*, FXSelector, void*);

    /// @brief Called when the user presses the button Toogle
    long onCmdToggle(FXObject*, FXSelector, void*);

    /// @brief Called when the user presses the button Guess
    long onCmdGuess(FXObject*, FXSelector, void*);

    /// @brief Called when the user creates a TLS
    long onCmdDefCreate(FXObject*, FXSelector, void*);

    /// @brief Called when the user deletes a TLS
    long onCmdDefDelete(FXObject*, FXSelector, void*);

    /// @brief Called when the user changes the offset of a TLS
    long onCmdDefOffset(FXObject*, FXSelector, void*);

    /// @brief Called when the user switchs a TLS
    long onCmdDefSwitch(FXObject*, FXSelector, void*);

    /// @brief Called when the user renames a TLS
    long onCmdDefRename(FXObject*, FXSelector, void*);

    /// @brief Called when the user sub-renames a TLS
    long onCmdDefSubRename(FXObject*, FXSelector, void*);

    /// @brief Called when the user adds a OFF
    long onCmdDefAddOff(FXObject*, FXSelector, void*);

    /// @brief Called when the user switchs a Phase
    long onCmdPhaseSwitch(FXObject*, FXSelector, void*);

    /// @brief Called when the user creates a Phase
    long onCmdPhaseCreate(FXObject*, FXSelector, void*);

    /// @brief Called when the user deletes a Phase
    long onCmdPhaseDelete(FXObject*, FXSelector, void*);

    /// @brief Called when the user edits a Phase
    long onCmdPhaseEdit(FXObject*, FXSelector, void*);

    /// @brief Called when the user makes RILSA
    long onCmdMakeRILSAConforming(FXObject*, FXSelector, void*);

    /// @brief Called when occurs an update of switch definition
    long onUpdDefSwitch(FXObject*, FXSelector, void*);

    /// @brief Called when occurs an update of needs definition
    long onUpdNeedsDef(FXObject*, FXSelector, void*);

    /// @brief Called when occurs an update of needs definition an dphase
    long onUpdNeedsDefAndPhase(FXObject*, FXSelector, void*);

    /// @brief Called when occurs an update of create definition
    long onUpdDefCreate(FXObject*, FXSelector, void*);

    /// @brief Called when occurs an update of modified
    long onUpdModified(FXObject*, FXSelector, void*);
    /// @}

    /// @brief update phase definition for the current traffic light and phase
    void handleChange(GNEInternalLane* lane);

    /// @brief update phase definition for the current traffic light and phase
    void handleMultiChange(GNELane* lane, FXObject* obj, FXSelector sel, void* data);

    /// @brief whether the given edge is controlled by the currently edited tlDef
    bool controlsEdge(GNEEdge& edge) const;

protected:
    /// @brief FOX needs this
    GNETLSEditorFrame() {}

private:
    /// @brief font for the phase table
    FXFont* myTableFont;

    /// @brief groupbox for current junction
    FXGroupBox* myGroupBoxJunction;

    /// @brief the label that shows the current editing state
    FXLabel* myDescription;

    /// @brief groupbox for TLS Definition buttons
    FXGroupBox* myGroupBoxTLSDef;

    /// @brief button for create new Traffic light program
    FXButton* myNewTLProgram;

    /// @brief button for delete traffic light program
    FXButton* myDeleteTLProgram;

    /// @brief groupbox for TLS attributes
    FXGroupBox* myGroupBoxAttributes;

    /// @brief matrix for attributes
    FXMatrix* myAttributeMatrix;

    /// @brief name label
    FXLabel* myNameLabel;

    /// @brief name text field
    FXTextField* myNameTextField;

    /// @brief program label
    FXLabel* myProgramLabel;

    /// @brief the comboBox for selecting the tl-definition to edit
    FXComboBox* myProgramComboBox;

    /// @brief offset label
    FXLabel* myOffsetLabel;

    /// @brief the control for modifying offset
    FXTextField* myOffset;

    /// @brief groupbox for phases
    FXGroupBox* myGroupBoxPhases;

    /// @brief table for selecting and rearranging phases and for changing duration
    FXTable* myPhaseTable;

    /// @brief label with the cycle duration
    FXLabel* myCycleDuration;

    /// @brief insert new phase button
    FXButton* myInsertDuplicateButton;

    /// @brief delete phase button
    FXButton* myDeleteSelectedPhaseButton;

    /// @brief groupbox for buttons modifications
    FXGroupBox* myGroupBoxModifications;

    /// @brief button for cancel modifications
    FXButton* myDiscardModificationsButtons;

    /// @brief button for save modifications
    FXButton* mySaveModificationsButtons;

    /// @brief the junction of the tls is being modified
    GNEJunction* myCurrentJunction;

    /// @brief whether the current tls was modified
    bool myHaveModifications;

    /// @brief the internal lanes belonging the the current junction indexed by their tl-index
    typedef std::map<int, std::vector<GNEInternalLane*> > TLIndexMap;
    TLIndexMap myInternalLanes;

    /// @brief the list of Definitions for the current junction
    std::vector<NBTrafficLightDefinition*> myDefinitions;

    /// @brief the traffic light definition being edited
    NBLoadedSUMOTLDef* myEditedDef;

    /// @brief index of the phase being shown
    int myPhaseIndex;

    /// @brief update descrition
    void updateDescription() const;

    /// @brief cleans up previous lanes
    void cleanup();

    /// @brief builds internal lanes for the given tlDef
    void buildIinternalLanes(NBTrafficLightDefinition* tlDef);

    /// @brief initializes the definitions and corresponding listbox
    void initDefinitions();

    /**@brief initialies the phase table
     * @param[in] index The index to select
     */
    void initPhaseTable(int index = 0);

    /// @brief the phase of the current traffic light
    const std::vector<NBTrafficLightLogic::PhaseDefinition>& getPhases();

    /// @brief recomputes cycle duration and updates label
    void updateCycleDuration();

    /// @brief converts to SUMOTime
    static SUMOTime getSUMOTime(const FXString& string);
};


#endif

/****************************************************************************/

