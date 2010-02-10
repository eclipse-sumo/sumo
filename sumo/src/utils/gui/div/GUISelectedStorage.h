/****************************************************************************/
/// @file    GUISelectedStorage.h
/// @author  Daniel Krajzewicz
/// @date    Jun 2004
/// @version $Id$
///
// Storage for "selected" objects
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include <vector>
#include <string>
#include <fstream>
#include <utils/common/UtilExceptions.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIDialog_GLChosenEditor;
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
 * Besides this, the class forces an active view of selected items (GUIDialog_GLChosenEditor)
 *  to refresh its contents if an item is added/removed. For this, a
 *  GUIDialog_GLChosenEditor has to make itself visible to GUISelectedStorage.
 *
 * @see GUIGlObject
 * @see GUIGlObjectType
 * @see GUIDialog_GLChosenEditor
 */
class GUISelectedStorage {
public:
    /// @brief Constructor
    GUISelectedStorage() throw();


    /// @brief Destructor
    ~GUISelectedStorage() throw();


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
    bool isSelected(int type, GLuint id) throw(ProcessError);


    /** @brief Adds the object with the given type and id to the list of selected objects
     *
     * If the type is ==-1, it is determined, first. If it could not be obtained,
     *  or if the type is not covered by any selection container, a ProcessError is thrown.
     *
     * Otherwise, the id of the object is added to the sub-container that is
     *  responsible for objects of the determined type using SingleTypeSelections::select
     *  and to the global list of chosen items if it is not already there.
     *
     * The optionally listening GUIDialog_GLChosenEditor is informed about the change.
     *
     * @param[in] type The type of the object (GUIGlObjectType or -1)
     * @param[in] id The id of the object
     * @exception ProcessError If the object is not known or the type is not covered by a sub-container
     * @see GUIGlObject
     * @see GUIGlObjectType
     * @see SingleTypeSelections::select
     * @see GUIDialog_GLChosenEditor
     */
    void select(int type, GLuint id, bool update=true) throw(ProcessError);


    /** @brief Deselects the object with the given type and id
     *
     * If the type is ==-1, it is determined, first. If it could not be obtained,
     *  or if the type is not covered by any selection container, a ProcessError is thrown.
     *
     * Otherwise, the id of the object is removed from the sub-container that is
     *  responsible for objects of the determined type using SingleTypeSelections::deselect
     *  and from the global list of chosen items if it is there.
     *
     * The optionally listening GUIDialog_GLChosenEditor is informed about the change.
     *
     * @param[in] type The type of the object (GUIGlObjectType or -1)
     * @param[in] id The id of the object
     * @exception ProcessError If the object is not known or the type is not covered by a sub-container
     * @see GUIGlObject
     * @see GUIGlObjectType
     * @see SingleTypeSelections::deselect
     * @see GUIDialog_GLChosenEditor
     */
    void deselect(int type, GLuint id) throw(ProcessError);


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
    void toggleSelection(GLuint id) throw(ProcessError);


    /** @brief Returns the list of ids of all selected objects
     *
     * "mySelected" is returned
     *
     * @return A list containing the ids of all selected objects
     */
    const std::vector<GLuint> &getSelected() const throw();


    /**  @brief Returns the list of ids of all selected objects' of a certain type
     *
     * The list of selected items is returned from the appropriate sub-container
     *  using SingleTypeSelections::getSelected.
     *
     * The method throws a ProcessError if the type does not match a sub-container.
     *
     * @param[in] type The type of the object
     * @return A list containing the ids of all selected objects of the given type
     * @exception ProcessError If the type is not covered by a sub-container
     * @see SingleTypeSelections::getSelected
     */
    const std::vector<GLuint> &getSelected(GUIGlObjectType type) const throw(ProcessError);


    /** @brief Clears the list of selected objects
     *
     * Clears the global container and all sub-containers via SingleTypeSelections::clear.
     *
     * The optionally listening GUIDialog_GLChosenEditor is informed about the change.
     */
    void clear() throw();


