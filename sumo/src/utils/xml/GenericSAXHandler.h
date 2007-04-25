/****************************************************************************/
/// @file    GenericSAXHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: GenericSAXHandler.h 3712 2007-03-28 14:23:50 +0200 (Mi, 28 Mrz 2007) dkrajzew $
///
// A combination between a GenericSAXHandler and an AttributesHandler
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
#ifndef GenericSAXHandler_h
#define GenericSAXHandler_h
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
#include <map>
#include <stack>
#include <sstream>
#include <vector>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>


// ===========================================================================
// xerces 2.2 compatibility
// ===========================================================================
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GenericSAXHandler
 *
 * This class is a combination of an AttributesHandler and a
 *  GenericSAX2Handler allowing direct access to and handling of XML-tags
 *  (elements) and attributes.
 * By now, almost every class that handles XML-data is derived from this
 *  class.
 */
class GenericSAXHandler : public DefaultHandler
{
public:
    /** the structure that describes the relationship between an attribute
        name and its numerical representation */
    struct Attr
    {
        /// The xml-attribute-name (ascii)
        const char *name;
        /// The numerical representation of the attribute
        int key;
    };

    /** a tag name with its numerical representation */
    struct Tag
    {
        /// The xml-element-name (ascii)
        const char *name;
        /// The numerical representation of the attribute
        int key;
    };


public:
    /** constructor */
    GenericSAXHandler();

    /** parametrised constructor */
    GenericSAXHandler(
        Tag *tags, Attr *attrs);

    /** destructor */
    virtual ~GenericSAXHandler();


    //{ methods for dealing with attributes
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
    //}


    //{ methods for dealing with attributes
    /** @brief method to assign an id to a name;
        the name will be transcoded into unicode */
    void add(int id, const std::string &name);

    bool hasAttribute(const Attributes &attrs, int id);
    bool hasAttribute(const Attributes &attrs, const std::string &id);
    bool hasAttribute(const Attributes &attrs, const XMLCh * const id);

    /** returns the named (by id) attribute as a bool */
    bool getBool(const Attributes &attrs, int id) const;
    bool getBoolSecure(const Attributes &attrs, int id, bool val) const;
    bool getBool(const Attributes &attrs, const std::string &id) const;
    bool getBoolSecure(const Attributes &attrs, const std::string &id, bool val) const;

    /** returns the named (by id) attribute as an int */
    int getInt(const Attributes &attrs, int id) const;
    int getIntSecure(const Attributes &attrs, int id, int def) const;
    int getInt(const Attributes &attrs, const std::string &id) const;
    int getIntSecure(const Attributes &attrs, const std::string &id, int def) const;

    /** returns the named (by id) attribute as a string */
    std::string getString(const Attributes &attrs, int id) const;
    std::string getStringSecure(const Attributes &attrs, int id,
                                const std::string &str) const;
    std::string getString(const Attributes &attrs, const std::string &id) const;
    std::string getStringSecure(const Attributes &attrs, const std::string &id,
                                const std::string &str) const;

    /** returns the named (by id) attribute as a long */
    long getLong(const Attributes &attrs, int id) const;
    long getLongSecure(const Attributes &attrs, int id, long def) const;

    /** returns the named (by id) attribute as a SUMOReal */
    SUMOReal getFloat(const Attributes &attrs, int id) const;
    SUMOReal getFloatSecure(const Attributes &attrs, int id, SUMOReal def) const;
    SUMOReal getFloat(const Attributes &attrs, const std::string &id) const;
    SUMOReal getFloatSecure(const Attributes &attrs, const std::string &id, SUMOReal def) const;
    SUMOReal getFloat(const Attributes &attrs, const XMLCh * const id) const;

    /** returns the named (by id) attribute as a c-string */
    char *getCharP(const Attributes &attrs, int id) const;
    //}

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

    /** build an error description */
    std::string buildErrorMessage(const std::string &file,
                                  const std::string &type,
                                  const SAXParseException& exception);


private:
    /** converts from c++-string into unicode */
    XMLCh *convert(const std::string &name) const;

    /** converts a tag from its string into its numerical representation */
    int convertTag(const std::string &tag) const;

private:
    //{ methods for dealing with attributes
    /** returns the xml-name of an attribute in a way that no NULL-pointer
        exceptions may occure */
    const XMLCh *const getAttributeNameSecure(int id) const;

    /** returns the xml-name of an attribute in a way that no NULL-pointer
        exceptions may occure */
    const XMLCh *const getAttributeNameSecure(const std::string &id) const;

    /** returns the xml-name of an attribute in a way that no NULL-pointer
        exceptions may occure */
    const XMLCh *getAttributeValueSecure(const Attributes &attrs,
                                         int id) const;

    /** returns the xml-name of an attribute in a way that no NULL-pointer
        exceptions may occure */
    const XMLCh *getAttributeValueSecure(const Attributes &attrs,
                                         const std::string &id) const;

    /** checks whether the id was not previously set */
    void check(int id) const;
    //}

private:
    //{ Variables for attributes parsing
    /** the type of the map from ids to their unicode-string representation */
    typedef std::map<int, XMLCh*> AttrMap;

    /** the map from ids to their unicode-string representation */
    AttrMap myPredefinedTags;

    /** the type of the map from ids to their unicode-string representation */
    typedef std::map<std::string, XMLCh*> StrAttrMap;

    /** the map from ids to their unicode-string representation */
    mutable StrAttrMap myStrTags;
    //}

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

    /// A list of characters string obtained so far to build the complete characters string at the end
    std::vector<std::string> myCharactersVector;


};


#endif

/****************************************************************************/

