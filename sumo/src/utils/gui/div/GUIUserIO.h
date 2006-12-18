#ifndef GUIUserIO_h
#define GUIUserIO_h
//---------------------------------------------------------------------------//
//                        GUIUserIO.h -
//  Some OS-dependant functions to ease cliboard manipulation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2006
//  copyright            : (C) 2006 by Daniel Krajzewicz
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
// Revision 1.3  2006/12/18 08:22:55  dkrajzew
// comments added
//
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

#include <string>
#include <fx.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIUserIO
 * @brief Some OS-dependant functions to ease cliboard manipulation
 */
class GUIUserIO {
public:
    /** @brief Copies the given text to clipboard
     *
     * !!! This method is only implemented for MS Windows
     */
    static void copyToClipboard(FXApp *app, const std::string &text);

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

