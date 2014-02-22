/****************************************************************************/
/// @file    GUISelectedStorage.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Jun 2004
/// @version $Id$
///
// Storage for "selected" objects
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUISelectedStorage_h
#define GUISelectedStorage_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <set>
#include <string>
#include <map>
#include <fstream>
#include <fx.h>
#include <utils/common/UtilExceptions.h>
#include <utils/gui/globjects/GUIGlObject.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUISelectedStorage
 * @brief Storage for "selected" objects
 *
 * Object selection is done by storing the "gl-ids" of selectable objects
 *  (GUIGlObjects) within internal containers of this class. Each id is stored
 *  twice - in a global container and within one of the type-aware containers.
 *
 * This class allows adding and removing objects (their ids) from the lists
 *  of selected objects, saving the lists into a file, and obtaining the lists
 *  of selected objects (both all and type-aware).
 *
 * Most of the adding/removing methods do not require a GUIGlObjectType as
 *  parameter, but an integer. This is done to perform the action on objects with
 *  a yet unknown type - in this case, the type is obtained internally.
 *
 * Besides this, the class forces an active view of selected items
 *  to refresh its contents if an item is added/removed. For this, an
 *  FXWindow has to make itself visible to GUISelectedStorage.
 *
 * @see GUIGlObject
 * @see GUIGlObjectType
 * @see GUIDialog_GLChosenEditor
 */
class GUISelectedStorage {

public:
    class UpdateTarget {
    public:
        virtual ~UpdateTarget() {};
        virtual void selectionUpdated() = 0;
    };

public:
    /// @brief Constructor
    GUISelectedStorage();


    /// @brief Destructor
    ~GUISelectedStorage();


    /** @brief Returns the information whether the object with the given type and id is selected
     *
     * If the type is ==-1, it is determined, first. If it could not be obtained,
     *  or if the type is not covered by any selection container, a ProcessError is thrown.
     *
     * Otherwise, the container holding objects of the determined type is
     *  asked whether the given id is stored using SingleTypeSelections::isSelected().
     *
     * @param[in] type The type of the object (GUIGlObjectType or -1)
     * @param[in] id The id of the object
     * @return Whether the object is selected
     * @exception ProcessError If the object is not known or the type is not covered by a sub-container
     * @see GUIGlObject
     * @see GUIGlObjectType
     * @see SingleTypeSelections::isSelected
     */
    bool isSelected(GUIGlObjectType type, GUIGlID id);


    /** @brief Adds the object with the given id
     *
     * The id of the object is added to the sub-container that is
     *  responsible for objects of the determined type using SingleTypeSelections::select
     *  and to the global list of chosen items if it is not already there.
     *
     * The optionally listening window is informed about the change.
     *
     * @param[in] id The id of the object
     * @exception ProcessError If the object is not known or the type is not covered by a sub-container
     * @see GUIGlObject
     * @see GUIGlObjectType
     * @see SingleTypeSelections::select
     * @see GUIDialog_GLChosenEditor
     */
    void select(GUIGlID id, bool update = true);


    /** @brief Deselects the object with the given id
     *
     * The id of the object is removed from the sub-container that is
     *  responsible for objects of the determined type using SingleTypeSelections::deselect
     *  and from the global list of chosen items if it is there.
     *
     * The optionally listening UpdateTarget is informed about the change.
     *
     * @param[in] id The id of the object
     * @exception ProcessError If the object is not known or the type is not covered by a sub-container
     * @see GUIGlObject
     * @see GUIGlObjectType
     * @see SingleTypeSelections::deselect
     * @see GUIDialog_GLChosenEditor
     */
    void deselect(GUIGlID id);


    /** @brief Toggles selection of an object
     *
     * If the object can not be obtained a ProcessError is thrown.
     *
     * Otherwise, it is determined whether the object is already selected or not.
     *  If so, it is deselected by calling "deselect", otherwise it is selected
     *  via "select".
     *
     * @param[in] id The id of the object
     * @exception ProcessError If the object is not known or the type is not covered by a sub-container
     * @see GUIGlObject
     * @see deselect
     * @see select
     */
    void toggleSelection(GUIGlID id);


