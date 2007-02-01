/****************************************************************************/
/// @file    GUIMessageWindow.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 25 Nov 2003
/// @version $Id: $
///
// A logging window for the gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIMessageWindow_h
#define GUIMessageWindow_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <fx.h>
#include <utils/gui/events/GUIEvent.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIMessageWindow
 * This class displays messages incoming to the gui from either the load or
 * the run thread.
 * The text is colored in dependence to its type (messages: green,
 * warnings: yellow, errors: red)
 *
 * Each time a new message is passed, the window is reopened.
 */
class GUIMessageWindow : public FXText
{
public:
    /// Constructor
    GUIMessageWindow(FXComposite *parent);

    /// Destructor
    ~GUIMessageWindow();

    /// Adds a a separator to this log window
    void addSeparator();

    /** @brief Adds new text to the window
        The type of the text is determined by the first parameter */
    void appendText(GUIEventType eType, const std::string &msg);

    /// Clears the window
    void clear();

private:
    /// The text colors used
    FXHiliteStyle* myStyles;

};


#endif

/****************************************************************************/

