/***************************************************************************
                          NLHandlerJunctionLogics.cpp
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
// Revision 1.1  2002/04/08 07:21:24  traffic
// Initial revision
//
// Revision 2.1  2002/03/15 09:36:38  traffic
// New declaration of data members now allows usage by derived classes; handling of map logics removed; unnecessary data members removed
//
// Revision 2.0  2002/02/14 14:43:23  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/13 15:40:44  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:08  traffic
// moved from netbuild
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include "NLContainer.h"
#include "NLHandlerJunctionLogics.h"
#include "SErrorHandler.h"
#include "../utils/XMLConvert.h"
#include "../utils/XMLBuildingExceptions.h"
#include "NLSAXHandler.h"
#include "NLNetBuilder.h"
#include "NLTags.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLHandlerJunctionLogics::NLHandlerJunctionLogics(NLContainer *container) : 
  NLSAXHandler(container)
{
}

NLHandlerJunctionLogics::~NLHandlerJunctionLogics() 
{
}

void NLHandlerJunctionLogics::init(string key, string file) {
  m_File = file;
  m_Key = key;
}


vector<bool> *
NLHandlerJunctionLogics::convertBoolVector(const char *str) 
{
  vector<bool> *ret = new vector<bool>();
  int len = strlen(str);
  ret->reserve(len);
  for(int i=0; i<len; i++) {
    if(str[i]==' ')
      ret->push_back(false);
    else
      ret->push_back(true);
  }
  return ret;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLHandlerJunctionLogics.icc"
//#endif

// Local Variables:
// mode:C++
// End:

