#ifndef ROLoader_h
#define ROLoader_h
//---------------------------------------------------------------------------//
//                        ROLoader.h -
//  Loader for networks and route imports
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
#include "RORouteDefList.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;
class RONet;
class RONetHandler;
class ROTypedRoutesLoader;
class ofstream;
class SAX2XMLReader;
class GenericSAX2Handler;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROLoader
 * The data loader. Loads the network and route descriptions using further
 * classes. Is capable to either load all routes in one step or go through
 * them step wise.
 */
class ROLoader {
public:
    /// Constructor
    ROLoader(OptionsCont &oc);

    /// Destructor
    ~ROLoader();

    /// Loads the network
    RONet *loadNet();

    /// Loads the net weights
    bool loadWeights(RONet &net);

    /** @brief Builds and opens all route loaders
        Route loaders are derived from ROTypedRoutesLoader */
    void openRoutes(RONet &net);

    /** @brief Loads routes stepwise
        This is done for all previously build route loaders */
    void processRoutesStepWise(long start, long end,
        std::ofstream &res, std::ofstream &altres, RONet &net);

    /** @brief Loads all routes at once
        This is done for all previously build route loaders */
    void processAllRoutes(long start, long end,
        std::ofstream &res, std::ofstream &altres, RONet &net);

    /** @brief Ends route reading
        This is done for all previously build route loaders */
    void closeReading();

private:
    /** @brief Loads the net
        The loading structures were built in previous */
    bool loadNet(SAX2XMLReader *reader, RONetHandler &handler,
        const std::string &files);

    /** @brief Opens routes
        The loading structures were built in previous */
    void openTypedRoutes(ROTypedRoutesLoader *handler,
        const std::string &optionName);

    /// Adds a route loader to the list of known route loaders
    void addToHandlerList(ROTypedRoutesLoader *handler,
        const std::string &fileList);

    /** @brief Skips routes which start before the wished time period
        This is done for all previously build route loaders */
    void skipPreviousRoutes(long start);

    /// Builds teh SAX2-reader
    SAX2XMLReader *getSAXReader(GenericSAX2Handler &handler);

    /// Returns the first known time step
    long getMinTimeStep() const;

private:
    /// Options to use
    OptionsCont &_options;

    /// Definition of route loader list
    typedef std::vector<ROTypedRoutesLoader*> RouteLoaderCont;

    /// List of route loaders
    RouteLoaderCont _handler;

private:
    /// invalidated copy constructor
    ROLoader(const ROLoader &src);

    /// invalidated assignment operator
    ROLoader &operator=(const ROLoader &src);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROLoader.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

