//---------------------------------------------------------------------------//
//                        ROLoader.cpp -
//  Loader for networks and route imports
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
// Revision 1.25  2006/01/26 08:44:14  dkrajzew
// adapted the new router API
//
// Revision 1.24  2006/01/09 12:00:58  dkrajzew
// debugging vehicle color usage
//
// Revision 1.23  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.22  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.21  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.20  2005/07/12 12:39:01  dkrajzew
// edge-based mean data implemented; previous lane-based is now optional
//
// Revision 1.19  2005/05/04 08:47:53  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.18  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.17  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.16  2004/04/14 13:53:50  roessel
// Changes and additions in order to implement supplementary-weights.
//
// Revision 1.15  2004/02/16 13:47:07  dkrajzew
// Type-dependent loader/generator-"API" changed
//
// Revision 1.14  2004/01/26 09:54:29  dkrajzew
// the loader now stops on errors as described within the manual
//
// Revision 1.13  2004/01/26 08:01:10  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work;
//  documentation added
//
// Revision 1.12  2003/08/18 12:44:54  dkrajzew
// xerces 2.2 and later compatibility patched
//
// Revision 1.11  2003/07/07 08:36:58  dkrajzew
// Warnings are now reported to the MsgHandler
//
// Revision 1.10  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message, warning
//  or an error now; it is reported to a Singleton (MsgHandler);
// this handler puts it further to output instances.
// changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.9  2003/05/20 09:48:35  dkrajzew
// debugging
//
// Revision 1.8  2003/04/09 15:39:11  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.7  2003/04/01 15:19:51  dkrajzew
// behaviour on broken nets patched
//
// Revision 1.6  2003/03/20 16:39:16  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.5  2003/03/12 16:39:19  dkrajzew
// artemis route support added
//
// Revision 1.4  2003/03/03 15:08:21  dkrajzew
// debugging
//
// Revision 1.3  2003/02/07 10:45:04  dkrajzew
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

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <parsers/SAXParser.hpp>
#include <util/PlatformUtils.hpp>
#include <util/TransService.hpp>
#include <sax2/SAX2XMLReader.hpp>
#include <utils/options/OptionsCont.h>
#include <utils/common/ToString.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/XMLHelpers.h>
#include <utils/importio/LineReader.h>
#include "RONet.h"
#include "RONetHandler.h"
#include "ROLoader.h"
#include "RORDLoader_TripDefs.h"
#include "ROWeightsHandler.h"
#include "ROSupplementaryWeightsHandler.h"
#include "RORDLoader_SUMORoutes.h"
#include "RORDLoader_Cell.h"
#include "RORDLoader_SUMOAlt.h"
#include "RORDLoader_Artemis.h"
#include "RORDGenerator_Random.h"
#include "RORDGenerator_ODAmounts.h"
#include "ROAbstractRouteDefLoader.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * xerces 2.2 compatibility
 * ======================================================================= */
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
ROLoader::ROLoader(OptionsCont &oc, ROVehicleBuilder &vb,
                   bool emptyDestinationsAllowed)
    : _options(oc), myEmptyDestinationsAllowed(emptyDestinationsAllowed),
    myVehicleBuilder(vb)
{
}


ROLoader::~ROLoader()
{
    for(RouteLoaderCont::iterator i=_handler.begin(); i!=_handler.end(); i++) {
        delete (*i);
    }
}


RONet *
ROLoader::loadNet(ROAbstractEdgeBuilder &eb)
{
    std::string file = _options.getString("n");
    if(file=="") {
        MsgHandler::getErrorInstance()->inform("Missing definition of network to load!");
        return 0;
    }
    if(!FileHelpers::exists(file)) {
        MsgHandler::getErrorInstance()->inform("The network file '" + file + "' could not be found.");
        return 0;
    }
    if(_options.getBool("v")) {
        MsgHandler::getMessageInstance()->inform("Loading net... ");
    }
    RONet *net = new RONet(_options.isSet("sumo-input"));
    RONetHandler handler(_options, *net, eb);
    handler.setFileName(file);
    XMLHelpers::runParser(handler, file);
    if(MsgHandler::getErrorInstance()->wasInformed()) {
        MsgHandler::getErrorInstance()->inform("failed.");
        delete net;
        return 0;
    } else {
        MsgHandler::getMessageInstance()->inform("done.");
    }
    // build and prepare the parser
    return net;
}


size_t
ROLoader::openRoutes(RONet &net, SUMOReal gBeta, SUMOReal gA)
{
    // build loader
        // load additional precomputed sumo-routes when wished
    openTypedRoutes("sumo-input", net);
        // load the XML-trip definitions when wished
    openTypedRoutes("trip-defs", net);
        // load the cell-routes when wished
    openTypedRoutes("cell-input", net);
        // load artemis routes when wished
    openTypedRoutes("artemis-input", net);
        // load the sumo-alternative file when wished
    openTypedRoutes("alternatives", net);
    // build generators
        // load the amount definitions if wished
    openTypedRoutes("flows", net);
        // check whether random routes shall be build, too
    if(_options.isSet("R")) {
        RORDGenerator_Random *randGen =
            new RORDGenerator_Random(myVehicleBuilder, net,
                _options.getInt("begin"), _options.getInt("end"),
                _options.getBool("prune-random"));
        randGen->init(_options);
        _handler.push_back(randGen);
    }
    if(!_options.getBool("unsorted")) {
        skipUntilBegin();
    }
    return _handler.size();
}

