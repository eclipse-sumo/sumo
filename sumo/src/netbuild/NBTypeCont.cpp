/***************************************************************************
                          NBTypeCont.cpp
              A storage for the available types of an edge
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
// Revision 1.6  2004/11/23 10:21:41  dkrajzew
// debugging
//
// Revision 1.5  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of geometry information
//
// Revision 1.4  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.3  2003/05/20 09:33:48  dkrajzew
// false computation of yielding on lane ends debugged; some debugging on tl-import; further work on vissim-import
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.5  2002/07/25 08:31:42  dkrajzew
// Report methods transfered from loader to the containers
//
// Revision 1.4  2002/06/11 16:00:42  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.3  2002/05/14 04:42:56  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:12  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.3  2002/04/09 12:21:25  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.2  2002/03/22 10:50:04  dkrajzew
// Memory leaks debugging added (MSVC++)
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:37:59  traffic
// files for the netbuilder
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <map>
#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>
#include "NBType.h"
#include "NBTypeCont.h"
#include "NBJunctionTypesMatrix.h"


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC       // exclude standard memory alloc procedures
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static members definitions
 * ======================================================================= */
string                    NBTypeCont::_formatName;
int                       NBTypeCont::_defaultNoLanes;
double                    NBTypeCont::_defaultSpeed;
int                       NBTypeCont::_defaultPriority;
NBTypeCont::TypesCont     NBTypeCont::_types;
NBJunctionTypesMatrix     NBTypeCont::_junctionTypes;


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
NBTypeCont::setDefaults(const string &formatName, int defaultNoLanes,
                        double defaultSpeed, int defaultPriority)
{
    _formatName = formatName;
    _defaultNoLanes = defaultNoLanes;
    _defaultSpeed = defaultSpeed;
    _defaultPriority = defaultPriority;
}


bool
NBTypeCont::insert(NBType *type)
{
    TypesCont::iterator i=_types.find(type->_name);
    if(i!=_types.end()) {
        return false;
    }
    _types[type->_name] = type;
    return true;
}


int
NBTypeCont::getNoLanes(const string &type)
{
    TypesCont::iterator i = _types.find(type);
    if(i==_types.end()) {
        return NBTypeCont::_defaultNoLanes;
    }
    int nolanes = (*i).second->_noLanes;
    return nolanes;
}


double
NBTypeCont::getSpeed(const string &type)
{
    TypesCont::iterator i = _types.find(type);
    if(i==_types.end()) {
        return NBTypeCont::_defaultSpeed;
    }
    double speed = (*i).second->_speed;
    return speed;
}


int
NBTypeCont::getPriority(const string &type)
{
    TypesCont::iterator i = _types.find(type);
    if(i==_types.end()) {
        return NBTypeCont::_defaultPriority;
    }
    int priority = (*i).second->_priority;
    return priority;
}


string
NBTypeCont::getFormatName()
{
    return _formatName;
}


int
NBTypeCont::getDefaultNoLanes()
{
    return NBTypeCont::_defaultNoLanes;
}


int
NBTypeCont::getDefaultPriority()
{
    return NBTypeCont::_defaultPriority;
}


double
NBTypeCont::getDefaultSpeed()
{
    return NBTypeCont::_defaultSpeed;
}


size_t
NBTypeCont::getNo()
{
    return _types.size();
}


NBNode::BasicNodeType
NBTypeCont::getJunctionType(int edgetype1, int edgetype2)
{
    return _junctionTypes.getType(edgetype1, edgetype2);
}


void
NBTypeCont::clear()
{
    for(TypesCont::iterator i=_types.begin(); i!=_types.end(); i++) {
        delete((*i).second);
    }
    _types.clear();
}


void
NBTypeCont::report()
{
    WRITE_MESSAGE(string("   ") + toString<int>(getNo()) + string(" types loaded."));
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBTypeCont.icc"
//#endif

// Local Variables:
// mode:C++
// End:
