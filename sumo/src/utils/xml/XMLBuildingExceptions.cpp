/****************************************************************************/
/// @file    XMLBuildingExceptions.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id: $
///
// Exceptions that may occure while building the
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
#include <exception>
#include "XMLBuildingExceptions.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * XMLBuildingException - methods
 * ----------------------------------------------------------------------- */
XMLBuildingException::XMLBuildingException()
{}


XMLBuildingException::~XMLBuildingException()
{}


string
XMLBuildingException::getMessage(const string &obj, const string &id)
{
    return "Unknown Exception (Object=" + obj + ", id=" + id + ").";
}




/* -------------------------------------------------------------------------
 * XMLIdNotKnownException - methods
 * ----------------------------------------------------------------------- */
XMLIdNotKnownException::XMLIdNotKnownException(const string object,
        const string id)
        : XMLBuildingException(), m_Object(object), m_Id(id)
{}


XMLIdNotKnownException::~XMLIdNotKnownException()
{}


string
XMLIdNotKnownException::getMessage(const string &, const string &)
{
    return "The object " + m_Object + " with the id " + m_Id
           + " is not known";
}



/* -------------------------------------------------------------------------
 * XMLIdAlreadyUsedException - methods
 * ----------------------------------------------------------------------- */
XMLIdAlreadyUsedException::XMLIdAlreadyUsedException(const string object,
        const string id)
        : XMLBuildingException(), m_Object(object), m_Id(id)
{}


XMLIdAlreadyUsedException::~XMLIdAlreadyUsedException()
{}


string
XMLIdAlreadyUsedException::getMessage(const string &, const string &)
{
    return "Another " + m_Object + " with the id " + m_Id + " exists";
}



/* -------------------------------------------------------------------------
 * XMLDepartLaneDuplicationException - methods
 * ----------------------------------------------------------------------- */
XMLDepartLaneDuplicationException::XMLDepartLaneDuplicationException()
        : XMLBuildingException()
{}


XMLDepartLaneDuplicationException::~XMLDepartLaneDuplicationException()
{}


string
XMLDepartLaneDuplicationException::getMessage(const string &,
        const string &id)
{
    string _myId = id;
    if (_myId=="") _myId = "<unknown>";
    return "The lane with the id " + _myId
           + " was tried to be added as a depart lane to an edge which already has a depart lane";
}



/* -------------------------------------------------------------------------
 * XMLInvalidChildException - methods
 * ----------------------------------------------------------------------- */
XMLInvalidChildException::XMLInvalidChildException()
        : XMLBuildingException()
{}


XMLInvalidChildException::~XMLInvalidChildException()
{}


string
XMLInvalidChildException::getMessage(const string &obj, const string &id)
{
    string _myObj = obj;
    string _myId = id;
    if (_myObj=="") {
        _myObj = "<unknown>";
    }
    if (_myId=="") {
        _myId = "<unknown>";
    }
    return "The object " + _myObj + " with the id " + _myId
           + " is not nested properly";
}



/* -------------------------------------------------------------------------
 * XMLListEmptyException - methods
 * ----------------------------------------------------------------------- */
XMLListEmptyException::XMLListEmptyException()
{}


XMLListEmptyException::~XMLListEmptyException()
{}


string
XMLListEmptyException::getMessage(const string &obj, const string &id)
{
    return "The list belonging to the '" + obj + "' with the id '"
           + id + "' is empty";
}



/****************************************************************************/

