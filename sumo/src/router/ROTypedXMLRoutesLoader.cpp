#include <string>
#include <parsers/SAXParser.hpp>
#include <util/PlatformUtils.hpp>
#include <util/TransService.hpp>
#include <sax2/SAX2XMLReader.hpp>
#include <sax2/XMLReaderFactory.hpp>
#include <sax2/DefaultHandler.hpp>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SErrorHandler.h>
#include "ROTypedRoutesLoader.h"
#include "ROTypedXMLRoutesLoader.h"
#include "RONet.h"

using namespace std;

ROTypedXMLRoutesLoader::ROTypedXMLRoutesLoader(RONet &net)
    : ROTypedRoutesLoader(net), SUMOSAXHandler(true, true), 
    _parser(0), _token()
{
}

ROTypedXMLRoutesLoader::ROTypedXMLRoutesLoader(RONet &net,
                                               const std::string &file)
    : ROTypedRoutesLoader(net, file), SUMOSAXHandler(true, true), 
    _parser(XMLReaderFactory::createXMLReader()), _token(), _file(file)
{
    _parser->setFeature(
        XMLString::transcode("http://xml.org/sax/features/validation"),
        false);
    _parser->setContentHandler(this);
    _parser->setErrorHandler(this);
}

ROTypedXMLRoutesLoader::~ROTypedXMLRoutesLoader()
{
    delete _parser;
}

void
ROTypedXMLRoutesLoader::closeReading()
{
    _parser->parseReset(_token);
}

bool
ROTypedXMLRoutesLoader::readNextRoute(long start)
{
    _netRouteRead = false;
    while(!_netRouteRead&&!_ended) {
        _parser->parseNext(_token);
    }
    return true;
}

bool
ROTypedXMLRoutesLoader::addAllRoutes()
{
    _parser->parse(_file.c_str());
    return !SErrorHandler::errorOccured();
}

bool
ROTypedXMLRoutesLoader::startReadingSteps()
{
/*    _parser->setContentHandler(this);
    _parser->setErrorHandler(this);*/
    return true;
}

bool
ROTypedXMLRoutesLoader::init(OptionsCont *options)
{
/*    _parser->setContentHandler(this);
    _parser->setErrorHandler(this);*/
    return _parser->parseFirst(_file.c_str(), _token);
}

void 
ROTypedXMLRoutesLoader::endDocument()
{
    _ended = true;
}


