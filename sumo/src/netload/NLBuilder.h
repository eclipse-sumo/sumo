/****************************************************************************/
/// @file    NLBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Container for MSNet during its building
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
#include "NLLoadFilter.h"
#include "NLGeomShapeBuilder.h"


// ===========================================================================
// xerces 2.2 compatibility
// ===========================================================================
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


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
class GNEImageProcWindow;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NLBuilder
 * @brief The class is the main interface to load simulations.
 *
 * It is a black-box where only the options and factories must be supplied
 * on the constructor call
 */
class NLBuilder
{
public:
    /// constructor
    NLBuilder(const OptionsCont &oc, MSNet &net,
              NLEdgeControlBuilder &eb, NLJunctionControlBuilder &jb,
              NLDetectorBuilder &db, NLTriggerBuilder &tb,
              NLGeomShapeBuilder &sb, NLHandler &xmlHandler);

    /// Destructor
    virtual ~NLBuilder();

    /// the net loading method
    virtual bool build();

    friend class GNEImageProcWindow;

protected:
    /// loads a described subpart form the given list of files
    bool load(const std::string &mmlWhat, NLLoadFilter what,
              SAX2XMLReader &parser);

    /// parses the files using the given initialised parser
    bool parse(const std::string &mmlWhat, SAX2XMLReader &parser);

    /// Closes the net building process
    void buildNet();

    /// Builds the route loader control
    MSRouteLoaderControl *buildRouteLoaderControl(const OptionsCont &oc);


    /**
     * @class EdgeFloatTimeLineRetriever_EdgeWeight
     * @brief Obtains edge weights from a weights handler and stores them within the edges
     */
class EdgeFloatTimeLineRetriever_EdgeWeight : public SAXWeightsHandler::EdgeFloatTimeLineRetriever
    {
    public:
        /// Constructor
        EdgeFloatTimeLineRetriever_EdgeWeight(MSNet *net);

        /// Destructor
        ~EdgeFloatTimeLineRetriever_EdgeWeight();

        /// Sets the given value as the edge weight for the given period
        void addEdgeWeight(const std::string &id,
                           SUMOReal val, SUMOTime beg, SUMOTime end);

    private:
        /// The network edges shall be obtained from
        MSNet *myNet;

    };


protected:
    /// the options to get the names from
    const OptionsCont &m_pOptions;

    /// The edge control builder to use
    NLEdgeControlBuilder &myEdgeBuilder;

    /// The junction control builder to use
    NLJunctionControlBuilder &myJunctionBuilder;

    /// The detector control builder to use
    NLDetectorBuilder &myDetectorBuilder;

    /// The trigger control builder to use
    NLTriggerBuilder &myTriggerBuilder;

    /// The geometry shapes builder to use
    NLGeomShapeBuilder &myShapeBuilder;

    /// The net to fill
    MSNet &myNet;

    /// The handler used to parse the net
    NLHandler &myXMLHandler;

private:
    /** invalidated copy operator */
    NLBuilder(const NLBuilder &s);

    /** invalidated assignment operator */
    NLBuilder &operator=(const NLBuilder &s);

};


#endif

/****************************************************************************/

