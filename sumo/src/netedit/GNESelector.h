/****************************************************************************/
/// @file    GNESelector.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// The Widget for modifying selections of network-elements
// (some elements adapted from GUIDialog_GLChosenEditor)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNESelector_h
#define GNESelector_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <GL/gl.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/gui/div/GUISelectedStorage.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEViewNet;
class GNEAttributeCarrier;
class GNEUndoList;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNESelector
 * The Widget for modifying selections of network-elements
 */
class GNESelector : public FXScrollWindow, public GUISelectedStorage::UpdateTarget {
    // FOX-declarations
    FXDECLARE(GNESelector)

public:

    enum SetOperation {
        SET_ADD      = 1,
        SET_SUB      = 2,
        SET_RESTRICT = 3,
        SET_REPLACE  = 4,
        SET_DEFAULT  = 5  // use mySetOperation instead of override
    };

    /** @brief Constructor
     * @param[in] parent The parent window
     */
    GNESelector(FXComposite* parent, GNEViewNet* updateTarget, GNEUndoList* undoList);


    /// @brief Destructor
    ~GNESelector();


    FXFont* getHeaderFont() {
        return myHeaderFont;
    }

    /// @name FOX-callbacks
    /// @{
    /** @brief Called when the user presses the Load-button
     *
     * Opens a file dialog and forces the parent to load the list of selected
     *  objects when a file was chosen. Rebuilds the list, then, and redraws
     *  itself.
     */
    long onCmdLoad(FXObject*, FXSelector, void*);

    /** @brief Called when the user presses the Save-button
     *
     * Opens a file dialog and forces the selection container to save the list
     *  of selected objects when a file was chosen.
     *
     * If the saveing failed, a message window is shown.
     */
    long onCmdSave(FXObject*, FXSelector, void*);

    /** @brief Called when the user presses the Clear-button
     *
     * Clear the internal list and calls GUISelectedStorage::clear.
     * Repaints itself
     */
    long onCmdClear(FXObject*, FXSelector, void*);

    /** @brief Called when the user presses the Invert-button
     * invert the selection
     * Repaints itself
     */
    long onCmdInvert(FXObject*, FXSelector, void*);

    /** @brief Called when the user selectes a tag in the match box
     * updates the attr listbox
     * Repaints itself
     */
    long onCmdSelMBTag(FXObject*, FXSelector, void*);

    /** @brief Called when the user enters a new selection expression
     * validates expression and modifies current selection
     */
    long onCmdSelMBString(FXObject*, FXSelector, void*);

    /** @brief Called when the user clicks the help button
     * pop up help window
     */
    long onCmdHelp(FXObject*, FXSelector, void*);

    /** @brief Called when the user changes visual scaling
     */
    long onCmdScaleSelection(FXObject*, FXSelector, void*);

    /// @}

    void show();

    void hide();

    /* apply list of ids to the current selection according to SetOperation,
     * if setop==SET_DEFAULT than the currently set mode (mySetOperation) is used */
    void handleIDs(std::vector<GUIGlID> ids, bool selectEdges, SetOperation setop = SET_DEFAULT);

    /* @brief called if currently registered for updates for changes of global selection */
    void selectionUpdated();

protected:
    /// @brief FOX needs this
    GNESelector() {}

private:
    /// @brief the panel to hold all member widgets
    FXVerticalFrame* myContentFrame;

    /// @brief Font for the widget
    FXFont* myHeaderFont;

    /// @brief the label for selection statistics
    FXLabel* myStats;

    /* @brief the window to inform when selection changes
     * we also need this to query for existing elements when inverting selection
     */
    GNEViewNet* myUpdateTarget;

    /// @brief how to modify selection
    FXuint mySetOperation;
    FXDataTarget mySetOperationTarget;

    /// @brief state variables for the match box
    FXListBox* myMatchTagBox;
    FXListBox* myMatchAttrBox;
    FXTextField* myMatchString;

    FXRealSpinDial* mySelectionScaling;

    static const int WIDTH;

    GNEUndoList* myUndoList;
private:

    std::string getStats() const;

    /** @brief return  objects of the given type with matching attrs
     * @param[in] compOp One of {<,>,=} for matching against val or '@' for
     *                   matching against expr
     */
    std::vector<GUIGlID> getMatches(
        SumoXMLTag tag, SumoXMLAttr attr, char compOp, SUMOReal val, const std::string& expr);

};


#endif

/****************************************************************************/

