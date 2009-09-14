/****************************************************************************/
/// @file    NIImporter_OpenDrive.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 14.04.2008
/// @version $Id: NIImporter_OpenDrive.cpp 7615 2009-08-06 23:59:51Z behrisch $
///
// Importer for networks stored in openDrive format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "NIImporter_OpenDrive.h"
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLSubSys.h>

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
// ---------------------------------------------------------------------------
// static methods (interface in this case)
// ---------------------------------------------------------------------------
void
NIImporter_OpenDrive::loadNetwork(const OptionsCont &oc, NBNetBuilder &nb) {
    // check whether the option is set (properly)
    if (!oc.isUsableFileList("opendrive")) {
        return;
    }
    // build the handler
    NIImporter_OpenDrive handler(nb.getNodeCont());
    // parse file(s)
    vector<string> files = oc.getStringVector("opendrive");
    for (vector<string>::const_iterator file=files.begin(); file!=files.end(); ++file) {
        if (!FileHelpers::exists(*file)) {
            MsgHandler::getErrorInstance()->inform("Could not open opendrive file '" + *file + "'.");
            return;
        }
        handler.setFileName(*file);
        MsgHandler::getMessageInstance()->beginProcessMsg("Parsing opendrive from '" + *file + "'...");
        XMLSubSys::runParser(handler, *file);
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
    // build edges/nodes
}



// ---------------------------------------------------------------------------
// loader methods
// ---------------------------------------------------------------------------
NIImporter_OpenDrive::NIImporter_OpenDrive(NBNodeCont &nc)
        : SUMOSAXHandler("opendrive"), myCurrentEdge("", "", -1)
{}


NIImporter_OpenDrive::~NIImporter_OpenDrive() throw() {
}


void
NIImporter_OpenDrive::myStartElement(SumoXMLTag element,
                                const SUMOSAXAttributes &attrs) throw(ProcessError) {
    bool ok = true;
    switch (element) {
    case SUMO_TAG_OPENDRIVE_HEADER: {
            int majorVersion = attrs.getIntReporting(SUMO_ATTR_OPENDRIVE_REVMAJOR, "opendrive header", 0, ok);
            int minorVersion = attrs.getIntReporting(SUMO_ATTR_OPENDRIVE_REVMINOR, "opendrive header", 0, ok);
            if(majorVersion!=1||minorVersion!=2) {
                MsgHandler::getWarningInstance()->inform("Given openDrive file '" + getFileName() + "' uses version " + toString(majorVersion) + "." + toString(minorVersion) + ";\n Version 1.2 is supported.");
            }
        }
        break;
    case SUMO_TAG_OPENDRIVE_ROAD: {
            std::string id = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_ID, "road", 0, ok);
            std::string junction = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_JUNCTION, "road", id.c_str(), ok);
            SUMOReal length = attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_LENGTH, "road", id.c_str(), ok);
            myCurrentEdge = OpenDriveEdge(id, junction, length);
        }
        break;
    case SUMO_TAG_OPENDRIVE_PREDECESSOR: 
        {
            std::string elementType = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_ELEMENTTYPE, "predecessor", myCurrentEdge.id.c_str(), ok);
            std::string elementID = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_ELEMENTID, "predecessor", myCurrentEdge.id.c_str(), ok);
            std::string contactPoint = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_CONTACTPOINT, "predecessor", myCurrentEdge.id.c_str(), ok);
            addLink(OPENDRIVE_LT_PREDECESSOR, elementType, elementID, contactPoint);
        }
        break;
    case SUMO_TAG_OPENDRIVE_SUCCESSOR: 
        {
            std::string elementType = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_ELEMENTTYPE, "successor", myCurrentEdge.id.c_str(), ok);
            std::string elementID = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_ELEMENTID, "successor", myCurrentEdge.id.c_str(), ok);
            std::string contactPoint = attrs.getStringReporting(SUMO_ATTR_OPENDRIVE_CONTACTPOINT, "successor", myCurrentEdge.id.c_str(), ok);
            addLink(OPENDRIVE_LT_SUCCESSOR, elementType, elementID, contactPoint);
        }
        break;
    case SUMO_TAG_OPENDRIVE_GEOMETRY: 
        {
            SUMOReal length = attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_LENGTH, "geometry", myCurrentEdge.id.c_str(), ok);
            SUMOReal s = attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_S, "geometry", myCurrentEdge.id.c_str(), ok);
            SUMOReal x = attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_X, "geometry", myCurrentEdge.id.c_str(), ok);
            SUMOReal y = attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_Y, "geometry", myCurrentEdge.id.c_str(), ok);
            SUMOReal hdg = attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_HDG, "geometry", myCurrentEdge.id.c_str(), ok);
            myCurrentEdge.geometries.push_back(OpenDriveGeometry(length, s, x, y, hdg));
        }
        break;
    case SUMO_TAG_OPENDRIVE_LINE: 
        {
            std::vector<SUMOReal> vals;
            addGeometryShape(OPENDRIVE_GT_LINE, vals);
        }
        break;
    case SUMO_TAG_OPENDRIVE_SPIRAL: 
        {
            std::vector<SUMOReal> vals;
            vals.push_back(attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_CURVSTART, "spiral", myCurrentEdge.id.c_str(), ok));
            vals.push_back(attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_CURVEND, "spiral", myCurrentEdge.id.c_str(), ok));
            addGeometryShape(OPENDRIVE_GT_LINE, vals);
        }
        break;
    case SUMO_TAG_OPENDRIVE_ARC: 
        {
            std::vector<SUMOReal> vals;
            vals.push_back(attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_CURVATURE, "arc", myCurrentEdge.id.c_str(), ok));
            addGeometryShape(OPENDRIVE_GT_LINE, vals);
        }
        break;
    case SUMO_TAG_OPENDRIVE_POLY3: 
        {
            std::vector<SUMOReal> vals;
            vals.push_back(attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_A, "poly3", myCurrentEdge.id.c_str(), ok));
            vals.push_back(attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_B, "poly3", myCurrentEdge.id.c_str(), ok));
            vals.push_back(attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_C, "poly3", myCurrentEdge.id.c_str(), ok));
            vals.push_back(attrs.getSUMORealReporting(SUMO_ATTR_OPENDRIVE_D, "poly3", myCurrentEdge.id.c_str(), ok));
            addGeometryShape(OPENDRIVE_GT_LINE, vals);
        }
        break;
    default:
        break;
    }
}


