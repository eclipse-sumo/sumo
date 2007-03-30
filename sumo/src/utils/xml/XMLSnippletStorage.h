/****************************************************************************/
/// @file    XMLSnippletStorage.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 27 Mar 2007
/// @version $Id: XMLSnippletStorage.h 3486 2007-02-19 13:38:00 +0100 (Mo, 19 Feb 2007) behrisch $
///
// Storage for XML-snipplets
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
#ifndef XMLSnippletStorage_h
#define XMLSnippletStorage_h
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

#include <iostream>
#include <vector>
#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class XMLSnippletStorage
 * @brief Storage for XML-snipplets
 */
class XMLSnippletStorage
{
public:
    /// Constructor (parent may be 0)
    XMLSnippletStorage(XMLSnippletStorage *parent, const std::string &name);

    /// Destructor
    ~XMLSnippletStorage();

    /// Adds an attribute/value pair
    void addAttribute(const std::string &name, const std::string &val);

    /// Adds characters
    void addCharacters(const std::string &chars);

    /// Adds a child
    XMLSnippletStorage *addChild(const std::string &name);

    /// Returns the parent element
    XMLSnippletStorage *getParent();

    /// Writes to the given stream including the children
    void flush(std::ostream &strm, int level);

    /// Returns a copy of this element and his children
    XMLSnippletStorage *duplicate(XMLSnippletStorage *parent=0);

    /// Returns the name of the element
    const std::string &getName() const;

protected:
    /// The parent element
    XMLSnippletStorage *myParent;

    /// The qname of this element
    std::string myName;

    /// Definition of a name/value pair describing an attribute
    typedef std::pair<std::string, std::string> Attribute;

    /// List of this element's attributes
    std::vector<Attribute> myAttributes;

    /// List of this element's children
    std::vector<XMLSnippletStorage*> myChildren;

    /// This element's characters
    std::string myCharacters;

};


#endif

/****************************************************************************/

