#ifndef GUINetBuilder_h
#define GUINetBuilder_h
//---------------------------------------------------------------------------//
//                        GUINetBuilder.h -
//  Builds the gui-network
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
// Revision 1.2  2003/02/07 10:38:19  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <netload/NLNetBuilder.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;
class GUINet;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * This is mostly the same as a NLNetBuilder, but allows to build a GUINet
 * instead of a MSNet
 */
class GUINetBuilder : public NLNetBuilder
{
public:
    /// constructor
    GUINetBuilder(const OptionsCont &oc);

    /// destructor
    ~GUINetBuilder();

    /// builds the gui network
    GUINet *buildGUINet();
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUINetBuilder.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

