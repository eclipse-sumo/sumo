#ifndef AttributesHandler_h
#define AttributesHandler_h
/***************************************************************************
                          AttributesHandler.h
                          This class realises the access to the
                            SAX2-Attributes
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
// Revision 1.3  2003/03/03 15:26:22  dkrajzew
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
#include <sax2/Attributes.hpp>
#include <string>
#include <map>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class AttributesHandler
 * To allow a better access to the SAX2-Attributes considering their representation
 * as unsigned short * and the so needed conversion utils, this class allows
 * to access them via a numerical id supplied before parsing of the document
 */
class AttributesHandler {
public:
    /** the structure that describes the relationship between an attribute
        name and its numerical representation */
    struct Attr { 
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

    /** returns the named (by id) attribute as a bool */
    bool getBool(const Attributes &attrs, int id) const;

    /** returns the named (by id) attribute as an int */
    int getInt(const Attributes &attrs, int id) const;

    /** returns the named (by id) attribute as an int */
    int getIntSecure(const Attributes &attrs, int id, int def) const;

    /** returns the named (by id) attribute as a string */
    std::string getString(const Attributes &attrs, int id) const;

    /** @brief returns the named (by id) attributes as a string;
        returns the third parameter when the attribute is not found */
    std::string getStringSecure(const Attributes &attrs, int id,
        const std::string &str) const;

    /** returns the named (by id) attribute as a long */
    long getLong(const Attributes &attrs, int id) const;

    /** returns a long even when no attribute is given */
    long getLongSecure(const Attributes &attrs, int id, long def) const;

    /** returns the named (by id) attribute as a float */
    float getFloat(const Attributes &attrs, int id) const;

    /** @brief returns the named (by id) attribute as a float
        returns the third parameter when the attribute does not
        exist or is not a float*/
    float getFloatSecure(const Attributes &attrs, int id, float def) const;

    /** returns the named (by id) attribute as a c-string */
    char *getCharP(const Attributes &attrs, int id) const;

    /** checks whether the id was not previously set */
    void check(int id) const;

    /** converts from c++-string into unicode */
    unsigned short *convert(const std::string &name) const;
private:
    /** returns the xml-name of an attribute in a way that no NULL-pointer
        exceptions may occure */
    const XMLCh *const getAttributeNameSecure(int id) const;

    /** returns the xml-name of an attribute in a way that no NULL-pointer
        exceptions may occure */
    const XMLCh *getAttributeValueSecure(const Attributes &attrs,
        int id) const;

private:
    /** invalidated copy constructor */
    AttributesHandler(const AttributesHandler &s);

    /** invalidated assignment operator */
    AttributesHandler &operator=(const AttributesHandler &s);

private:
    /** the type of the map from ids to their unicode-string representation */
    typedef std::map<int, unsigned short*> AttrMap;
    /** the map from ids to their unicode-string representation */
    AttrMap _tags;
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "AttributesHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
//
