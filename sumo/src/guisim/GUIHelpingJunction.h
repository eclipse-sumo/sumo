#ifndef GUIHelpingJunction_H
#define GUIHelpingJunction_H
//---------------------------------------------------------------------------//
//                        GUIHelpingJunction.h -
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

#include <vector>
#include <microsim/MSJunction.h>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIJunctionWrapper;
class GUIGlObjectStorage;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIHelpingJunction
    : public MSJunction
{
public:
    static void fill(std::vector<GUIJunctionWrapper*> &list,
        GUIGlObjectStorage &idStorage);

private:
    /// Default constructor.
    GUIHelpingJunction();

    /// Copy constructor.
    GUIHelpingJunction( const GUIHelpingJunction& );

    /// Assignment operator.
    GUIHelpingJunction& operator=( const GUIHelpingJunction& );

    /// Destructor.
    ~GUIHelpingJunction();

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
