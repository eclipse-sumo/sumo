/****************************************************************************/
/// @file    NLBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The main interface for loading a microsim
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
#ifndef NLBuilder_h
#define NLBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <vector>
#include <utils/xml/SAXWeightsHandler.h>
#include <microsim/MSNet.h>
#include "NLGeomShapeBuilder.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class NLContainer;
class MSEmitControl;
class MSJunctionLogic;
class MSDetectorControl;
class OptionsCont;
class NLHandler;
class NLEdgeControlBuilder;
class NLJunctionControlBuilder;
class NLDetectorBuilder;
class NLTriggerBuilder;
class MSRouteLoader;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NLBuilder
 * @brief The main interface for loading a microsim
 *
 * It is a black-box where only the options and factories must be supplied
 *  on the constructor call. An (empty) instance of the network must be
 *  supplied, too, and is filled during loading.
 */
class NLBuilder {
public:
    /** @brief Constructor
     *
     * @param[in] oc The options to use
     * @param[in, out] net The network to fill
     * @param[in] eb The builder of edges to use
     * @param[in] jb The builder of junctions to use
     * @param[in] db The detector builder to use
     * @param[in] tb The trigger builder to use
     * @param[in] xmlHandler The xml handler to use
     */
    NLBuilder(OptionsCont &oc, MSNet &net,
              NLEdgeControlBuilder &eb, NLJunctionControlBuilder &jb,
              NLDetectorBuilder &db,
              NLHandler &xmlHandler) throw();


    /// @brief Destructor
    virtual ~NLBuilder() throw();


    /** @brief Builds and initialises the simulation
     *
     * At first, the network is loaded and the built using "buildNet".
     *  If this could be done, additional information is loaded (state dump,
     *  weight files, route files, and additional files).
     * If everything could be done, true is returned, otherwise false.
     *
     * @see buildNet
     * @exception ProcessError If something fails on network building
     * @todo Again, both returning a bool and throwing an exception; quite inconsistent
     */
    virtual bool build() throw(ProcessError);


protected:
    /// loads a described subpart form the given list of files
    bool load(const std::string &mmlWhat);

    /// Closes the net building process
    void buildNet() throw(ProcessError);

    /// Builds the route loader control
    MSRouteLoaderControl *buildRouteLoaderControl(const OptionsCont &oc) throw(ProcessError);


    /**
     * @class EdgeFloatTimeLineRetriever_EdgeTravelTime
     * @brief Obtains edge efforts from a weights handler and stores them within the edges
     * @see SAXWeightsHandler::EdgeFloatTimeLineRetriever
     */
    class EdgeFloatTimeLineRetriever_EdgeEffort : public SAXWeightsHandler::EdgeFloatTimeLineRetriever {
    public:
        /// @brief Constructor
        EdgeFloatTimeLineRetriever_EdgeEffort(MSNet &net) throw() : myNet(net) {}

        /// @brief Destructor
        ~EdgeFloatTimeLineRetriever_EdgeEffort() throw() { }

        /** @brief Adds an effort for a given edge and time period
         *
         * @param[in] id The id of the object to add a weight for
         * @param[in] val The effort
         * @param[in] beg The begin of the interval the weight is valid for
         * @param[in] end The end of the interval the weight is valid for
         * @see SAXWeightsHandler::EdgeFloatTimeLineRetriever::addEdgeWeight
         */
        void addEdgeWeight(const std::string &id,
                           SUMOReal val, SUMOTime beg, SUMOTime end) const throw();

    private:
        /// @brief The network edges shall be obtained from
        MSNet &myNet;

    };


    /**
     * @class EdgeFloatTimeLineRetriever_EdgeTravelTime
     * @brief Obtains edge travel times from a weights handler and stores them within the edges
     * @see SAXWeightsHandler::EdgeFloatTimeLineRetriever
     */
    class EdgeFloatTimeLineRetriever_EdgeTravelTime : public SAXWeightsHandler::EdgeFloatTimeLineRetriever {
    public:
        /// @brief Constructor
        EdgeFloatTimeLineRetriever_EdgeTravelTime(MSNet &net) throw() : myNet(net) {}

        /// @brief Destructor
        ~EdgeFloatTimeLineRetriever_EdgeTravelTime() throw() { }

        /** @brief Adds a travel time for a given edge and time period
         *
         * @param[in] id The id of the object to add a weight for
         * @param[in] val The travel time
         * @param[in] beg The begin of the interval the weight is valid for
         * @param[in] end The end of the interval the weight is valid for
         * @see SAXWeightsHandler::EdgeFloatTimeLineRetriever::addEdgeWeight
         */
        void addEdgeWeight(const std::string &id,
                           SUMOReal val, SUMOTime beg, SUMOTime end) const throw();

    private:
        /// @brief The network edges shall be obtained from
        MSNet &myNet;

    };


protected:
    /// @brief The options to get the names of the files to load and further information from
    OptionsCont &myOptions;

    /// @brief The edge control builder to use
    NLEdgeControlBuilder &myEdgeBuilder;

    /// @brief The junction control builder to use
    NLJunctionControlBuilder &myJunctionBuilder;

    /// @brief The detector control builder to use
    NLDetectorBuilder &myDetectorBuilder;

    /// @brief The net to fill
    MSNet &myNet;

    /// @brief The handler used to parse the net
    NLHandler &myXMLHandler;

private:
    /** @brief invalidated copy operator */
    NLBuilder(const NLBuilder &s);

    /** @brief invalidated assignment operator */
    NLBuilder &operator=(const NLBuilder &s);

};


#endif

/****************************************************************************/

