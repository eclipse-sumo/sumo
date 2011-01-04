/****************************************************************************/
/// @file    GUIParam_PopupMenu.h
/// @author  Daniel Krajzewicz
/// @date    Mai 2003
/// @version $Id$
///
// A popup-menu for dynamic patameter table entries
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIParam_PopupMenu_h
#define GUIParam_PopupMenu_h


// ===========================================================================
// included modules
// ===========================================================================
#include <string>

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/common/ValueSource.h>
#include <utils/gui/div/GUIParameterTableWindow.h>


// ===========================================================================
// class definitions
// ===========================================================================
class GUIGlObject;
class GUIMainWindow;


// ===========================================================================
// class declarataions
// ===========================================================================
/**
 * @class GUIParam_PopupMenuInterface
 * @brief A popup-menu for dynamic patameter table entries
 *
 */
class GUIParam_PopupMenuInterface : public FXMenuPane {
    FXDECLARE(GUIParam_PopupMenuInterface)
public:
    /** @brief Constructor
     *
     * @param[in] app The main application window
     * @param[in] parentWindow The parent window (the table the popup belongs to)
     * @param[in] o The object represented by the table
     * @param[in] varName The name of the value
     * @param[in] src The value source
     */
    GUIParam_PopupMenuInterface(GUIMainWindow &app,
                                GUIParameterTableWindow &parentWindow,
                                GUIGlObject &o, const std::string &varName,
                                ValueSource<SUMOReal> *src) throw();


    /// @brief Destructor
    ~GUIParam_PopupMenuInterface() throw();


    /// @name FOX-callbacks
    /// @{

    /** @brief Called when a tracker for the value shall be opened
     *
     * Builds a new GUIParameterTracker adding the stored value to it.
     * Initialises this tracker, then.
     */
    long onCmdOpenTracker(FXObject*,FXSelector,void*);
    /// @}


protected:
    /// @brief The object the table displays
    GUIGlObject *myObject;

    /// @brief The parameter window this popup was initiated by
    GUIParameterTableWindow *myParentWindow;

    /** @brief The main application window; holder of some needed values */
    GUIMainWindow *myApplication;

    /// @brief The name of the value
    std::string myVarName;

    /// @brief The source of the value
    ValueSource<SUMOReal> *mySource;

protected:
    /// @brief FOX needs this
    GUIParam_PopupMenuInterface() { }

};


#endif

/****************************************************************************/

