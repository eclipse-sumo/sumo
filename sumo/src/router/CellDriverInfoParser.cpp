/****************************************************************************/
/// @file    CellDriverInfoParser.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A parser for single driver informations during FastLane-import
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fstream>
#include <climits>
#include <sstream>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "RORouteDef.h"
#include "CellDriverInfoParser.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
CellDriverInfoParser::CellDriverInfoParser(bool useLast, bool intel)
        : _useLast(useLast), _intel(intel)
{}


CellDriverInfoParser::~CellDriverInfoParser()
{}


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
    if (_useLast) {
        if (_driver.lastroute<0||_driver.lastroute>3) {
            throw ProcessError("An invalid route index occured in a .driver file (" + toString(_driver.lastroute) + " at " + toString(_routeNo) + "\n Retry with a combination of '--intel-cell' and '--no-last-cell'.");
        }
        _routeNo = _driver.route[_driver.lastroute];
    } else {
        // get the shortest route
        SUMOReal min = SUMOReal(1E+37); // !!! some kind of a SUMOReal-max
        for (int i=0; i<3; i++) {
            if (_driver.p[i] <= 1.0 &&
                    _driver.p[i] >=0 &&
                    _driver.cost[i]<min &&
                    _driver.route[i]>0) {
                _routeNo = _driver.route[i];
                min = _driver.cost[i];
            }
        }
    }
    if (_routeNo<0) {
        throw ProcessError("A negative route index occured in a .driver file (" + toString(_routeNo) + "\n Retry with '--intel-cell'.");
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


SUMOReal
CellDriverInfoParser::getAlternativeCost(size_t pos) const
{
    return _driver.cost[pos];
}


SUMOReal
CellDriverInfoParser::getAlternativeProbability(size_t pos) const
{
    return _driver.p[pos];
}


int
CellDriverInfoParser::getRouteNo(size_t pos) const
{
    return _driver.route[pos];
}



/****************************************************************************/

