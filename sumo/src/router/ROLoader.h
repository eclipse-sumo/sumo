/****************************************************************************/
/// @file    ROLoader.h
/// @author  Daniel Krajzewicz
/// @author  Christian Roessel
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// Loader for networks and route imports
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <utils/common/SUMOTime.h>
#include <utils/vehicle/SUMOAbstractRouter.h>
#include <utils/common/ValueTimeLine.h>
#include <utils/xml/SAXWeightsHandler.h>
#include <utils/xml/SUMORouteLoaderControl.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class ROAbstractEdgeBuilder;
class RONet;
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
     * @param[in] emptyDestinationsAllowed Whether trips may be given without destinations
     * @todo Recheck usage of emptyDestinationsAllowed
     */
    ROLoader(OptionsCont& oc, const bool emptyDestinationsAllowed, const bool logSteps);


    /// @brief Destructor
    virtual ~ROLoader();

    /// Loads the network
    virtual void loadNet(RONet& toFill, ROAbstractEdgeBuilder& eb);

    /// Loads the net weights
    bool loadWeights(RONet& net, const std::string& optionName,
                     const std::string& measure, bool useLanes);

    /** @brief Builds and opens all route loaders */
    void openRoutes(RONet& net);

    /** @brief Loads routes from all previously build route loaders */
    void processRoutes(const SUMOTime start, const SUMOTime end, const SUMOTime increment,
                       RONet& net, SUMOAbstractRouter<ROEdge, ROVehicle>& router);

    /** @brief Loads all routes and processes them with BulkStarRouter */
    void processAllRoutesWithBulkRouter(SUMOTime start, SUMOTime end,
                                        RONet& net, SUMOAbstractRouter<ROEdge, ROVehicle>& router);

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
    bool openTypedRoutes(const std::string& optionName, RONet& net);


    /**
     * @class EdgeFloatTimeLineRetriever_EdgeWeight
     * @brief Obtains edge weights from a weights handler and stores them within the edges
     * @see SAXWeightsHandler::EdgeFloatTimeLineRetriever
     */
    class EdgeFloatTimeLineRetriever_EdgeWeight : public SAXWeightsHandler::EdgeFloatTimeLineRetriever {
    public:
        /// @brief Constructor
        EdgeFloatTimeLineRetriever_EdgeWeight(RONet& net) : myNet(net) {}

        /// @brief Destructor
        ~EdgeFloatTimeLineRetriever_EdgeWeight() { }

        /** @brief Adds an effort for a given edge and time period
         *
         * @param[in] id The id of the object to add a weight for
         * @param[in] val The weight
         * @param[in] beg The begin of the interval the weight is valid for
         * @param[in] end The end of the interval the weight is valid for
         * @see SAXWeightsHandler::EdgeFloatTimeLineRetriever::addEdgeWeight
         */
        void addEdgeWeight(const std::string& id,
                           SUMOReal val, SUMOReal beg, SUMOReal end) const;

    private:
        /// @brief The network edges shall be obtained from
        RONet& myNet;

    };


    /**
     * @class EdgeFloatTimeLineRetriever_EdgeTravelTime
     * @brief Obtains edge travel times from a weights handler and stores them within the edges
     * @see SAXWeightsHandler::EdgeFloatTimeLineRetriever
     */
    class EdgeFloatTimeLineRetriever_EdgeTravelTime : public SAXWeightsHandler::EdgeFloatTimeLineRetriever {
    public:
        /// @brief Constructor
        EdgeFloatTimeLineRetriever_EdgeTravelTime(RONet& net) : myNet(net) {}

        /// @brief Destructor
        ~EdgeFloatTimeLineRetriever_EdgeTravelTime() {}

        /** @brief Adds a travel time for a given edge and time period
         *
         * @param[in] id The id of the object to add a weight for
         * @param[in] val The travel time
         * @param[in] beg The begin of the interval the weight is valid for
         * @param[in] end The end of the interval the weight is valid for
         * @see SAXWeightsHandler::EdgeFloatTimeLineRetriever::addEdgeWeight
         */
        void addEdgeWeight(const std::string& id,
                           SUMOReal val, SUMOReal beg, SUMOReal end) const;

    private:
        /// @brief The network edges shall be obtained from
        RONet& myNet;

    };



protected:
    void writeStats(SUMOTime time, SUMOTime start, int absNo, bool endGiven);


private:
    /// @brief Options to use
    OptionsCont& myOptions;

    /// @brief Information whether empty destinations are allowed
    const bool myEmptyDestinationsAllowed;

    /// @brief Information whether the routing steps should be logged
    const bool myLogSteps;

    /// @brief List of route loaders
    SUMORouteLoaderControl myLoaders;


private:
    /// @brief Invalidated copy constructor
    ROLoader(const ROLoader& src);

    /// @brief Invalidated assignment operator
    ROLoader& operator=(const ROLoader& src);
};


#endif

/****************************************************************************/

