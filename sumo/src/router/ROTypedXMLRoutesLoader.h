#ifndef ROTypedXMLRoutesLoader_h
#define ROTypedXMLRoutesLoader_h

#include <string>
#include <sax2/SAX2XMLReader.hpp>
#include <framework/XMLPScanToken.hpp>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include "ROTypedRoutesLoader.h"

class RONet;
class Options;

class ROTypedXMLRoutesLoader : public ROTypedRoutesLoader, 
                               public SUMOSAXHandler {
protected:
    SAX2XMLReader *_parser;
    XMLPScanToken _token;
    std::string _file;
public:
    ROTypedXMLRoutesLoader(RONet &net);
    ROTypedXMLRoutesLoader(RONet &net, const std::string &file);
    virtual ~ROTypedXMLRoutesLoader();
    void closeReading();
    virtual bool addAllRoutes();
    bool init(OptionsCont *options);
    void endDocument();
protected:
    bool startReadingSteps();
    bool readNextRoute(long start);
};

#endif
