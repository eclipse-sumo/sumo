/****************************************************************************/
/// @file    MSRouteLoader.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 6 Nov 2002
/// @version $Id$
///
// A class that performs the loading of routes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSRouteLoader_h
#define MSRouteLoader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <string>
#include <microsim/MSNet.h>
#include "MSVehicleContainer.h"
#include "MSRouteHandler.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSRouteLoader
 */
class MSRouteLoader {
public:
    /// constructor
    MSRouteLoader(MSNet &net,
                  MSRouteHandler *handler);

    /// destructor
    ~MSRouteLoader();

    /** loads vehicles until a vehicle is read that starts after
        the specified time */
    void loadUntil(SUMOTime time, MSVehicleContainer &into);

    /// resets the reader
    void init();

    /// returns the information whether new data is available
    bool moreAvailable() const;
private:
    /// the used SAX2XMLReader
    SAX2XMLReader* myParser;

    /// the token for saving the current position
    XMLPScanToken  myToken;

    /// information whether more vehicles should be available
    bool myMoreAvailable;

    MSRouteHandler *myHandler;

};


#endif

/****************************************************************************/