    /** @brief Returns the list of ids of all selected objects
     *
     *
     * @return The set of all selected objects (ids)
     */
    const std::set<GUIGlID>& getSelected() const;


    /**  @brief Returns the set of ids of all selected objects' of a certain type
     *
     * @param[in] type The type of the object
     * @return A set containing the ids of all selected objects of the given type
     * @see SingleTypeSelections::getSelected
     */
    const std::set<GUIGlID>& getSelected(GUIGlObjectType type);


    /** @brief Clears the list of selected objects
     *
     * Clears the global container and all sub-containers via SingleTypeSelections::clear.
     *
     * The optionally listening UpdateTarget is informed about the change.
     */
    void clear();


    /** @brief Loads a selection list (optionally with restricted type)
     *
     * @param[in] filename The name of the file to load the list of selected objects from
     * @param[in] type The type of the objects to load if changed from default
     * @return error messages if errors occured or the empty string
     */
    std::string load(const std::string& filename, GUIGlObjectType type = GLO_MAX);


    /** @brief Loads a selection list (optionally with restricted type) and
     * returns the ids of all active objects
     *
     * @param[in] filename The name of the file to load the list of selected objects from
     * @param[out] msg Any error messages while loading or the empty string
     * @param[in] type The type of the objects to load if changed from default
     * @param[in] maxErrors The maximum Number of errors to return
     * @return the set of loaded ids
     */
    std::set<GUIGlID> loadIDs(const std::string& filename, std::string& msgOut, GUIGlObjectType type = GLO_MAX, int maxErrors = 16);


    /** @brief Saves a selection list
     *
     * @param[in] type The type of the objects to save
     * @param[in] filename The name of the file to save the list of selected objects into
     */
    void save(GUIGlObjectType type, const std::string& filename);

    /** @brief Saves the combined selection of all types
     *
     * @param[in] filename The name of the file to save the list of selected objects into
     */
    void save(const std::string& filename) const;


    /** @brief Adds a dialog to be updated
     * @param[in] updateTarget the callback for selection changes
     */
    void add2Update(UpdateTarget* updateTarget);


    /** @brief Removes the dialog to be updated
     */
    void remove2Update();


    /**
     * @class SingleTypeSelections
     * @brief A container for ids of selected objects of a certain type.
     */
    class SingleTypeSelections {
    public:
        /// @brief Constructor
        SingleTypeSelections();

        /// @brief Destructor
        ~SingleTypeSelections();


        /** @brief Returns the information whether the object with the given id is qithin the selection
         * @param[in] id The id of the object
         * @return Whether the object is selected
         */
        bool isSelected(GUIGlID id);


        /** @brief Adds the object with the given id to the list of selected objects
         * @param[in] id The id of the object
         */
        void select(GUIGlID id);


        /** @brief Deselects the object with the given id from the list of selected objects
         * @param[in] id The id of the object
         */
        void deselect(GUIGlID id);


        /// @brief Clears the list of selected objects
        void clear();


        /** @brief Saves the list of selected objects to a file named as given
         * @param[in] filename The name of the file to save the list into
         */
        void save(const std::string& filename);


        /** @brief Returns the list of selected ids
         * @return A list containing the ids of all selected objects
         */
        const std::set<GUIGlID>& getSelected() const;

    private:
        /// @brief The list of selected ids
        std::set<GUIGlID> mySelected;

    };
    friend class SingleTypeSelections;


private:
    std::map<GUIGlObjectType, SingleTypeSelections> mySelections;

    /// @brief List of selected objects
    std::set<GUIGlID> myAllSelected;

    /// @brief The dialog to be updated
    UpdateTarget* myUpdateTarget;

    /* @brief load items into the given set, optionally restricting to type
     */
    std::string load(GUIGlObjectType type, const std::string& filename, bool restrictType, std::set<GUIGlID>& into);


    /// @brief saves items from the given set
    static void save(const std::string& filename, const std::set<GUIGlID>& ids);

};


#endif

/****************************************************************************/

