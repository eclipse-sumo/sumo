#ifndef MSTriggeredXMLReader_h
#define MSTriggeredXMLReader_h

#include <string>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include "MSTriggeredReader.h"

class MSNet;

class MSTriggeredXMLReader : public MSTriggeredReader,
                             public SUMOSAXHandler {
private:
    SAX2XMLReader* myParser;
    XMLPScanToken  myToken;
    bool _nextRead;
public:
    virtual ~MSTriggeredXMLReader();
protected:
    MSTriggeredXMLReader(MSNet &net, const std::string &filename);
    void init(MSNet &net);
    bool readNextTriggered();
};

#endif

