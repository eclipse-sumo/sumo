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
// Revision 1.1  2002/04/08 07:21:25  traffic
// Initial revision
//
// Revision 2.0  2002/02/14 14:43:28  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/13 15:48:20  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include <string>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * OptionsLoader
 * A SAX - HandlerBase - derivation for the parsing of an XML-configuration
 * file.
 * Usage:
 * The file must not contain unknown informations as errors are reported then.
 * The class assumes all options are unset or using default values only.
 */
class OptionsLoader : public HandlerBase {
 private:
    /** the information whether an error occured */
    bool  _error;
    /** the information whether warnings should be printed */
    bool  _warn;
    /** the parsed file */
    const char  *_file;
    /** the information whether the builder shall be run in verbose mode */
    bool  _verbose;
    /** the container to store the informations into */
    OptionsCont *_options;
    /** the name of the currently parsed option */
    std::string _item;
 public:
    /** standard constructor */
    OptionsLoader(OptionsCont *oc, const char *file, bool warn, bool verbose);
    /** destructor */
    ~OptionsLoader();
    // -----------------------------------------------------------------------
    //  Handlers for the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    /// called on the occurence of the beginning of a tag; sets the name of the last item and the last item string
    virtual void startElement(const XMLCh* const name, AttributeList& attributes);
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
    /// called on a XML-fatal error; the error is reported to the SErrorHandler
    void fatalError(const SAXParseException& exception);
    // ------------------------------------------------------------------------
    //  The io to the application 
    // ------------------------------------------------------------------------
    /** returns the information whether an error occured */
    bool errorOccured();
 private:
    /** invalid copy constructor */
    OptionsLoader(const OptionsLoader &s);
    /** invalid assignment operator */
    OptionsLoader &operator=(const OptionsLoader &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "OptionsLoader.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

