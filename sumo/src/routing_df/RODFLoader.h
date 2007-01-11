#ifndef RODFLoader_h
#define RODFLoader_h
//---------------------------------------------------------------------------//
//                        RODFLoader.h -
//  Loader used while online-routing
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 17 Jun 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.2  2007/01/11 12:39:56  dkrajzew
// debugging building (missing, unfinished classes added)
//
// Revision 1.1  2006/03/28 06:17:18  dkrajzew
// extending the dfrouter by distance/length factors
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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

#include <router/ROLoader.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
class RODFLoader : public ROLoader {
public:
    /// Constructor
    RODFLoader(OptionsCont &oc, ROVehicleBuilder &vb,
        bool emptyDestinationsAllowed);

    /// Destructor
    ~RODFLoader();

    /// Builds the ROONet
    RONet *loadNet(ROAbstractEdgeBuilder &eb);

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

