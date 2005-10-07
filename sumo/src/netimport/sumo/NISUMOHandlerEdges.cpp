//---------------------------------------------------------------------------//
//                        NISUMOHandlerEdges.cpp -
//  A handler for SUMO edges
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
// Revision 1.7  2005/10/07 11:39:47  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:02:34  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/04/27 12:24:35  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.3  2004/01/12 15:31:53  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.2  2003/06/18 11:15:05  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.1  2003/02/07 11:13:27  dkrajzew
// names changed
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
#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>
#include <netbuild/NLLoadFilter.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "NISUMOHandlerEdges.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

using namespace std;

NISUMOHandlerEdges::NISUMOHandlerEdges(NBEdgeCont &ec, NBNodeCont &nc,
                                       LoadFilter what)
    : SUMOSAXHandler("sumo-network"),
    _loading(what), myEdgeCont(ec), myNodeCont(nc)
{
}


NISUMOHandlerEdges::~NISUMOHandlerEdges()
{
}


void
NISUMOHandlerEdges::myStartElement(int element, const std::string &name,
                                   const Attributes &attrs)
{
    if(element==SUMO_TAG_EDGE&&_loading==LOADFILTER_ALL) {
        addEdge(attrs);
    }
}


void
NISUMOHandlerEdges::addEdge(const Attributes &attrs)
{
    string id;
    try {
        // get the id
        id = getString(attrs, SUMO_ATTR_ID);
        // get the name
        string name;
        try {
            name = getString(attrs, SUMO_ATTR_NAME);
        } catch (EmptyData) {
        }
        // get the type
        string type;
        try {
            type = getString(attrs, SUMO_ATTR_TYPE);
        } catch (EmptyData) {
        }
        // get the origin and the destination node
        NBNode *from = getNode(attrs, SUMO_ATTR_FROMNODE, "from", id);
        NBNode *to = getNode(attrs, SUMO_ATTR_TONODE, "to", id);
        if(from==0||to==0) {
            return;
        }
        // get some other parameter
        SUMOReal speed = getFloatReporting(attrs, SUMO_ATTR_SPEED, "speed", id);
        SUMOReal length = getFloatReporting(attrs, SUMO_ATTR_LENGTH, "length",
            id);
        int nolanes = getIntReporting(attrs, SUMO_ATTR_NOLANES,
            "number of lanes", id);
        int priority = getIntReporting(attrs, SUMO_ATTR_PRIORITY,
            "priority", id);
        if(speed>0&&length>0&&nolanes>0&&priority>0) {
            myEdgeCont.insert(new NBEdge(id, name, from, to, type, speed,
                nolanes, length, priority));
        }
    } catch (EmptyData) {
        addError("An edge with an unknown id occured.");
    }
}

NBNode *
NISUMOHandlerEdges::getNode(const Attributes &attrs, unsigned int id,
                            const std::string &dir, const std::string &name)
{
    try {
        string nodename = getString(attrs, id);
        NBNode *node = myNodeCont.retrieve(nodename);
        if(node==0) {
            addError(
                string("The ") + dir + string("-node '") + nodename +
                string("' used within edge '") + name + string("' is not known."));
        }
        return node;
    } catch (EmptyData) {
        addError(
            string("Missing ") + dir + string("-node name for edge with id '")
            + name + string("'"));
    }
    return 0;
}

SUMOReal
NISUMOHandlerEdges::getFloatReporting(const Attributes &attrs,
                                      AttrEnum id, const std::string &name,
                                      const std::string &objid)
{
    try {
        return getFloat(attrs, id);
    } catch (EmptyData) {
        addError(
            string("The ") + name + string(" is not given within the object '")
            + objid + string("'."));
    } catch (NumberFormatException) {
        addError(
            string("The ") + name + string(" is not numeric within the object '")
            + objid + string("'."));
    }
    return -1;
}


int
NISUMOHandlerEdges::getIntReporting(const Attributes &attrs,
                                    AttrEnum id, const std::string &name,
                                    const std::string &objid)
{
    try {
        return getInt(attrs, id);
    } catch (EmptyData) {
        addError(
            string("The ") + name + string(" is not given within the object '")
            + objid + string("'."));
    } catch (NumberFormatException) {
        addError(
            string("The ") + name + string(" is not numeric within the object '")
            + objid + string("'."));
    }
    return -1;
}

void NISUMOHandlerEdges::myCharacters(int element, const std::string &name,
                                      const std::string &chars)
{
    myCharactersDump(element, name, chars);
}


void NISUMOHandlerEdges::myEndElement(int element, const std::string &name)
{
    myEndElementDump(element, name);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


