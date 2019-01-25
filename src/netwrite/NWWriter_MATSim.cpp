/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NWWriter_MATSim.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 04.05.2011
/// @version $Id$
///
// Exporter writing networks using the MATSim format
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>
#include "NWWriter_MATSim.h"
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>



// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods
// ---------------------------------------------------------------------------
void
NWWriter_MATSim::writeNetwork(const OptionsCont& oc, NBNetBuilder& nb) {
    // check whether a matsim-file shall be generated
    if (!oc.isSet("matsim-output")) {
        return;
    }
    OutputDevice& device = OutputDevice::getDevice(oc.getString("matsim-output"));
    device << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    device << "<!DOCTYPE network SYSTEM \"http://www.matsim.org/files/dtd/network_v1.dtd\">\n\n";
    device << "<network name=\"NAME\">\n"; // !!! name
    // write nodes
    device << "   <nodes>\n";
    NBNodeCont& nc = nb.getNodeCont();
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        device << "      <node id=\"" << (*i).first
               << "\" x=\"" << (*i).second->getPosition().x()
               << "\" y=\"" << (*i).second->getPosition().y()
               << "\"/>\n";
    }
    device << "   </nodes>\n";
    // write edges
    device << "   <links capperiod=\"01:00:00\">\n";
    NBEdgeCont& ec = nb.getEdgeCont();
    for (std::map<std::string, NBEdge*>::const_iterator i = ec.begin(); i != ec.end(); ++i) {
        device << "      <link id=\"" << (*i).first
               << "\" from=\"" << (*i).second->getFromNode()->getID()
               << "\" to=\"" << (*i).second->getToNode()->getID()
               << "\" length=\"" << (*i).second->getLoadedLength()
               << "\" capacity=\"" << (oc.getFloat("lanes-from-capacity.norm") * (*i).second->getNumLanes())
               << "\" freespeed=\"" << (*i).second->getSpeed()
               << "\" permlanes=\"" << (*i).second->getNumLanes()
               << "\"/>\n";
    }
    device << "   </links>\n";
    //
    device << "</network>\n"; // !!! name
    device.close();
}


/****************************************************************************/

