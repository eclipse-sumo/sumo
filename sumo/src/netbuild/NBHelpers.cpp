/***************************************************************************
                          NBHelpers.cpp
			  Some mathematical helper methods
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
// Revision 1.3  2003/04/04 07:43:03  dkrajzew
// Yellow phases must be now explicetely given; comments added; order of edge sorting (false lane connections) debugged
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.6  2002/07/25 08:34:16  dkrajzew
// conversion to a unique string representation of integer ids added
//
// Revision 1.5  2002/07/02 09:01:10  dkrajzew
// Bug on the computation of an edges direction (angle) fixed
//
// Revision 1.4  2002/06/11 16:00:41  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.3  2002/05/14 04:42:55  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:11  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.4  2002/04/09 12:21:24  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.3  2002/03/22 12:02:20  dkrajzew
// Outgoing edges rotation fixed
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:38:00  traffic
// files for the netbuilder
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <cmath>
#include <string>
#include <sstream>
#include "NBNode.h"
#include "NBHelpers.h"
#include <utils/common/StringTokenizer.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/GeomHelper.h>
#include <iostream>


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
double
NBHelpers::angle(double x1, double y1, double x2, double y2)
{
    double angle = atan2(x1-x2, y1-y2) * 180.0 / 3.14159265;
    if(angle<0) {
        angle = 360 + angle;
    }
    return angle;
}


double
NBHelpers::relAngle(double angle, double x1, double y1, double x2, double y2)
{
    double sec_angle = NBHelpers::angle(x1, y1, x2, y2);
    return relAngle(angle, sec_angle);
}


double
NBHelpers::relAngle(double angle1, double angle2)
{
    angle2 -= angle1;
    if(angle2>180)
        angle2 = (360 - angle2) * -1;
    while(angle2<-180)
        angle2 = 360 + angle2;
    return angle2;
}


double
NBHelpers::normRelAngle(double angle1, double angle2) {
    double rel = relAngle(angle1, angle2);
    if(rel<-170||rel>170)
        rel = -180;
    return rel;
}


long
NBHelpers::computeLogicSize(string key) {
    long size = 1;
    size_t index = key.find('_');
    while(index<key.size()-1) {
        if(key.at(index+1)=='+') {
            int nolanes = key.at(index+2) - '0';
            int pos = index + 4;
            for(int i=0; i<nolanes; i++) {
                int noconn = key.at(pos) - 'a';
                size *= (noconn+1);
                pos += noconn * 2 + 1;
            }
        }
        index = key.find('_', index+1);
    }
    return size;
}


std::string
NBHelpers::normalIDRepresentation(const std::string &id)
{
    stringstream strm1(id);
    long numid;
    strm1 >> numid;
    stringstream strm2;
    strm2 << numid;
    return strm2.str();
}


double
NBHelpers::distance(NBNode *node1, NBNode *node2)
{
    double xb = node1->getXCoordinate();
    double xe = node2->getXCoordinate();
    double yb = node1->getYCoordinate();
    double ye = node2->getYCoordinate();
    return sqrt((xb-xe)*(xb-xe) + (yb-ye)*(yb-ye));
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBHelpers.icc"
//#endif

// Local Variables:
// mode:C++
// End:

