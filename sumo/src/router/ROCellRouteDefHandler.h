#ifndef ROCellRouteDefHandler_h
#define ROCellRouteDefHandler_h

#include <string>
#include <vector>
#include "ROTypedRoutesLoader.h"
#include "CellDriverInfoParser.h"
#include "ROEdgeVector.h"
#include <utils/importio/LineHandler.h>
#include <utils/importio/LineReader.h>
#include <utils/router/IDSupplier.h>

class RONet;
class Options;
class RORoute;

class ROCellRouteDefHandler : public LineHandler, public ROTypedRoutesLoader {
private:
    IDSupplier _routeIdSupplier;
    IDSupplier _vehicleIdSupplier;
    LineReader _lineReader;
    std::string     _driverFile;
    std::ifstream    _driverStrm;
    std::string     _routeDefFile;
    std::string     _routeIdxFile;
    int _lastRoute;
    std::vector<unsigned long> _routes;
    CellDriverInfoParser _driverParser;
    bool _hasIndexFile;
    bool _isIntel;
    double _gawronBeta;
    double _gawronA;
public:
    ROCellRouteDefHandler(RONet &net, double gawronBeta, double gawronA, 
        std::string file="");
    ~ROCellRouteDefHandler();
    /* ----- from the LineHandler - "interface" ----- */
    bool report(const std::string &result);
    /* ----- from the "RORoutesHandler - "interface" ----- */
    ROTypedRoutesLoader *getAssignedDuplicate(const std::string &file) const;
    void closeReading();
    std::string getDataName() const;
    bool init(OptionsCont *_options);
    bool checkFile(const std::string &file) const;
protected:
    bool startReadingSteps();
    bool readNextRoute(long start);
private:
    //bool assureRouteExistance(int routeNo);
    bool initDriverFile();
    RORoute *getAlternative(size_t pos);

    ROEdgeVector *getRouteInfoFrom(unsigned long routeNo);
};

#endif

