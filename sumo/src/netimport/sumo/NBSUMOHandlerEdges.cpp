#include <string>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>
#include <netbuild/NLLoadFilter.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "NBSUMOHandlerEdges.h"

using namespace std;

NBSUMOHandlerEdges::NBSUMOHandlerEdges(LoadFilter what, bool warn, bool verbose)
    : SUMOSAXHandler(warn, verbose),
    _loading(what)
{
}


NBSUMOHandlerEdges::~NBSUMOHandlerEdges()
{
}


void
NBSUMOHandlerEdges::myStartElement(int element, const std::string &name,
                                   const Attributes &attrs)
{
    if(element==SUMO_TAG_EDGE&&_loading==LOADFILTER_ALL) {
        addEdge(attrs);
    }
}


void
NBSUMOHandlerEdges::addEdge(const Attributes &attrs)
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
        double speed = getFloatReporting(attrs, SUMO_ATTR_SPEED, "speed", id);
        double length = getFloatReporting(attrs, SUMO_ATTR_LENGTH, "length",
            id);
        int nolanes = getIntReporting(attrs, SUMO_ATTR_NOLANES,
            "number of lanes", id);
        int priority = getIntReporting(attrs, SUMO_ATTR_PRIORITY,
            "priority", id);
        if(speed>0&&length>0&&nolanes>0&&priority>0) {
            NBEdgeCont::insert(new NBEdge(id, name, from, to, type, speed,
                nolanes, length, priority));
        }
    } catch (EmptyData) {
        addError("sumo-file", "An edge with an unknown id occured.");
    }
}

NBNode *
NBSUMOHandlerEdges::getNode(const Attributes &attrs, unsigned int id,
                            const std::string &dir, const std::string &name)
{
    try {
        string nodename = getString(attrs, id);
        NBNode *node = NBNodeCont::retrieve(nodename);
        if(node==0) {
            addError("sumo-file",
                string("The ") + dir + string("-node '") + nodename +
                string("' used within edge '") + name + string("' is not known."));
        }
        return node;
    } catch (EmptyData) {
        addError("sumo-file",
            string("Missing ") + dir + string("-node name for edge with id '")
            + name + string("'"));
    }
    return 0;
}

float
NBSUMOHandlerEdges::getFloatReporting(const Attributes &attrs,
                                      AttrEnum id, const std::string &name,
                                      const std::string &objid)
{
    try {
        return getFloat(attrs, id);
    } catch (EmptyData) {
        addError("sumo-file",
            string("The ") + name + string(" is not given within the object '")
            + objid + string("'."));
    } catch (NumberFormatException) {
        addError("sumo-file",
            string("The ") + name + string(" is not numeric within the object '")
            + objid + string("'."));
    }
    return -1;
}


int
NBSUMOHandlerEdges::getIntReporting(const Attributes &attrs,
                                    AttrEnum id, const std::string &name,
                                    const std::string &objid)
{
    try {
        return getInt(attrs, id);
    } catch (EmptyData) {
        addError("sumo-file",
            string("The ") + name + string(" is not given within the object '")
            + objid + string("'."));
    } catch (NumberFormatException) {
        addError("sumo-file",
            string("The ") + name + string(" is not numeric within the object '")
            + objid + string("'."));
    }
    return -1;
}

void NBSUMOHandlerEdges::myCharacters(int element, const std::string &name,
                                      const std::string &chars)
{
    myCharactersDump(element, name, chars);
}


void NBSUMOHandlerEdges::myEndElement(int element, const std::string &name)
{
    myEndElementDump(element, name);
}



