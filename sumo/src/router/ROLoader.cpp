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
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <iostream>
#include <fstream>
#include <string>
#include <parsers/SAXParser.hpp>
#include <util/PlatformUtils.hpp>
#include <util/TransService.hpp>
#include <sax2/SAX2XMLReader.hpp>
#include <sax2/XMLReaderFactory.hpp>
#include <sax2/DefaultHandler.hpp>
#include <utils/options/OptionsCont.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/SErrorHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/importio/LineReader.h>
#include "RONet.h"
#include "RONetHandler.h"
#include "ROLoader.h"
#include "ROTripHandler.h"
#include "ROWeightsHandler.h"
#include "ROSUMORoutesHandler.h"
#include "ROCellRouteDefHandler.h"
#include "ROSUMOAltRoutesHandler.h"
#include "ROArtemisRouteDefHandler.h"
#include "ROTypedRoutesLoader.h"


using namespace std;

ROLoader::ROLoader(OptionsCont &oc)
    : _options(oc)
{
}


ROLoader::~ROLoader()
{
    for(RouteLoaderCont::iterator i=_handler.begin(); i!=_handler.end(); i++) {
        delete (*i);
    }
}


RONet *
ROLoader::loadNet()
{
    RONet *net = new RONet(_options.isSet("sumo-input"));
    std::string files = _options.getString("n");
    if(!FileHelpers::checkFileList(files)) {
        SErrorHandler::add("Net not found!");
        delete net;
        return 0;
    }
    RONetHandler handler(_options, *net);
    // build and prepare the parser
    SAX2XMLReader *reader = getSAXReader(handler);
    bool ok = loadNet(reader, handler, files);
    // delete the reader;
    delete reader;
    if(!ok) {
        delete net;
        return 0;
    }
    return net;
}

void
ROLoader::openRoutes(RONet &net)
{
    // load additional precomputed sumo-routes when wished
    if(_options.isSet("s")) {
        openTypedRoutes(new ROSumoRoutesHandler(net), "s");
    }
    // load the XML-route definitions when wished
    if(_options.isSet("t")) {
        openTypedRoutes(new ROTripHandler(net), "t");
    }
    // load the cell-routes when wished
    if(_options.isSet("cell-input")) {
        openTypedRoutes(
            new ROCellRouteDefHandler(
                net,
                _options.getFloat("gBeta"),
                _options.getFloat("gA")),
            "cell-input");
    }
    // load artemis routes when wished
    if(_options.isSet("artemis-input")) {
        openTypedRoutes(
            new ROArtemisRouteDefHandler(
                net),
            "artemis-input");
    }
    // load the sumo-alternative file when wished
    if(_options.isSet("alternatives")) {
        openTypedRoutes(
            new ROSUMOAltRoutesHandler(
                net,
                _options.getFloat("gBeta"),
                _options.getFloat("gA")),
            "alternatives");
    }
}

void
ROLoader::skipPreviousRoutes(long start)
{
    for(RouteLoaderCont::iterator i=_handler.begin(); i!=_handler.end(); i++) {
        (*i)->skipPreviousRoutes(start);
    }
}

void
ROLoader::processRoutesStepWise(long start, long end,
                                std::ofstream &res,
                                std::ofstream &altres,
                                RONet &net)
{
    // skip routes that begin before the simulation's begin
    skipPreviousRoutes(start);
    // loop till the end
    bool endReached = false;
    for(long time=getMinTimeStep(); (!endReached||net.furtherStored())&&time<end; time++) {
        if(_options.getBool("v")) {
            cout << "Reading time step: " << time << endl;
        }
        endReached = true;
        RouteLoaderCont::iterator i;
        // go through all handlers
        for(i=_handler.begin(); i!=_handler.end(); i++) {
            // load routes until the time point is reached
            if(!(*i)->addRoutesUntil(time)) {
                return;
            }
            // save the routes
            net.saveAndRemoveRoutesUntil(_options, res, altres, time);
        }
        // check whether further data exist
        endReached = true;
        for(i=_handler.begin(); endReached&&i!=_handler.end(); i++) {
            if(!(*i)->ended()) {
                endReached = false;
            }
        }
    }
}

