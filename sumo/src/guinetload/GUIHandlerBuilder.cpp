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
#include "GUIContainer.h"
#include "GUIHandlerBuilder.h"

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
    if(wanted(LOADFILTER_NET) && element==NLTag_edgepos) {
        addSourceDestinationInformation(attrs);
    }
}


void
GUIHandlerBuilder::addSourceDestinationInformation(const Attributes &attrs) {
    try {
        string id = _attrHandler.getString(attrs, ATTR_ID);
        string from = _attrHandler.getString(attrs, ATTR_FROM);
        string to = _attrHandler.getString(attrs, ATTR_TO);
        string func= _attrHandler.getString(attrs, ATTR_FUNC);
        static_cast<GUIContainer&>(myContainer).addSrcDestInfo(id, from, 
            to, func);
    } catch (EmptyData &e) {
        SErrorHandler::add(
            "Error in description: An edge has no information about the from/to-node");
    }
}


