#include <string>
#include <iostream>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include <netload/NLHandlerBuilder.h>
#include <netload/NLContainer.h>
#include <utils/common/SErrorHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "GUIContainer.h"
#include "GUIHandlerBuilder.h"

using namespace std;

GUIHandlerBuilder::GUIHandlerBuilder(NLContainer &container)
    : NLHandlerBuilder(container)
{
}


GUIHandlerBuilder::~GUIHandlerBuilder()
{
}



void
GUIHandlerBuilder::myStartElement(int element, const std::string &name,
                                  const Attributes &attrs)
{
    NLHandlerBuilder::myStartElement(element, name, attrs);
    if(wanted(LOADFILTER_NET) && element==SUMO_TAG_EDGEPOS) {
        addSourceDestinationInformation(attrs);
    }
}


void
GUIHandlerBuilder::addSourceDestinationInformation(const Attributes &attrs) {
    try {
        string id = getString(attrs, SUMO_ATTR_ID);
        string from = getString(attrs, SUMO_ATTR_FROM);
        string to = getString(attrs, SUMO_ATTR_TO);
        string func= getString(attrs, SUMO_ATTR_FUNC);
        static_cast<GUIContainer&>(myContainer).addSrcDestInfo(id, from, 
            to, func);
    } catch (EmptyData) {
        SErrorHandler::add(
            "Error in description: An edge has no information about the from/to-node");
    }
}


