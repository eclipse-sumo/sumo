/****************************************************************************/
/// @file    GNEUndoList.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// FXUndoList is pretty dandy but some features are missing:
//   - we cannot find out wether we have currently begun an undo-group and
//     thus abort() is hard to use.
//   - onUpd-methods do not disable undo/redo while in an undo-group
//
// GNEUndoList inherits from FXUndoList and patches some methods. these are
// prefixed with p_
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
#ifndef GNEUndoList_h
#define GNEUndoList_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <stack>
#include <fx.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GNEChange_Attribute;
class GNEApplicationWindow;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class  GNEUndoList
 */
class GNEUndoList : public FXUndoList {
    /// @brief FOX declaration
    FXDECLARE_ABSTRACT(GNEUndoList)

public:
    /// @brief constructor
    /// @note be aware that "parent" may be not fully initialized when stored here, so don't call any methods on it.
    GNEUndoList(GNEApplicationWindow* parent);

    /**@brief Begin undo command sub-group. This begins a new group of commands that
     * are treated as a single command.  Must eventually be followed by a
     * matching end() after recording the sub-commands. The new sub-group
     * will be appended to its parent group's undo list when end() is called.
     */
    void p_begin(const std::string& description);

    /**@brief End undo command sub-group.  If the sub-group is still empty, it will
     * be deleted; otherwise, the sub-group will be added as a new command
     * into parent group.
     * A matching begin() must have been called previously.
     */
    void p_end();

    /// @brief clears the undo list (implies abort)
    void p_clear();

    /// @brief reverts and discards ALL active command groups
    void p_abort();

    /// @brief undo the last command group
    void undo();

    /// @brief redo the last command group
    void redo();

    /// @brief special method, avoid empty changes, always execute
    void p_add(GNEChange_Attribute* cmd);

    /// @name FOX-callbacks
    /// @{
    /// @brief event after Undo
    long p_onUpdUndo(FXObject*, FXSelector, void*);

    /// @brief event after Redo
    long p_onUpdRedo(FXObject*, FXSelector, void*);
    /// @}

    /// @brief Check if undoList has command group
    bool hasCommandGroup() const;

private:
    /// @brief class CommandGroup
    class CommandGroup : public FXCommandGroup {
    public:
        /// @brief Constructor
        CommandGroup(std::string description);

        /// @brief get description
        const std::string& getDescription();

        /// @brief get undo Name
        FXString undoName() const;

        /// @brief get redo name
        FXString redoName() const;

    private:
        /// @brief description of command
        const std::string myDescription;
    };

    // the stack of currently active command groups
    std::stack<CommandGroup*> myCommandGroups;

    // the parent application for this undolist
    GNEApplicationWindow* const myParent;
};


#endif

/****************************************************************************/

