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
// Revision 1.1  2002/04/08 07:21:25  traffic
// Initial revision
//
// Revision 2.0  2002/02/14 14:43:29  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/13 15:48:20  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// ------------------------------------------
// moved to 'utils'
// ------------------------------------------
// Revision 1.1  2001/12/06 13:38:04  traffic
// files for the netbuilder
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <exception>
#include "XMLBuildingExceptions.h"

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
 * XMLUngivenParameterException - methods
 * ----------------------------------------------------------------------- */
XMLUngivenParameterException::XMLUngivenParameterException() : XMLBuildingException()  
{
}


XMLUngivenParameterException::~XMLUngivenParameterException() 
{
}


string 
XMLUngivenParameterException::getMessage(const string &obj, const string &id) 
{
  string _myObj = obj;
  string _myId = id;
  if(_myObj=="") _myObj = "<unknown>";
  if(_myId=="") _myId = "<unknown>";
  return "The description of the object '" + _myObj + "' with the id '" + _myId + "' is not complete.";
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
  if(_myObj=="") _myObj = "<unknown>";
  return "The id of the object " + _myObj + " is missing in the description.";
}



/* -------------------------------------------------------------------------
 * XMLIdNotKnownException - methods
 * ----------------------------------------------------------------------- */
XMLIdNotKnownException::XMLIdNotKnownException(const string object, const string id) : XMLBuildingException(), m_Object(object), m_Id(id)
{
}


XMLIdNotKnownException::~XMLIdNotKnownException() 
{
}


string 
XMLIdNotKnownException::getMessage(const string &obj, const string &id) 
{
  return "The object " + m_Object + " with the id " + m_Id + " is not known";
}



/* -------------------------------------------------------------------------
 * XMLIdAlreadyUsedException - methods
 * ----------------------------------------------------------------------- */
XMLIdAlreadyUsedException::XMLIdAlreadyUsedException(const string object, const string id) : XMLBuildingException(), m_Object(object), m_Id(id)
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
XMLDepartLaneDuplicationException::XMLDepartLaneDuplicationException() : XMLBuildingException()  
{
}


XMLDepartLaneDuplicationException::~XMLDepartLaneDuplicationException() 
{
}


string 
XMLDepartLaneDuplicationException::getMessage(const string &obj, const string &id) 
{
  string _myId = id;
  if(_myId=="") _myId = "<unknown>";
  return "The lane with the id " + _myId + " was tried to be added as a depart lane to an edge which already has a depart lane";
}



/* -------------------------------------------------------------------------
 * XMLInvalidChildException - methods
 * ----------------------------------------------------------------------- */
XMLInvalidChildException::XMLInvalidChildException() : XMLBuildingException()  
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
  if(_myObj=="") _myObj = "<unknown>";
  if(_myId=="") _myId = "<unknown>";
  return "The object " + _myObj + " with the id " + _myId + " is not nested properly";
}



/* -------------------------------------------------------------------------
 * XMLInvalidParentException - methods
 * ----------------------------------------------------------------------- */
XMLInvalidParentException::XMLInvalidParentException() : XMLBuildingException()  
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
  return "The object " + _myObj + " with the id " + _myId + " is not nested properly";
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
  return "The list belonging to the '" + obj + "' with the id '" + id + "' must not but is empty";
}



/* -------------------------------------------------------------------------
 * XMLNumericFormatException - methods
 * ----------------------------------------------------------------------- */
XMLNumericFormatException::XMLNumericFormatException() 
{
}


XMLNumericFormatException::~XMLNumericFormatException() 
{
}


string 
XMLNumericFormatException::getMessage(const string &obj, const string &id) 
{
  return "Numeric format mismatch.";
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "XMLBuildingExceptions.icc"
//#endif

// Local Variables:
// mode:C++
// End:
