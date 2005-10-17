/***************************************************************************
                          NBJunctionTypesMatrix.cpp
                          Definition of a junction's type in dependence
                          on the incoming edges
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
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
// Revision 1.10  2005/10/17 09:02:44  dkrajzew
// got rid of the old MSVC memory leak checker; memory leaks removed
//
// Revision 1.9  2005/10/07 11:38:18  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/23 06:01:06  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/04/27 11:48:25  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.5  2004/01/12 15:25:09  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.4  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage
//  of geometry information
//
// Revision 1.3  2003/04/10 15:45:18  dkrajzew
// some lost changes reapplied
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.5  2002/07/25 08:35:19  dkrajzew
// Exception handlig within the determination of the crossing type improved
//
// Revision 1.4  2002/06/11 16:00:41  dkrajzew
// windows eol removed; template class definition inclusion depends now on
//  the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.3  2002/05/14 04:42:55  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:11  dkrajzew
// Windows eol removed; minor SUMOReal to int conversions removed;
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

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "NBJunctionTypesMatrix.h"
#include <utils/common/UtilExceptions.h>
#include "nodes/NBNode.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NBJunctionTypesMatrix::NBJunctionTypesMatrix()
{
    _map['t'] = NBNode::NODETYPE_PRIORITY_JUNCTION; // !!! was: NODETYPE_SIMPLE_TRAFFIC_LIGHT;
    _map['x'] = NBNode::NODETYPE_NOJUNCTION;
    _map['p'] = NBNode::NODETYPE_PRIORITY_JUNCTION;
    _map['r'] = NBNode::NODETYPE_RIGHT_BEFORE_LEFT;
    _ranges.push_back(pair<int, int>(90, 90));
    _ranges.push_back(pair<int, int>(65, 85));
    _ranges.push_back(pair<int, int>(59, 41));
    _ranges.push_back(pair<int, int>(20, 20));
    _ranges.push_back(pair<int, int>(15, 15));
    _ranges.push_back(pair<int, int>(10, 10));
    _ranges.push_back(pair<int, int>(5, 1));
    _values.push_back("xxxxpxx");
    _values.push_back(" ttttpp");
    _values.push_back("  ppppp");
    _values.push_back("   pppp");
    _values.push_back("    xxx");
    _values.push_back("     rp");
    _values.push_back("      p");
}


NBJunctionTypesMatrix::~NBJunctionTypesMatrix()
{
}


NBNode::BasicNodeType
NBJunctionTypesMatrix::getType(int prio1, int prio2) const
{
    RangeCont::const_iterator p1 = find_if(_ranges.begin(), _ranges.end(),
        priority_finder(prio1));
    RangeCont::const_iterator p2 = find_if(_ranges.begin(), _ranges.end(),
        priority_finder(prio2));
    if(p1==_ranges.end()||p2==_ranges.end()) {
        throw OutOfBoundsException();
    }
    char name = getNameAt(distance(_ranges.begin(), p1),
        distance(_ranges.begin(), p2));
    return _map.find(name)->second;
}


char
NBJunctionTypesMatrix::getNameAt(int pos1, int pos2) const
{
    string str = _values[pos1];
    if(str[pos2]==' ') {
        return getNameAt(pos2, pos1);
    }
    return str[pos2];
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

