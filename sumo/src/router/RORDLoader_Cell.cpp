//---------------------------------------------------------------------------//
//                        RORDLoader_Cell.cpp -
//  A handler for reading FastLane-files
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
// Revision 1.4  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.3  2004/03/19 13:03:01  dkrajzew
// some style adaptions
//
// Revision 1.2  2004/02/16 13:47:07  dkrajzew
// Type-dependent loader/generator-"API" changed
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// ------------------------------------------------
// Revision 1.8  2003/07/30 09:26:33  dkrajzew
// all vehicles, routes and vehicle types may now have specific colors
//
// Revision 1.7  2003/07/16 15:36:50  dkrajzew
// vehicles and routes may now have colors
//
// Revision 1.6  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
// this handler puts it further to output instances.
// changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.5  2003/03/20 16:39:15  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.4  2003/03/03 15:08:20  dkrajzew
// debugging
//
// Revision 1.3  2003/02/07 10:45:04  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

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
#include <utils/convert/STRConvert.h>
#include <utils/convert/ToString.h>
#include <utils/common/UtilExceptions.h>
#include "RORoute.h"
#include "RORouteDef_Alternatives.h"
#include "ROEdgeVector.h"
#include "RONet.h"
#include "RORDLoader_Cell.h"
#include "ROVehicleBuilder.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
RORDLoader_Cell::RORDLoader_Cell(ROVehicleBuilder &vb, RONet &net,
                                 unsigned int begin, unsigned int end,
                                 double gawronBeta, double gawronA,
                                 string file)
    : ROAbstractRouteDefLoader(vb, net, begin, end),
    _routeIdSupplier(string("Cell_")+file, 0),
    _vehicleIdSupplier(string("Cell_")+file, 0),
    _driverParser(true, true),
    _gawronBeta(gawronBeta), _gawronA(gawronA), myHaveEnded(false)
{
    if(file.length()!=0) {
        // initialise the .rinfo-reader
        _routeDefFile = file + string(".rinfo");
        // pre-initialise the .driver-reader
        _driverFile = file + string(".driver");
        _driverStrm.open(_driverFile.c_str(), fstream::in|fstream::binary);
        // compute the name of the index file
        _routeIdxFile = file + string(".rindex");
    }
}


RORDLoader_Cell::~RORDLoader_Cell()
{
}


void
RORDLoader_Cell::closeReading()
{
}


std::string
RORDLoader_Cell::getDataName() const
{
    return "cell routes";
}


bool
RORDLoader_Cell::myReadRoutesAtLeastUntil(unsigned int time)
{
    if(ended()) {
        return true;
    }
    do {
        if(_driverStrm.eof()) {
            myHaveEnded = true;
            return true;
        }
        // parse the route information
        _driverParser.parseFrom(_driverStrm);
        // return when the route shall be skipped
        myCurrentTime = _driverParser.getRouteStart();
        // check whether the route is not the end
        if(_driverParser.getRouteStart()==INT_MAX) {
            return true;
        }
        if(myCurrentTime<myBegin||myCurrentTime>=myEnd) {
            return true;
        }
        // get the route-number
        int routeNo = _driverParser.getRouteNo();
        if(routeNo<0) {
            MsgHandler::getWarningInstance()->inform(
                string("Skipping Route: ") + toString<int>(routeNo));
            return true; // !!!
        }
        // add the route when it is not yet known
        RORouteDef_Alternatives *altDef =
            new RORouteDef_Alternatives(_routeIdSupplier.getNext(),
                RGBColor(-1, -1, -1), _driverParser.getLast(), _gawronBeta, _gawronA);
        for(size_t i=0; i<3; i++) {
            RORoute *alt = getAlternative(i);
            if(alt!=0) {
                altDef->addLoadedAlternative(alt);
            }
        }
        _net.addRouteDef(altDef);
        // add the vehicle type, the vehicle and the route to the net
        string id = _vehicleIdSupplier.getNext();
        _net.addVehicle(id, myVehicleBuilder.buildVehicle(
            id, altDef, _driverParser.getRouteStart(),
            _net.getDefaultVehicleType(), RGBColor(), -1, 0));
    } while(!ended()&&time<_driverParser.getRouteStart());
    return true;
}


RORoute *
RORDLoader_Cell::getAlternative(size_t pos)
{
    double cost = _driverParser.getAlternativeCost(pos);
    double prop = _driverParser.getAlternativePropability(pos);
    int routeNo = _driverParser.getRouteNo(pos);
    // check whether the route was already read
    //  read the route if not
    ROEdgeVector *route = 0;
    if(_routes.size()>routeNo) {
        // get the route from the file
        route = getRouteInfoFrom(_routes[routeNo]);
    }
    if(_routes.size()<=routeNo||route==0) {
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
    if(st.size()==0) {
        return 0;
    }
    // build the route representation
    ROEdgeVector *edges = new ROEdgeVector();
    while(st.hasNext()) {
        string id = st.next();
        ROEdge *edge = _net.getEdge(id);
        if(edge==0) {
            MsgHandler::getErrorInstance()->inform(
                string("A route read from '") + _routeDefFile +
                string("' contains an unknown edge ('") +
                id + string("')."));
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
    if(FileHelpers::exists(_routeIdxFile)) {
        _hasIndexFile = true;
        _lineReader.setFileName(_routeIdxFile);
    } else {
        _hasIndexFile = false;
        _routes.push_back(0);
        _lineReader.setFileName(_routeDefFile);
    }
    _lineReader.readAll(*this);
    // save the index file when wished
    if(!_hasIndexFile&&options.getBool("save-cell-rindex")) {
        MsgHandler::getMessageInstance()->inform(
            string("Saving the cell-rindex file '") + _routeIdxFile
            + string("'... "));
        std::ofstream out(_routeIdxFile.c_str());
        for(std::vector<unsigned long>::iterator i=_routes.begin(); i!=_routes.end(); i++) {
            out << (*i) << endl;
        }
        MsgHandler::getMessageInstance()->inform("done.");
    }
    // prepare the .driver file for reading
    return initDriverFile();
}


bool RORDLoader_Cell::report(const std::string &result)
{
    if(_hasIndexFile) {
        try {
            _routes.push_back(STRConvert::_2int(result)); // !!!
        } catch (NumberFormatException) {
            MsgHandler::getErrorInstance()->inform(
                string("Your '") + _routeIdxFile + string("' contains non-digits."));
            throw ProcessError();
        } catch (EmptyData) {
        }
    } else {
        unsigned long pos = _lineReader.getPosition();
        _routes.push_back(_lineReader.getPosition());
    }
    return true;
}


bool
RORDLoader_Cell::initDriverFile()
{
    if(!_driverStrm.good()) {
        MsgHandler::getErrorInstance()->inform(
            string("Problems on opening '") + _driverFile + string("'."));
        return false;
    }
    // check for header
    int mark = FileHelpers::readInt(_driverStrm, _isIntel);
    int offset = FileHelpers::readInt(_driverStrm, _isIntel);
    int makeRouteTime = FileHelpers::readInt(_driverStrm, _isIntel);
    if(mark==INT_MAX) {
        // skip header
        _driverStrm.seekg(offset, ios::beg);
    }
    //
    return _driverStrm.good() && !_driverStrm.eof();
}


unsigned int
RORDLoader_Cell::getCurrentTimeStep() const
{
    return _driverParser.getRouteStart();
}


bool
RORDLoader_Cell::ended() const
{
    return myHaveEnded;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


