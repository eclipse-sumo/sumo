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
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
        : myUseLast(useLast), myIntel(intel)
{}


CellDriverInfoParser::~CellDriverInfoParser()
{}


void
CellDriverInfoParser::parseFrom(std::ifstream &strm)
{
    myDriver.start = FileHelpers::readInt(strm, myIntel);
    myDriver.age = FileHelpers::readInt(strm, myIntel);

    myDriver.route[0] = FileHelpers::readInt(strm, myIntel);
    myDriver.route[1] = FileHelpers::readInt(strm, myIntel);
    myDriver.route[2] = FileHelpers::readInt(strm, myIntel);

    myDriver.p[0] = FileHelpers::readFloat(strm, myIntel);
    myDriver.p[1] = FileHelpers::readFloat(strm, myIntel);
    myDriver.p[2] = FileHelpers::readFloat(strm, myIntel);

    myDriver.cost[0] = FileHelpers::readFloat(strm, myIntel);
    myDriver.cost[1] = FileHelpers::readFloat(strm, myIntel);
    myDriver.cost[2] = FileHelpers::readFloat(strm, myIntel);

    myDriver.lastcost = FileHelpers::readInt(strm, myIntel);
    myDriver.lastroute = FileHelpers::readInt(strm, myIntel);

    computeRouteNo();
}


int
CellDriverInfoParser::computeRouteNo()
{
    if (myUseLast) {
        if (myDriver.lastroute<0||myDriver.lastroute>3) {
            throw ProcessError("An invalid route index occured in a .driver file (" + toString(myDriver.lastroute) + " at " + toString(myRouteNo) + "\n Retry with a combination of '--intel-cell' and '--no-last-cell'.");
        }
        myRouteNo = myDriver.route[myDriver.lastroute];
    } else {
        // get the shortest route
        SUMOReal min = SUMOReal(1E+37); // !!! some kind of a SUMOReal-max
        for (int i=0; i<3; i++) {
            if (myDriver.p[i] <= 1.0 &&
                    myDriver.p[i] >=0 &&
                    myDriver.cost[i]<min &&
                    myDriver.route[i]>0) {
                myRouteNo = myDriver.route[i];
                min = myDriver.cost[i];
            }
        }
    }
    if (myRouteNo<0) {
        throw ProcessError("A negative route index occured in a .driver file (" + toString(myRouteNo) + "\n Retry with '--intel-cell'.");
    }
    return myRouteNo;
}


int
CellDriverInfoParser::getRouteNo() const
{
    return myRouteNo;
}


int
CellDriverInfoParser::getRouteStart() const
{
    return myDriver.start;
}


void
CellDriverInfoParser::isIntel(bool value)
{
    myIntel = value;
}


void
CellDriverInfoParser::useLast(bool value)
{
    myUseLast = value;
}


int
CellDriverInfoParser::getLast() const
{
    return myDriver.lastroute;
}


SUMOReal
CellDriverInfoParser::getAlternativeCost(size_t pos) const
{
    return myDriver.cost[pos];
}


SUMOReal
CellDriverInfoParser::getAlternativeProbability(size_t pos) const
{
    return myDriver.p[pos];
}


int
CellDriverInfoParser::getRouteNo(size_t pos) const
{
    return myDriver.route[pos];
}



/****************************************************************************/

