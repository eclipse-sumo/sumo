#include <string>
#include <utility>
#include <iostream>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SErrorHandler.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "ODDistrict.h"
#include "ODDistrictCont.h"
#include "ODDistrictHandler.h"

using namespace std;

ODDistrictHandler::ODDistrictHandler(ODDistrictCont &cont,
                                     bool warn, bool verbose)
    : SUMOSAXHandler(warn, verbose), _cont(cont), _current(0)
{
}


ODDistrictHandler::~ODDistrictHandler()
{
}


void
ODDistrictHandler::myStartElement(int element, const std::string &name,
                                  const Attributes &attrs)
{
    switch(element) {
    case SUMO_TAG_DISTRICT:
        openDistrict(attrs);
        break;
    case SUMO_TAG_DSOURCE:
        addSource(attrs);
        break;
    case SUMO_TAG_DSINK:
        addSink(attrs);
        break;
    }
}


void
ODDistrictHandler::myEndElement(int element, const std::string &name)
{
    if(element==SUMO_TAG_DISTRICT) {
        closeDistrict();
    }
}


void
ODDistrictHandler::myCharacters(int element, const std::string &name,
                                const std::string &chars)
{
}


void
ODDistrictHandler::openDistrict(const Attributes &attrs)
{
    _current = 0;
    try {
        _current = new ODDistrict(getString(attrs, SUMO_ATTR_ID));
    } catch (EmptyData) {
        SErrorHandler::add("A district without an id occured.");
    }
}


void
ODDistrictHandler::addSource(const Attributes &attrs)
{
    std::pair<std::string, double> vals = getValues(attrs, "source");
    if(vals.second>=0) {
        _current->addSource(vals.first, vals.second);
    }
}


void
ODDistrictHandler::addSink(const Attributes &attrs)
{
    std::pair<std::string, double> vals = getValues(attrs, "sink");
    if(vals.second>=0) {
        _current->addSink(vals.first, vals.second);
    }
}



std::pair<std::string, double>
ODDistrictHandler::getValues(const Attributes &attrs, const std::string &type)
{
    // check the current district first
    if(_current==0) {
        return std::pair<std::string, double>("", -1);
    }
    // get the id first
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        SErrorHandler::add(
            string("A ") + type
            + string(" without an id occured within district '")
            + _current->getID() + string("'."));
        return std::pair<std::string, double>("", -1);
    }
    // get the weight
    double weight = getFloatSecure(attrs, SUMO_ATTR_WEIGHT, -1);
    if(weight==-1) {
        SErrorHandler::add(
            string("The weight of the ") + type + string(" '") + id
            + string("' within district '")
            + _current->getID()
            + string("' is not numeric."));
        return std::pair<std::string, double>("", -1);
    }
    // return the values
    return std::pair<std::string, double>(id, weight);
}


void
ODDistrictHandler::closeDistrict()
{
    _cont.add(_current->getID(), _current);
}


