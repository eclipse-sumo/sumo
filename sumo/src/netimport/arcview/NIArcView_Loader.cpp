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
//
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
#include "NIArcView_ShapeReader.h"
#include "NIArcView_Loader.h"



/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIArcView_Loader::NIArcView_Loader(const std::string &dbf_name,
                                   const std::string &shp_name)
    : FileErrorReporter("Navtech Edge description", dbf_name),
    myShapeReader(shp_name), myIsFirstLine(true),
    myLineReader(dbf_name)
{
    dbfname = dbf_name;
    shpname = shp_name;
}


NIArcView_Loader::~NIArcView_Loader()
{
}


void
NIArcView_Loader::load(OptionsCont &)
{
    int i = s.openFiles(shpname.c_str(), dbfname.c_str() );
    if( i == 0 )
    {
        bin_modus = true;
        parseBin();
    }
    else
    {
    bin_modus = false;
    // read names from first line
    myIsFirstLine = true;
    myLineReader.readLine(*this);
    // read data
    myCurrentLink = 0;
    myIsFirstLine = false;
    myLineReader.readAll(*this);
    }
}


bool
NIArcView_Loader::report(const std::string &line)
{
    // skip first line
    if(myIsFirstLine) {
        myColumnsParser.reinit(line, ";", ";", true);
        return false;
    }
    // parse following lines
    return parseLine(line);
}

//en
bool
NIArcView_Loader::parseBin()
{
    char * ret;
    ret = "falsch";
    for ( int i =0 ; i < s.getShapeCount(); i++){
        string id = s.getAttribute( "LINK_ID" );
        string name = s.getAttribute("ST_NAME");
        string from_node = s.getAttribute("REF_IN_ID");
        string to_node = s.getAttribute("NREF_IN_ID");
        string type = s.getAttribute("ST_TYP_AFT");
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
        Position2D from_pos = s.getFromNodePosition();
        if(!NBNodeCont::insert(from_node, from_pos.x(), from_pos.y())) {
            addError(
                string("A false from-node occured (id='") + from_node
                + string("', pos=(") + toString(from_pos.x()) + string(", ")
                + toString(from_pos.y()) + string("))."));
            return false;
        } else {
            from = NBNodeCont::retrieve(from_pos.x(), from_pos.y());
        }
        Position2D to_pos = s.getToNodePosition();
        if(!NBNodeCont::insert(to_node, to_pos.x(), to_pos.y())) {
            addError(
                string("A false to-node occured (id='") + from_node
                + string("', pos=(") + toString(to_pos.x()) + string(", ")
                + toString(to_pos.y()) + string("))."));
            return false;
        } else {
            to = NBNodeCont::retrieve(to_pos.x(), to_pos.y());
        }
            // retrieve length
        double length = s.getLength();

        // retrieve the information whether the street is bi-directional
        string dir = s.getAttribute("DIR_TRAVEL");
            // add positive direction if wanted
        if(dir=="B"||dir=="F") {
            if(NBEdgeCont::retrieve(id)==0) {
                NBEdge::LaneSpreadFunction spread = dir=="B"
                    ? NBEdge::LANESPREAD_RIGHT
                    : NBEdge::LANESPREAD_CENTER;
                NBEdge *edge = new NBEdge(id, name, from, to, type, speed, nolanes,
                    length, priority, s.getShape(), spread, function);
                NBEdgeCont::insert(edge);
            }
        }
            // add negative direction if wanted
        if(dir=="B"||dir=="T") {
            id = "-" + id;
            if(NBEdgeCont::retrieve(id)==0) {
                NBEdge::LaneSpreadFunction spread = dir=="B"
                    ? NBEdge::LANESPREAD_RIGHT
                    : NBEdge::LANESPREAD_CENTER;
                NBEdge *edge = new NBEdge(id, name, to, from, type, speed, nolanes,
                    length, priority, s.getReverseShape(), spread, function);
                NBEdgeCont::insert(edge);
            }
        }
        s.forwardShape();
    }
    return !MsgHandler::getErrorInstance()->wasInformed();


}
//en

