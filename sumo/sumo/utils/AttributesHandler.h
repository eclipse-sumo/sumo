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
 * AttributesHandler
 * To allow a better access to the SAX2-Attributes considering their representation
 * as unsigned short * and the so needed conversion utils, this class allows
 * to access them via a numerical id supplied before parsing of the document
 */
class AttributesHandler {
private:
    /** the type of the map from ids to their unicode-string representation */
    typedef std::map<int, unsigned short*> TagMap;
    /** the map from ids to their unicode-string representation */
    TagMap _tags;
public:
    /** constructor */
    AttributesHandler();
    /** destuctor */
    ~AttributesHandler();
    /** method to assign an id to a name; the name will be transcoded into unicode */
    void add(int id, const std::string &name);
    /** returns the named (by id) attribute as a bool */
    bool getBool(const Attributes &attrs, int id) const;
    /** returns the named (by id) attribute as an int */
    int getInt(const Attributes &attrs, int id) const;
    /** returns the named (by id) attribute as a string */    
    std::string getString(const Attributes &attrs, int id) const;
    /** returns the named (by id) attribute as a long */
    long getLong(const Attributes &attrs, int id) const;
    /** returns the named (by id) attribute as a float */
    float getFloat(const Attributes &attrs, int id) const;
    /** returns the named (by id) attribute as a c-string */
    char *getCharP(const Attributes &attrs, int id) const;
    /** checks whether the id was not previously set */
    void check(int id) const;
    /** converts from c++-string into unicode */
    unsigned short *convert(const std::string &name) const;
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "AttributesHandler.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// 

