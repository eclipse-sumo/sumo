#include <string>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <utils/common/SErrorHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include "MSTriggeredReader.h"
#include "MSTriggeredXMLReader.h"

using namespace std;

MSTriggeredXMLReader::MSTriggeredXMLReader(MSNet &net, 
                                           const std::string &filename)
    : MSTriggeredReader(net, filename), SUMOSAXHandler(true, true) // !!! (options)
{
}


MSTriggeredXMLReader::~MSTriggeredXMLReader()
{
}


void 
MSTriggeredXMLReader::init(MSNet &net)
{
    myParser = XMLReaderFactory::createXMLReader();
    myParser->setFeature(
        XMLString::transcode(
            "http://xml.org/sax/features/namespaces" ), false );
    myParser->setFeature(
        XMLString::transcode(
            "http://apache.org/xml/features/validation/schema" ), false );
    myParser->setFeature(
        XMLString::transcode(
            "http://apache.org/xml/features/validation/schema-full-checking"),
        false );
    myParser->setFeature(
        XMLString::transcode(
            "http://xml.org/sax/features/validation"), false );
    myParser->setFeature(
        XMLString::transcode(
            "http://apache.org/xml/features/validation/dynamic" ), false );
    if(!myParser->parseFirst(_filename.c_str(), myToken)) {
        SErrorHandler::add(
            string("Can not read XML-file '") + _filename + string("'."));
        throw ProcessError();
    }
    while(!readNextTriggered());
}


bool 
MSTriggeredXMLReader::readNextTriggered()
{
    _nextRead = false;
    while(myParser->parseNext(myToken)) {
        if(_nextRead) {
            return true;
        }
    }
    return false;
}

