/****************************************************************************/
/// @file    NLBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The main interface for loading a microsim
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
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
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
class NLBuilder
{
public:
    /** @brief Constructor
     *
     * @param[in] oc The options to use
     * @param[in, out] net The network to fill
     * @param[in] eb The builder of edges to use
     * @param[in] jb The builder of junctions to use
     * @param[in] db The detector builder to use
     * @param[in] tb The trigger builder to use
     * @param[in] sb The geometric shapes builder to use
     * @param[in] xmlHandler The xml handler to use
     */
    NLBuilder(const OptionsCont &oc, MSNet &net,
              NLEdgeControlBuilder &eb, NLJunctionControlBuilder &jb,
              NLDetectorBuilder &db, NLTriggerBuilder &tb,
              NLGeomShapeBuilder &sb, NLHandler &xmlHandler);


    /// @brief Destructor
    virtual ~NLBuilder();


    /// the net loading method
    virtual bool build();

protected:
    /// loads a described subpart form the given list of files
    bool load(const std::string &mmlWhat, SAX2XMLReader &parser);

    /// Closes the net building process
    void buildNet();

    /// Builds the route loader control
    MSRouteLoaderControl *buildRouteLoaderControl(const OptionsCont &oc);


    /**
     * @class EdgeFloatTimeLineRetriever_EdgeWeight
     * @brief Obtains edge weights from a weights handler and stores them within the edges
     *
     * @todo Why is the network not a reference?
     */
class EdgeFloatTimeLineRetriever_EdgeWeight : public SAXWeightsHandler::EdgeFloatTimeLineRetriever
    {
    public:
        /// @brief Constructor
        EdgeFloatTimeLineRetriever_EdgeWeight(MSNet *net);

        /// @brief Destructor
        ~EdgeFloatTimeLineRetriever_EdgeWeight();

        /// Sets the given value as the edge weight for the given period
        void addEdgeWeight(const std::string &id,
                           SUMOReal val, SUMOTime beg, SUMOTime end);

    private:
        /// @brief The network edges shall be obtained from
        MSNet *myNet;

    };


protected:
    /// @brief The options to get the names of the files to load and further information from
    const OptionsCont &myOptions;

    /// @brief The edge control builder to use
    NLEdgeControlBuilder &myEdgeBuilder;

    /// @brief The junction control builder to use
    NLJunctionControlBuilder &myJunctionBuilder;

    /// @brief The detector control builder to use
    NLDetectorBuilder &myDetectorBuilder;

    /// @brief The trigger control builder to use
    NLTriggerBuilder &myTriggerBuilder;

    /// The geometry shapes builder to use
    NLGeomShapeBuilder &myShapeBuilder;

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

