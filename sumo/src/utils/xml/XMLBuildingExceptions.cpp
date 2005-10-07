/***************************************************************************
                          XMLBuildingExceptions.cpp
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
namespace
{
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.5  2005/10/07 11:47:41  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 12:22:26  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2004/11/23 10:36:50  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:53:52  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:54:04  dkrajzew
// initial commit for xml-related utility functions
//
// Revision 1.4  2002/06/11 14:38:23  dkrajzew
// windows eol removed
//
// Revision 1.3  2002/06/11 13:43:36  dkrajzew
// Windows eol removed
//
// Revision 1.2  2002/06/10 08:33:23  dkrajzew
// Parsing of strings into other data formats generelized;
//  Options now recognize false numeric values; documentation added
//
// Revision 1.4  2002/06/10 06:54:30  dkrajzew
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
// Revision 1.3  2002/04/09 12:20:38  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.2  2002/03/22 10:59:38  dkrajzew
// Memory leak tracing added; ostrstreams replaces by ostringstreams
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// ------------------------------------------
// moved to 'utils'
// ------------------------------------------
// Revision 1.1  2001/12/06 13:38:04  traffic
// files for the netbuilder
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <exception>
#include "XMLBuildingExceptions.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * XMLBuildingException - methods
 * ----------------------------------------------------------------------- */
XMLBuildingException::XMLBuildingException()
{
}


XMLBuildingException::~XMLBuildingException()
{
}


string
XMLBuildingException::getMessage(const string &obj, const string &id)
{
    return "Unknown Exception (Object=" + obj + ", id=" + id + ").";
}




/* -------------------------------------------------------------------------
 * XMLIdNotGivenException - methods
 * ----------------------------------------------------------------------- */
XMLIdNotGivenException::XMLIdNotGivenException() : XMLBuildingException()
{
}


XMLIdNotGivenException::~XMLIdNotGivenException()
{
}


string
XMLIdNotGivenException::getMessage(const string &obj, const string &id)
{
    string _myObj = obj;
    if(_myObj=="") {
        _myObj = "<unknown>";
    }
    return "The id of the object " + _myObj
        + " is missing in the description.";
}



/* -------------------------------------------------------------------------
 * XMLIdNotKnownException - methods
 * ----------------------------------------------------------------------- */
XMLIdNotKnownException::XMLIdNotKnownException(const string object,
                                               const string id)
    : XMLBuildingException(), m_Object(object), m_Id(id)
{
}


XMLIdNotKnownException::~XMLIdNotKnownException()
{
}


string
XMLIdNotKnownException::getMessage(const string &obj, const string &id)
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
{
}


XMLIdAlreadyUsedException::~XMLIdAlreadyUsedException()
{
}


string
XMLIdAlreadyUsedException::getMessage(const string &obj, const string &id)
{
  return "Another " + m_Object + " with the id " + m_Id + " exists";
}



/* -------------------------------------------------------------------------
 * XMLDepartLaneDuplicationException - methods
 * ----------------------------------------------------------------------- */
XMLDepartLaneDuplicationException::XMLDepartLaneDuplicationException()
    : XMLBuildingException()
{
}


XMLDepartLaneDuplicationException::~XMLDepartLaneDuplicationException()
{
}


string
XMLDepartLaneDuplicationException::getMessage(const string &obj,
                                              const string &id)
{
  string _myId = id;
  if(_myId=="") _myId = "<unknown>";
  return "The lane with the id " + _myId
      + " was tried to be added as a depart lane to an edge which already has a depart lane";
}



/* -------------------------------------------------------------------------
 * XMLInvalidChildException - methods
 * ----------------------------------------------------------------------- */
XMLInvalidChildException::XMLInvalidChildException()
    : XMLBuildingException()
{
}


XMLInvalidChildException::~XMLInvalidChildException()
{
}


string
XMLInvalidChildException::getMessage(const string &obj, const string &id)
{
    string _myObj = obj;
    string _myId = id;
    if(_myObj=="") {
        _myObj = "<unknown>";
    }
    if(_myId=="") {
        _myId = "<unknown>";
    }
    return "The object " + _myObj + " with the id " + _myId
        + " is not nested properly";
}



/* -------------------------------------------------------------------------
 * XMLInvalidParentException - methods
 * ----------------------------------------------------------------------- */
XMLInvalidParentException::XMLInvalidParentException()
    : XMLBuildingException()
{
}


XMLInvalidParentException::~XMLInvalidParentException()
{
}


string
XMLInvalidParentException::getMessage(const string &obj, const string &id)
{
    string _myObj = obj;
    string _myId = id;
    if(_myObj=="") _myObj = "<unknown>";
    if(_myId=="") _myId = "<unknown>";
    return "The object " + _myObj + " with the id " + _myId
        + " is not nested properly";
}



/* -------------------------------------------------------------------------
 * XMLKeyDuplicateException - methods
 * ----------------------------------------------------------------------- */
XMLKeyDuplicateException::XMLKeyDuplicateException()
{
}


XMLKeyDuplicateException::~XMLKeyDuplicateException()
{
}


string
XMLKeyDuplicateException::getMessage(const string &obj, const string &id)
{
    return "Double request-respond definition occured";
}



/* -------------------------------------------------------------------------
 * XMLListEmptyException - methods
 * ----------------------------------------------------------------------- */
XMLListEmptyException::XMLListEmptyException()
{
}


XMLListEmptyException::~XMLListEmptyException()
{
}


string
XMLListEmptyException::getMessage(const string &obj, const string &id)
{
    return "The list belonging to the '" + obj + "' with the id '"
        + id + "' is empty";
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