bool
NIArcView_Loader::parseLine(const std::string &line)
{
    // omit empty lines
    if(StringUtils::prune(line)=="") {
        return false;
    }
    myColumnsParser.parseLine(line);
    // extract values
    string id = getStringSecure("LINK_ID");
    string name = getStringSecure("ST_NAME");
    string from_node = getStringSecure("REF_IN_ID");
    string to_node = getStringSecure("NREF_IN_ID");
    string type = getStringSecure("ST_TYP_AFT");
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
        // extract shape
    myShapeReader.readShape(myCurrentLink++);
    if(MsgHandler::getErrorInstance()->wasInformed()) {
        return false;
    }
    // build and check nodes
    NBNode *from = 0;
    NBNode *to = 0;
    Position2D from_pos = myShapeReader.getFromNodePosition();
    if(!NBNodeCont::insert(from_node, from_pos.x(), from_pos.y())) {
        addError(
            string("A false from-node occured (id='") + from_node
            + string("', pos=(") + toString(from_pos.x()) + string(", ")
            + toString(from_pos.y()) + string("))."));
        return false;
    } else {
        from = NBNodeCont::retrieve(from_pos.x(), from_pos.y());
    }
    Position2D to_pos = myShapeReader.getToNodePosition();
    if(!NBNodeCont::insert(to_node, to_pos.x(), to_pos.y())) {
        addError(
            string("A false to-node occured (id='") + from_node
            + string("', pos=(") + toString(to_pos.x()) + string(", ")
            + toString(to_pos.y()) + string("))."));
        return false;
    } else {
        to = NBNodeCont::retrieve(to_pos.x(), to_pos.y());
    }
        // retrieve length
    double length = getLength(/*id, */from_pos, to_pos);

    // retrieve the information whether the street is bi-directional
    string dir = myColumnsParser.get("DIR_TRAVEL", true);
        // add positive direction if wanted
    if(dir=="B"||dir=="F") {
        if(NBEdgeCont::retrieve(id)==0) {
            NBEdge::LaneSpreadFunction spread = dir=="B"
                ? NBEdge::LANESPREAD_RIGHT
                : NBEdge::LANESPREAD_CENTER;
            NBEdge *edge = new NBEdge(id, name, from, to, type, speed, nolanes,
                length, priority, myShapeReader.getShape(), spread, function);
            NBEdgeCont::insert(edge);
        }
    }
        // add negative direction if wanted
    if(dir=="B"||dir=="T") {
        id = "-" + id;
        if(NBEdgeCont::retrieve(id)==0) {
            NBEdge::LaneSpreadFunction spread = dir=="B"
                ? NBEdge::LANESPREAD_RIGHT
                : NBEdge::LANESPREAD_CENTER;
            NBEdge *edge = new NBEdge(id, name, to, from, type, speed, nolanes,
                length, priority, myShapeReader.getReverseShape(), spread, function);
            NBEdgeCont::insert(edge);
        }
    }
    return !MsgHandler::getErrorInstance()->wasInformed();
}


string
NIArcView_Loader::getStringSecure(const std::string &which)
{
    try {
        return myColumnsParser.get(which, true);
    } catch (UnknownElement &e) {
        addError(
            string("The attribute '") + which
            + string("' is not given within the file!"));
    } catch (OutOfBoundsException &e) {
        addError(
            string("The attribute '") + which
            + string("' is not given within the file!"));
    }
}