void
ROLoader::skipUntilBegin()
{
    MsgHandler::getMessageInstance()->inform("Skipping...");
    for(RouteLoaderCont::iterator i=_handler.begin(); i!=_handler.end(); i++) {
        (*i)->skipUntilBegin();
    }
    MsgHandler::getMessageInstance()->inform("Skipped until: " + toString<SUMOTime>(getMinTimeStep()));
}


void
ROLoader::processRoutesStepWise(SUMOTime start, SUMOTime end,
                                RONet &net, ROAbstractRouter &router)
{
    SUMOTime absNo = end - start;
    // skip routes that begin before the simulation's begin
    // loop till the end
    bool endReached = false;
    bool errorOccured = false;
    SUMOTime time = getMinTimeStep();
    SUMOTime firstStep = time;
    SUMOTime lastStep = time;
    for(; time<end&&!errorOccured&&!endReached; time++) {
        writeStats(time, start, absNo);
        RouteLoaderCont::iterator i;
        // go through all handlers
        for(i=_handler.begin(); i!=_handler.end(); i++) {
            // load routes until the time point is reached
            (*i)->readRoutesAtLeastUntil(time);
            // save the routes
            net.saveAndRemoveRoutesUntil(_options, router, time);
        }
        // check whether further data exist
        endReached = !net.furtherStored();
        lastStep = time;
        for(i=_handler.begin(); endReached&&i!=_handler.end(); i++) {
            if(!(*i)->ended()) {
                endReached = false;
            }
        }
        errorOccured =
            MsgHandler::getErrorInstance()->wasInformed()
            &&
            !_options.getBool("continue-on-unbuild");
    }
    time = end;
    writeStats(time, start, absNo);
    MsgHandler::getMessageInstance()->inform(
        "Routes found between time steps " + toString<int>(firstStep)
        + " and " + toString<int>(lastStep) + ".");
}


bool
ROLoader::makeSingleStep(SUMOTime end, RONet &net, ROAbstractRouter &router)
{
    RouteLoaderCont::iterator i;
    // go through all handlers
	if(_handler.size()!= 0){
        for(i=_handler.begin(); i!=_handler.end(); i++) {
		    // load routes until the time point is reached
			(*i)->readRoutesAtLeastUntil(end);
			// save the routes
			net.saveAndRemoveRoutesUntil(_options, router, end);
    	}

        return MsgHandler::getErrorInstance()->wasInformed();
    } else {
        cout<<"ROLoader::makeSingleStep: _handler ist leer \n"; // !!!
        return false;
    }
}


SUMOTime
ROLoader::getMinTimeStep() const
{
    SUMOTime ret = LONG_MAX;
    for(RouteLoaderCont::const_iterator i=_handler.begin(); i!=_handler.end(); i++) {
        SUMOTime akt = (*i)->getCurrentTimeStep();
        if(akt<ret) {
            ret = akt;
        }
    }
    return ret;
}


void
ROLoader::processAllRoutes(SUMOTime start, SUMOTime end,
                           RONet &net,
                           ROAbstractRouter &router)
{
    long absNo = end - start;
    bool ok = true;
    for(RouteLoaderCont::iterator i=_handler.begin(); ok&&i!=_handler.end(); i++) {
        (*i)->readRoutesAtLeastUntil(INT_MAX);
    }
    // save the routes
    SUMOTime time = start;
    for(; time<end; time++) {
        writeStats(time, start, absNo);
        net.saveAndRemoveRoutesUntil(_options, router, time);
    }
    writeStats(time, start, absNo);
}


void
ROLoader::closeReading()
{
    // close the reading
    for(RouteLoaderCont::iterator i=_handler.begin(); i!=_handler.end(); i++) {
        (*i)->closeReading();
    }
}


void
ROLoader::openTypedRoutes(const std::string &optionName,
                          RONet &net)
{
    // check whether the current loader is wished
    if(!_options.isSet(optionName)) {
        return;
    }
    // check the given files
    if(!FileHelpers::checkFileList(optionName, _options.getString(optionName))) {
        throw ProcessError();
    }
    // allocate a reader and add it to the list
    addToHandlerList(optionName, net);
}


void
ROLoader::addToHandlerList(const std::string &optionName,
                           RONet &net)
{
    string fileList = _options.getString(optionName);
    StringTokenizer st(fileList, ";");
    while(st.hasNext()) {
        // get the file name
        string file = st.next();
        // check whether the file can be used
        //  necessary due to the extensions within cell-import
        checkFile(optionName, file);
        // build the instance when everything's all right
        ROAbstractRouteDefLoader *instance =
            buildNamedHandler(optionName, file, net);
        if(!instance->init(_options)) {
            delete instance;
            MsgHandler::getErrorInstance()->inform("The loader for "
                + optionName + " from file '" + file + "' could not be initialised.");
            throw ProcessError();
        }
        _handler.push_back(instance);
    }
}