    /** @brief Loads a selection list
     *
     * @param[in] type The type of the objects to load
     * @param[in] filename The name of the file to load the list of selected objects from
     * @exception IOError recheck!!!
     * @todo Recheck: is this used(?)
     * @todo Recheck usage of IOError
     */
    void load(int type, const std::string &filename) throw(IOError);


    /** @brief Saves a selection list
     *
     * @param[in] type The type of the objects to save
     * @param[in] filename The name of the file to save the list of selected objects into
     * @exception IOError recheck!!!
     * @todo Recheck: is this used(?)
     * @todo Recheck usage of IOError
     */
    void save(int type, const std::string &filename) throw(IOError);


    /** @brief Adds a selected-dialog to be updated
     *
     * The editor pointer is stored in my2Update.
     *
     * @param[in] ed The selected objects editor
     */
    void add2Update(GUIDialog_GLChosenEditor *ed) throw();


    /** @brief Removes a selected-dialog to be updated
     *
     * my2Update is set to 0.
     *
     * @param[in] ed The selected objects editor
     */
    void remove2Update(GUIDialog_GLChosenEditor *ed) throw();



    /**
     * @class SingleTypeSelections
     * @brief A container for ids of selected objects of a certain type.
     */
    class SingleTypeSelections {
    public:
        /// @brief Constructor
        SingleTypeSelections() throw();

        /// @brief Destructor
        ~SingleTypeSelections() throw();


        /** @brief Returns the information whether the object with the given id is qithin the selection
         * @param[in] id The id of the object
         * @return Whether the object is selected
         */
        bool isSelected(GLuint id) throw();


        /** @brief Adds the object with the given id to the list of selected objects
         * @param[in] id The id of the object
         */
        void select(GLuint id) throw();


        /** @brief Deselects the object with the given id from the list of selected objects
         * @param[in] id The id of the object
         */
        void deselect(GLuint id) throw();


        /// @brief Clears the list of selected objects
        void clear() throw();


        /** @brief Loads the list of selected objects to a file named as given
         * @param[in] filename The name of the file to read the list from
         * @exception IOError recheck!!!
         * @todo Recheck: is this used(?)
         * @todo Recheck usage of IOError
         */
        void load(const std::string &filename) throw(IOError);


        /** @brief Saves the list of selected objects to a file named as given
         * @param[in] filename The name of the file to save the list into
         * @exception IOError recheck!!!
         * @todo Recheck: is this used(?)
         * @todo Recheck usage of IOError
         */
        void save(const std::string &filename) throw(IOError);


        /** @brief Returns the list of selected ids
         * @return A list containing the ids of all selected objects
         */
        const std::vector<GLuint> &getSelected() const throw();

    private:
        /// @brief The list of selected ids
        std::vector<GLuint> mySelected;

    };


private:
    /// @brief List of selected vehicles
    SingleTypeSelections mySelectedVehicles;

    /// @brief List of selected lanes
    SingleTypeSelections mySelectedLanes;

    /// @brief List of selected edges
    SingleTypeSelections mySelectedEdges;

    /// @brief List of selected junctions
    SingleTypeSelections mySelectedJunctions;

    /// @brief List of selected emitters
    SingleTypeSelections mySelectedEmitters;

    /// @brief List of selected detectors
    SingleTypeSelections mySelectedDetectors;

    /// @brief List of selected tl-logics
    SingleTypeSelections mySelectedTLLogics;

    /// @brief List of selected triggers
    SingleTypeSelections mySelectedTriggers;

    /// @brief List of selected shapes
    SingleTypeSelections mySelectedShapes;

    /// @brief List of selected objects
    std::vector<GLuint> mySelected;

    /// @brief The dialog to be updated
    GUIDialog_GLChosenEditor *my2Update;

};


#endif

/****************************************************************************/

