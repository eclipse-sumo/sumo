#ifndef XMLBuildingExceptions_h
#define XMLBuildingExceptions_h
/***************************************************************************
                          XMLBuildingException.h
              Exceptions that may occure while building the
              structures
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.6  2005/09/15 12:22:26  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/04/28 09:02:50  dkrajzew
// level3 warnings removed
//
// Revision 1.4  2004/11/23 10:36:50  dkrajzew
// debugging
//
// Revision 1.3  2003/06/05 14:32:33  dkrajzew
// corrupt class derivation patched
//
// Revision 1.2  2003/02/07 10:53:52  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:54:04  dkrajzew
// initial commit for xml-related utility functions
//
// Revision 1.5  2002/06/11 14:38:22  dkrajzew
// windows eol removed
//
// Revision 1.4  2002/06/11 13:43:37  dkrajzew
// Windows eol removed
//
// Revision 1.3  2002/06/10 08:33:23  dkrajzew
// Parsing of strings into other data formats generelized;
//  Options now recognize false numeric values; documentation added
//
// Revision 1.4  2002/06/10 06:54:31  dkrajzew
// Conversion of strings (XML and c-strings) to numerical values generalized;
//  options now recognize false numerical input
//
// Revision 1.3  2002/05/14 04:45:50  dkrajzew
// Bresenham added; some minor changes; windows eol removed
//
// Revision 1.2  2002/04/26 10:08:39  dkrajzew
// Windows eol removed
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:01  dkrajzew
// new version-free project name
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// ------------------------------------------
// moved to 'utils'
// ------------------------------------------
// Revision 1.1  2001/12/06 13:38:05  traffic
// files for the netbuilder
//
// Revision 1.6  2001/08/16 12:53:59  traffic
// further exception handling (now validated) and new comments
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * XMLBuildingException
 * Basic class for all following exception classes which are exceptions
 * which may be thrown during the parsing of XML-files.
 */
class XMLBuildingException {
public:
    /** constructor */
    XMLBuildingException();

    /** destructor */
    virtual ~XMLBuildingException();

    /** returns the error message */
    virtual std::string getMessage(const std::string &obj,
        const std::string &id);

};



/**
 * XMLIdNotGivenException
 * thrown when the id is not supplied
 */
class XMLIdNotGivenException : public XMLBuildingException  {
public:
    /** constructor */
    XMLIdNotGivenException();

    /** destructor */
    ~XMLIdNotGivenException();

    /** returns the error message */
    std::string getMessage(const std::string &obj, const std::string &id);

};



/**
 * XMLIdNotKnownException
 * thrown when an object of the named class and id is not known
 * (was not loaded/parsed) but its parent is known
 */
class XMLIdNotKnownException : public XMLBuildingException  {
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



/**
 * XMLIdAlreadyUsedException
 * thrown when a duplicate key for the same object type is given
 */
class XMLIdAlreadyUsedException : public XMLBuildingException  {
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



/**
 * XMLDepartLaneDuplicationException
 * thrown when an edge has two or more defined depart lanes
 */
class XMLDepartLaneDuplicationException : public XMLBuildingException  {
public:
    /** constructor */
    XMLDepartLaneDuplicationException();

    /** destructor */
    ~XMLDepartLaneDuplicationException();

    /** returns the error message */
    std::string getMessage(const std::string &obj, const std::string &id);

};



/**
 * XMLInvalidChildException
 * thrown when a child to add to a larger structure is invalid
 * (f.i. was not formerly known)
 */
class XMLInvalidChildException : public XMLBuildingException  {
public:
    /** constructor */
    XMLInvalidChildException();

    /** destructor */
    ~XMLInvalidChildException();

    /** returns the error message */
    std::string getMessage(const std::string &obj, const std::string &id);

};



/**
 * XMLInvalidParentException
 * thrown when a parent to add a child to is not known
 * (f.i. when the child is not inside of valid tags)
 */
class XMLInvalidParentException : public XMLBuildingException  {
public:
    /** constructor */
    XMLInvalidParentException();

    /** destructor */
    ~XMLInvalidParentException();

    /** returns the error message */
    std::string getMessage(const std::string &obj, const std::string &id);

};



/**
 * XMLKeyDuplicateException
 * thrown when a key-value pair with the same key as given was already set
 */
class XMLKeyDuplicateException  : public XMLBuildingException {
public:
    /** constructor */
    XMLKeyDuplicateException();

    /** destructor */
    ~XMLKeyDuplicateException();

    /** returns the error message */
    std::string getMessage(const std::string &obj, const std::string &id);

};



/**
 * XMLListEmptyException
 * thrown when an empty list which should have values is used
 */
class XMLListEmptyException  : public XMLBuildingException {
public:
    /** constructor */
    XMLListEmptyException();

    /** destructor */
    ~XMLListEmptyException();

    /** returns the error message */
    std::string getMessage(const std::string &obj, const std::string &id);

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
