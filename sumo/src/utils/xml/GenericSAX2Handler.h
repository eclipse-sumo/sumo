#ifndef GenericSAX2Handler_h
#define GenericSAX2Handler_h
/***************************************************************************
                          GenericSAX2Handler.h
                          A class extending the SAX-parser functionality
                             -------------------
    begin                : Mon, 15 Apr 2002
    copyright            : (C) 2002 by Daniel Krajzewicz
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
    Attention!!!
    As one of few, this module is under the
        Lesser GNU General Public Licence
    *********************************************************************
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
 ***************************************************************************/
// $Log$
// Revision 1.4  2003/03/18 13:16:58  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/03 15:26:23  dkrajzew
// documentation added
//
// Revision 1.2  2003/02/07 10:53:52  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:54:04  dkrajzew
// initial commit for xml-related utility functions
//
// Revision 1.6  2002/06/11 14:38:21  dkrajzew
// windows eol removed
//
// Revision 1.5  2002/06/11 13:43:36  dkrajzew
// Windows eol removed
//
// Revision 1.4  2002/06/10 08:33:22  dkrajzew
// Parsing of strings into other data formats generelized; Options now recognize false numeric values; documentation added
//
// Revision 1.3  2002/04/17 11:19:57  dkrajzew
// windows-carriage returns removed
//
// Revision 1.2  2002/04/16 06:52:01  dkrajzew
// documentation added; coding standard attachements added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <stack>
#include <map>
#include <sstream>
#include <sax2/Attributes.hpp>
#include <sax2/DefaultHandler.hpp>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GenericSAX2Handler
 * GenericSAX2Handler is an extended SAX2-DefaultHandler which provides a faster access
 * to a enumeration of tags through a map
 * This handler has no parsing functionality which is only implemented in the
 * derived classes, where each class solves a single step of the parsing
 */
class GenericSAX2Handler : public DefaultHandler
{
public:
    /** a tag name with its numerical representation */
    struct Tag {
        /// The xml-element-name (latin1)
        const char *name;
        /// The numerical representation of the attribute
        int value;
    };

public:
    /** constructor */
    GenericSAX2Handler();

    /** constructor */
    GenericSAX2Handler(const Tag *tags, int tagNo);

    /** destructor */
    ~GenericSAX2Handler();

    /** adds a known tag to the list */
    void addTag(const std::string &name, int id);

    /** returns the information whether an error occured during the parsing */
    bool errorOccured() const;

    /** returns the information whether an unknown tag occured */
    bool unknownOccured() const;

    /** @brief The inherited method called when a new tag opens
        This method calls the user-implemented methof myStartElement */
    void startElement(const XMLCh* const uri, const XMLCh* const localname,
        const XMLCh* const qname, const Attributes& attrs);

    /** @brief The inherited method called when characters occured
        The characters are appended into a private buffer and given to
        myCharacters when the according tag is being closed */
    void characters(const XMLCh* const chars, const unsigned int length);

    /** @brief The inherited method called when a tag is being closed
        This method calls the user-implemented methods myCharacters and
        and myEndElement */
    void endElement(const XMLCh* const uri, const XMLCh* const localname,
        const XMLCh* const qname);

    /** called when ignorable whitespaces occure */
    void ignorableWhitespace(const XMLCh* const chars,
        const unsigned int length);

    /** called when the document shall be resetted */
    virtual void resetDocument();

    /** called when a XML-warning occures */
    virtual void warning(const SAXParseException& exception);

    /** called when a XML-error occures */
    virtual void error(const SAXParseException& exception);

    /** called when a XML-fatal error occures */
    virtual void fatalError(const SAXParseException& exception);

protected:
    /** @brief handler method for an opening tag to implement by derived classes
        This method is only called when the tag name was supplied by the user */
    virtual void myStartElement(int element, const std::string &name,
        const Attributes &attrs) = 0;

    /** @brief handler method for characters to implement by derived classes
        This method is only called when tha tag name was supplied by the user */
    virtual void myCharacters(int element, const std::string &name,
        const std::string &chars) = 0;

    /** @brief handler method for a closing tag to implement by derived classes
        This tag is only called when tha tag name was supplied by the user */
    virtual void myEndElement(int element, const std::string &name) = 0;

    /** a dump-methods that may be used to avoid "unused attribute"-warnings */
    void myStartElementDump(int element, const std::string &name,
        const Attributes &attrs);

    /** a dump-methods that may be used to avoid "unused attribute"-warnings */
    void myCharactersDump(int element, const std::string &name,
        const std::string &chars);

    /** a dump-methods that may be used to avoid "unused attribute"-warnings */
    void myEndElementDump(int element, const std::string &name);

    /** build an error description */
    std::string buildErrorMessage(const std::string &file,
        const std::string &type,
        const SAXParseException& exception);

protected:
    /** the type of the map the maps tag names to ints */
    typedef std::map<std::string, int> TagMap;

    /** the information whether an error occured during the parsing */
    bool _errorOccured;

    /** the information whether an unknown tag occured */
    bool _unknownOccured;

    /** the map of tag names to their internal numerical representation */
    TagMap _tagMap;

    /** the current position in the xml-tree as a stack */
    std::stack<int> _tagTree;

    /** the current characters */
    std::string _characters;

private:
    /** converts a tag from its string into its numerical representation */
    int convertTag(const std::string &tag) const;
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "GenericSAX2Handler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
//
