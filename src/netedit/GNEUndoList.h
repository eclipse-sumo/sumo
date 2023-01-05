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
/// @file    GNEUndoList.h
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2011
///
/****************************************************************************/
#pragma once
#include <config.h>

#include <stack>
#include <string>

#include <netedit/changes/GNEChangeGroup.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GNEChange;
class GNEChange_Attribute;
class GNEApplicationWindow;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class  GNEUndoList
 */
class GNEUndoList : public GNEChangeGroup {
    /// @brief FOX declaration
    FXDECLARE_ABSTRACT(GNEUndoList)

public:
    /// @brief iterator
    class Iterator {

    public:
        /// @brief destructor
        ~Iterator();

        /// @brief check if iterator is at the end
        bool end() const;

        /// @brief get index
        int getIndex() const;

        /// @brief get description
        const std::string getDescription() const;

        /// @brief get timeStamp
        const std::string getTimeStamp() const;

        /// @brief get icon
        FXIcon* getIcon() const;

        /// @brief increment operator
        Iterator& operator++(int);

    protected:
        /// @brief constructor for GNEUndoList
        Iterator(GNEChange* change);

    private:
        /// @brief default constructor
        Iterator();

        /// @brief current change
        GNEChange* myCurrentChange;

        /// @brief counter
        int myIndex;
    };

    /// @brief undo iterator
    class UndoIterator : public Iterator {

    public:
        /// @brief constructor for GNEUndoList
        UndoIterator(const GNEUndoList* undoList);
    };

    /// @brief redo iterator
    class RedoIterator : public Iterator {

    public:
        /// @brief constructor for GNEUndoList
        RedoIterator(const GNEUndoList* undoList);
    };

    /// @brief constructor
    GNEUndoList(GNEApplicationWindow* parent);

    /// @brief destructor
    ~GNEUndoList();

    /// @brief undo the last command group
    void undo();

    /// @brief redo the last command group
    void redo();

    /**@brief Return name of the first undo command available; if no
     * undo command available this will return the empty string.
     */
    std::string undoName() const;

    /**@brief Return name of the first redo command available; if no
     * Redo command available this will return the empty string.
     */
    std::string redoName() const;

    /**@brief Begin undo command sub-group with current supermode.
     * This begins a new group of commands that
     * are treated as a single command.  Must eventually be followed by a
     * matching end() after recording the sub-commands. The new sub-group
     * will be appended to its parent group's undo list when end() is called.
     */
    void begin(GUIIcon icon, const std::string& description);

    /**@brief Begin undo command sub-group specifying supermode.
     * This begins a new group of commands that
     * are treated as a single command.  Must eventually be followed by a
     * matching end() after recording the sub-commands. The new sub-group
     * will be appended to its parent group's undo list when end() is called.
     */
    void begin(Supermode supermode, GUIIcon icon, const std::string& description);

    /**@brief End undo command sub-group.  If the sub-group is still empty, it will
     * be deleted; otherwise, the sub-group will be added as a new command
     * into parent group.
     * @note A matching begin() must have been called previously.
     */
    void end();

    /**@brief Add new command, executing it if desired. The new command will be merged
     * with the previous command if merge is TRUE and we're not at a marked position
     * and the commands are mergeable.  Otherwise the new command will be appended
     * after the last undo command in the currently active undo group.
     * If the new command is successfully merged, it will be deleted.  Furthermore,
     * all redo commands will be deleted since it is no longer possible to redo
     * from this point.
     */
    void add(GNEChange* command, bool doit = false, bool merge = true);

    /// @brief special method for change attributes, avoid empty changes, always execute
    void changeAttribute(GNEChange_Attribute* change);

    /* @brief clears the undo list (implies abort)
     * All undo and redo information will be destroyed.
     */
    void clear();

    /// @brief reverts and discards ALL active chained change groups
    void abortAllChangeGroups();

    /// @brief reverts last active chained change group
    void abortLastChangeGroup();

    /// @brief get size of current CommandGroup
    int currentCommandGroupSize() const;

    /// @brief get undo supermode
    Supermode getUndoSupermode() const;

    /// @brief get redo supermode
    Supermode getRedoSupermode() const;

    /// @brief Check if undoList has command group
    bool hasCommandGroup() const;

    /**@brief Return TRUE if currently inside undo or redo operation; this
     * is useful to avoid generating another undo command while inside
     * an undo operation.
     */
    bool busy() const;

    /// @name FOX-callbacks
    /// @{
    /// @brief undo change
    long onCmdUndo(FXObject*, FXSelector, void*);

    /// @brief event after Undo
    long onUpdUndo(FXObject*, FXSelector, void*);

    /// @brief redo change
    long onCmdRedo(FXObject*, FXSelector, void*);

    /// @brief event after Redo
    long onUpdRedo(FXObject*, FXSelector, void*);
    /// @}

protected:
    /**@brief Cut the redo list.
     * This is automatically invoked when a new undo command is added.
     */
    void cut();

    /** @brief Abort the current command sub-group being compiled.  All commands
     * already added to the sub-groups undo list will be discarded.
     * Intermediate command groups will be left intact.
     */
    void abortCurrentSubGroup();

    ///@brief Can we undo more commands
    bool canUndo() const;

    ///@brief Can we redo more commands
    bool canRedo() const;

private:
    /// @brief  Currently busy with undo or redo
    bool myWorking;

    // @brief the stack of currently active change groups
    std::stack<GNEChangeGroup*> myChangeGroups;

    // @brief the parent GNEApplicationWindow for this undolist
    GNEApplicationWindow* const myGNEApplicationWindowParent;
};
