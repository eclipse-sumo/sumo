#ifndef help_h
#define help_h
//---------------------------------------------------------------------------//
//                        netgen_help.h -
//  Help-screen of the gui version
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 16 Jul 2003
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
// Revision 1.2  2003/10/28 08:35:01  dkrajzew
// random number specification options added
//
// Revision 1.1  2003/07/16 15:41:17  dkrajzew
// network generator added
//
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


/* =========================================================================
 * definitions
 * ======================================================================= */
char *help[] =
{
    "Usage: sumo-netgen!!!",
    "Simulation of Urban MObility v0.7 - Simulation Module",
    "  http://sumo.sourceforge.net",
    " ",
    "Options:",
    " General options:",
    "   --srand <INT>                   Initialises the random number generator",
    "                                    with the given value",
    "   --abs-rand                      Set this when the current time shall be",
    "                                    used for random number initialisation",
    0
};

#endif