ROAbstractRouteDefLoader*
ROLoader::buildNamedHandler(const std::string &optionName,
                            const std::string &file,
                            RONet &net)
{
    if(optionName=="sumo-input") {
        return new RORDLoader_SUMORoutes(myVehicleBuilder, net,
            _options.getInt("begin"), _options.getInt("end"), file);
    }
    if(optionName=="trip-defs") {
        return new RORDLoader_TripDefs(myVehicleBuilder, net,
            _options.getInt("begin"), _options.getInt("end"),
            myEmptyDestinationsAllowed, file);
    }
    if(optionName=="cell-input") {
        return new RORDLoader_Cell(myVehicleBuilder, net,
            _options.getInt("begin"), _options.getInt("end"),
            _options.getFloat("gBeta"), _options.getFloat("gA"),
            _options.getInt("max-alternatives"), file);
    }
    if(optionName=="artemis-input") {
        return new RORDLoader_Artemis(myVehicleBuilder, net,
            _options.getInt("begin"), _options.getInt("end"), file);
    }
    if(optionName=="alternatives") {
        return new RORDLoader_SUMOAlt(myVehicleBuilder, net,
            _options.getInt("begin"), _options.getInt("end"),
            _options.getFloat("gBeta"), _options.getFloat("gA"),
            _options.getInt("max-alternatives"), file);
    }
    if(optionName=="flows") {
        return new RORDGenerator_ODAmounts(myVehicleBuilder, net,
            _options.getInt("begin"), _options.getInt("end"),
            myEmptyDestinationsAllowed, file);
    }
    throw 1;
}


void
ROLoader::checkFile(const std::string &optionName,
                    const std::string &file)
{
    if(optionName=="sumo-input"&&FileHelpers::exists(file)) {
        return;
    }
    if(optionName=="trip-defs"&&FileHelpers::exists(file)) {
        return;
    }
    if(optionName=="cell-input"&&FileHelpers::exists(file+".driver")&&FileHelpers::exists(file+".rinfo")) {
        return;
    }
    if(optionName=="artemis-input"&&FileHelpers::exists(file + "/HVdests.txt")&&FileHelpers::exists(file + "/Flows.txt")) {
        return;
    }
    if(optionName=="alternatives"&&FileHelpers::exists(file)) {
        return;
    }
    if(optionName=="flows"&&FileHelpers::exists(file)) {
        return;
    }
    MsgHandler::getErrorInstance()->inform("File '" + file + "' used as " + optionName + " not found.");
    throw ProcessError();
}


bool
ROLoader::loadWeights(RONet &net, const std::string &file,
                      bool useLanes)
{
    // check whether the file exists
    if(!FileHelpers::exists(file)) {
        MsgHandler::getErrorInstance()->inform("The weights file '" + file + "' does not exist!");
        return false;
    }
    // build and prepare the weights handler
    ROWeightsHandler handler(_options, net, file, useLanes);
    MsgHandler::getMessageInstance()->inform("Loading precomputed net weights.");
    // build and prepare the parser
    XMLHelpers::runParser(handler, file);
    bool ok = !MsgHandler::getErrorInstance()->wasInformed();
    // report whe wished
    if(ok) {
        MsgHandler::getMessageInstance()->inform("done.");
    } else {
        MsgHandler::getMessageInstance()->inform("failed.");
    }
    return ok;
}


void
ROLoader::loadSupplementaryWeights( RONet& net )
{
    string filename = _options.getString( "S" );
    if( ! FileHelpers::exists( filename ) ) {
        MsgHandler::getErrorInstance()->inform("The supplementary-weights file '" + filename + "' does not exist!" );
        throw ProcessError();
    }

    ROSupplementaryWeightsHandler handler( _options, net, filename );
    MsgHandler::getMessageInstance()->inform(
        "Loading precomputed supplementary net-weights." );

    XMLHelpers::runParser( handler, filename );
    if ( ! MsgHandler::getErrorInstance()->wasInformed() ) {
        MsgHandler::getMessageInstance()->inform( "done." );
    } else {
        MsgHandler::getMessageInstance()->inform( "failed." );
		throw ProcessError();
    }
}


void
ROLoader::writeStats(SUMOTime time, SUMOTime start, int absNo)
{
    if(_options.getBool("v")) {
        SUMOReal perc =
            (SUMOReal) (time-start) / (SUMOReal) absNo;
        cout.setf ( ios::fixed , ios::floatfield ) ; // use decimal format
        cout.setf ( ios::showpoint ) ; // print decimal point
        cout << "Reading time step: " << time
            << "  (" << (time-start) << "/" <<  absNo
        << " = " << setprecision( 2 ) << perc * 100 << "% done)       " << (char) 13;
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


