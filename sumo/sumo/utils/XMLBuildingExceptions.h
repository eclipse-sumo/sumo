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
// Revision 1.4  2002/06/11 13:43:37  dkrajzew
// Windows eol removed
//
// Revision 1.3  2002/06/10 08:33:23  dkrajzew
// Parsing of strings into other data formats generelized; Options now recognize false numeric values; documentation added
//
// Revision 1.4  2002/06/10 06:54:31  dkrajzew
// Conversion of strings (XML and c-strings) to numerical values generalized; options now recognize false numerical input
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
 * included modules
 * ======================================================================= */
#include <string>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * XMLBuildingException
 * basic class for all following exception classes
 */
class XMLBuildingException {
public:
    /** constructor */
    XMLBuildingException();
    /** destructor */
    virtual ~XMLBuildingException();
    /** returns the error message */
    virtual std::string getMessage(const std::string &obj, const std::string &id);
};

/**
 * XMLIdNotGivenException
 * thrown when the id is not supplied
 */
class XMLIdNotGivenException : XMLBuildingException  {
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
class XMLIdNotKnownException : XMLBuildingException  {
private:
    /** the object type with the unknown id */
    std::string   m_Object;
    /** the id of the parent */
    std::string   m_Id;
public:
    /** constructor */
    XMLIdNotKnownException(std::string object, std::string id);
    /** destructor */
    ~XMLIdNotKnownException();
    /** returns the error message */
    std::string getMessage(const std::string &obj, const std::string &id);
};

/**
 * XMLIdAlreadyUsedException
 * thrown when a duplicate key for the same object type is given
 */
class XMLIdAlreadyUsedException : XMLBuildingException  {
private:
    /** the object type with the duplicate id */
    std::string   m_Object;
    /** previous id of the object */
    std::string   m_Id;
public:
    /** constructor */
    XMLIdAlreadyUsedException(const std::string object, const std::string id);
    /** destructor */
    ~XMLIdAlreadyUsedException();
    /** returns the error message */
    std::string getMessage(const std::string &obj, const std::string &id);
};

/**
 * XMLDepartLaneDuplicationException
 * thrown when an edge has two or more defined depart lanes
 */
class XMLDepartLaneDuplicationException : XMLBuildingException  {
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
class XMLInvalidChildException : XMLBuildingException  {
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
class XMLInvalidParentException : XMLBuildingException  {
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
class XMLKeyDuplicateException  : XMLBuildingException {
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
class XMLListEmptyException  : XMLBuildingException {
public:
    /** constructor */
    XMLListEmptyException();
    /** destructor */
    ~XMLListEmptyException();
    /** returns the error message */
    std::string getMessage(const std::string &obj, const std::string &id);
};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "XMLBuildingExceptions.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


