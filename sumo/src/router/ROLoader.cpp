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
#include "RORouteDefHandler.h"
#include "ROWeightsHandler.h"
#include "ROSUMORoutesHandler.h"
#include "ROCellRouteDefHandler.h"
#include "ROSUMOAltRoutesHandler.h"
#include "ROTypedRoutesLoader.h"


using namespace std;

ROLoader::ROLoader(OptionsCont *oc)
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
    RONet *net = new RONet(_options->isSet("sumo-input"));
    std::string files = _options->getString("n");
    if(!FileHelpers::checkFileList(files)) {
        SErrorHandler::add("No net given found!");
        return false;
    }
    RONetHandler handler(*_options, *net);
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
    if(_options->isSet("s")) {
        openTypedRoutes(new ROSumoRoutesHandler(net), "s");
    }
    // load the XML-route definitions when wished
    if(_options->isSet("t")) {
        openTypedRoutes(new RORouteDefHandler(net), "t");
    }
    // load the cell-routes when wished
    if(_options->isSet("cell-input")) {
        openTypedRoutes(
            new ROCellRouteDefHandler(
                net, 
                _options->getFloat("gBeta"),
                _options->getFloat("gA")), 
            "cell-input");
    }
    // load the sumo-alternative file when wished
    if(_options->isSet("alternatives")) {
        openTypedRoutes(
            new ROSUMOAltRoutesHandler(
                net,
                _options->getFloat("gBeta"),
                _options->getFloat("gA")), 
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
    for(long time=getMinTimeStep(); !endReached&&time<end; time++) {
        cout << "Reading time step: " << time << endl;
        endReached = true;
        RouteLoaderCont::iterator i;
        // go through all handlers
        for(i=_handler.begin(); i!=_handler.end(); i++) {
            // load routes until the time point is reached
            if(!(*i)->addRoutesUntil(time)) {
                return;
            }
            // save the routes
            net.saveAndRemoveRoutes(res, altres);
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
    net.saveAndRemoveRoutes(res, altres);
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
    if(!FileHelpers::checkFileList(_options->getString(optionName))) {
        SErrorHandler::add(
            string("The list of ") + handler->getDataName() +
            string("' is empty!"), true);
        throw ProcessError();
    }
    // allocate a reader and add it to the list
    addToHandlerList(handler, _options->getString(optionName));
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
    string weightsFileName = _options->getString("w");
    // check whether the file exists
    if(!FileHelpers::exists(weightsFileName)) {
        SErrorHandler::add(string("The weights file '") + weightsFileName + string("' does not exist!"), true);
        return false;
    }
    // build and prepare the weights handler
    ROWeightsHandler handler(*_options, net, weightsFileName);
    // build and prepare the parser
    SAX2XMLReader *reader = getSAXReader(handler);
    // report whe wished
    if(_options->getBool("v"))
        cout << "Loading precomputed net weights." << endl;
    // read the file
    reader->parse(weightsFileName.c_str());
    bool ok = !SErrorHandler::errorOccured();
    // report whe wished
    if(_options->getBool("v")) {
        if(ok)
            cout << "done." << endl;
        else
            cout << "failed." << endl;
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
        XMLString::transcode("http://xml.org/sax/features/validation"),
        false);
    reader->setContentHandler(&handler);
    reader->setErrorHandler(&handler);
    return reader;
}

/*
bool
ROLoader::loadSumoRoutes(RONet &net)
{
    string routesFileName = _options->getString("s");
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
    if(_options->getBool("v"))
        cout << "Loading sumo routes... ";
    // read the file
    bool ok = true;
    for(size_t i=0; ok&&i<2; i++) {
        _parser->parse(routesFileName.c_str());
        ok = !SErrorHandler::errorOccured();
        handler.incStep();
    }
    // report whe wished
    if(_options->getBool("v")) {
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
    string routesFileName = _options->getString("cell-input");
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
    string routesFileName = _options->getString("r");
    // check whether the file exists
    if(!FileHelpers::exists(routesFileName)) {
        SErrorHandler::add(string("The route definitions file '") + routesFileName + string("' does not exist!"), true);
        return false;
    }
    // build and prepare the weights handler
    RORouteDefHandler handler(*_options, net);
    _parser->setContentHandler(&handler);
    _parser->setErrorHandler(&handler);
    handler.setFileName(routesFileName);
    // report whe wished
    if(_options->getBool("v"))
        cout << "Loading route definitions...";
    // read the file
    _parser->parse(routesFileName.c_str());
    bool ok = !SErrorHandler::errorOccured();
    // report whe wished
    if(_options->getBool("v")) {
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
    std::string files = _options->getString("n");
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
    if(_options->getBool("v")) {
        cout << "Loading net... ";
    }
    bool ok = true;
    while(ok&&st.hasNext()) {
        string tmp = st.next();
	    if(FileHelpers::exists(tmp)) {
	        handler.setFileName(tmp);
	        reader->parse(tmp.c_str());
	        ok = !(SErrorHandler::errorOccured());
	    } else {
            if(_options->getBool("v"))
                cout << "failed." << endl;
    	    SErrorHandler::add(string("The given file '") + tmp + string("' does not exist!"), true);
            ok = false;
	    }
    }
    if(_options->getBool("v"))
        cout << "done." << endl;
    return ok;
}


