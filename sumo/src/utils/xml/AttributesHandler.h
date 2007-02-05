/****************************************************************************/
/// @file    AttributesHandler.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 15 Apr 2002
/// @version $Id$
///
// This class realises the access to the
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
#ifndef AttributesHandler_h
#define AttributesHandler_h
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

#include <xercesc/sax2/Attributes.hpp>
#include <string>
#include <map>


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
 * @class AttributesHandler
 * To allow a better access to the SAX2-Attributes considering their representation
 * as unsigned short * and the so needed conversion utils, this class allows
 * to access them via a numerical id supplied before parsing of the document
 */
class AttributesHandler
{
public:
    /** the structure that describes the relationship between an attribute
        name and its numerical representation */
    struct Attr
    {
        /// The xml-attribute-name (latin1)
        const char *name;
        /// The numerical representation of the attribute
        int key;
    };

public:
    /** constructor */
    AttributesHandler();

    /** constructor */
    AttributesHandler(Attr *attrs, int noAttrs);

    /** destructor */
    virtual ~AttributesHandler();

    /** @brief method to assign an id to a name;
        the name will be transcoded into unicode */
    void add(int id, const std::string &name);

    bool hasAttribute(const Attributes &attrs, int id);
    bool hasAttribute(const Attributes &attrs, const std::string &id);

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

    /** returns the named (by id) attribute as a c-string */
    char *getCharP(const Attributes &attrs, int id) const;

    /** checks whether the id was not previously set */
    void check(int id) const;

    /** converts from c++-string into unicode */
    XMLCh *convert(const std::string &name) const;
private:
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

private:
    /** invalidated copy constructor */
    AttributesHandler(const AttributesHandler &s);

    /** invalidated assignment operator */
    AttributesHandler &operator=(const AttributesHandler &s);

private:
    /** the type of the map from ids to their unicode-string representation */
    typedef std::map<int, XMLCh*> AttrMap;

    /** the map from ids to their unicode-string representation */
    AttrMap myPredefinedTags;

    /** the type of the map from ids to their unicode-string representation */
    typedef std::map<std::string, XMLCh*> StrAttrMap;

    /** the map from ids to their unicode-string representation */
    mutable StrAttrMap myStrTags;


};


#endif

/****************************************************************************/

