/***************************************************************************
                          NLNetBuilder.cpp
			  Container for MSNet during its building
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.1  2002/04/08 07:21:24  traffic
// Initial revision
//
// Revision 2.0  2002/02/14 14:43:24  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:40:45  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:09  traffic
// moved from netbuild
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "NLNetBuilder.h"
#include "../microsim/MSNet.h"
#include <iostream>
#include <strstream>
#include <parsers/SAXParser.hpp>
#include <util/PlatformUtils.hpp>
#include <util/TransService.hpp>
#include <sax/HandlerBase.hpp>
#include <string>
#include <map>
#include "SErrorHandler.h"
#include "SLogging.h"
#include "NLContainer.h"
#include "NLHandlerCounter.h"
#include "NLHandlerEdgeAllocator.h"
#include "NLHandlerBuilder1.h"
#include "NLHandlerBuilder2.h"
#include "NLHandlerBuilder3.h"
#include "../utils/XMLConvert.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * static member definitions
 * ======================================================================= */
const char*              NLNetBuilder::encodingName    = "LATIN1";
bool                     NLNetBuilder::check;
bool                     NLNetBuilder::verbose;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLNetBuilder::NLNetBuilder(bool doCheck, bool printAll) 
{
  check = doCheck;
  verbose = printAll;
  // during the net parsing
  // initialisation of the xml-helper and the sax-parser
  try {
    XMLPlatformUtils::Initialize();
  } catch (const XMLException& toCatch) {
    SErrorHandler::add("Error during XML-initialization: " + XMLConvert::_2str(toCatch.getMessage()));
  }
}



NLNetBuilder::~NLNetBuilder() 
{
  // termination of the XML-helpers
  XMLPlatformUtils::Terminate();
}



HandlerBase **
NLNetBuilder::getNetHandlerPreJunctions(NLContainer *container) 
{
  HandlerBase **ret = new HandlerBase*[4];
  ret[0] = new NLHandlerCounter(container, false);
  ret[1] = new NLHandlerEdgeAllocator(container);
  ret[2] = new NLHandlerBuilder1(container, false);
  ret[3] = 0;
  return ret;
}

HandlerBase **
NLNetBuilder::getNetHandlerPostJunctions(NLContainer *container) 
{
  HandlerBase **ret = new HandlerBase*[3];
  ret[0] = new NLHandlerBuilder2(container, false);
  ret[1] = new NLHandlerBuilder3(container, false);
  ret[2] = 0;
  return ret;
}

HandlerBase **
NLNetBuilder::getVehiclesHandler(NLContainer *container) 
{
  HandlerBase **ret = new HandlerBase*[5];
  ret[0] = new NLHandlerCounter(container, true);
  ret[1] = new NLHandlerBuilder1(container, true);
  ret[2] = new NLHandlerBuilder2(container, true);
  ret[3] = new NLHandlerBuilder3(container, true);
  ret[4] = 0;
  return ret;
}



MSNet *
NLNetBuilder::loadNet(const char *path, const char *junctionsfolder) 
{
  MSNet *net = 0;
  // initialise the container holding the parsed structures
  NLContainer *container = new NLContainer();
  // parse
  HandlerBase **handlerlist = getNetHandlerPreJunctions(container);
  int step = parse(path, "Parsing the network...", handlerlist, 0);
  if(step>0) {
    container->loadJunctions(junctionsfolder);
    handlerlist = getNetHandlerPostJunctions(container);
    parse(path, "Parsing the network...", handlerlist, step);
    // the file was parsed now, let's concate all memers into a 
    // single net and return this
    if(!SErrorHandler::errorOccured()) 
      net = container->buildNet();
    subreport("Parsing ended... Net build.", "Parsing ended... Net building failed.");
  }
  delete container;
  return net;
}



MSEmitControl *
NLNetBuilder::loadVehicles(const char *path) 
{
  // initialise the container holding the parsed structures
  NLContainer *container = new NLContainer();
  // parse
  HandlerBase **handlerlist = getVehiclesHandler(container);
  parse(path, "Parsing the routes...", handlerlist, 0);
  // the file was parsed now, let's concate all memers into a 
  // single net and return this
  MSEmitControl *ret = 0;
  if(!SErrorHandler::errorOccured()) 
    ret = container->buildVehicles();
  subreport("Parsing ended... Routes build.", "Parsing ended... Route building failed.");
  delete container;
  return ret;
}


int
NLNetBuilder::parse(const char *path, const char *msg1, HandlerBase **handlerlist, int step) 
{
  // check if the path is correct
  if(path==0) { 
    SErrorHandler::add("No input path given!!!");
    return -1;
  }
  // build parser
  SAXParser parser;
  SLogging::add("Starting parsing...");
  parser.setValidationScheme(SAXParser::Val_Auto);
  parser.setDoNamespaces(false);
  parser.setDoSchema(false);
  SLogging::add(msg1);
  // start the parsing
  try {
    bool abort = false;
    for(int i=0; !abort&&!SErrorHandler::wasFatal(); i++) {
      HandlerBase *handler = handlerlist[i];
      if(handler==0) 
      	abort = true;
      else {
        ostrstream buf;
        buf << "Step " << step++ << "... " << ends;
	      SLogging::add(buf.str(), true, false);
	      parser.setDocumentHandler(handler);
	      parser.setErrorHandler(handler);
	      parser.parse(path);
	      delete handler;
	      SLogging::add("done");
      }
    }
  } catch (const XMLException& toCatch) {
    SErrorHandler::add("An XML-error occured: " + XMLConvert::_2str(toCatch.getMessage()));
  }
  return step;
}



void
NLNetBuilder::subreport(char *ok, char *wrong) 
{
  if(!SErrorHandler::errorOccured()) 
    SLogging::add(ok);
  else {
    SErrorHandler::print();
    SLogging::add(wrong);
  }
}



void
NLNetBuilder::report(NLContainer *container) 
{
  if(SErrorHandler::errorOccured()) 
    SErrorHandler::print();
  if(!SErrorHandler::errorOccured()) {
    const char * const tmp = container->getStatistics();
    SLogging::add(tmp);
  }
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLNetBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:
