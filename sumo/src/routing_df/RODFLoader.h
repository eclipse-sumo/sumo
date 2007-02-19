/****************************************************************************/
/// @file    RODFLoader.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 17 Jun 2004
/// @version $Id$
///
// Loader used while online-routing
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef RODFLoader_h
#define RODFLoader_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <router/ROLoader.h>


// ===========================================================================
// class definitions
// ===========================================================================
class RODFLoader : public ROLoader
{
public:
    /// Constructor
    RODFLoader(OptionsCont &oc, ROVehicleBuilder &vb,
               bool emptyDestinationsAllowed);

    /// Destructor
    ~RODFLoader();

    /// Builds the ROONet
    RONet *loadNet(ROAbstractEdgeBuilder &eb);

};


#endif

/****************************************************************************/

