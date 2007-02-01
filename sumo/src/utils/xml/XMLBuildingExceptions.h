/****************************************************************************/
/// @file    XMLBuildingExceptions.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id: $
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

// ===========================================================================
// class definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * for XMLBuildingException
 * ----------------------------------------------------------------------- */
/**
 * @class XMLBuildingException
 *
 * Basic class for all following exception classes which are exceptions
 * which may be thrown during the parsing of XML-files.
 */
class XMLBuildingException
{
public:
    /** constructor */
    XMLBuildingException();

    /** destructor */
    virtual ~XMLBuildingException();

    /** returns the error message */
    virtual std::string getMessage(const std::string &obj,
                                   const std::string &id);

};


/* -------------------------------------------------------------------------
 * for XMLIdNotKnownException
 * ----------------------------------------------------------------------- */
/**
 * @class XMLIdNotKnownException
 * thrown when an object of the named class and id is not known
 * (was not loaded/parsed) but its parent is known
 */
class XMLIdNotKnownException : public XMLBuildingException
{
public:
    /** constructor */
    XMLIdNotKnownException(std::string object, std::string id);

    /** destructor */
    ~XMLIdNotKnownException();

    /** returns the error message */
    std::string getMessage(const std::string &obj, const std::string &id);

private:
    /** the object type with the unknown id */
    std::string   m_Object;

    /** the id of the parent */
    std::string   m_Id;

};


/* -------------------------------------------------------------------------
 * for XMLIdAlreadyUsedException
 * ----------------------------------------------------------------------- */
/**
 * @class XMLIdAlreadyUsedException
 * thrown when a duplicate key for the same object type is given
 */
class XMLIdAlreadyUsedException : public XMLBuildingException
{
public:
    /** constructor */
    XMLIdAlreadyUsedException(const std::string object,
                              const std::string id);

    /** destructor */
    ~XMLIdAlreadyUsedException();

    /** returns the error message */
    std::string getMessage(const std::string &obj, const std::string &id);

private:
    /** the object type with the duplicate id */
    std::string   m_Object;

    /** previous id of the object */
    std::string   m_Id;

};


/* -------------------------------------------------------------------------
 * for XMLDepartLaneDuplicationException
 * ----------------------------------------------------------------------- */
/**
 * @class XMLDepartLaneDuplicationException
 * thrown when an edge has two or more defined depart lanes
 */
class XMLDepartLaneDuplicationException : public XMLBuildingException
{
public:
    /** constructor */
    XMLDepartLaneDuplicationException();

    /** destructor */
    ~XMLDepartLaneDuplicationException();

    /** returns the error message */
    std::string getMessage(const std::string &obj, const std::string &id);

};


/* -------------------------------------------------------------------------
 * for XMLInvalidChildException
 * ----------------------------------------------------------------------- */
/**
 * @class XMLInvalidChildException
 * thrown when a child to add to a larger structure is invalid
 * (f.i. was not formerly known)
 */
class XMLInvalidChildException : public XMLBuildingException
{
public:
    /** constructor */
    XMLInvalidChildException();

    /** destructor */
    ~XMLInvalidChildException();

    /** returns the error message */
    std::string getMessage(const std::string &obj, const std::string &id);

};


/* -------------------------------------------------------------------------
 * for XMLListEmptyException
 * ----------------------------------------------------------------------- */
/**
 * @class XMLListEmptyException
 * thrown when an empty list which should have values is used
 */
class XMLListEmptyException  : public XMLBuildingException
{
public:
    /** constructor */
    XMLListEmptyException();

    /** destructor */
    ~XMLListEmptyException();

    /** returns the error message */
    std::string getMessage(const std::string &obj, const std::string &id);

};


#endif

/****************************************************************************/

