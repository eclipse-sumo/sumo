#ifndef ROFrame_h
#define ROFrame_h
//---------------------------------------------------------------------------//
//                        ROFrame.h -
//  Some helping methods for usage within sumo and sumo-gui
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
// Revision 1.2  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:26  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROFrame
 */
class ROFrame {
public:
    /// Builds the simulation options
    static void fillOptions(OptionsCont &oc);

    static bool checkOptions(OptionsCont &oc);

    static void setDefaults(OptionsCont &oc);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

