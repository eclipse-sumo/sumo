#ifndef GUIDialog_AboutSUMO_H
#define GUIDialog_AboutSUMO_H

//---------------------------------------------------------------------------//
//                        GUIDialog_AboutSUMO.h -
//                      The "About" dialog
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//


//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.7  2006/10/12 10:14:26  dkrajzew
// synchronized with internal CVS (mainly the documentation has changed)
//
// Revision 1.6  2006/07/06 05:38:32  dkrajzew
// documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <fx.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
