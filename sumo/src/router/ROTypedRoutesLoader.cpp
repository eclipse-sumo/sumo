//---------------------------------------------------------------------------//
//                        ROTypedRoutesLoader.cpp -
//  The basic class for loading routes
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2003/03/03 15:22:36  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:06  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <string>
#include "ROTypedRoutesLoader.h"
#include <utils/common/SErrorHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/options/OptionsCont.h>
#include "RONet.h"

using namespace std;

ROTypedRoutesLoader::ROTypedRoutesLoader(RONet &net, const std::string &file)
    : _net(net), _currentTimeStep(-1), _ended(false),
    _nextRouteRead(false), _options(0)
{
}

ROTypedRoutesLoader::~ROTypedRoutesLoader()
{
}

bool
ROTypedRoutesLoader::skipPreviousRoutes(long start)
{
    bool ok = startReadingSteps();
    /// skip routes 
    if(_options->getBool("v")) {
        cout << "Skipping" << endl;
    }
    while(!_ended&&ok&&_currentTimeStep<start) {
        ok = readNextRoute(start);
        cout << "Skipping.";
    }
    if(_options->getBool("v")) {
        cout << "Skipped until: " << _currentTimeStep << endl;
    }
    // check whether errors occured
    if(!ok) {
        SErrorHandler::add(
            string("Problems on parsing the ") + getDataName() +
            string(" file."), true);
        throw ProcessError();
    }
    return ok;
}

bool
ROTypedRoutesLoader::addRoutesUntil(long time)
{
    bool ok = startReadingSteps();
    while(ok&&_currentTimeStep<=time&&!_ended) {
        ok = readNextRoute(_currentTimeStep);
    }
    if(!ok) {
        SErrorHandler::add(
            string("Problems on parsing the ") + getDataName() +
            string(" file."), true);
        throw ProcessError();
    }
    return ok;
}

bool
ROTypedRoutesLoader::addAllRoutes()
{
    bool ok = startReadingSteps();
    while(ok&&!_ended) {
        ok = readNextRoute(_currentTimeStep);
    }
    return ok;
}

bool
ROTypedRoutesLoader::ended() const
{
    return _ended;
}

bool
ROTypedRoutesLoader::checkFile(const std::string &file) const
{
    return FileHelpers::exists(file);
}

long
ROTypedRoutesLoader::getCurrentTimeStep() const
{
    return _currentTimeStep;
}

bool 
ROTypedRoutesLoader::init(OptionsCont &options)
{
    _options = &options;
    return myInit(options);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROTypedRoutesLoader.icc"
//#endif

// Local Variables:
// mode:C++
// End:


