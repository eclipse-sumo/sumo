#include <fstream>
#include <climits>
#include <sstream>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SErrorHandler.h>
#include "RORouteDef.h"
#include "CellDriverInfoParser.h"

using namespace std;

CellDriverInfoParser::CellDriverInfoParser(bool useLast, bool intel)
    : _useLast(useLast), _intel(intel)
{
}

CellDriverInfoParser::~CellDriverInfoParser()
{
}

void
CellDriverInfoParser::parseFrom(std::ifstream &strm)
{
    _driver.start = FileHelpers::readInt(strm, _intel);
    _driver.age = FileHelpers::readInt(strm, _intel);

    _driver.route[0] = FileHelpers::readInt(strm, _intel);
    _driver.route[1] = FileHelpers::readInt(strm, _intel);
    _driver.route[2] = FileHelpers::readInt(strm, _intel);

    _driver.p[0] = FileHelpers::readFloat(strm, _intel);
    _driver.p[1] = FileHelpers::readFloat(strm, _intel);
    _driver.p[2] = FileHelpers::readFloat(strm, _intel);

    _driver.cost[0] = FileHelpers::readFloat(strm, _intel);
    _driver.cost[1] = FileHelpers::readFloat(strm, _intel);
    _driver.cost[2] = FileHelpers::readFloat(strm, _intel);

    _driver.lastcost = FileHelpers::readInt(strm, _intel);
    _driver.lastroute = FileHelpers::readInt(strm, _intel);

    computeRouteNo();
}


int
CellDriverInfoParser::computeRouteNo()
{
    if(_useLast) {
        if(_driver.lastroute<0||_driver.lastroute>3) {
            stringstream buf;
            buf << "An invalid route index occured in a .driver file ("
                << _driver.lastroute << " at " << _routeNo << ").";
            SErrorHandler::add(buf.str().c_str(), true);
            SErrorHandler::add(" Retry with a combination of '--intel-cell' and '--no-last-cell'", true);
            throw ProcessError();
        }
        _routeNo = _driver.route[_driver.lastroute];
    } else {
        // get the shortest route
        float min = 1E+37; // !!! some kind of a float-max
        for(int i=0; i<3; i++) {
            if (_driver.p[i] <= 1.0 &&
                _driver.p[i] >=0 &&
                _driver.cost[i]<min &&
                _driver.route[i]>0 ) {
                _routeNo = _driver.route[i];
                min = _driver.cost[i];
            }
        }
    }
    if(_routeNo<0) {
        stringstream buf;
        buf << "A negative route index occured in a .driver file ("
            << _routeNo << ").";
        SErrorHandler::add(buf.str().c_str(), true);
        SErrorHandler::add(" Retry with '--intel-cell'.", true);
        throw ProcessError();
    }
    return _routeNo;
}


int
CellDriverInfoParser::getRouteNo() const
{
    return _routeNo;
}


int
CellDriverInfoParser::getRouteStart() const
{
    return _driver.start;
}


void
CellDriverInfoParser::isIntel(bool value)
{
    _intel = value;
}


void
CellDriverInfoParser::useLast(bool value)
{
    _useLast = value;
}


int 
CellDriverInfoParser::getLast() const
{
    return _driver.lastroute;
}


double 
CellDriverInfoParser::getAlternativeCost(size_t pos) const
{
    return _driver.cost[pos];
}


double 
CellDriverInfoParser::getAlternativePropability(size_t pos) const
{
    return _driver.p[pos];
}


int 
CellDriverInfoParser::getRouteNo(size_t pos) const
{
    return _driver.route[pos];
}



