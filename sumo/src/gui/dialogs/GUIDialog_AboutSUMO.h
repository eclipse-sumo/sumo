#ifndef GUIDialog_AboutSUMO_H
#define GUIDialog_AboutSUMO_H

//---------------------------------------------------------------------------//
//                        GUIDialog_AboutSUMO.h -
//
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
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <fx.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIDialog_AboutSUMO : public FXDialogBox
{
public:
    GUIDialog_AboutSUMO(FXWindow* parent,  const char* name,
        int x, int y);
    ~GUIDialog_AboutSUMO();
    void create();
private:
    FXIcon *myDLRIcon, *myZAIKIcon;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
