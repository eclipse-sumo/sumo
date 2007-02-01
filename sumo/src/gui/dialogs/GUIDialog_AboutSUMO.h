/****************************************************************************/
/// @file    GUIDialog_AboutSUMO.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id: $
///
// The "About" dialog
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
#ifndef GUIDialog_AboutSUMO_h
#define GUIDialog_AboutSUMO_h
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

#include <fx.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDialog_AboutSUMO
 * @brief The "About" dialog
 */
class GUIDialog_AboutSUMO : public FXDialogBox
{
public:
    /// Constructor
    GUIDialog_AboutSUMO(FXWindow* parent,  const char* name,
                        int x, int y);

    /// Destructor
    ~GUIDialog_AboutSUMO();

    /// Creates the widget
    void create();

private:
    /// Icons for the widget
    FXIcon *myDLRIcon, *myZAIKIcon;

};


#endif

/****************************************************************************/

