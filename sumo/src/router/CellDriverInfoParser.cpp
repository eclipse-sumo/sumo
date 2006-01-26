//---------------------------------------------------------------------------//
//                        CellDriverInfoParser.cpp -
//  A parser for single driver informations during FastLane-import
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.10  2006/01/26 08:37:23  dkrajzew
// removed warnings 4786
//
// Revision 1.9  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.8  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2004/12/16 12:26:52  dkrajzew
// debugging
//
// Revision 1.5  2004/01/26 08:01:10  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// Revision 1.4  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.3  2003/02/07 10:45:03  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <fstream>
#include <climits>
#include <sstream>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include "RORouteDef.h"
#include "CellDriverInfoParser.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
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
            MsgHandler::getErrorInstance()->inform(buf.str());
            MsgHandler::getErrorInstance()->inform(
                " Retry with a combination of '--intel-cell' and '--no-last-cell'");
            throw ProcessError();
        }
        _routeNo = _driver.route[_driver.lastroute];
    } else {
        // get the shortest route
        SUMOReal min = SUMOReal(1E+37); // !!! some kind of a SUMOReal-max
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
        MsgHandler::getErrorInstance()->inform(buf.str());
        MsgHandler::getErrorInstance()->inform(" Retry with '--intel-cell'.");
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


