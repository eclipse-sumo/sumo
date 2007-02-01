/****************************************************************************/
/// @file    NIVisumParser_EdgePolys.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 02. Nov 2004
/// @version $Id: $
///
// Parser for visum-edge-geometries
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

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
#include <utils/geoconv/GeoConvHelper.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NIVisumParser_EdgePolys::NIVisumParser_EdgePolys(NIVisumLoader &parent,
        NBNodeCont &nc, const std::string &dataName)
        : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
        myNodeCont(nc)
{}


NIVisumParser_EdgePolys::~NIVisumParser_EdgePolys()
{}


void
NIVisumParser_EdgePolys::myDependentReport()
{
    string id;
    try {
        // get the from- & to-node and validate them
        NBNode *from = getNamedNode(myNodeCont, "STRECKE", "VonKnot", "VonKnotNr");
        NBNode *to = getNamedNode(myNodeCont, "STRECKE", "NachKnot", "NachKnotNr");
        if (from==0||to==0||!checkNodes(from, to)) {
            return;
        }
        bool failed = false;
        int index;
        SUMOReal x, y;
        try {
            index = TplConvert<char>::_2int(myLineParser.get("INDEX").c_str());
            x = getNamedFloat("XKoord");
            y = getNamedFloat("YKoord");
        } catch (NumberFormatException&) {
            MsgHandler::getErrorInstance()->inform("Error in geometry description from node '" + from->getID() + "' to node '" + to->getID() + "'.");
            return;
        }
        myNodeCont.addGeoreference(Position2D((SUMOReal)(x / 100000.0), (SUMOReal)(y / 100000.0)));
        Position2D pos(x, y);
        GeoConvHelper::remap(pos);
        NBEdge *e = from->getConnectionTo(to);
        if (e!=0) {
            e->addGeometryPoint(index, pos);
        } else {
            failed = true;
        }
        e = to->getConnectionTo(from);
        if (e!=0) {
            e->addGeometryPoint(-index, pos);
            failed = false;
        }
        // check whether the operation has failed
        if (failed) {
            // we should report this to the warning instance only if we have removed
            //  some nodes or edges...
            if (OptionsSubSys::getOptions().isSet("edges-min-speed")
                    ||
                    OptionsSubSys::getOptions().isSet("keep-edges")) {

                WRITE_WARNING("Could not set geometry between node '" + from->getID() + "' and node '" + to->getID() + "'.");
            } else {
                // ... in the other case we report this to the error instance
                MsgHandler::getErrorInstance()->inform("There is no edge from node '" + from->getID() + "' to node '" + to->getID() + "'.");
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
    if (from==0) {
        addError(" The from-node was not found within the net");
    }
    if (to==0) {
        addError(" The to-node was not found within the net");
    }
    if (from==to) {
        addError(" Both nodes are the same");
    }
    return from!=0&&to!=0&&from!=to;
}



/****************************************************************************/

