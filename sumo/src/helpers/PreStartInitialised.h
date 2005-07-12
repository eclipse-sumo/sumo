#ifndef PreStartInitialised_h
#define PreStartInitialised_h
//---------------------------------------------------------------------------//
//                        PreStartInitialised.h -
//  Basic class for artifacts that must be reinitialised before a simulation
//      may be restarted
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
// Revision 1.3  2005/07/12 11:37:41  dkrajzew
// level 3 warnings removed; code style adapted
//
// Revision 1.2  2004/12/16 12:25:51  dkrajzew
// code beautifying
//
// Revision 1.1  2003/02/07 10:40:13  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSNet;
class MSEventControl;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * PreStartInitialised
 * Some artifacts used within the simulation must be reinitialised each time
 * a simulation is restarted. May they be lanes containing statistics about
 * traffic from the past simulation which otherwise would be kept in the
 * memory and so influence the latter statistics or route loaders which must
 * be reinitialised to read routes again starting from the specified
 * simulation begin.
 * Such classes must implement the here given interface to allow their storage
 * in a container of instances that shall be preinitialised and to allow
 * their preinitialisation.
 */
class PreStartInitialised {
public:
    /** @brief Constructor
        The network is needed as PreStartInitialised-objects are added to it
        so it knows their init-methods must be called after the net has been
        loaded */
    PreStartInitialised(MSNet &net);

    /// Destructor
    virtual ~PreStartInitialised();

    /// Initialises the object with the complete net
    virtual void init(MSNet &net) = 0;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

