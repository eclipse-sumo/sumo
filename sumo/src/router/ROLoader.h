/****************************************************************************/
/// @file    ROLoader.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Loader for networks and route imports
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
#include <utils/common/FloatValueTimeLine.h>
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
class ROLoader
{
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
    bool loadWeights(RONet &net, const std::string &file,
                     bool useLanes);

    /// Parse the supplementary-weights-file.
    void loadSupplementaryWeights(RONet& net);

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
     */
    class EdgeFloatTimeLineRetriever_EdgeWeight : public SAXWeightsHandler::EdgeFloatTimeLineRetriever
    {
    public:
        /// Constructor
        EdgeFloatTimeLineRetriever_EdgeWeight(RONet *net);

        /// Destructor
        ~EdgeFloatTimeLineRetriever_EdgeWeight();

        /// Sets the given value as the edge weight for the given period
        void addEdgeWeight(const std::string &id,
                           SUMOReal val, SUMOTime beg, SUMOTime end);

    private:
        /// The network edges shall be obtained from
        RONet *myNet;

    };



    /**
     * @class EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight
     * @brief Obtains supplementary edge weights from a weights handler and stores them within the edges
     */
    class EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight
    {
    public:
        /**
         * enum Type
         * @brief Enumeration of possible supplementary weights
         */
        enum Type {
            /// The given weight is an absolute value
            ABSOLUTE,
            /// The given weight shall be added to the original
            ADD,
            /// The given weight shall be multiplied by the original
            MULT
        };

        /**
         * enum SingleWeightRetriever
         * @brief Retriever of one of the possible supplementary weights
         */
        class SingleWeightRetriever : public SAXWeightsHandler::EdgeFloatTimeLineRetriever
        {
        public:
            /// Constructor
            SingleWeightRetriever(Type type, EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight *parent);

            /// Destructor
            ~SingleWeightRetriever();

            /// Informs the parent about having obtained the given value
            void addEdgeWeight(const std::string &id,
                               SUMOReal val, SUMOTime beg, SUMOTime end);

        private:
            /// The parent
            EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight *myParent;

            /// The type of the supp weight this retriever gets
            Type myType;

        };

        /// Constructor
        EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight(RONet *net);

        /// Destructor
        ~EdgeFloatTimeLineRetriever_SupplementaryEdgeWeight();
        void addTypedWeight(Type type, const std::string &id,
                            SUMOReal val, SUMOTime beg, SUMOTime end);

        /// Returns the retriever for absolute values
        SingleWeightRetriever &getAbsoluteRetriever();

        /// Returns the retriever for additional values
        SingleWeightRetriever &getAddRetriever();

        /// Returns the retriever for multiplicative values
        SingleWeightRetriever &getMultRetriever();

    private:
        /**
         * struct SuppWeights
         * @brief A set of all three possible supplementary values of an edge
         */
        struct SuppWeights {
            /// The absolute time line
            FloatValueTimeLine *absolute;
            /// The additive time line
            FloatValueTimeLine *add;
            /// The multiplicative time line
            FloatValueTimeLine *mult;
        };

        /// A map from edges to supplementary weights
        std::map<ROEdge*, SuppWeights> myWeights;

        /// The network to get the edges from
        RONet *myNet;

        /// The retrievers (one for each type of supplementary weights)
        SingleWeightRetriever *myAbsoluteRetriever, *myAddRetriever,
        *myMultRetriever;

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

