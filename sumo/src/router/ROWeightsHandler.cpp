#include <string>
#include <utils/options/OptionsCont.h>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/xml/AttributesHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SErrorHandler.h>
#include "ROEdge.h"
#include "RONet.h"
#include "ROWeightsHandler.h"


using namespace std;

GenericSAX2Handler::Tag ROWeightsHandler::_tags[3] =
{
/* 01 */  { "interval", RO_Tag_interval },
/* 02 */  { "edge", RO_Tag_edge },
/* 03 */  { "lane", RO_Tag_lane }
};


ROWeightsHandler::ROWeightsHandler(OptionsCont &oc, RONet &net,
                                   const std::string &file)
    : GenericSAX2Handler(_tags, 3), _options(oc), _net(net), _file(file),
    _currentTimeBeg(-1), _currentTimeEnd(-1), _currentEdge(0)
{
    _scheme = _options.getString("scheme");
    _attrHandler.add(RO_ATTR_value, "traveltime");
    _attrHandler.add(RO_ATTR_beg, "begin");
    _attrHandler.add(RO_ATTR_end, "end");
    _attrHandler.add(RO_ATTR_id, "id");
}


ROWeightsHandler::~ROWeightsHandler()
{
}


void ROWeightsHandler::myStartElement(int element, const std::string &name,
                                      const Attributes &attrs)
{
    switch(element) {
    case RO_Tag_interval:
        parseTimeStep(attrs);
        break;
    case RO_Tag_edge:
        parseEdge(attrs);
        break;
    case RO_Tag_lane:
        parseLane(attrs);
        break;
    default:
        break;
    }
}


void
ROWeightsHandler::parseTimeStep(const Attributes &attrs) {
    try {
        _currentTimeBeg = _attrHandler.getLong(attrs, RO_ATTR_beg);
        _currentTimeEnd = _attrHandler.getLong(attrs, RO_ATTR_end);
    } catch (...) {
        SErrorHandler::add("Problems with timestep value.");
    }
}


void
ROWeightsHandler::parseEdge(const Attributes &attrs) {
    _currentEdge = 0;
    try {
        string id = _attrHandler.getString(attrs, RO_ATTR_id);
        _currentEdge = _net.getEdge(id);
    } catch (EmptyData) {
        SErrorHandler::add("An edge without an id occured.");
        SErrorHandler::add(" Contact your weight data supplier.");
    }
}


void
ROWeightsHandler::parseLane(const Attributes &attrs) {
    string id;
    float value = -1;
    // try to get the lane id
    try {
        id = _attrHandler.getString(attrs, RO_ATTR_id);
    } catch (EmptyData) {
        SErrorHandler::add("A lane without an id occured.");
        SErrorHandler::add(" Contact your weight data supplier.");
    }
    // try to get the lane value - depending on the used scheme
    try {
        value = _attrHandler.getFloat(attrs, RO_ATTR_value);
    } catch (EmptyData) {
        SErrorHandler::add(string("Missing value '") + _scheme + string("' in lane."));
        SErrorHandler::add("Contact your weight data supplier.");
    } catch (NumberFormatException) {
        SErrorHandler::add(string("The value should be numeric, but is not ('") +
            _attrHandler.getString(attrs, RO_ATTR_value) +
            string("'"));
        if(id.length()!=0)
            SErrorHandler::add(string(" In lane '") + id + string("'"));
    }
    // set the values when retrieved (no errors)
    if(id.length()!=0&&value>0&&_currentEdge!=0) {
        _currentEdge->setLane(_currentTimeBeg, _currentTimeEnd, 
            id, value);
    }
}


void ROWeightsHandler::myCharacters(int element, const std::string &name,
                                    const std::string &chars)
{
}


void ROWeightsHandler::myEndElement(int element, const std::string &name)
{
    if(element==RO_Tag_edge) {
        _currentEdge = 0;
    }
}

