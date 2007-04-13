/****************************************************************************/
/// @file    SUMOSAXHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: SUMOSAXHandler.h 3736 2007-03-30 12:47:23 +0200 (Fr, 30 Mrz 2007) dkrajzew $
///
// The basic SAX-handler for SUMO-files
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
#ifndef SUMOSAXHandler_h
#define SUMOSAXHandler_h
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
#include <utils/xml/GenericSAXHandler.h>
#include <utils/xml/XMLSnippletStorage.h>
#include <utils/common/FileErrorReporter.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOSAXHandler
 * @brief Base class for XML-loading. 
 *
 * This class knows all tags SUMO uses, so all SUMO-XML - loading classes 
 *  should be derived from it.
 */
class SUMOSAXHandler : public FileErrorReporter,
            public GenericSAXHandler
{
public:
    /// Constructor
    SUMOSAXHandler(const std::string &filetype,
                   const std::string &file="");

    /// Destructor
    virtual ~SUMOSAXHandler();

    ///{  Handlers for the SAX ErrorHandler interface
    /// called on a XML-warning; the error is reported to the SErrorHandler
    void warning(const SAXParseException& exception);

    /// called on a XML-error; the error is reported to the SErrorHandler
    void error(const SAXParseException& exception);

    /// called on a XML-fatal error; the error is reported to the SErrorHandler
    void fatalError(const SAXParseException& exception);
    ///}

protected:
    /// Begins processing of an unknown snippet
    void addUnknownSnippet(const std::string &name, const Attributes &attrs);

    /// Adds characters
    void addSnippetCharacters(const std::string &chars);

    /// Moves one level down
    void closeSnippet();

    /// Writes the snippet to the given stream
    void flushSnippet(std::ostream &strm, int level);

    /// Deletes the loaded snippets
    void deleteSnippet();

    /// Returns loaded snippet, detaching it from the parent
    XMLSnippletStorage *extractSnippet();

protected:
    XMLSnippletStorage *myCurrentSnippet;

private:
    /// invalidated copy constructo
    SUMOSAXHandler(const SUMOSAXHandler &s);

    /// invalidated assignment operator
    const SUMOSAXHandler &operator=(const SUMOSAXHandler &s);

};


#endif

/****************************************************************************/

