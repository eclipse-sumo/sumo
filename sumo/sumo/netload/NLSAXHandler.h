#ifndef NLSAXHandler_h
#define NLSAXHandler_h
/***************************************************************************
                          NLSAXHandler.h
			  Virtual handler that reacts on incoming tags;
			  parent to the handlers that represent the parsing
			  steps
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
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
// Revision 1.5  2002/06/11 13:44:34  dkrajzew
// Windows eol removed
//
// Revision 1.4  2002/05/14 04:54:25  dkrajzew
// Unexisting files are now catched independent to the Xerces-error mechanism; error report generation moved to XMLConvert
//
// Revision 1.3  2002/04/17 11:18:48  dkrajzew
// windows-newlines removed
//
// Revision 1.2  2002/04/15 07:07:56  dkrajzew
// new loading paradigm implemented
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:25  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:46  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:11  traffic
// moved from netbuild
//
// Revision 1.4  2001/08/16 12:53:59  traffic
// further exception handling (now validated) and new comments
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "../utils/AttributesHandler.h"
#include "../utils/GenericSAX2Handler.h"
#include "NLNetBuilder.h"
#include "NLTags.h"
#include <map>
#include <string>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class AttributeList;
class NLContainer;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NLSAXHandler
 * NLSAXHandler is an extended GenericSAX2Hendler which provides error handling.
 * This handler has no parsing functionality which is only implemented in the
 * derived classes, where each class solves a single step of the parsing
 */
class NLSAXHandler : public GenericSAX2Handler {
protected:
    /// the container (storage) for build data
    NLContainer                &myContainer;
    /// the handler for the SAX2-attributes
    AttributesHandler          _attrHandler;
    /// the definition of what to load
    LoadFilter                 _filter;
    /// the name of the file that is currently build
    std::string                _file;
private:
    static Tag  _tags[21];
public:
    /// standard constructor
    NLSAXHandler(NLContainer &container, LoadFilter filter);
    /// standard destructor
    ~NLSAXHandler();
    /// returns the begin of processing message
    virtual std::string getMessage() const = 0;
    /// does nothing; just for allowing of instantiations of derived classed
    virtual void myStartElement(int element, const std::string &name, const Attributes &attrs);
    /// does nothing; just for allowing of instantiations of derived classed
    virtual void myEndElement(int element, const std::string &name);
    /// does nothing; just for allowing of instantiations of derived classed
    virtual void myCharacters(int element, const std::string &name, const std::string &chars);
    /// sets the file name
    void setFileName(const std::string &file);
    // -----------------------------------------------------------------------
    //  Handlers for the SAX ErrorHandler interface
    // -----------------------------------------------------------------------
    /// called on a XML-warning; the error is reported to the SErrorHandler
    virtual void warning(const SAXParseException& exception);
    /// called on a XML-error; the error is reported to the SErrorHandler
    virtual void error(const SAXParseException& exception);
    /// called on a XML-fatal error; the error is reported to the SErrorHandler
    virtual void fatalError(const SAXParseException& exception);
protected:
    /** adds the message about the occured error to the error handler
	after building it */
    void setError(const std::string &type, const SAXParseException& exception);
    /** returns the information whether instances belonging to the
        given class of data shall be extracted during this parsing */
    bool wanted(LoadFilter filter) const;
private:
    /** invalid copy constructor */
    NLSAXHandler(const NLSAXHandler &s);
    /** invalid assignment operator */
    NLSAXHandler &operator=(const NLSAXHandler &s);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NLSAXHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
