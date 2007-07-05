/****************************************************************************/
/// @file    OptionsLoader.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// Loads a configuration (XML) using a SAX-Parser
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
#ifndef OptionsLoader_h
#define OptionsLoader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <string>


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


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OptionsLoader
 * A SAX - HandlerBase - derivation for the parsing of an XML-configuration
 * file.
 * Usage:
 * The file must not contain unknown informations as errors are reported
 *  then.
 * The class assumes all options are unset or using default values only.
 */
class OptionsLoader : public HandlerBase
{
public:
    /** standard constructor */
    OptionsLoader(const char *file, bool verbose);

    /** destructor */
    ~OptionsLoader();

    // -----------------------------------------------------------------------
    //  Handlers for the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    /** called on the occurence of the beginning of a tag; sets the name of
        the last item and the last item string */
    virtual void startElement(const XMLCh* const name,
                              AttributeList& attributes);

    /// called on the occurence of character data; nothing is done
    void characters(const XMLCh* const chars, const unsigned int length);

    /// called on the end of an element; nothing is done
    void endElement(const XMLCh* const name);

    // -----------------------------------------------------------------------
    //  Handlers for the SAX ErrorHandler interface
    // -----------------------------------------------------------------------
    /// called on a XML-warning; the error is reported to the SErrorHandler
    void warning(const SAXParseException& exception);

    /// called on a XML-error; the error is reported to the SErrorHandler
    void error(const SAXParseException& exception);

    /** called on a XML-fatal error; the error is reported to the
        SErrorHandler */
    void fatalError(const SAXParseException& exception);

    // ------------------------------------------------------------------------
    //  The io to the application
    // ------------------------------------------------------------------------
    /** returns the information whether an error occured */
    bool errorOccured();

private:
    /** checks the item whether it was default before setting it
        returns the information whether the item was set before (was not a default value) */
    bool setSecure(const std::string &name, bool value);

    /** checks the item whether it was default before setting it
        returns the information whether the item was set before (was not a default value) */
    bool setSecure(const std::string &name, const std::string &value);

private:
    /** invalid copy constructor */
    OptionsLoader(const OptionsLoader &s);

    /** invalid assignment operator */
    OptionsLoader &operator=(const OptionsLoader &s);

private:
    /** the information whether an error occured */
    bool  _error;

    /** the parsed file */
    const char  *_file;

    /** the information whether the builder shall be run in verbose mode */
    bool  _verbose;

    /** the container to store the informations into */
    OptionsCont &_options;

    /** the name of the currently parsed option */
    std::string _item;
};


#endif

/****************************************************************************/

