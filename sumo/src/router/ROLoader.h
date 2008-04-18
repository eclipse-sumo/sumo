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
    /// @todo Refactor this; a network instance should be given and filled
    virtual RONet *loadNet(ROAbstractEdgeBuilder &eb);

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
     * Checks whether the given option name is set and his value is one
     *  or a set of valid (existing) files. This is done via a call to
     *  "OptionsCont::isUsableFileList" (which generates a proper error 
     *  message).
     *
     * If the given files are valid, the proper instance is built using
     *  "buildNamedHandler" and if this could be done, it is added to
     *  the list of route handlers to use ("myHandler")
     *
     * Returns whether the wished handlers could be built.
     *
     * @param[in] optionName The name of the option that refers to which handler and which files shall be used
     * @param[in] net The net to assign to the built handlers
     * @return Whether the wished handler(s) could be built
     */
    bool openTypedRoutes(const std::string &optionName, RONet &net) throw();


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
    ROAbstractRouteDefLoader* buildNamedHandler(const std::string &optionName, 
        const std::string &file, RONet &net) throw(ProcessError);


    void writeStats(SUMOTime time, SUMOTime start, int absNo);


    /** @brief Deletes all handlers and clears their container ("myHandler") */
    void destroyHandlers() throw();


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
    /// @brief Invalidated copy constructor
    ROLoader(const ROLoader &src);

    /// @brief Invalidated assignment operator
    ROLoader &operator=(const ROLoader &src);

};


#endif

/****************************************************************************/

