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
// Revision 1.4  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.3  2003/03/03 15:22:38  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:07  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <sax2/SAX2XMLReader.hpp>
#include <framework/XMLPScanToken.hpp>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include "ROTypedRoutesLoader.h"


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
class ROTypedXMLRoutesLoader : public ROTypedRoutesLoader,
                               public SUMOSAXHandler {
public:
    /// Constructor
    ROTypedXMLRoutesLoader(RONet &net, const std::string &file="");

    /// Destructor
    virtual ~ROTypedXMLRoutesLoader();

    /// Closes the reading of routes
    void closeReading();

    /// Reads all routes from the file
    virtual bool addAllRoutes();

    /// called when the document has ended
    void endDocument();

protected:
    /// Begins a stepwise reading
    bool startReadingSteps();

    /// Reads the next route
    bool readNextRoute(long start);

    /// initialises the reading
    bool myInit(OptionsCont &options);

protected:
    /// The parser used
    SAX2XMLReader *_parser;

    /// Information about the current position within the file
    XMLPScanToken _token;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ROTypedXMLRoutesLoader.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

