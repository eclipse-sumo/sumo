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
// Revision 1.1  2002/04/08 07:21:24  traffic
// Initial revision
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
#include <sax/HandlerBase.hpp>
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
 * NLSAXHandler is an extended XML-SAX-Handler which provides a faster access 
 * to a enumeration of tags through a map and error handling.
 * This handler has no parsing functionality which is only implemented in the 
 * derived classes, where each class solves a single step of the parsing
 */
class NLSAXHandler : public HandlerBase {
private:
    /// definition of a map for XML-tag-names to their enumerations
    std::map<std::string, NLTag> m_Tags;
protected:
    /// the container (storage) for build data 
    NLContainer                *myContainer;
    /// the last XML-item for errorhandling
    std::string                     m_LastItem;
    /// the tag-name of the last XML-item for errorhandling and data assignment
    std::string                     m_LastName;
public:
    /// standard constructor
    NLSAXHandler(NLContainer *container);
    /// standard destructor
    ~NLSAXHandler();
    /// tag to enum conversion methods
    virtual NLTag convert(const XMLCh* const name);
    virtual NLTag convert(const std::string name);
    // -----------------------------------------------------------------------
    //  Handlers for the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    /** called on the occurence of the beginning of a tag; 
        sets the name of the last item and the last item string */
    virtual void startElement(const XMLCh* const name, AttributeList& attributes);
    /// called on the occurence of character data; nothing is done
    virtual void characters(const XMLCh* const chars, const unsigned int length);
    /// called on resetting the document parsing; nothing is done
    virtual void resetDocument();
    /// called whe the document parsing ends; nothing is done
    virtual void endDocument();
    /// called on the end of an element; nothing is done
    virtual void endElement(const XMLCh* const name);
    /// called on metainstructions; nothing is done
    virtual void processingInstruction(const   XMLCh* const    target, const XMLCh* const    data);
    // -----------------------------------------------------------------------
    //  Handlers for the SAX ErrorHandler interface
    // -----------------------------------------------------------------------
    /// called on a XML-warning; the error is reported to the SErrorHandler
    virtual void warning(const SAXParseException& exception);
    /// called on a XML-error; the error is reported to the SErrorHandler
    virtual void error(const SAXParseException& exception);
    /// called on a XML-fatal error; the error is reported to the SErrorHandler
    virtual void fatalError(const SAXParseException& exception);
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
