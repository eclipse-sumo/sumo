#include <string>
#include <iostream>
#include <utils/xml/AttributesReadingGenericSAX2Handler.h>
#include <utils/convert/TplConvert.h>
#include <utils/common/FileErrorReporter.h>
#include "SUMOXMLDefinitions.h"
#include "SUMOSAXHandler.h"

using namespace std;

SUMOSAXHandler::SUMOSAXHandler(bool warn, bool verbose)
    : AttributesReadingGenericSAX2Handler(sumotags, noSumoTags,
                                          sumoattrs, noSumoAttrs),
    _warn(warn), _verbose(verbose)
{
}


SUMOSAXHandler::~SUMOSAXHandler()
{
}

void
SUMOSAXHandler::warning(const SAXParseException& exception)
{
    cout << "Warning: "
        << TplConvert<XMLCh>::_2str(exception.getMessage()) << endl;
    cout << " (At line/column " << exception.getLineNumber()+1
        << '/' << exception.getColumnNumber() << ")." << endl;
    _errorOccured = true;
}

void
SUMOSAXHandler::error(const SAXParseException& exception)
{
    cout << "Error: "
        << TplConvert<XMLCh>::_2str(exception.getMessage()) << endl;
    cout << " (At line/column " << exception.getLineNumber()+1
        << '/' << exception.getColumnNumber() << ")." << endl;
    _errorOccured = true;
}

void
SUMOSAXHandler::fatalError(const SAXParseException& exception)
{
    cout << "Error: "
        << TplConvert<XMLCh>::_2str(exception.getMessage()) << endl;
    cout << " (At line/column " << exception.getLineNumber()+1
        << '/' << exception.getColumnNumber() << ")." << endl;
    _errorOccured = true;
}



