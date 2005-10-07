/***************************************************************************
                          NIVisumParser_EdgePolys.cpp
              Parser for visum-edge-geometries
                             -------------------
    project              : SUMO
    begin                : Tue, 02. Nov 2004
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
// Revision 1.6  2005/10/07 11:41:01  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/23 06:03:50  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.4  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/04/27 12:24:41  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.2  2004/12/16 12:23:51  dkrajzew
// a further network prune option added
//
// Revision 1.1  2004/11/23 10:24:54  dkrajzew
// added the possibility to read Visum geometries
//
// Revision 1.1  2004/11/22 12:47:11  dksumo
// added the possibility to parse visum-geometries
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

#include <utils/common/TplConvert.h>
#include <utils/common/TplConvertSec.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBTypeCont.h>
#include <utils/geom/GeomHelper.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_EdgePolys.h"
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>

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
NIVisumParser_EdgePolys::NIVisumParser_EdgePolys(NIVisumLoader &parent,
        NBNodeCont &nc, const std::string &dataName)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
    myNodeCont(nc)
{
}


NIVisumParser_EdgePolys::~NIVisumParser_EdgePolys()
{
}


void
NIVisumParser_EdgePolys::myDependentReport()
{
    string id;
    try {
        // get the from- & to-node and validate them
        NBNode *from = myNodeCont.retrieve(
            NBHelpers::normalIDRepresentation(myLineParser.get("VonKnot")));
        NBNode *to = myNodeCont.retrieve(
            NBHelpers::normalIDRepresentation(myLineParser.get("NachKnot")));
        if(!checkNodes(from, to)) {
            return;
        }
        bool failed = false;
        int index;
        SUMOReal x, y;
        try {
            index = TplConvert<char>::_2int(myLineParser.get("INDEX").c_str());
            x = TplConvert<char>::_2SUMOReal(myLineParser.get("XKoord").c_str());
            y = TplConvert<char>::_2SUMOReal(myLineParser.get("YKoord").c_str());
        } catch(NumberFormatException&) {
            MsgHandler::getErrorInstance()->inform(
                string("Error in geometry description from node '") + from->getID()
                + string("' to node '") + to->getID() + string("'."));
            return;
        }
        NBEdge *e = from->getConnectionTo(to);
        if(e!=0) {
            e->addGeometryPoint(index, Position2D(x, y));
        } else {
            failed = true;
        }
        e = to->getConnectionTo(from);
        if(e!=0) {
            e->addGeometryPoint(-index, Position2D(x, y));
            failed = false;
        }
        if(failed) {
            if( OptionsSubSys::getOptions().isSet("edges-min-speed")
                ||
                OptionsSubSys::getOptions().isSet("keep-edges")) {

                WRITE_WARNING(\
                    string("Could not set geometry between node '") + from->getID()\
                    + string("' and node '") + to->getID() + string("'."));
            } else {
                MsgHandler::getErrorInstance()->inform(
                    string("There is no edge from node '") + from->getID()
                    + string("' to node '") + to->getID() + string("'."));
            }
        }
    } catch (OutOfBoundsException) {
        addError2("STRECKE", id, "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("STRECKE", id, "NumberFormat");
    } catch (UnknownElement) {
        addError2("STRECKE", id, "UnknownElement");
    }
}


bool
NIVisumParser_EdgePolys::checkNodes(NBNode *from, NBNode *to) const
{
    if(from==0) {
        addError(" The from-node was not found within the net");
    }
    if(to==0) {
        addError(" The to-node was not found within the net");
    }
    if(from==to) {
        addError(" Both nodes are the same");
    }
    return from!=0&&to!=0&&from!=to;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
