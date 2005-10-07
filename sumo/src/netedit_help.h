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
// Revision 1.6  2005/10/07 11:48:01  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/15 12:27:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2004/12/20 15:00:37  dkrajzew
// version patched
//
// Revision 1.3  2004/12/20 14:05:16  dkrajzew
// version patched
//
// Revision 1.2  2004/12/20 13:15:58  dkrajzew
// options output corrected
//
// Revision 1.1  2004/11/23 10:45:07  dkrajzew
// netedit by A. Gaubatz added
//
// Revision 1.1  2004/11/22 13:02:17  dksumo
// 'netedit' and 'giant' added
//
// Revision 1.1  2004/10/22 12:48:56  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.1  2004/08/02 13:03:19  dkrajzew
// applied better names
//
// Revision 1.7  2003/10/27 10:47:49  dkrajzew
// added to possibility to close the application after a simulations end
//  without user interaction
//
// Revision 1.6  2003/07/07 08:40:09  dkrajzew
// included new options into the help-screens
//
// Revision 1.5  2003/06/24 08:06:36  dkrajzew
// implemented SystemFrame and applied the changes to all applications
//
// Revision 1.4  2003/04/16 09:57:05  dkrajzew
// additional parameter of maximum display size added
//
// Revision 1.3  2003/02/07 10:37:30  dkrajzew
// files updated
//
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

char *help[] = {
    "Simulation of Urban MObility v0.8.2 - netedit application",
    "  A graphical network editor.",
    "  http://sumo.sourceforge.net",
    "Usage: netedit [OPTION]*",
    " ",
    "Options:",
    " General options:",
    "   -w, --max-gl-width <UINT>       The maximum width of openGL-windows",
    "   -h, --max-gl-height <UINT>      The maximum height of openGL-windows",
    "   -c, --configuration <FILE>      Loads the named config on startup",
    "   -Q, --quit-on-end               Quits the gui when the simulation stops",
    "   -S, --suppress-end-info         Suppresses the information about the",
    "                                    simulation's end",
    "   -?, --help                      Prints this screen",
    "   -p, --print-options             Prints option values before processing",
    0
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
//
