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
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
        myRouteIdSupplier("Cell_"+file, 0),
        myVehicleIdSupplier("Cell_"+file, 0),
        myDriverParser(true, true),
        myGawronBeta(gawronBeta), myGawronA(gawronA), myHaveEnded(false),
        myMaxRoutes(maxRoutes)
{
    if (file.length()!=0) {
        // initialise the .rinfo-reader
        myRouteDefFile = file + ".rinfo";
        // pre-initialise the .driver-reader
        myDriverFile = file + ".driver";
        myDriverStrm.open(myDriverFile.c_str(), fstream::in|fstream::binary);
        // compute the name of the index file
        myRouteIdxFile = file + ".rindex";
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
        if (myDriverStrm.eof()) {
            myHaveEnded = true;
            return true;
        }
        // parse the route information
        myDriverParser.parseFrom(myDriverStrm);
        // return when the route shall be skipped
        myCurrentTime = myDriverParser.getRouteStart();
        // check whether the route is not the end
        if (myDriverParser.getRouteStart()==INT_MAX) {
            return true;
        }
        if (myCurrentTime<myBegin||myCurrentTime>=myEnd) {
            return true;
        }
        // get the route-number
        int routeNo = myDriverParser.getRouteNo();
        if (routeNo<0) {
            WRITE_WARNING("Skipping Route: " + toString<int>(routeNo));
            return true; // !!!
        }
        // add the route when it is not yet known
        RORouteDef_Alternatives *altDef =
            new RORouteDef_Alternatives(myRouteIdSupplier.getNext(),
                                        RGBColor(-1, -1, -1), myDriverParser.getLast(),
                                        myGawronBeta, myGawronA, myMaxRoutes);
        for (size_t i=0; i<3; i++) {
            RORoute *alt = getAlternative(i);
            if (alt!=0) {
                altDef->addLoadedAlternative(alt);
            }
        }
        myNet.addRouteDef(altDef);
        // add the vehicle type, the vehicle and the route to the net
        string id = myVehicleIdSupplier.getNext();
        myNet.addVehicle(id, myVehicleBuilder.buildVehicle(
                            id, altDef, myDriverParser.getRouteStart(),
                            0, RGBColor(), -1, 0));
    } while (!ended()&&(int) time<myDriverParser.getRouteStart());
    return true;
}


RORoute *
RORDLoader_Cell::getAlternative(size_t pos)
{
    SUMOReal cost = myDriverParser.getAlternativeCost(pos);
    SUMOReal prop = myDriverParser.getAlternativeProbability(pos);
    int routeNo = myDriverParser.getRouteNo(pos);
    // check whether the route was already read
    //  read the route if not
    ROEdgeVector *route = 0;
    assert(routeNo>=0);
    if (myRoutes.size()>(size_t) routeNo) {
        // get the route from the file
        route = getRouteInfoFrom(myRoutes[routeNo]);
    }
    if (myRoutes.size()<=(size_t) routeNo||route==0) {
        stringstream buf;
        buf << "The file '" << myDriverFile
        << "'" << endl
        << " references the route #" << routeNo
        << " which is not available in '" << myRouteDefFile
        << "'.";
        MsgHandler::getErrorInstance()->inform(buf.str());
        return 0;
    }
    RORoute *ret = new RORoute(myRouteIdSupplier.getNext(), cost,
                               prop, *route);
    delete route;
    return ret;
}


ROEdgeVector *
RORDLoader_Cell::getRouteInfoFrom(unsigned long position)
{
    ifstream strm(myRouteDefFile.c_str());
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
        ROEdge *edge = myNet.getEdge(id);
        if (edge==0) {
            MsgHandler::getErrorInstance()->inform("A route read from '" + myRouteDefFile + "' contains an unknown edge ('" + id + "').");
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
    myIsIntel = options.getBool("intel-cell");
    myDriverParser.isIntel(options.getBool("intel-cell"));
    myDriverParser.useLast(!options.getBool("no-last-cell"));
    // read int the positions of routes within the rinfo-file
    if (FileHelpers::exists(myRouteIdxFile)) {
        myHasIndexFile = true;
        myLineReader.setFileName(myRouteIdxFile);
    } else {
        myHasIndexFile = false;
        myRoutes.push_back(0);
        myLineReader.setFileName(myRouteDefFile);
    }
    myLineReader.readAll(*this);
    // save the index file when wished
    if (!myHasIndexFile&&options.getBool("save-cell-rindex")) {
        MsgHandler::getMessageInstance()->beginProcessMsg("Saving the cell-rindex file '" + myRouteIdxFile + "'...");
        std::ofstream out(myRouteIdxFile.c_str());
        for (std::vector<unsigned long>::iterator i=myRoutes.begin(); i!=myRoutes.end(); i++) {
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
    if (myHasIndexFile) {
        try {
            myRoutes.push_back(TplConvert<char>::_2int(result.c_str())); // !!!
        } catch (NumberFormatException &) {
            throw ProcessError("Your '" + myRouteIdxFile + "' contains non-digits.");
        } catch (EmptyData &) {}
    }
    else {
        myRoutes.push_back(myLineReader.getPosition());
    }
    return true;
}


bool
RORDLoader_Cell::initDriverFile()
{
    if (!myDriverStrm.good()) {
        MsgHandler::getErrorInstance()->inform("Problems on opening '" + myDriverFile + "'.");
        return false;
    }
    // check for header
    int mark = FileHelpers::readInt(myDriverStrm, myIsIntel);
    int offset = FileHelpers::readInt(myDriverStrm, myIsIntel);
    /*int makeRouteTime = */
    FileHelpers::readInt(myDriverStrm, myIsIntel);
    if (mark==INT_MAX) {
        // skip header
        myDriverStrm.seekg(offset, ios::beg);
    }
    //
    return myDriverStrm.good() && !myDriverStrm.eof();
}


SUMOTime
RORDLoader_Cell::getCurrentTimeStep() const
{
    return myDriverParser.getRouteStart();
}


bool
RORDLoader_Cell::ended() const
{
    return myHaveEnded;
}



/****************************************************************************/

