/****************************************************************************/
/// @file    XMLBuildingExceptions.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// missing_desc
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
#ifndef XMLBuildingExceptions_h
#define XMLBuildingExceptions_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <stdexcept>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class XMLBuildingException
 *
 * Basic class for all following exception classes which are exceptions
 * which may be thrown during the parsing of XML-files.
 */
class XMLBuildingException : public std::runtime_error
{
public:
    /** constructor */
    XMLBuildingException(const std::string &message)
            : std::runtime_error(message)
    {}

};


/**
 * @class XMLIdNotKnownException
 * thrown when an object of the named class and id is not known
 * (was not loaded/parsed) but its parent is known
 */
class XMLIdNotKnownException : public XMLBuildingException
{
public:
    /** constructor */
    XMLIdNotKnownException(const std::string &object, const std::string &id)
            : XMLBuildingException("The object " + object + " with the id " + id
                                   + " is not known")
    {}

};






#endif

/****************************************************************************/
