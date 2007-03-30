/****************************************************************************/
/// @file    RORDLoader_Cell.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A handler for reading FastLane-files
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

#include <string>
#include <iostream> // !!! debug only
#include <fstream>
#include <sstream>
#include <utils/importio/LineHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/router/IDSupplier.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/common/UtilExceptions.h>
#include "RORoute.h"
#include "RORouteDef_Alternatives.h"
#include "ROEdgeVector.h"
#include "RONet.h"
#include "RORDLoader_Cell.h"
#include "ROVehicleBuilder.h"

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
RORDLoader_Cell::RORDLoader_Cell(ROVehicleBuilder &vb, RONet &net,
                                 SUMOTime begin, SUMOTime end,
                                 SUMOReal gawronBeta, SUMOReal gawronA,
                                 int maxRoutes,
                                 string file)
        : ROAbstractRouteDefLoader(vb, net, begin, end),
        _routeIdSupplier("Cell_"+file, 0),
        _vehicleIdSupplier("Cell_"+file, 0),
        _driverParser(true, true),
        _gawronBeta(gawronBeta), _gawronA(gawronA), myHaveEnded(false),
        myMaxRoutes(maxRoutes)
{
    if (file.length()!=0) {
        // initialise the .rinfo-reader
        _routeDefFile = file + ".rinfo";
        // pre-initialise the .driver-reader
        _driverFile = file + ".driver";
        _driverStrm.open(_driverFile.c_str(), fstream::in|fstream::binary);
        // compute the name of the index file
        _routeIdxFile = file + ".rindex";
    }
}


RORDLoader_Cell::~RORDLoader_Cell()
{}


void
RORDLoader_Cell::closeReading()
{}


std::string
RORDLoader_Cell::getDataName() const
{
    return "cell routes";
}


bool
RORDLoader_Cell::myReadRoutesAtLeastUntil(SUMOTime time)
{
    if (ended()) {
        return true;
    }
    do {
        if (_driverStrm.eof()) {
            myHaveEnded = true;
            return true;
        }
        // parse the route information
        _driverParser.parseFrom(_driverStrm);
        // return when the route shall be skipped
        myCurrentTime = _driverParser.getRouteStart();
        // check whether the route is not the end
        if (_driverParser.getRouteStart()==INT_MAX) {
            return true;
        }
        if (myCurrentTime<myBegin||myCurrentTime>=myEnd) {
            return true;
        }
        // get the route-number
        int routeNo = _driverParser.getRouteNo();
        if (routeNo<0) {
            WRITE_WARNING("Skipping Route: " + toString<int>(routeNo));
            return true; // !!!
        }
        // add the route when it is not yet known
        RORouteDef_Alternatives *altDef =
            new RORouteDef_Alternatives(_routeIdSupplier.getNext(),
                                        RGBColor(-1, -1, -1), _driverParser.getLast(),
                                        _gawronBeta, _gawronA, myMaxRoutes);
        for (size_t i=0; i<3; i++) {
            RORoute *alt = getAlternative(i);
            if (alt!=0) {
                altDef->addLoadedAlternative(alt);
            }
        }
        _net.addRouteDef(altDef);
        // add the vehicle type, the vehicle and the route to the net
        string id = _vehicleIdSupplier.getNext();
        _net.addVehicle(id, myVehicleBuilder.buildVehicle(
                            id, altDef, _driverParser.getRouteStart(),
                            0, RGBColor(), -1, 0));
    } while (!ended()&&(int) time<_driverParser.getRouteStart());
    return true;
}