long
ROLoader::getMinTimeStep() const {
    long ret = LONG_MAX;
    for(RouteLoaderCont::const_iterator i=_handler.begin(); i!=_handler.end(); i++) {
        long akt = (*i)->getCurrentTimeStep();
        if(akt<ret) {
            ret = akt;
        }
    }
    return ret;
}

void
ROLoader::processAllRoutes(long start, long end,
                           std::ofstream &res,
                           std::ofstream &altres,
                           RONet &net)
{
    bool ok = true;
    for(RouteLoaderCont::iterator i=_handler.begin(); ok&&i!=_handler.end(); i++) {
        if(!(*i)->addAllRoutes()) {
            return;
        }
    }
    // save the routes
    net.saveAndRemoveRoutesUntil(_options, res, altres, end);
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
ROLoader::openTypedRoutes(ROTypedRoutesLoader *handler,
                          const std::string &optionName)
{
    // check the given files
    if(!FileHelpers::checkFileList(_options.getString(optionName))) {
        SErrorHandler::add(
            string("The list of ") + handler->getDataName() +
            string("' is empty!"), true);
        throw ProcessError();
    }
    // allocate a reader and add it to the list
    addToHandlerList(handler, _options.getString(optionName));
}

void
ROLoader::addToHandlerList(ROTypedRoutesLoader *handler,
                           const std::string &fileList)
{
    StringTokenizer st(fileList, ";");
    while(st.hasNext()) {
        // get the file name
        string file = st.next();
        // check whether the file can be used
        //  necessary due to the extensions within cell-import
        if(!handler->checkFile(file)) {
            SErrorHandler::add(
                string("Problems with ")
                + handler->getDataName() + string("-typed input '")
                + file + string("'."));
            throw ProcessError();
        }
        // build the instance when everything's all right
        ROTypedRoutesLoader *instance = handler->getAssignedDuplicate(file);
        if(!instance->init(_options)) {
            delete instance;
            SErrorHandler::add(string("The loader for ") + handler->getDataName()
                + string(" from file '") + file + string("' could not be initialised."));
            throw ProcessError();
        }
        _handler.push_back(instance);
    }
    // delete the archetype
    delete handler;
}


bool
ROLoader::loadWeights(RONet &net) {
    string weightsFileName = _options.getString("w");
    // check whether the file exists
    if(!FileHelpers::exists(weightsFileName)) {
        SErrorHandler::add(string("The weights file '") + weightsFileName + string("' does not exist!"), true);
        return false;
    }
    // build and prepare the weights handler
    ROWeightsHandler handler(_options, net, weightsFileName);
    // build and prepare the parser
    SAX2XMLReader *reader = getSAXReader(handler);
    // report whe wished
    if(_options.getBool("v")) {
        cout << "Loading precomputed net weights." << endl;
    }
    // read the file
    reader->parse(weightsFileName.c_str());
    bool ok = !SErrorHandler::errorOccured();
    // report whe wished
    if(_options.getBool("v")) {
        if(ok) {
            cout << "done." << endl;
        } else {
            cout << "failed." << endl;
        }
    }
    delete reader;
    return ok;
}

SAX2XMLReader *
ROLoader::getSAXReader(GenericSAX2Handler &handler)
{
    SAX2XMLReader *reader = XMLReaderFactory::createXMLReader();
    if(reader==0) {
        SErrorHandler::add("The XML-parser could not be build", true);
        return 0;
    }
    reader->setFeature(
        XMLString::transcode(
            "http://xml.org/sax/features/namespaces" ), false );
    reader->setFeature(
        XMLString::transcode(
            "http://apache.org/xml/features/validation/schema" ), false );
    reader->setFeature(
        XMLString::transcode(
            "http://apache.org/xml/features/validation/schema-full-checking"),
        false );
    reader->setFeature(
        XMLString::transcode(
            "http://xml.org/sax/features/validation"), false );
    reader->setFeature(
        XMLString::transcode(
            "http://apache.org/xml/features/validation/dynamic" ), false );
    reader->setContentHandler(&handler);
    reader->setErrorHandler(&handler);
    return reader;
}

/*
bool
ROLoader::loadSumoRoutes(RONet &net)
{
    string routesFileName = _options.getString("s");
    // check whether the file exists
    if(!FileHelpers::exists(routesFileName)) {
        SErrorHandler::add(string("The route definitions file '") + routesFileName + string("' does not exist!"), true);
        return false;
    }
    // build and prepare the weights handler
    ROSumoRoutesHandler handler(net);
    _parser->setContentHandler(&handler);
    _parser->setErrorHandler(&handler);
    // report whe wished
    if(_options.getBool("v"))
        cout << "Loading sumo routes... ";
    // read the file
    bool ok = true;
    for(size_t i=0; ok&&i<2; i++) {
        _parser->parse(routesFileName.c_str());
        ok = !SErrorHandler::errorOccured();
        handler.incStep();
    }
    // report whe wished
    if(_options.getBool("v")) {
        if(ok)
            cout << "done." << endl;
        else
            cout << "failed." << endl;
    }
    return ok;
}


bool
ROLoader::loadCellRoutes(RONet &net)
{
    string routesFileName = _options.getString("cell-input");
    // check whether the file exists
    if(!FileHelpers::exists(routesFileName)) {
        SErrorHandler::add(string("The route definitions file '") + routesFileName + string("' does not exist!"), true);
        return false;
    }
    StringTokenizer st(routesFileName, ";");
    while(st.hasNext()) {
        string file = st.next();
        ROCellRouteDefHandler handler(net, file);
        LineReader reader(file);
        reader.readAll(handler);
    }
    bool ok = true;
    return ok;
}



bool
ROLoader::loadXMLRouteDefs(RONet &net)
{
    string routesFileName = _options.getString("r");
    // check whether the file exists
    if(!FileHelpers::exists(routesFileName)) {
        SErrorHandler::add(string("The route definitions file '") + routesFileName + string("' does not exist!"), true);
        return false;
    }
    // build and prepare the weights handler
    ROTripHandler handler(*_options, net);
    _parser->setContentHandler(&handler);
    _parser->setErrorHandler(&handler);
    handler.setFileName(routesFileName);
    // report whe wished
    if(_options.getBool("v"))
        cout << "Loading route definitions...";
    // read the file
    _parser->parse(routesFileName.c_str());
    bool ok = !SErrorHandler::errorOccured();
    // report whe wished
    if(_options.getBool("v")) {
        if(ok)
            cout << "done." << endl;
        else
            cout << "failed." << endl;
    }
    return ok;
}
*/

/*
bool
ROLoader::loadNetInto(RONet &net)
{
    std::string files = _options.getString("n");
    if(!FileHelpers::checkFileList(files)) {
        SErrorHandler::add("No net given found!");
        return false;
    }
    RONetHandler handler(*_options, net);
    _parser->setContentHandler(&handler);
    _parser->setErrorHandler(&handler);
    bool ok = loadNetStep(handler, files, 0);
    if(ok)
        ok = loadNetStep(handler, files, 1);
    return ok;
}*/

bool
ROLoader::loadNet(SAX2XMLReader *reader, RONetHandler &handler,
                  const string &files)
{
    StringTokenizer st(files, ';');
    if(_options.getBool("v")) {
        cout << "Loading net... ";
    }
    bool ok = true;
    while(ok&&st.hasNext()) {
        ok = false;
        string tmp = st.next();
	    if(FileHelpers::exists(tmp)) {
	        handler.setFileName(tmp);
	        reader->parse(tmp.c_str());
	        ok = !(SErrorHandler::errorOccured());
	    } else {
            if(_options.getBool("v")) {
                cout << "failed." << endl;
            }
    	    SErrorHandler::add(string("The given file '") + tmp + string("' does not exist!"), true);
            ok = false;
	    }
    }
    if(_options.getBool("v")) {
        if(ok) {
            cout << "done." << endl;
        }
    }
    return ok;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROLoader.icc"
//#endif

// Local Variables:
// mode:C++
// End:


