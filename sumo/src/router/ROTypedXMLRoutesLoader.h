/****************************************************************************/
/// @file    ROTypedXMLRoutesLoader.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// The basic class for loading routes from XML-files
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
#ifndef ROTypedXMLRoutesLoader_h
#define ROTypedXMLRoutesLoader_h
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
#include <sax2/SAX2XMLReader.hpp>
#include <framework/XMLPScanToken.hpp>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include "ROAbstractRouteDefLoader.h"


// ===========================================================================
// xerces 2.2 compatibility
// ===========================================================================
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class RONet;
class Options;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROTypedXMLRoutesLoader
 * Base class for loaders which load route definitions which use XML-derived
 * formats. Some methods as the initialisation and the file processing, together
 * with the need for a parser are common to all such loaders.
 */
class ROTypedXMLRoutesLoader : public ROAbstractRouteDefLoader,
            public SUMOSAXHandler
{
public:
    /// Constructor
    ROTypedXMLRoutesLoader(ROVehicleBuilder &vb, RONet &net,
                           SUMOTime begin, SUMOTime end, const std::string &file="");

    /// Destructor
    virtual ~ROTypedXMLRoutesLoader();

    /// Closes the reading of routes
    virtual void closeReading();

    /// called when the document has ended
    void endDocument();

    /// Returns the information whether no routes are available from this loader anymore
    bool ended() const;

    /// Initialises the handler for reading
    virtual bool init(OptionsCont &options);

protected:
    /** @brief Reads the until the specified time is reached
        Do read the comments on ROAbstractRouteDefLoader::myReadRoutesAtLeastUntil
        for the modalities! */
    bool myReadRoutesAtLeastUntil(SUMOTime time);

protected:
    /// Return the information whether a route was read
    virtual bool nextRouteRead() = 0;

    /// Initialises the reading of a further route
    virtual void beginNextRoute() = 0;

protected:
    /// The parser used
    SAX2XMLReader *_parser;

    /// Information about the current position within the file
    XMLPScanToken _token;

    /// Information whether the whole file has been parsed
    bool _ended;

};


#endif

/****************************************************************************/