RORoute *
RORDLoader_Cell::getAlternative(size_t pos)
{
    SUMOReal cost = _driverParser.getAlternativeCost(pos);
    SUMOReal prop = _driverParser.getAlternativeProbability(pos);
    int routeNo = _driverParser.getRouteNo(pos);
    // check whether the route was already read
    //  read the route if not
    ROEdgeVector *route = 0;
    assert(routeNo>=0);
    if (_routes.size()>(size_t) routeNo) {
        // get the route from the file
        route = getRouteInfoFrom(_routes[routeNo]);
    }
    if (_routes.size()<=(size_t) routeNo||route==0) {
        stringstream buf;
        buf << "The file '" << _driverFile
        << "'" << endl
        << " references the route #" << routeNo
        << " which is not available in '" << _routeDefFile
        << "'.";
        MsgHandler::getErrorInstance()->inform(buf.str());
        return 0;
    }
    RORoute *ret = new RORoute(_routeIdSupplier.getNext(), cost,
                               prop, *route);
    delete route;
    return ret;
}


ROEdgeVector *
RORDLoader_Cell::getRouteInfoFrom(unsigned long position)
{
    ifstream strm(_routeDefFile.c_str());
    string result;
    strm.seekg(position, ios::beg);
    getline(strm, result);
    // split items
    StringTokenizer st(result);
    // check whether the route is not empty
    if (st.size()==0) {
        return 0;
    }
    // build the route representation
    ROEdgeVector *edges = new ROEdgeVector();
    while (st.hasNext()) {
        string id = st.next();
        ROEdge *edge = _net.getEdge(id);
        if (edge==0) {
            MsgHandler::getErrorInstance()->inform("A route read from '" + _routeDefFile + "' contains an unknown edge ('" + id + "').");
            delete edges;
            return 0;
        }
        edges->add(edge);
    }
    // add route definition
    return edges;
}


bool
RORDLoader_Cell::init(OptionsCont &options)
{
    // check whether non-intel-format shall be used
    _isIntel = options.getBool("intel-cell");
    _driverParser.isIntel(options.getBool("intel-cell"));
    _driverParser.useLast(!options.getBool("no-last-cell"));
    // read int the positions of routes within the rinfo-file
    if (FileHelpers::exists(_routeIdxFile)) {
        _hasIndexFile = true;
        _lineReader.setFileName(_routeIdxFile);
    } else {
        _hasIndexFile = false;
        _routes.push_back(0);
        _lineReader.setFileName(_routeDefFile);
    }
    _lineReader.readAll(*this);
    // save the index file when wished
    if (!_hasIndexFile&&options.getBool("save-cell-rindex")) {
        MsgHandler::getMessageInstance()->beginProcessMsg("Saving the cell-rindex file '" + _routeIdxFile + "'...");
        std::ofstream out(_routeIdxFile.c_str());
        for (std::vector<unsigned long>::iterator i=_routes.begin(); i!=_routes.end(); i++) {
            out << (*i) << endl;
        }
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
    // prepare the .driver file for reading
    return initDriverFile();
}


bool
RORDLoader_Cell::report(const std::string &result)
{
    if (_hasIndexFile) {
        try {
            _routes.push_back(TplConvert<char>::_2int(result.c_str())); // !!!
        } catch (NumberFormatException) {
            MsgHandler::getErrorInstance()->inform("Your '" + _routeIdxFile + "' contains non-digits.");
            throw ProcessError();
        } catch (EmptyData) {}
    }
    else {
        _routes.push_back(_lineReader.getPosition());
    }
    return true;
}


bool
RORDLoader_Cell::initDriverFile()
{
    if (!_driverStrm.good()) {
        MsgHandler::getErrorInstance()->inform("Problems on opening '" + _driverFile + "'.");
        return false;
    }
    // check for header
    int mark = FileHelpers::readInt(_driverStrm, _isIntel);
    int offset = FileHelpers::readInt(_driverStrm, _isIntel);
    /*int makeRouteTime = */
    FileHelpers::readInt(_driverStrm, _isIntel);
    if (mark==INT_MAX) {
        // skip header
        _driverStrm.seekg(offset, ios::beg);
    }
    //
    return _driverStrm.good() && !_driverStrm.eof();
}


SUMOTime
RORDLoader_Cell::getCurrentTimeStep() const
{
    return _driverParser.getRouteStart();
}


bool
RORDLoader_Cell::ended() const
{
    return myHaveEnded;
}



/****************************************************************************/

