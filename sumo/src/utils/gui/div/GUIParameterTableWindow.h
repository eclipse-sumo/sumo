/****************************************************************************/
/// @file    GUIParameterTableWindow.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// The window that holds the table of an object's parameter
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIParameterTableWindow_h
#define GUIParameterTableWindow_h


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
#include <fx.h>
#include <utils/foxtools/MFXMutex.h>
#include <utils/common/ValueSource.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGlObject;
class GUIMainWindow;
class GUIParameterTableItemInterface;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIParameterTableWindow
 * @brief A window containing a gl-object's parameter
 *
 * This class realises a view on some parameter of a gl-object. The gl-object
 *  itself is responsible for filling this table.
 *
 * After allocating such a table, the object should fill the rows by calling
 *  one of the mkItem-methods for each. The building is closed using closeBuilding
 *  what forces the table to be displayed.
 *
 * Each row is represented using an instance of GUIParameterTableItemInterface.
 */
class GUIParameterTableWindow : public FXMainWindow {
    FXDECLARE(GUIParameterTableWindow)
public:
    /** @brief Constructor
     *
     * @param[in] app The application this window belongs to
     * @param[in] o The gl-object this table describes
     * @param[in] noRows Number of rows to allocate
     */
    GUIParameterTableWindow(GUIMainWindow& app,
                            GUIGlObject& o, size_t noRows);


    /// @brief Destructor
    ~GUIParameterTableWindow();


    /** @brief Closes the building of the table
     *
     * Adds the table to the list of child windows of the applications also
     *  telling the application that the window should be updated in each
     *  simulation step. Shows the table.
     *
     * @see GUIMainWindow::addChild
     */
    void closeBuilding();



    /** @brief Lets this window know the object shown is being deleted
     * @param[in] o The deleted (shown) object
     */
    void removeObject(GUIGlObject* const o);



    /// @name Row adding functions
    /// @{

    /** @brief Adds a row which obtains its value from an unsigned-ValueSource
     *
     * @param[in] name The name of the row entry
     * @param[in] dynamic Information whether the entry is dynamic
     * @param[in] src The value source to use
     */
    void mkItem(const char* name, bool dynamic, ValueSource<unsigned>* src);

    /** @brief Adds a row which obtains its value from an integer-ValueSource
     *
     * @param[in] name The name of the row entry
     * @param[in] dynamic Information whether the entry is dynamic
     * @param[in] src The value source to use
     */
    void mkItem(const char* name, bool dynamic, ValueSource<int>* src);


    /** @brief Adds a row which obtains its value from an SUMOReal-ValueSource
     *
     * @param[in] name The name of the row entry
     * @param[in] dynamic Information whether the entry is dynamic
     * @param[in] src The value source to use
     */
    void mkItem(const char* name, bool dynamic, ValueSource<SUMOReal>* src);


    /** @brief Adds a row which shows a string-value
     *
     * @param[in] name The name of the row entry
     * @param[in] dynamic Information whether the entry is dynamic
     * @param[in] value The value to show
     * @todo the dynamic-parameter is obsolete(?)
     */
    void mkItem(const char* name, bool dynamic, std::string value);


    /** @brief Adds a row which shows a unsigned-value
     *
     * @param[in] name The name of the row entry
     * @param[in] dynamic Information whether the entry is dynamic
     * @param[in] value The value to show
     * @todo the dynamic-parameter is obsolete
     */
    void mkItem(const char* name, bool dynamic, unsigned value);


    /** @brief Adds a row which shows a integer-value
     *
     * @param[in] name The name of the row entry
     * @param[in] dynamic Information whether the entry is dynamic
     * @param[in] value The value to show
     * @todo the dynamic-parameter is obsolete
     */
    void mkItem(const char* name, bool dynamic, int value);


    /** @brief Adds a row which shows a SUMOReal-value
     *
     * @param[in] name The name of the row entry
     * @param[in] dynamic Information whether the entry is dynamic
     * @param[in] value The value to show
     * @todo the dynamic-parameter is obsolete
     */
    void mkItem(const char* name, bool dynamic, SUMOReal value);


    /// @}



    /// @name FOX-callbacks
    /// @{

    /** @brief Updates the table due to a simulation step */
    long onSimStep(FXObject*, FXSelector, void*);

    /** @brief Does nothing
     * @todo Recheck whether this is needed (to override FXTable-behaviour?)
     */
    long onTableSelected(FXObject*, FXSelector, void*);

    /** @brief Does nothing
     * @todo Recheck whether this is needed (to override FXTable-behaviour?)
     */
    long onTableDeselected(FXObject*, FXSelector, void*);

    /** @brief Shows a popup
     *
     * Callback for right-mouse-button pressing event. Obtains the selected row
     *  and determines whether it is dynamic. If so, a popup-menu which allows
     *  to open a tracker for this value is built and shown.
     *
     * @see GUIParameterTableItemInterface
     * @see GUIParam_PopupMenuInterface
     */
    long onRightButtonPress(FXObject*, FXSelector, void*);
    /// @}


protected:
    /** @brief Updates the table
     *
     * Goes through all entries and updates them using GUIParameterTableItemInterface::update.
     *
     * @see GUIParameterTableItemInterface::update
     */
    void updateTable();


private:
    /// @brief The object to get the information from
    GUIGlObject* myObject;

    /// @brief The table to display the information in
    FXTable* myTable;

    /// @brief The main application window
    GUIMainWindow* myApplication;

    /// @brief The list of table rows
    std::vector<GUIParameterTableItemInterface*> myItems;

    /// @brief The index of the next row to add - used while building
    unsigned myCurrentPos;

    /// @brief A lock assuring save updates in cse of object deletion
    mutable MFXMutex myLock;

protected:
    /// FOX needs this
    GUIParameterTableWindow() { }

};


#endif

/****************************************************************************/