void
NIImporter_OpenDrive::myCharacters(SumoXMLTag element,
                              const std::string &chars) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_LANE:
        break;
    }
}



void
NIImporter_OpenDrive::myEndElement(SumoXMLTag element) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_EDGE:
        break;
    }
}



void 
NIImporter_OpenDrive::addLink(LinkType lt, const std::string &elementType, 
                              const std::string &elementID, 
                              const std::string &contactPoint) throw(ProcessError)
{
    OpenDriveLink l(lt, elementID);
    // elementType
    if(elementType=="road") {
        l.elementType = OPENDRIVE_ET_ROAD;
    } else if(elementType=="junction") {
        l.elementType = OPENDRIVE_ET_JUNCTION;
    }
    // contact point
    if(contactPoint=="start") {
        l.contactPoint = OPENDRIVE_CP_START;
    } else if(contactPoint=="end") {
        l.contactPoint = OPENDRIVE_CP_END;
    }
    // add
    myCurrentEdge.links.push_back(l);
}


void 
NIImporter_OpenDrive::addGeometryShape(GeometryType type, const std::vector<SUMOReal> &vals) throw(ProcessError)
{
    // checks
    if(myCurrentEdge.geometries.size()==0) {
        throw ProcessError("Mismatching paranthesis in geometry definition for road '" + myCurrentEdge.id + "'");
    }
    OpenDriveGeometry &last = myCurrentEdge.geometries[myCurrentEdge.geometries.size()-1];
    if(last.type==OPENDRIVE_GT_UNKNOWN) {
        throw ProcessError("Double geometry information for road '" + myCurrentEdge.id + "'");
    }
    // set
    last.type = type;
    last.params = vals;
}


/****************************************************************************/

