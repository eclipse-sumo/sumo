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
class ROAbstractEdgeBuilder;
class ROVehicleBuilder;
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
 * Is capable to either load all routes in one step or go through them step wise.
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

    /// Parse the supplementary-weights-file.
    void loadSupplementaryWeights(RONet& net);

    /** @brief Builds and opens all route loaders
        Route loaders are derived from ROAbstractRouteDefLoader */
    size_t openRoutes(RONet &net, SUMOReal gBeta, SUMOReal gA);

    /** @brief Loads routes stepwise
        This is done for all previously build route loaders */
    virtual void processRoutesStepWise(SUMOTime start, SUMOTime end,
                                       RONet &net, SUMOAbstractRouter<ROEdge,ROVehicle> &router);

    /** @brief Loads all routes at once
        This is done for all previously build route loaders */
    virtual void processAllRoutes(SUMOTime start, SUMOTime end,
                                  RONet &net, SUMOAbstractRouter<ROEdge,ROVehicle> &router);

    /** @brief Ends route reading
        This is done for all previously build route loaders */
    void closeReading();

    bool makeSingleStep(SUMOTime end, RONet &net, SUMOAbstractRouter<ROEdge,ROVehicle> &router);

    friend class GUIRouterRunThread;

protected:
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
    ROAbstractRouteDefLoader* buildNamedHandler(
        const std::string &optionName, const std::string &file
        , RONet &net);

    void checkFile(const std::string &optionName
                   , const std::string &file);

    void writeStats(SUMOTime time, SUMOTime start, int absNo);


protected:
    /// Options to use
    OptionsCont &myOptions;

    /// Definition of route loader list
    typedef std::vector<ROAbstractRouteDefLoader*> RouteLoaderCont;

    /// List of route loaders
    RouteLoaderCont myHandler;

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

