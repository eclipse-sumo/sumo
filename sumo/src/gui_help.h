#ifndef help_h
#define help_h
//---------------------------------------------------------------------------//
//                        gui_help.h -
//  Help-screen of the gui version
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.4  2003/04/16 09:57:05  dkrajzew
// additional parameter of maximum display size added
//
// Revision 1.3  2003/02/07 10:37:30  dkrajzew
// files updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

char *help[] = {
    "Usage: sumo-gui",
    "Simulation of Urban MObility v0.7 - Simulation Module",
    "  http://sumo.sourceforge.net",
    " ",
    "Options:",
    " Input files:",
    "   -w, --max-gl-width <UINT>       The maximum width of openGL-windows",
    "   -h, --max-gl-height <UINT>      The maximum height of openGL-windows",
    0
};

#endif

