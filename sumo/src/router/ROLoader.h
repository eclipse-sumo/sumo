#ifndef ROLoader_h
#define ROLoader_h

#include <string>
#include "RORouteDefList.h"

class OptionsCont;
class RONet;
class RONetHandler;
class ROTypedRoutesLoader;
class ofstream;
class SAX2XMLReader;
class GenericSAX2Handler;

class ROLoader {
private:
    OptionsCont * const _options;
    typedef std::vector<ROTypedRoutesLoader*> RouteLoaderCont;
    RouteLoaderCont _handler;
public:
    ROLoader(OptionsCont *oc);
    ~ROLoader();
    RONet *loadNet();
    void openRoutes(RONet &net);
    void processRoutesStepWise(long start, long end, 
        std::ofstream &res, std::ofstream &altres, RONet &net);
    void processAllRoutes(long start, long end, 
        std::ofstream &res, std::ofstream &altres, RONet &net);
    bool loadWeights(RONet &net);
    void closeReading();
/*    bool loadSumoRoutes(RONet &net);
    bool loadCellRoutes(RONet &net);
    bool loadXMLRouteDefs(RONet &net);*/

private:
//    bool loadNetInto(RONet &net);
    bool loadNet(SAX2XMLReader *reader, RONetHandler &handler,
        const std::string &files);
    void openTypedRoutes(ROTypedRoutesLoader *handler,
        const std::string &optionName);
    void addToHandlerList(ROTypedRoutesLoader *handler,
        const std::string &fileList);
    void skipPreviousRoutes(long start);
    SAX2XMLReader *getSAXReader(GenericSAX2Handler &handler);
    long getMinTimeStep() const;
private:
    /// we made the copy constructor invalid
    ROLoader(const ROLoader &src);
    /// we made the assignment operator invalid
    ROLoader &operator=(const ROLoader &src);
};

#endif
