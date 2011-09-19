/****************************************************************************/
/// @file    NWWriter_OpenDrive.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 04.05.2011
/// @version $Id$
///
// Exporter writing networks using the openDRIVE format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include "NWWriter_OpenDrive.h"
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods
// ---------------------------------------------------------------------------
void
NWWriter_OpenDrive::writeNetwork(const OptionsCont &oc, NBNetBuilder &nb) {
    // check whether a matsim-file shall be generated
    if (!oc.isSet("opendrive-output")) {
        return;
    }
    OutputDevice& device = OutputDevice::getDevice(oc.getString("opendrive-output"));
    device << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    device << "<OpenDRIVE>\n";
    device << "    <header revMajor=\"1\" revMinor=\"3\" name=\"\" version=\"1.00\" date=\"!!!\" north=\"0.0000000000000000e+00\" south=\"0.0000000000000000e+00\" east=\"0.0000000000000000e+00\" west=\"0.0000000000000000e+00\" maxRoad=\"517\" maxJunc=\"2\" maxPrg=\"0\">\n";
    // write normal edges (road)
    const NBEdgeCont &ec = nb.getEdgeCont();
    for (std::map<std::string, NBEdge*>::const_iterator i=ec.begin(); i!=ec.end(); ++i) {
        const NBEdge *e = (*i).second;
        device << "    <road name=\"" << e->getStreetName() << "\" length=\"" << e->getLength() << "\" id=\"" << e->getID() << "\" junction=\"-1\">\n";
        device << "        <link>\n";
        device << "            <predecessor elementType=\"junction\" elementId=\"" << e->getFromNode()->getID() << "\"/>\n";
        device << "            <successor elementType=\"junction\" elementId=\"" << e->getToNode()->getID() << "\"/>\n";
        device << "        </link>\n";
        device << "        <type s=\"0\" type=\"town\"/>\n";
        device << "        <planView>\n";
        const PositionVector &pv = e->getGeometry();
        SUMOReal offset = 0;
        for(unsigned int j=0; j<pv.size()-1; ++j) {
            const Position &p = pv[j];
            Line l = pv.lineAt(j);
            device << "            <geometry s=\"" << offset << "\" x=\"" << p.x() << "\" y=\"" << p.y() << "\" hdg=\"" << l.atan2Angle() << "\" length=\"" << l.length() << "\"><line/></geometry>\n";
        }
        device << "        </planView>\n";
        device << "        <elevationProfile><elevation s=\"0\" a=\"0\" b=\"0\" c=\"0\" d=\"0\"/></elevationProfile>\n";
        device << "        <lateralProfile></lateralProfile>\n";
        device << "        <lanes>\n";
        device << "            <laneSection s=\"0\">\n";
        device << "                <center>\n";
        device << "                    <lane id=\"0\" type=\"driving\" level= \"0\">\n";
        device << "                        <link></link>\n";
        device << "                        <roadMark sOffset=\"0\" type=\"solid\" weight=\"standard\" color=\"standard\" width=\"0.13\"/>\n";
        device << "                    </lane>\n";
        device << "                </center>\n";
        device << "                <right>\n";
        const std::vector<NBEdge::Lane> &lanes = e->getLanes();
        for(int j=e->getNumLanes(); --j>=0;) {
            device << "                    <lane id=\"-" << e->getNumLanes()-j <<"\" type=\"driving\" level=\"0\">\n";
            device << "                        <link>\n";
            //device << "                            <predecessor id=\"1\"/>\n";// !!!
            //device << "                            <successor id=\"-1\"/>\n";// !!!
            device << "                        </link>\n";
            device << "                        <width sOffset=\"0\" a=\"" << lanes[j].width << "\" b=\"0\" c=\"0\" d=\"0\"/>\n";
            device << "                        <roadMark sOffset=\"0\" type=\"broken\" weight=\"standard\" color=\"standard\" width=\"0.13\"/>\n";
            device << "                    </lane>\n";
        }
        device << "                 </right>\n";
        device << "            </laneSection>\n";
        device << "        </lanes>\n";
        device << "        <objects></objects>        <signals></signals>\n";
        device << "    </road>\n";
    }
    // write junction-internal edges (road)
    const NBNodeCont &nc = nb.getNodeCont();
    for (std::map<std::string, NBNode*>::const_iterator i=nc.begin(); i!=nc.end(); ++i) {
    }

    device << "</OpenDRIVE>\n";
    device.close();
}


/****************************************************************************/

