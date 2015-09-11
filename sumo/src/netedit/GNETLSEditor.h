/****************************************************************************/
/// @file    GNETLSEditor.h
/// @author  Jakob Erdmann
/// @date    May 2011
/// @version $Id$
///
// The Widget for modifying traffic lights
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
#ifndef GNETLSEditor_h
#define GNETLSEditor_h


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
#include <utils/gui/div/GUISelectedStorage.h>
#include <netbuild/NBTrafficLightLogic.h>

// ===========================================================================
// class declarations
// ===========================================================================
class NBTrafficLightDefinition;
class NBLoadedSUMOTLDef;
class GNEViewNet;
class GNEUndoList;
class GNEInternalLane;
class GNEJunction;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNETLSEditor
 * The Widget for modifying selections of network-elements
 */
class GNETLSEditor : public FXScrollWindow {
    // FOX-declarations
    FXDECLARE(GNETLSEditor)

public:
    /** @brief Constructor
     * @param[in] parent The parent window
     */
    GNETLSEditor(FXComposite* parent, GNEViewNet* updateTarget, GNEUndoList* undoList);


    /// @brief Destructor
    ~GNETLSEditor();


    FXFont* getHeaderFont() {
        return myHeaderFont;
    }

    /** @brief edits the traffic light for the given junction
     * @param[in] junction The junction of which the traffic light shall be edited
     */
    void editJunction(GNEJunction* junction);

    /// @name FOX-callbacks
    /// @{
    /** @brief Called when the user presses the OK-Button
     * saves any modifications
     */
    long onCmdOK(FXObject*, FXSelector, void*);

    /** @brief Called when the user presses the Cancel-button
     * discards any modifications
     */
    long onCmdCancel(FXObject*, FXSelector, void*);

    /** @brief Called when the user presses the Corresponding-button */
    long onCmdToggle(FXObject*, FXSelector, void*);
    long onCmdGuess(FXObject*, FXSelector, void*);

    long onCmdDefCreate(FXObject*, FXSelector, void*);
    long onCmdDefDelete(FXObject*, FXSelector, void*);
    long onCmdDefOffset(FXObject*, FXSelector, void*);
    long onCmdDefSwitch(FXObject*, FXSelector, void*);
    long onCmdDefRename(FXObject*, FXSelector, void*);
    long onCmdDefSubRename(FXObject*, FXSelector, void*);
    long onCmdDefAddOff(FXObject*, FXSelector, void*);

    long onCmdPhaseSwitch(FXObject*, FXSelector, void*);
    long onCmdPhaseCreate(FXObject*, FXSelector, void*);
    long onCmdPhaseDelete(FXObject*, FXSelector, void*);
    long onCmdPhaseEdit(FXObject*, FXSelector, void*);

    long onCmdMakeRILSAConforming(FXObject*, FXSelector, void*);

    long onUpdDefSwitch(FXObject*, FXSelector, void*);
    long onUpdNeedsDef(FXObject*, FXSelector, void*);
    long onUpdDefCreate(FXObject*, FXSelector, void*);
    long onUpdModified(FXObject*, FXSelector, void*);
    /// @}

    /// @brief update phase definition for the current traffic light and phase
    void handleChange(GNEInternalLane* lane);

protected:
    /// @brief FOX needs this
    GNETLSEditor() {}

private:
    /// @brief the panel to hold all member widgets
    FXVerticalFrame* myContentFrame;

    /// @brief Font for the widget
    FXFont* myHeaderFont;

    /// @brief font for the phase table
    FXFont* myTableFont;

    /// @brief the label that shows the current editing state
    FXLabel* myDescription;

    /// @brief the junction of the tls is being modified
    GNEJunction* myCurrentJunction;

    /// @brief the undolist with wich to register changes
    GNEUndoList* myUndoList;

    /* @brief the window to inform when the tls is modfied */
    GNEViewNet* myUpdateTarget;

    /// @brief whether the current tls was modified
    bool myHaveModifications;

    /// @brief the internal lanes belonging the the current junction indexed by their tl-index
    typedef std::map<int, std::vector<GNEInternalLane*> > TLIndexMap;
    TLIndexMap myInternalLanes;

    /// @brief the list of Definitions for the current junction
    std::vector<NBTrafficLightDefinition*> myDefinitions;

    /// @brief the traffic light definition being edited
    NBLoadedSUMOTLDef* myEditedDef;

    /// @brief the listbox for selecting the tl-definition to edit
    FXListBox* myDefBox;

    /// @brief the control for modifying offset
    FXTextField* myOffset;

    FXLabel* myCycleDuration;

    /// @brief table for selecting and rearranging phases and for changing duration
    FXTable* myPhaseTable;

    /// @brief index of the phase being shown
    unsigned int myPhaseIndex;

    /// @brief the width of the widget
    static const int WIDTH;

private:
    void updateDescription() const;

    /// @brief cleans up previous lanes
    void cleanup();

    /* @brief builds internal lanes for the given tlDef */
    void buildIinternalLanes(NBTrafficLightDefinition* tlDef);

    /* @brief initializes the definitions and corresponding listbox */
    void initDefinitions();

    /* @brief initialies the phase table
     * @param[in] index The index to select
     */
    void initPhaseTable(unsigned int index = 0);

    /// @brief the phase of the current traffic light
    const std::vector<NBTrafficLightLogic::PhaseDefinition>& getPhases();

    /// @brief recomputes cycle duration and updates label
    void updateCycleDuration();

    /// @brief converts to SUMOTime
    static SUMOTime getSUMOTime(const FXString& string);

};


#endif

/****************************************************************************/

