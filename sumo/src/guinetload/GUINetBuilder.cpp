#include <vector>
#include <parsers/SAXParser.hpp>
#include <sax2/SAX2XMLReader.hpp>
#include <sax2/XMLReaderFactory.hpp>
#include <sax2/DefaultHandler.hpp>
#include <utils/common/SErrorHandler.h>
#include <utils/options/OptionsCont.h>
#include <guisim/GUINet.h>
#include <netload/NLHandlerBuilder.h>
#include <netload/NLLoadFilter.h>
#include "GUIHandlerBuilder.h"
#include "GUIEdgeControlBuilder.h"
#include "GUIContainer.h"
#include "GUINetBuilder.h"

GUINetBuilder::GUINetBuilder(const OptionsCont &oc)
    : NLNetBuilder(oc)
{
    GUINet::preInitGUINet(oc.getLong("b"));
}


GUINetBuilder::~GUINetBuilder()
{
}


GUINet *
GUINetBuilder::buildGUINet()
{
    GUIContainer *container = new GUIContainer(new GUIEdgeControlBuilder());
    SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
    parser->setFeature(
        XMLString::transcode("http://xml.org/sax/features/validation"),
        false);
    GUINet *net = 0;
    // get the matching handler
    NLHandlerBuilder *handler = new GUIHandlerBuilder(*container);
    bool ok = load(handler, *parser);
    subreport("Loading done.", "Loading failed.");
    if(!SErrorHandler::errorOccured()) {
        net = container->buildGUINet(
            m_pOptions.getUIntVector("dump-intervals"),
            m_pOptions.getString("dump-basename"));
    }
    delete parser;
    if(ok)
        report(*container);
    delete handler;
    delete container;
    return net;
}


