#include <string>
#include "ROTypedRoutesLoader.h"
#include <utils/common/SErrorHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include "RONet.h"

using namespace std;

ROTypedRoutesLoader::ROTypedRoutesLoader(RONet &net, const std::string &file)
    : _net(net), _currentTimeStep(-1), _ended(false),
    _netRouteRead(false)
{
}

ROTypedRoutesLoader::~ROTypedRoutesLoader()
{
}

bool
ROTypedRoutesLoader::skipPreviousRoutes(long start)
{
    bool ok = startReadingSteps();
    cout << "Skipping" << endl;
    while(!_ended&&ok&&_currentTimeStep<start) {
        ok = readNextRoute(start);
        cout << "Skipping.";
    }
    cout << "Skipped until: " << _currentTimeStep << endl;
    if(!ok) {
        SErrorHandler::add(
            string("Problems arised on parsing the ") + getDataName() +
            string(" file."), true);
        throw ProcessError();
    }
    return ok;
}

bool
ROTypedRoutesLoader::addRoutesUntil(long time)
{
    bool ok = startReadingSteps();
    while(ok&&_currentTimeStep<time&&!_ended) {
        ok = readNextRoute(_currentTimeStep);
    }
    if(!ok) {
        SErrorHandler::add(
            string("Problems arised on parsing the ") + getDataName() +
            string(" file '"), true);
        throw ProcessError();
    }
    return ok;
}

bool
ROTypedRoutesLoader::addAllRoutes()
{
    bool ok = startReadingSteps();
    while(ok&&!_ended) {
        ok = readNextRoute(_currentTimeStep);
    }
    return ok;
}

bool
ROTypedRoutesLoader::ended() const
{
    return _ended;
}

bool
ROTypedRoutesLoader::checkFile(const std::string &file) const
{
    return FileHelpers::exists(file);
}

long
ROTypedRoutesLoader::getCurrentTimeStep() const
{
    return _currentTimeStep;
}


