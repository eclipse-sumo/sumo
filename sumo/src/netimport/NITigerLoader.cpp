/****************************************************************************/
/// @file    NITigerLoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 29 Jun 2004
/// @version $Id:NITigerLoader.cpp 4701 2007-11-09 14:29:29Z dkrajzew $
///
// Importer for TIGER networks
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/StringTokenizer.h>
#include <utils/importio/LineReader.h>
#include <utils/options/OptionsCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/GeoConvHelper.h>
#include "NITigerLoader.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NITigerLoader::NITigerLoader(NBEdgeCont &ec, NBNodeCont &nc,
                             const std::string &file)
        : FileErrorReporter("tiger-network", file),
        myWasSet(false), myInitX(-1), myInitY(-1), myEdgeCont(ec), myNodeCont(nc)
{}


NITigerLoader::~NITigerLoader()
{}


void
NITigerLoader::load(OptionsCont &)
{
    LineReader tgr1r((myFile + ".rt1").c_str());
    LineReader tgr2r((myFile + ".rt2").c_str());
    if (!tgr1r.good()) {
        throw ProcessError("Could not open '" + myFile + ".rt1" + "'.");

    }
    string line1, line2, tmp;
    std::vector<std::string> values2;
    if (tgr2r.good()&&tgr2r.hasMore()) {
        line2 = StringUtils::prune(tgr2r.readLine());
        StringTokenizer st2(line2, StringTokenizer::WHITECHARS);
        values2 = st2.getVector();
    }
    while (tgr1r.hasMore()) {
        string line1 = StringUtils::prune(tgr1r.readLine());
        StringTokenizer st1(line1, StringTokenizer::WHITECHARS);
        std::vector<std::string> values1 = st1.getVector();
        string eid = values1[1];
        std::vector<std::string> poses;
        poses.push_back(values1[values1.size()-2]);
        // check whether any additional information exists
        if (values2.size()!=0&&values2[1]==eid) {
            copy(values2.begin()+3, values2.end(),
                 back_inserter(poses));
            if (tgr2r.good()&&tgr2r.hasMore()) {
                line2 = tgr2r.readLine();
                StringTokenizer st2(line2, StringTokenizer::WHITECHARS);
                values2 = st2.getVector();
            }
        }
        poses.push_back(values1[values1.size()-1]);
        assert(poses.size()>1);
        Position2DVector cposes = convertShape(poses);
        assert(cposes.size()>1);
        NBNode *from = getNode(cposes[0]);
        NBNode *to = getNode(cposes[-1]);
        if (from==0||to==0) {
            if (from==0) {
                throw ProcessError("Could not find start node of edge '" + eid + "'.");

            }
            if (to==0) {
                throw ProcessError("Could not find end node of edge '" + eid + "'.");

            }
        }
        // !!!
        std::string type = getType(values1);
        SUMOReal speed = getSpeed(type);
        int nolanes = getLaneNo(type);
        SUMOReal length = cposes.length();
        if (nolanes!=-1&&length>0) {
            int priority = -1;
            NBEdge *e =
                new NBEdge(eid, from, to, type, speed, nolanes,
                           priority, cposes);
            if (!myEdgeCont.insert(e)) {
                delete e;
                throw ProcessError("Could not insert edge '" + eid + "'.");

            }
            eid = "-" + eid;
            e =
                new NBEdge(eid, to, from, type, speed, nolanes,
                           priority, cposes.reverse());
            if (!myEdgeCont.insert(e)) {
                delete e;
                throw ProcessError("Could not insert edge '" + eid + "'.");

            }
        }
    }
}


Position2DVector
NITigerLoader::convertShape(const std::vector<std::string> &sv)
{
    Position2DVector ret;
    std::vector<std::string>::const_iterator i;
    for (i=sv.begin(); i!=sv.end(); ++i) {
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
            SUMOReal x = TplConvert<char>::_2SUMOReal(p1.c_str());
            SUMOReal y = TplConvert<char>::_2SUMOReal(p2.c_str());

            Position2D pos(x, y);
            GeoConvHelper::x2cartesian(pos);
            ret.push_back(pos);
        } catch (NumberFormatException &) {
            throw ProcessError("Could not convert position '" + p1 + "/" + p2 + "'.");

        }
    }
    return ret;
}

int bla = 0;

NBNode *
NITigerLoader::getNode(const Position2D &p)
{
    NBNode *n = myNodeCont.retrieve(p);
    if (n==0) {
        n = new NBNode(toString<int>(bla++), p);
        if (!myNodeCont.insert(n)) {
            throw ProcessError("Could not insert node at position " + toString(p.x()) + "/" + toString(p.y()) + ".");

        }
    }
    return n;
}


std::string
NITigerLoader::getType(const std::vector<std::string> &sv) const
{
    for (std::vector<std::string>::const_iterator i=sv.begin(); i!=sv.end(); ++i) {
        std::string tc = *i;
        // some checks whether it's the type
        if (tc.length()!=3) {
            continue;
        }
        if (tc.find_first_of("ABCDEFGHPX")!=0) {
            continue;
        }
        if (tc[1]<'0'||tc[1]>'9') {
            continue;
        }
        if (tc[2]<'0'||tc[2]>'9') {
            continue;
        }
        // ok, its the type (let's hope)
        return tc;
    }
    throw ProcessError("Could not determine type for an edge..."); // !!! be more verbose

}


SUMOReal
NITigerLoader::getSpeed(const std::string &type) const
{
    switch (type[0]) {
    case 'A':
        switch (type[1]) {
        case '1':
            return (SUMOReal) 85.0 / (SUMOReal) 3.6 *(SUMOReal) 1.6;
        case '2':
            return (SUMOReal) 85.0 / (SUMOReal) 3.6 *(SUMOReal) 1.6;
        case '3':
            return (SUMOReal) 55.0 / (SUMOReal) 3.6 *(SUMOReal) 1.6;
        case '4':
            return (SUMOReal) 35.0 / (SUMOReal) 3.6 *(SUMOReal) 1.6;
        case '5':
            return (SUMOReal) 10.0 / (SUMOReal) 3.6 *(SUMOReal) 1.6;
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
    switch (type[0]) {
    case 'A':
        switch (type[1]) {
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



/****************************************************************************/

