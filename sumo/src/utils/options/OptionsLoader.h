#ifndef OptionsLoader_h
#define OptionsLoader_h
/***************************************************************************
                          OptionsLoader.h
              Loads a configuration (XML) using a SAX-Parser
              The class itself is a derivation of the
              SAX-HandlerBase
                             -------------------
    project              : SUMO
    begin                : Mon, 17 Dec 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.6  2005/09/15 12:21:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/05/04 09:28:01  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2004/11/23 10:36:02  dkrajzew
// debugging
//
// Revision 1.3  2003/08/18 12:49:30  dkrajzew
// xerces 2.2 and later compatibility patched
//
// Revision 1.2  2003/02/07 10:51:59  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:58:18  dkrajzew
// initial release for utilities that handle program options
//
// Revision 1.5  2002/07/31 17:30:06  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.5  2002/07/11 07:42:59  dkrajzew
// Usage of relative pathnames within configuration files implemented
//
// Revision 1.4  2002/06/11 15:58:25  dkrajzew
// windows eol removed
//
// Revision 1.3  2002/05/14 04:45:50  dkrajzew
// Bresenham added; some minor changes; windows eol removed
//
// Revision 1.2  2002/04/26 10:08:39  dkrajzew
// Windows eol removed
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:01  dkrajzew
// new version-free project name
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include <string>


/* =========================================================================
 * xerces 2.2 compatibility
 * ======================================================================= */
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class OptionsLoader
 * A SAX - HandlerBase - derivation for the parsing of an XML-configuration
 * file.
 * Usage:
 * The file must not contain unknown informations as errors are reported
 *  then.
 * The class assumes all options are unset or using default values only.
 */
class OptionsLoader : public HandlerBase {
public:
    /** standard constructor */
    OptionsLoader(OptionsCont *oc, const char *file, bool verbose);

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
    OptionsCont *_options;

    /** the name of the currently parsed option */
    std::string _item;
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
