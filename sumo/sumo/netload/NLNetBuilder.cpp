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
// Revision 1.6  2002/05/14 04:54:25  dkrajzew
// Unexisting files are now catched independent to the Xerces-error mechanism; error report generation moved to XMLConvert
//
// Revision 1.5  2002/04/24 10:32:05  dkrajzew
// Unfound files are now only reported once
//
// Revision 1.4  2002/04/17 11:18:47  dkrajzew
// windows-newlines removed
//
// Revision 1.3  2002/04/16 06:06:29  dkrajzew
// Build report debugged
//
// Revision 1.2  2002/04/15 07:07:56  dkrajzew
// new loading paradigm implemented
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
#include "../microsim/MSEmitControl.h"
#include <iostream>
#include <strstream>
#include <vector>
#include <parsers/SAXParser.hpp>
#include <util/PlatformUtils.hpp>
#include <util/TransService.hpp>
#include <sax2/SAX2XMLReader.hpp>
#include <sax2/XMLReaderFactory.hpp>
#include <sax2/DefaultHandler.hpp>
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
#include "../utils/StringTokenizer.h"
#include "../utils/OptionsCont.h"
#include "../utils/XMLConvert.h"
#include "../utils/FileHelpers.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLNetBuilder::NLNetBuilder(const OptionsCont &oc)
    : m_pOptions(oc)
{
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


MSNet *
NLNetBuilder::build() {
    NLContainer *container = new NLContainer();
    SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
    parser->setFeature(XMLString::transcode("http://xml.org/sax/features/validation"), false);
    bool ok = count(*container, *parser);
    MSNet *net = 0;
    if(ok) {
        load(*container, *parser);
        subreport("Loading done.", "Loading failed.");
        if(!SErrorHandler::errorOccured())
            net = container->buildNet();
    }
    delete parser;
    if(ok)
        report(*container);
    delete container;
    return net;
}


bool
NLNetBuilder::count(NLContainer &container, SAX2XMLReader &parser) {
    bool ok = true;
    NLSAXHandler *handler = new NLHandlerCounter(container, LOADFILTER_ALL);
    prepareParser(parser, handler, 0);
    ok = parse(m_pOptions.getString("n"), handler, parser);
    if(ok&&m_pOptions.isSet("j")) {
        (static_cast<NLHandlerCounter*>(handler))->changeLoadFilter(LOADFILTER_LOGICS);
        ok = parse(m_pOptions.getString("j"), handler, parser);
    }
    if(ok&&m_pOptions.isSet("d")) {
        (static_cast<NLHandlerCounter*>(handler))->changeLoadFilter(LOADFILTER_DETECTORS);
        ok = parse(m_pOptions.getString("d"), handler, parser);
    }
    if(ok&&m_pOptions.isSet("r")) {
        (static_cast<NLHandlerCounter*>(handler))->changeLoadFilter(LOADFILTER_DYNAMIC);
        ok = parse(m_pOptions.getString("r"), handler, parser);
    }
    delete handler;
    container.preallocate();
    return ok;
}

void
NLNetBuilder::load(NLContainer &container, SAX2XMLReader &parser) {
    load(LOADFILTER_ALL, m_pOptions.getString("n"), container, parser);
    if(m_pOptions.isSet("j"))
        load(LOADFILTER_LOGICS, m_pOptions.getString("j"), container, parser);
    if(m_pOptions.isSet("d"))
        load(LOADFILTER_DETECTORS, m_pOptions.getString("d"), container, parser);
    if(m_pOptions.isSet("r"))
        load(LOADFILTER_DYNAMIC, m_pOptions.getString("r"), container, parser);
}

void
NLNetBuilder::load(LoadFilter what, string files, NLContainer &cont, SAX2XMLReader &parser) {
    if(!checkFilenames(files)) {
        SErrorHandler::add("No " + getDataName(what) + " found!");
        SErrorHandler::add("Check your program parameter.");
        return;
    }
    std::vector<NLSAXHandler*> steps = getHandler(what, cont);
    if(m_pOptions.getBool("v"))
        cout << "Loading " << getDataName(what) << "..." << endl;
    int step = 0;
    for(std::vector<NLSAXHandler*>::iterator i=steps.begin(); i!=steps.end(); i++) {
        prepareParser(parser, *i, step);
        parse(files, *i, parser);
        delete *i;
        step++;
    }
    subreport("Loading of " + getDataName(what) + " done.", "Loading of " + getDataName(what) + " failed.");
}

void
NLNetBuilder::prepareParser(SAX2XMLReader &parser, NLSAXHandler *handler, int step) {
    parser.setContentHandler(handler);
    parser.setErrorHandler(handler);
    if(m_pOptions.getBool("v"))
        cout << "Step " << step << ": " << handler->getMessage() << endl;
}

bool
NLNetBuilder::parse(const string &files, NLSAXHandler *handler, SAX2XMLReader &parser)
{
    bool ok = true;
    StringTokenizer st(files, ';');
    while(ok&&st.hasNext()) {
        string tmp = st.next();
	if(FileHelpers::exists(tmp)) { 
	    handler->setFileName(tmp);
	    parser.parse(tmp.c_str());
	    ok = !(SErrorHandler::errorOccured());
	} else {
	    SErrorHandler::add(string("The given file '") + tmp + string("' does not exist!"), true);
	}
    }
    return ok;
}

string
NLNetBuilder::getDataName(LoadFilter forWhat) {
    switch(forWhat) {
    case LOADFILTER_ALL:
        return "net";
        break;
    case LOADFILTER_LOGICS:
        return "junction logics";
        break;
    case LOADFILTER_DETECTORS:
        return "detectors";
        break;
    case LOADFILTER_DYNAMIC:
        return "vehicles and routes";
        break;
    default:
        break;
    }
    throw exception();
}

std::vector<NLSAXHandler*>
NLNetBuilder::getHandler(LoadFilter forWhat, NLContainer &container)  {
    std::vector<NLSAXHandler*> ret;
    switch(forWhat) {
    case LOADFILTER_ALL:
        ret.push_back(new NLHandlerEdgeAllocator(container, forWhat));
        ret.push_back(new NLHandlerBuilder1(container, forWhat));
        ret.push_back(new NLHandlerBuilder2(container, forWhat));
        ret.push_back(new NLHandlerBuilder3(container, forWhat));
        break;
    case LOADFILTER_LOGICS:
        ret.push_back(new NLHandlerBuilder1(container, forWhat));
        break;
    case LOADFILTER_DETECTORS:
        ret.push_back(new NLHandlerBuilder2(container, forWhat));
        break;
    case LOADFILTER_DYNAMIC:
        ret.push_back(new NLHandlerBuilder1(container, forWhat));
        ret.push_back(new NLHandlerBuilder2(container, forWhat));
        break;
    default:
        break;
    }
    return ret;
}


void
NLNetBuilder::subreport(const std::string &ok, const std::string &wrong)
{
    if(!SErrorHandler::errorOccured())
        SLogging::add(ok.c_str());
    else {
        SErrorHandler::print();
        SLogging::add(wrong.c_str());
    }
}



void
NLNetBuilder::report(const NLContainer &container)
{
    if(SErrorHandler::errorOccured())
        SErrorHandler::print();
    else {
        if(m_pOptions.getBool("v"))
            SLogging::add(container.getStatistics());
    }
}


bool
NLNetBuilder::checkFilenames(const std::string &files) {
    StringTokenizer st(files, ';');
    return st.size()!=0;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLNetBuilder.icc"
//#endif

// Local Variables:
// mode:C++
//

