#ifndef ROTypedRoutesLoader_h
#define ROTypedRoutesLoader_h

#include <string>

class RONet;
class SAX2XMLReader;
class OptionsCont;

class ROTypedRoutesLoader {
protected:
    RONet &_net;
    long _currentTimeStep;
    bool _ended;
    bool _netRouteRead;
public:
    ROTypedRoutesLoader(RONet &net, const std::string &file="");
    virtual ~ROTypedRoutesLoader();
    virtual ROTypedRoutesLoader *getAssignedDuplicate(const std::string &file) const = 0;
    bool skipPreviousRoutes(long start);
    bool addRoutesUntil(long time);
    virtual bool addAllRoutes();
    virtual void closeReading() = 0;
    virtual std::string getDataName() const = 0;
    virtual bool init(OptionsCont *options) = 0;
    virtual bool ended() const;
    virtual bool checkFile(const std::string &file) const;
    long getCurrentTimeStep() const;
protected:
    virtual bool startReadingSteps() = 0;
    virtual bool readNextRoute(long start) = 0;
};

#endif
