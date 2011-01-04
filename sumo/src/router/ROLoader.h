/****************************************************************************/
/// @file    ROLoader.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Loader for networks and route imports
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOAbstractRouter.h>
#include <utils/common/ValueTimeLine.h>
#include <utils/xml/SAXWeightsHandler.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class RONet;
class ROAbstractRouteDefLoader;
class ROAbstractEdgeBuilder;
class GUIRouterRunThread;
class ROVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROLoader
 * @brief The data loader.
 *
 * Loads the network and route descriptions using further classes.
 *
 * Is capable to either load all routes in one step or go through them step wise.
 */
class ROLoader {
public:
    /** @brief Constructor
     *
     * @param[in] oc The options to use
     * @param[in] emptyDestinationsAllowed Whether tripdefs may be given without destinations
     * @todo Recheck usage of emptyDestinationsAllowed
     */
    ROLoader(OptionsCont &oc, bool emptyDestinationsAllowed) throw();


    /// @brief Destructor
    virtual ~ROLoader();

    /// Loads the network
    virtual void loadNet(RONet &toFill, ROAbstractEdgeBuilder &eb);

    /// Loads the net weights
    bool loadWeights(RONet &net, const std::string &optionName,
                     const std::string &measure, bool useLanes);

    /** @brief Builds and opens all route loaders
        Route loaders are derived from ROAbstractRouteDefLoader */
    unsigned int openRoutes(RONet &net);

    /** @brief Loads routes stepwise
        This is done for all previously build route loaders */
    virtual void processRoutesStepWise(SUMOTime start, SUMOTime end,
                                       RONet &net, SUMOAbstractRouter<ROEdge,ROVehicle> &router);

    /** @brief Loads all routes at once
        This is done for all previously build route loaders */
    virtual void processAllRoutes(SUMOTime start, SUMOTime end,
                                  RONet &net, SUMOAbstractRouter<ROEdge,ROVehicle> &router);

    bool makeSingleStep(SUMOTime end, RONet &net, SUMOAbstractRouter<ROEdge,ROVehicle> &router);

protected:
    /** @brief Opens route handler of the given type
     *
     * Checks whether the given option name is known, returns true if
     *  not (this means that everything's ok, though the according
     *  handler is not built).
     *
     * Checks then whether the given option name is set and his value is one
     *  or a set of valid (existing) files. This is done via a call to
     *  "OptionsCont::isUsableFileList" (which generates a proper error
     *  message).
     *
     * If the given files are valid, the proper instance(s) is built using
     *  "buildNamedHandler" and if this could be done, it is added to
     *  the list of route handlers to use ("myHandler")
     *
     * Returns whether the wished handler(s) could be built.
     *
     * @param[in] optionName The name of the option that refers to which handler and which files shall be used
     * @param[in] net The net to assign to the built handlers
     * @return Whether the wished handler(s) could be built
     */
    bool openTypedRoutes(const std::string &optionName, RONet &net) throw();


    /** @brief Returns the first time step known by the built handlers
     *
     * The handlers are responsible for not adding route definitions
     *  prior to "begin"-Option's value. These priori departures
     *  must also not be reported by them whaen asking via "getLastReadTimeStep".
     *
     * @return The first time step of loaded routes
     * @see ROAbstractRouteDefLoader::getLastReadTimeStep
     */
    SUMOTime getMinTimeStep() const throw();



    /**
     * @class EdgeFloatTimeLineRetriever_EdgeWeight
     * @brief Obtains edge weights from a weights handler and stores them within the edges
     * @see SAXWeightsHandler::EdgeFloatTimeLineRetriever
     */
    class EdgeFloatTimeLineRetriever_EdgeWeight : public SAXWeightsHandler::EdgeFloatTimeLineRetriever {
    public:
        /// @brief Constructor
        EdgeFloatTimeLineRetriever_EdgeWeight(RONet &net) throw() : myNet(net) {}

        /// @brief Destructor
        ~EdgeFloatTimeLineRetriever_EdgeWeight() throw() { }

        /** @brief Adds an effort for a given edge and time period
         *
         * @param[in] id The id of the object to add a weight for
         * @param[in] val The weight
         * @param[in] beg The begin of the interval the weight is valid for
         * @param[in] end The end of the interval the weight is valid for
         * @see SAXWeightsHandler::EdgeFloatTimeLineRetriever::addEdgeWeight
         */
        void addEdgeWeight(const std::string &id,
                           SUMOReal val, SUMOReal beg, SUMOReal end) const throw();

    private:
        /// @brief The network edges shall be obtained from
        RONet &myNet;

    };


    /**
     * @class EdgeFloatTimeLineRetriever_EdgeTravelTime
     * @brief Obtains edge travel times from a weights handler and stores them within the edges
     * @see SAXWeightsHandler::EdgeFloatTimeLineRetriever
     */
    class EdgeFloatTimeLineRetriever_EdgeTravelTime : public SAXWeightsHandler::EdgeFloatTimeLineRetriever {
    public:
        /// @brief Constructor
        EdgeFloatTimeLineRetriever_EdgeTravelTime(RONet &net) throw() : myNet(net) {}

        /// @brief Destructor
        ~EdgeFloatTimeLineRetriever_EdgeTravelTime() throw() {}

        /** @brief Adds a travel time for a given edge and time period
         *
         * @param[in] id The id of the object to add a weight for
         * @param[in] val The travel time
         * @param[in] beg The begin of the interval the weight is valid for
         * @param[in] end The end of the interval the weight is valid for
         * @see SAXWeightsHandler::EdgeFloatTimeLineRetriever::addEdgeWeight
         */
        void addEdgeWeight(const std::string &id,
                           SUMOReal val, SUMOReal beg, SUMOReal end) const throw();

    private:
        /// @brief The network edges shall be obtained from
        RONet &myNet;

    };



protected:
    ROAbstractRouteDefLoader* buildNamedHandler(const std::string &optionName,
            const std::string &file, RONet &net) throw(ProcessError);


    void writeStats(SUMOTime time, SUMOTime start, int absNo) throw();


    /** @brief Deletes all handlers and clears their container ("myHandler") */
    void destroyHandlers() throw();


protected:
    /// @brief Options to use
    OptionsCont &myOptions;

    /// @brief Definition of route loader list
    typedef std::vector<ROAbstractRouteDefLoader*> RouteLoaderCont;

    /// @brief List of route loaders
    RouteLoaderCont myHandler;

    /// @brief Information whether empty destinations are allowed
    bool myEmptyDestinationsAllowed;


private:
    /// @brief Invalidated copy constructor
    ROLoader(const ROLoader &src);

    /// @brief Invalidated assignment operator
    ROLoader &operator=(const ROLoader &src);

};


#endif

/****************************************************************************/

