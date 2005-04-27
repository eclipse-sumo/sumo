//---------------------------------------------------------------------------//
//                        NIArcView_Loader.cpp -
//  The loader of arcview-files
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.13  2005/04/27 12:24:24  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.12  2004/08/02 12:43:07  dkrajzew
// got rid of the shapelib-interface; conversion of geocoordinates added
//
// Revision 1.11  2004/01/27 10:33:22  dkrajzew
// patched some linux-warnings
//
// Revision 1.10  2004/01/12 15:53:00  dkrajzew
// work on code style
//
// Revision 1.9  2004/01/12 15:28:39  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.8  2003/12/04 16:53:53  dkrajzew
// native ArcView-importer by ericnicolay added
//
// Revision 1.7  2003/07/22 15:11:24  dkrajzew
// removed warnings
//
// Revision 1.6  2003/07/07 08:24:17  dkrajzew
// adapted the importer to the lane geometry description
//
// Revision 1.5  2003/06/18 11:34:25  dkrajzew
// the arcview-import should be more stable nw when dealing with false tables
//
// Revision 1.4  2003/06/05 11:44:14  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/common/FileErrorReporter.h>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>
#include <utils/convert/TplConvert.h>
#include <utils/importio/LineHandler.h>
#include <utils/importio/LineReader.h>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeomHelper.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NIArcView_Loader.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIArcView_Loader::NIArcView_Loader(NBNodeCont &nc,
                                   NBEdgeCont &ec,
                                   const std::string &dbf_name,
                                   const std::string &shp_name)
    : FileErrorReporter("Navtech Edge description", dbf_name),
//    myShapeReader(shp_name), myIsFirstLine(true),
    /*myLineReader(dbf_name),*/ myDBFName(dbf_name), mySHPName(shp_name),
    myNameAddition(0),
    myNodeCont(nc), myEdgeCont(ec)
{
}


NIArcView_Loader::~NIArcView_Loader()
{
}


void
NIArcView_Loader::load(OptionsCont &)
{
    int i = myBinShapeReader.openFiles(mySHPName.c_str(), myDBFName.c_str() );
    if( i != 0 ) {
        MsgHandler::getErrorInstance()->inform(
            "Could not open shape description.");
        return;
    }
    parseBin();
}


//en
bool
NIArcView_Loader::parseBin()
{
    for ( int i =0 ; i < myBinShapeReader.getShapeCount(); i++) {
        string id = myBinShapeReader.getAttribute( "LINK_ID" );
        string name = myBinShapeReader.getAttribute("ST_NAME");
        string from_node = myBinShapeReader.getAttribute("REF_IN_ID");
        string to_node = myBinShapeReader.getAttribute("NREF_IN_ID");
        string type = myBinShapeReader.getAttribute("ST_TYP_AFT");
        double speed = 0;
        size_t nolanes = 0;
        int priority = 0;
        try {
            speed = getSpeed(id);
            nolanes = getLaneNo(id);
            priority = getPriority(id);
        } catch (...) {
            addError(
                string("An attribute is not given within the file!"));
            return false;
        }
        NBEdge::EdgeBasicFunction function = NBEdge::EDGEFUNCTION_NORMAL;
        NBNode *from = 0;
        NBNode *to = 0;
        // build from-node
        Position2D from_pos = myBinShapeReader.getFromNodePosition();
        if(!myNodeCont.insert(from_node, from_pos)) {
            from = new NBNode(from_node + string("___") + toString<int>(myNameAddition++),
                from_pos);
            myNodeCont.insert(from);
        } else {
            from = myNodeCont.retrieve(from_pos);
        }
        // build to-node
        Position2D to_pos = myBinShapeReader.getToNodePosition();
        if(!myNodeCont.insert(to_node, to_pos)) {
            to = new NBNode(to_node + string("___") + toString<int>(myNameAddition++),
                to_pos);
            myNodeCont.insert(to);
        } else {
            to = myNodeCont.retrieve(to_pos);
        }
            // retrieve length
        double length = myBinShapeReader.getLength();

        // retrieve the information whether the street is bi-directional
        string dir = myBinShapeReader.getAttribute("DIR_TRAVEL");
            // add positive direction if wanted
        if(dir=="B"||dir=="F") {
            if(myEdgeCont.retrieve(id)==0) {
                NBEdge::LaneSpreadFunction spread = dir=="B"
                    ? NBEdge::LANESPREAD_RIGHT
                    : NBEdge::LANESPREAD_CENTER;
                NBEdge *edge = new NBEdge(id, name, from, to, type, speed, nolanes,
                    length, priority, myBinShapeReader.getShape(), spread, function);
                myEdgeCont.insert(edge);
            }
        }
            // add negative direction if wanted
        if(dir=="B"||dir=="T") {
            id = "-" + id;
            if(myEdgeCont.retrieve(id)==0) {
                NBEdge::LaneSpreadFunction spread = dir=="B"
                    ? NBEdge::LANESPREAD_RIGHT
                    : NBEdge::LANESPREAD_CENTER;
                NBEdge *edge = new NBEdge(id, name, to, from, type, speed, nolanes,
                    length, priority, myBinShapeReader.getReverseShape(), spread, function);
                myEdgeCont.insert(edge);
            }
        }
        myBinShapeReader.forwardShape();
    }
    return !MsgHandler::getErrorInstance()->wasInformed();


}
//en


double
NIArcView_Loader::getSpeed(const std::string &edgeid)
{
    try {
        int speedcat =
            TplConvert<char>::_2int(myBinShapeReader.getAttribute("SPEED_CAT").c_str());
        switch(speedcat) {
        case 1:
            return 300.0 / 3.6;
        case 2:
            return 130. / 3.6;
        case 3:
            return 100. / 3.6;
        case 4:
            return 90. / 3.6;
        case 5:
            return 70. / 3.6;
        case 6:
            return 50. / 3.6;
        case 7:
            return 30. / 3.6;
        case 8:
            return 10. / 3.6;
        default:
            throw 1;
        }
    } catch (...) {
        addError(
            string("Error on parsing edge speed definition for edge '")
            + edgeid + string("'."));
    }
    return 0;
}


size_t
NIArcView_Loader::getLaneNo(const std::string &edgeid)
{

    try {
        try{
            size_t lanecat =
            TplConvert<char>::_2int(myBinShapeReader.getAttribute("rnol").c_str());
            return lanecat;
        } catch(...) {
            size_t lanecat =
                TplConvert<char>::_2int(myBinShapeReader.getAttribute("LANE_CAT").c_str());
            switch(lanecat) {
            case 1:
                return 1;
            case 2:
                return 2;
            case 3:
                return 4;
            default:
                throw 1;
            }
        }
    } catch (...) {
        addError(
            string("Error on parsing edge's noumber of lanes information for edge '")
            + edgeid + string("'."));
        return 0;
    }
}

double
NIArcView_Loader::getLength(const Position2D &from_pos, const Position2D &to_pos)
{
    return GeomHelper::distance(from_pos, to_pos);
}


int
NIArcView_Loader::getPriority(const std::string &edgeid)
{
    try {
        int prio =
            TplConvert<char>::_2int(myBinShapeReader.getAttribute("FUNC_CLASS").c_str());
        return 5 - prio;
    } catch (...) {
        addError(
            string("Error on parsing edge priority information for edge '")
            + edgeid + string("'."));
        return 0;
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


