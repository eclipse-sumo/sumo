#ifndef GUIIconSubSys_h
#define GUIIconSubSys_h
//---------------------------------------------------------------------------//
//                        GUIIconSubSys.h -
//  A class dividing the network in rectangular cells
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jul 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.3  2005/09/15 12:19:55  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/07/12 11:55:37  dkrajzew
// fonts are now drawn using polyfonts; dialogs have icons; searching for structures improved;
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <fx.h>
#include "GUIIcons.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIIconSubSys {
public:
    static void init(FXApp *a);
    static FXIcon *getIcon(GUIIcon which);
    static void close();
private:
    GUIIconSubSys(FXApp *a);
    ~GUIIconSubSys();

private:
    static GUIIconSubSys *myInstance;

private:
    FXIcon *myIcons[ICON_MAX];

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