double
NIArcView_Loader::getSpeed(const std::string &edgeid)
{
    try{
    if ( bin_modus )
    {
        int speedcat = TplConvert<char>::_2int(s.getAttribute("SPEED_CAT").c_str());
        switch(speedcat) {
            case 1:
                return 300 / 3.6;
            case 2:
                return 130 / 3.6;
            case 3:
                return 100 / 3.6;
            case 4:
                return 90 / 3.6;
            case 5:
                return 70 / 3.6;
            case 6:
                return 50 / 3.6;
            case 7:
                return 30 / 3.6;
            case 8:
                return 10 / 3.6;
            default:
                throw 1;
            }
    }
    else
    {

            int speedcat = TplConvert<char>::_2int(myColumnsParser.get("SPEED_CAT", true).c_str());
            switch(speedcat) {
            case 1:
                return 300 / 3.6;
            case 2:
                return 130 / 3.6;
            case 3:
                return 100 / 3.6;
            case 4:
                return 90 / 3.6;
            case 5:
                return 70 / 3.6;
            case 6:
                return 50 / 3.6;
            case 7:
                return 30 / 3.6;
            case 8:
                return 10 / 3.6;
            default:
                throw 1;
            }
            }
        } catch (...) {
            if(myColumnsParser.get("SPEED_CAT", true)=="NA") {
                MsgHandler::getWarningInstance()->inform(
                    string("non-applicable speed definition found for edge '")
                    + edgeid + string("'")); // !!! Warning-level
                MsgHandler::getWarningInstance()->inform("Using 30km/h");
                return 30 / 3.6;
            }
            addError(
                string("Error on parsing edge speed definition for edge '")
                + edgeid + string("'."));
            return 0;
        }
}


size_t
NIArcView_Loader::getLaneNo(const std::string &edgeid)
{

    try {
    if ( bin_modus )
    {
        try{
	        size_t lanecat = TplConvert<char>::_2int(s.getAttribute("rnol").c_str());
			return lanecat;
		}
		catch(...)
		{
	        size_t lanecat = TplConvert<char>::_2int(s.getAttribute("LANE_CAT").c_str());
		    switch(lanecat) {
	        case 1:
		        return 1;
			case 2:
				return 2;
	        case 3:
		        return 4;
			default:
				throw 1;
	}	}
    }
    else
    {
		try
		{
	        size_t lanecat = TplConvert<char>::_2int(myColumnsParser.get("rnol", true).c_str());
			return lanecat;
		}
		catch(...)
		{
	        size_t lanecat = TplConvert<char>::_2int(myColumnsParser.get("LANE_CAT", true).c_str());
		    switch(lanecat) {
	        case 1:
		        return 1;
			case 2:
				return 2;
	        case 3:
		        return 4;
			default:
				throw 1;
	}		}
	}
    } catch (...) {
        if(myColumnsParser.get("LANE_CAT", true)=="NA") {
            MsgHandler::getWarningInstance()->inform(
                string("non-applicable number of lanes found for edge '")
                + edgeid + string("'"));// !!! Warning-level
            MsgHandler::getWarningInstance()->inform("Using '1'");
            return 1;
        }
        addError(
            string("Error on parsing edge's noumber of lanes information for edge '")
            + edgeid + string("'."));
        return 0;
    }
}

double
NIArcView_Loader::getLength(//const std::string &,
                           const Position2D &from_pos, const Position2D &to_pos)
{
    return GeomHelper::distance(from_pos, to_pos);
}


int
NIArcView_Loader::getPriority(const std::string &edgeid)
{
    try {
        if ( bin_modus )
        {
            int prio = TplConvert<char>::_2int(s.getAttribute("FUNC_CLASS").c_str());
            return 5 - prio;
        }
        else
        {
            int prio = TplConvert<char>::_2int(myColumnsParser.get("FUNC_CLASS", true).c_str());
            return 5 - prio;
        }
    } catch (...) {
        if(myColumnsParser.get("FUNC_CLASS", true)=="NA") {
            MsgHandler::getWarningInstance()->inform(
                string("non-applicable priority found for edge '")
                + edgeid + string("'")); // !!! Warning-level
            MsgHandler::getWarningInstance()->inform("Using '0'");
            return 0;
        }
        addError(
            string("Error on parsing edge priority information for edge '")
            + edgeid + string("'."));
        return 0;
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIArcView_Loader.icc"
//#endif

// Local Variables:
// mode:C++
// End:


