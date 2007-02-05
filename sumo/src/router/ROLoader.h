/****************************************************************************/
/// @file    ROLoader.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Loader for networks and route imports
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
#ifndef ROLoader_h
#define ROLoader_h
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

#include <string>
#include "RORouteDefList.h"
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <utils/common/SUMOTime.h>
#include "ROAbstractRouter.h"


// ===========================================================================
// xerces 2.2 compatibility
// ===========================================================================
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class RONet;
class RONetHandler;
class ROAbstractRouteDefLoader;
class GenericSAX2Handler;
class ROAbstractEdgeBuilder;
class ROVehicleBuilder;
class GUIRouterRunThread;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROLoader
 * The data loader. Loads the network and route descriptions using further
 * classes. Is capable to either load all routes in one step or go through
 * them step wise.
 */
class ROLoader
{
public:
    /// Constructor
    ROLoader(OptionsCont &oc, ROVehicleBuilder &vb,
             bool emptyDestinationsAllowed);

    /// Destructor
    virtual ~ROLoader();

    /// Loads the network
    virtual RONet *loadNet(ROAbstractEdgeBuilder &eb);

    /// Loads the net weights
    bool loadWeights(RONet &net, const std::string &file,
                     bool useLanes);

    /// Parse the supplementary-weights-file. This will add
    /// supplementary weights to the RONet's ROEdges.
    ///
    /// @param net The net to which the weights should be aaded.
    ///
    /// @return True on successful parsing.
    ///
    void loadSupplementaryWeights(RONet& net);

    /** @brief Builds and opens all route loaders
        Route loaders are derived from ROAbstractRouteDefLoader */
    size_t openRoutes(RONet &net, SUMOReal gBeta, SUMOReal gA);

    /** @brief Loads routes stepwise
        This is done for all previously build route loaders */
    virtual void processRoutesStepWise(SUMOTime start, SUMOTime end,
                                       RONet &net, ROAbstractRouter &router);

    /** @brief Loads all routes at once
        This is done for all previously build route loaders */
    virtual void processAllRoutes(SUMOTime start, SUMOTime end,
                                  RONet &net, ROAbstractRouter &router);

    /** @brief Ends route reading
        This is done for all previously build route loaders */
    void closeReading();

    bool makeSingleStep(SUMOTime end, RONet &net, ROAbstractRouter &router);

    friend class GUIRouterRunThread;

protected:
    /** @brief Loads the net
        The loading structures were built in previous */
    /*    virtual bool loadNet(SAX2XMLReader *reader, RONetHandler &handler,
            const std::string &files);*/

    /** @brief Opens routes
        The loading structures were built in previous */
    void openTypedRoutes(const std::string &optionName, RONet &net);

    /// Adds a route loader to the list of known route loaders
    void addToHandlerList(const std::string &optionName, RONet &net);

    /** @brief Skips routes which start before the wished time period
        This is done for all previously build route loaders */
    void skipUntilBegin();

    /// Returns the first known time step
    SUMOTime getMinTimeStep() const;

protected:
    ROAbstractRouteDefLoader* buildNamedHandler(
        const std::string &optionName, const std::string &file
        , RONet &net);

    void checkFile(const std::string &optionName
                   , const std::string &file);

    void writeStats(SUMOTime time, SUMOTime start, int absNo);


protected:
    /// Options to use
    OptionsCont &_options;

    /// Definition of route loader list
    typedef std::vector<ROAbstractRouteDefLoader*> RouteLoaderCont;

    /// List of route loaders
    RouteLoaderCont _handler;

    /// Information whether empty destinations are allowed
    bool myEmptyDestinationsAllowed;

    /// The vehicle builder to use
    ROVehicleBuilder &myVehicleBuilder;

private:
    /// invalidated copy constructor
    ROLoader(const ROLoader &src);

    /// invalidated assignment operator
    ROLoader &operator=(const ROLoader &src);

};


#endif

/****************************************************************************/

