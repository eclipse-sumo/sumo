#ifndef ROTypedRoutesLoader_h
#define ROTypedRoutesLoader_h
//---------------------------------------------------------------------------//
//                        ROTypedRoutesLoader.h -
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
// $Log$
// Revision 1.3  2003/03/03 15:22:37  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
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

/* =========================================================================
 * class declarations
 * ======================================================================= */
class RONet;
class SAX2XMLReader;
class OptionsCont;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROTypedRoutesLoader
 * Base class for loaders of route which do have a certain format. XML-reading
 * loaders are not derived directly, but use the derived ROTypedXMLRoutesLoader
 * class as their upper class.
 */
class ROTypedRoutesLoader {
public:
    /// Constructor
    ROTypedRoutesLoader(RONet &net, const std::string &file="");

    /// Destructor
    virtual ~ROTypedRoutesLoader();

    /// Returns the fully configurated loader of this type
    virtual ROTypedRoutesLoader *getAssignedDuplicate(const std::string &file) const = 0;

    /// Skips routes which begin before the given time
    bool skipPreviousRoutes(long start);

    /// Adds routes from the file until the given time is reached
    bool addRoutesUntil(long time);

    /// Adds all routes from the file
    virtual bool addAllRoutes();

    /// Closes the reading of the routes
    virtual void closeReading() = 0;

    /// Returns the name of the route type
    virtual std::string getDataName() const = 0;

    /// Initialises the reader
    bool init(OptionsCont &options);

    /// Returns the information whether no routes are available anymore
    virtual bool ended() const;

    /// Checks whether the file to use exists
    virtual bool checkFile(const std::string &file) const;

    /// Returns the time the current (last read) route starts at
    long getCurrentTimeStep() const;

protected:
    /// Begins the reading
    virtual bool startReadingSteps() = 0;

    /// reads a single route from the file
    virtual bool readNextRoute(long start) = 0;

    /// reader-typ dependent initialisation
    virtual bool myInit(OptionsCont &options) = 0;

protected:
    /// The network to add routes to
    RONet &_net;

    /// The time step of the last read route
    long _currentTimeStep;

    /// Information whether the file is completely parsed
    bool _ended;

    /** Information whether a further route was read
        @brief If not and there are further routes, the method "readNextRoute"
        must be recalled */
    bool _nextRouteRead;

    /// The processing options (set in init)
    OptionsCont *_options;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROTypedRoutesLoader.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

