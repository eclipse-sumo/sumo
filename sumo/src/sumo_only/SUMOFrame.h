#ifndef SUMOFrame_h
#define SUMOFrame_h
//---------------------------------------------------------------------------//
//                        SUMOFrame.h -
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
// Revision 1.2  2003/02/07 11:19:37  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;
class MSNet;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class SUMOFrame
 * This class holds some helping methods needed both by the sumo and the
 * sumo-gui module.
 */
class SUMOFrame {
public:
    /// Builds teh simulation options
    static OptionsCont *getOptions();

    /// Build the output stream
    static std::ostream *buildRawOutputStream(OptionsCont *oc);

    /// Performs the post-initialisation of the net (!!! should be somewhere else)
    static void postbuild(MSNet &net);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "SUMOFrame.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

