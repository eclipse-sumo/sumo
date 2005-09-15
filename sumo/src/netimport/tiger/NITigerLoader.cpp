/***************************************************************************
                          NITigerLoader.cpp
              A loader tiger-files
                             -------------------
    project              : SUMO
    begin                : Tue, 29 Jun 2004
    copyright            : (C) 2004 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.6  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/04/27 12:24:36  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.4  2004/11/23 10:23:52  dkrajzew
// debugging
//
// Revision 1.3  2004/08/02 12:44:13  dkrajzew
// using Position2D instead of two doubles
//
// Revision 1.2  2004/07/05 09:32:22  dkrajzew
// false geometry assignment patched
//
// Revision 1.1  2004/07/02 09:34:38  dkrajzew
// elmar and tiger import added
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
#include <cassert>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/convert/TplConvert.h>
#include <utils/convert/ToString.h>
#include <utils/options/OptionsCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <utils/geom/GeomHelper.h>

#include "NITigerLoader.h"

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
NITigerLoader::NITigerLoader(NBEdgeCont &ec, NBNodeCont &nc,
                             const std::string &file)
    : FileErrorReporter("tiger-network", file),
    myWasSet(false), myInitX(-1), myInitY(-1), myEdgeCont(ec), myNodeCont(nc)
{
}


NITigerLoader::~NITigerLoader()
{
}


void
NITigerLoader::load(OptionsCont &options)
{
    LineReader tgr1r((_file + string(".rt1")).c_str());
    LineReader tgr2r((_file + string(".rt2")).c_str());
    if(!tgr1r.good()) {
        MsgHandler::getErrorInstance()->inform(
            string("Could not open '") + _file + string(".rt1")
            + string("'."));
        throw ProcessError();
    }
    string line1, line2, tmp;
    std::vector<std::string> values2;
    if(tgr2r.good()&&tgr2r.hasMore()) {
        line2 = StringUtils::prune(tgr2r.readLine());
        StringTokenizer st2(line2, StringTokenizer::WHITECHARS);
        values2 = st2.getVector();
    }
    while(tgr1r.hasMore()) {
        string line1 = StringUtils::prune(tgr1r.readLine());
        StringTokenizer st1(line1, StringTokenizer::WHITECHARS);
        std::vector<std::string> values1 = st1.getVector();
        string eid = values1[1];
        std::vector<std::string> poses;
        poses.push_back(values1[values1.size()-2]);
        // check whether any additional information exists
        if(values2.size()!=0&&values2[1]==eid) {
            copy(values2.begin()+3, values2.end(),
                back_inserter(poses));
            if(tgr2r.good()&&tgr2r.hasMore()) {
                line2 = tgr2r.readLine();
                StringTokenizer st2(line2, StringTokenizer::WHITECHARS);
                values2 = st2.getVector();
            }
        }
        poses.push_back(values1[values1.size()-1]);
        assert(poses.size()>1);
        Position2DVector cposes = convertShape(poses);
        assert(cposes.size()>1);
        NBNode *from = getNode(cposes.at(0));
        NBNode *to = getNode(cposes.at(cposes.size()-1));
        if(from==0||to==0) {
            if(from==0) {
                MsgHandler::getErrorInstance()->inform(
                    string("Could not find start node of edge '")
                    + eid + string("'."));
                throw ProcessError();
            }
            if(to==0) {
                MsgHandler::getErrorInstance()->inform(
                    string("Could not find end node of edge '")
                    + eid + string("'."));
                throw ProcessError();
            }
        }
        // !!!
        std::string type = getType(values1);
        double speed = getSpeed(type);
        int nolanes = getLaneNo(type);
        double length = cposes.length();
        if(nolanes!=-1&&length>0) {
            int priority = 1;
            NBEdge *e =
                new NBEdge(eid, eid, from, to, type, speed, nolanes,
                    cposes.length(), priority, cposes);
            if(!myEdgeCont.insert(e)) {
                delete e;
                MsgHandler::getErrorInstance()->inform(
                    string("Could not insert edge '")
                    + eid + string("'."));
                throw ProcessError();
            }
            eid = string("-") + eid;
            e =
                new NBEdge(eid, eid, to, from, type, speed, nolanes,
                    cposes.length(), priority, cposes.reverse());
            if(!myEdgeCont.insert(e)) {
                delete e;
                MsgHandler::getErrorInstance()->inform(
                    string("Could not insert edge '")
                    + eid + string("'."));
                throw ProcessError();
            }
        }
    }
}


Position2DVector
NITigerLoader::convertShape(const std::vector<std::string> &sv)
{
    Position2DVector ret;
    std::vector<std::string>::const_iterator i;
    for(i=sv.begin(); i!=sv.end(); ++i) {
        string info = *i;
        size_t b1 = info.find_first_of("+-");
        assert(b1!=string::npos);
        size_t b2 = info.find_first_of("+-", b1+1);
        size_t b3 =
            b2==string::npos
            ? string::npos
            : info.find_first_of("+-", b2+1);
        string p1 = info.substr(0, b2);
        string p2 =
            b3==string::npos
            ? info.substr(b2)
            : info.substr(b2, b3-b2);
        try {
            double x = TplConvert<char>::_2float(p1.c_str());
            double y = TplConvert<char>::_2float(p2.c_str());
            x = x / 100000.0;
            y = y / 100000.0;
            double ys = y;
            if(!myWasSet) {
                myWasSet = true;
                myInitX = x;
                myInitY = y;
            }
            x = (x-myInitX);
            y = (y-myInitY);
            double x1 = x * 111.320*1000;
            double y1 = y * 111.136*1000;
            x1 *= cos(ys*PI/180.0);
            Position2D p(x1, y1);
            ret.push_back(p);
        } catch(NumberFormatException &) {
            MsgHandler::getErrorInstance()->inform(
                string("Could not convert position '")
                + p1 + string("/") + p2 + string("'."));
            throw ProcessError();
        }
    }
    return ret;
}

int bla = 0;

NBNode *
NITigerLoader::getNode(const Position2D &p)
{
    NBNode *n = myNodeCont.retrieve(p);
    if(n==0) {
        n = new NBNode(toString<int>(bla++), p);
        if(!myNodeCont.insert(n)) {
            MsgHandler::getErrorInstance()->inform(
                string("Could not insert node at position ")
                + toString(p.x()) + string("/") + toString(p.y())
                + string("."));
            throw ProcessError();
        }
    }
    return n;
}


std::string
NITigerLoader::getType(const std::vector<std::string> &sv) const
{
    for(std::vector<std::string>::const_iterator i=sv.begin(); i!=sv.end(); ++i) {
        std::string tc = *i;
        // some checks whether it's the type
        if(tc.length()!=3) {
            continue;
        }
        if(tc.find_first_of("ABCDEFGHPX")!=0) {
            continue;
        }
        if(tc[1]<'0'||tc[1]>'9') {
            continue;
        }
        if(tc[2]<'0'||tc[2]>'9') {
            continue;
        }
        // ok, its the type (let's hope)
        return tc;
    }
    MsgHandler::getErrorInstance()->inform(
        string("Could not determine type for an edge...")); // !!! be more verbose
    throw ProcessError();
}


double
NITigerLoader::getSpeed(const std::string &type) const
{
    switch(type[0]) {
    case 'A':
        switch(type[1]) {
        case '1':
            return 85.0 / 3.6 * 1.6;
        case '2':
            return 85.0 / 3.6 * 1.6;
        case '3':
            return 55.0 / 3.6 * 1.6;
        case '4':
            return 35.0 / 3.6 * 1.6;
        case '5':
            return 10.0 / 3.6 * 1.6;
        default:
            return -1;
        };
        break;
    case 'B':
        return -1; // rail roads
    case 'C':
        return -1; // Miscellaneous ground transport
    case 'D':
        return -1; // landmark
    case 'E':
        return -1; // physical feature
    case 'F':
        return -1; // nonvisible features
    case 'G':
        return -1; // census bureau usage
    case 'H':
        return -1; // hydrography
    case 'P':
        return -1; // ? !!!
    case 'X':
        return -1; // not yet classified
    default:
        throw 1;
    }
}


int
NITigerLoader::getLaneNo(const std::string &type) const
{
    switch(type[0]) {
    case 'A':
        switch(type[1]) {
        case '1':
            return 5;
        case '2':
            return 4;
        case '3':
            return 1;
        case '4':
            return 1;
        case '5':
            return 1;
        default:
            return -1;
        };
        break;
    case 'B':
        return -1; // rail roads
    case 'C':
        return -1; // Miscellaneous ground transport
    case 'D':
        return -1; // landmark
    case 'E':
        return -1; // physical feature
    case 'F':
        return -1; // nonvisible features
    case 'G':
        return -1; // census bureau usage
    case 'H':
        return -1; // hydrography
    case 'P':
        return -1; // ? !!!
    case 'X':
        return -1; // not yet classified
    default:
        throw 1;
    }
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

