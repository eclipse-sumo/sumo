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
#include <stdexcept>


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class XMLBuildingException
 *
 * Basic class for all following exception classes which are exceptions
 * which may be thrown during the parsing of XML-files.
 */
class XMLBuildingException : public runtime_error
{
public:
    /** constructor */
    XMLBuildingException(const string &message)
        : runtime_error(message)
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
    XMLIdNotKnownException(const string &object, const string &id)
        : XMLBuildingException("The object " + object + " with the id " + id
                        + " is not known")
    {}

};


/**
 * @class XMLIdAlreadyUsedException
 * thrown when a duplicate key for the same object type is given
 */
class XMLIdAlreadyUsedException : public XMLBuildingException
{
public:
    /** constructor */
    XMLIdAlreadyUsedException(const string &object, const string &id)
        : XMLBuildingException("Another " + object + " with the id " + id + " exists")
    {}

};


/**
 * @class XMLDepartLaneDuplicationException
 * thrown when an edge has two or more defined depart lanes
 */
class XMLDepartLaneDuplicationException : public XMLBuildingException
{
public:
    /** constructor */
    XMLDepartLaneDuplicationException(const string &id)
        : XMLBuildingException("The lane with the id " + id
                        + " was tried to be added as a depart lane to an edge which already has a depart lane")
    {}

};


/**
 * @class XMLInvalidChildException
 * thrown when a child to add to a larger structure is invalid
 * (f.i. was not formerly known)
 */
class XMLInvalidChildException : public XMLBuildingException
{
public:
    /** constructor */
    XMLInvalidChildException(const string &object, const string &id)
        : XMLBuildingException("The object " + object + " with the id " + id
                        + " is not nested properly")
    {}

};


/**
 * @class XMLListEmptyException
 * thrown when an empty list which should have values is used
 */
class XMLListEmptyException : public XMLBuildingException
{
public:
    /** constructor */
    XMLListEmptyException(const string &object, const string &id)
        : XMLBuildingException("The list belonging to the '" + object + "' with the id '"
                        + id + "' is empty")
    {}

};


#endif

/****************************************************************************/
