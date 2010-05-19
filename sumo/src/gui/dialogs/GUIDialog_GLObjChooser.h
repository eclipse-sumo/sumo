/****************************************************************************/
/// @file    GUIDialog_GLObjChooser.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Class for the window that allows to choose a street, junction or vehicle
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
#ifndef GUIDialog_GLObjChooser_h
#define GUIDialog_GLObjChooser_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <fx.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOViewParent;
class GUIGlObjectStorage;
class GUIGlObject;


// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class GUIDialog_GLObjChooser
 * Instances of this class are windows that display the list of instances
 * from a given artifact like vehicles, edges or junctions and allow
 * one of their items
 */
class GUIDialog_GLObjChooser : public FXMainWindow {
    // FOX-declarations
    FXDECLARE(GUIDialog_GLObjChooser)

public:
    /** @brief Constructor
     * @param[in] parent The calling view
     * @param[in] icon The icon to use
     * @param[in] title The title to use
     * @param[in] type The type of gl-objects to show instances of
     * @param[in] glStorage The storage to retrieve ids from
     */
    GUIDialog_GLObjChooser(GUISUMOViewParent *parent, FXIcon *icon, const FXString &title,
                           GUIGlObjectType type, GUIGlObjectStorage &glStorage) throw();


    /// @brief Destructor
    ~GUIDialog_GLObjChooser() throw();


    /** @brief Returns the chosen (selected) object
     * @return The selected object
     */
    GUIGlObject *getObject() const throw() {
        return static_cast<GUIGlObject*>(mySelected);
    }



    /// @name FOX-callbacks
    /// @{

    /// @brief Callback: The selected item shall be centered within the calling view
    long onCmdCenter(FXObject*,FXSelector,void*);

    /// @brief Callback: The dialog shall be closed
    long onCmdClose(FXObject*,FXSelector,void*);

    /// @brief Callback: Something has been typed into the the field
    long onChgText(FXObject*,FXSelector,void*);

    /// @brief Callback: Selects to current item if enter is pressed
    long onCmdText(FXObject*,FXSelector,void*);

    /// @brief Callback: Selects to current item if enter is pressed
    long onListKeyPress(FXObject*,FXSelector,void*);
    /// @}


private:
    /// @brief The list that holds the ids
    FXList *myList;

    /// @brief The artifact to choose
    GUIGlObjectType myObjectType;

    /// @brief The parent window
    GUISUMOViewParent *myParent;

    /// @brief The chosen id
    GUIGlObject *mySelected;

    /// @brief The text field
    FXTextField *myTextEntry;


protected:
    /// FOX needs this
    GUIDialog_GLObjChooser() { }

};


#endif

/****************************************************************************/

