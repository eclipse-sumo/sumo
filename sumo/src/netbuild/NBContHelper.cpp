/***************************************************************************
                          NBContHelper.cpp
			  Some methods for traversing lists of edges
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
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
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2003/03/17 14:22:32  dkrajzew
// further debug and windows eol removed
//
// Revision 1.3  2003/03/12 16:47:52  dkrajzew
// extension for artemis-import
//
// Revision 1.2  2003/02/07 10:43:43  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.4  2002/06/11 16:00:41  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.3  2002/05/14 04:42:54  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:10  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.3  2002/04/09 12:21:24  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.2  2002/03/22 10:50:03  dkrajzew
// Memory leaks debugging added (MSVC++)
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <map>
#include "NBContHelper.h"


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
EdgeVector::const_iterator
NBContHelper::nextCW(const EdgeVector * edges, EdgeVector::const_iterator from) {
    from++;
    if(from==edges->end()) {
        return edges->begin();
    }
    return from;
}


EdgeVector::const_iterator
NBContHelper::nextCCW(const EdgeVector * edges, EdgeVector::const_iterator from) {
  if(from==edges->begin())
    return edges->end() - 1;
  return --from;
}


int
NBContHelper::countPriorities(const EdgeVector &s) {
    if(s.size()==0)
        return 0;
    map<int, int> knownPrios;
    for(EdgeVector::const_iterator i=s.begin(); i!=s.end(); i++) {
        knownPrios.insert(map<int, int>::value_type((*i)->getPriority(), 0));
    }
    return knownPrios.size();
}


std::ostream &
NBContHelper::out(std::ostream &os, const std::vector<bool> &v)
{
    for(std::vector<bool>::const_iterator i=v.begin(); i!=v.end(); i++) {
        os << *i;
    }
    return os;
}


NBEdge *
NBContHelper::findConnectingEdge(const EdgeVector &edges,
                                 NBNode *from, NBNode *to)
{
    for(EdgeVector::const_iterator i=edges.begin(); i!=edges.end(); i++) {
        if((*i)->getToNode()==to && (*i)->getFromNode()==from) {
            return *i;
        }
    }
    return 0;
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBContHelper.icc"
//#endif

// Local Variables:
// mode:C++
// End:

