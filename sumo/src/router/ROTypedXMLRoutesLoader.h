#ifndef ROTypedXMLRoutesLoader_h
#define ROTypedXMLRoutesLoader_h
//---------------------------------------------------------------------------//
//                        ROTypedXMLRoutesLoader.h -
//  The basic class for loading routes from XML-files
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.11  2006/01/26 08:37:24  dkrajzew
// removed warnings 4786
//
// Revision 1.10  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.9  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/05/04 08:55:13  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.7  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.6  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.5  2003/08/18 12:44:54  dkrajzew
// xerces 2.2 and later compatibility patched
//
// Revision 1.4  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.3  2003/03/03 15:22:38  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <sax2/SAX2XMLReader.hpp>
#include <framework/XMLPScanToken.hpp>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include "ROAbstractRouteDefLoader.h"


/* =========================================================================
 * xerces 2.2 compatibility
 * ======================================================================= */
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


/* =========================================================================
 * class declarations
 * ======================================================================= */
class RONet;
class Options;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ROTypedXMLRoutesLoader
 * Base class for loaders which load route definitions which use XML-derived
 * formats. Some methods as the initialisation and the file processing, together
 * with the need for a parser are common to all such loaders.
 */
class ROTypedXMLRoutesLoader : public ROAbstractRouteDefLoader,
                               public SUMOSAXHandler {
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

