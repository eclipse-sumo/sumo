/****************************************************************************/
/// @file    dfrouter_main.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// Main for the DFROUTER
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <utils/common/TplConvert.h>
#include <iostream>
#include <string>
#include <limits.h>
#include <ctime>
#include <router/ROLoader.h>
#include <router/RONet.h>
#include <router/ROVehicleType_Krauss.h>
#include <routing_df/RODFEdgeBuilder.h>
#include <router/ROFrame.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <routing_df/RODFFrame.h>
#include <routing_df/RODFNet.h>
#include <routing_df/RODFEdge.h>
#include <routing_df/RODFDetector.h>
#include <routing_df/RODFDetectorHandler.h>
#include <routing_df/RODFRouteCont.h>
#include <routing_df/RODFDetectorFlow.h>
#include <routing_df/RODFDetFlowLoader.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/FileHelpers.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// functions
// ===========================================================================
/* -------------------------------------------------------------------------
 * data processing methods
 * ----------------------------------------------------------------------- */
void
readDetectors(RODFDetectorCon &detectors, OptionsCont &oc, RODFNet *optNet) {
    if (!oc.isSet("detector-files")) {
        throw ProcessError("No detector file given (use --detector-files <FILE>).");
    }
    // read definitions stored in XML-format
    vector<string> files = oc.getStringVector("detector-files");
    for (vector<string>::const_iterator fileIt=files.begin(); fileIt!=files.end(); ++fileIt) {
        if (!FileHelpers::exists(*fileIt)) {
            throw ProcessError("Could not open detector file '" + *fileIt + "'");
        }
        MsgHandler::getMessageInstance()->beginProcessMsg("Loading detector definitions from '" + *fileIt + "'... ");
        RODFDetectorHandler handler(oc, detectors, *fileIt);
        if (XMLSubSys::runParser(handler, *fileIt)) {
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        } else {
            MsgHandler::getMessageInstance()->endProcessMsg("failed.");
            throw ProcessError();
        }
    }
}


void
readDetectorFlows(RODFDetectorFlows &flows, OptionsCont &oc, RODFDetectorCon &dc) {
    if (!oc.isSet("detector-flow-files")) {
        // ok, not given, return an empty container
        return;
    }
    // check whether the file exists
    vector<string> files = oc.getStringVector("detector-flow-files");
    for (vector<string>::const_iterator fileIt=files.begin(); fileIt!=files.end(); ++fileIt) {
        if (!FileHelpers::exists(*fileIt)) {
            throw ProcessError("The detector-flow-file '" + *fileIt + "' can not be opened.");
        }
        // parse
        MsgHandler::getMessageInstance()->beginProcessMsg("Loading flows from '" + *fileIt + "'...");
        RODFDetFlowLoader dfl(dc, flows, oc.getInt("begin"), oc.getInt("end"), oc.getInt("time-offset"));
        dfl.read(*fileIt);
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
}


void
startComputation(RODFNet *optNet, RODFDetectorFlows &flows, RODFDetectorCon &detectors, OptionsCont &oc) {
    if (oc.getBool("print-absolute-flows")) {
        flows.printAbsolute();
    }

    // if a network was loaded... (mode1)
    if (optNet!=0) {
        if (oc.getBool("remove-empty-detectors")) {
            MsgHandler::getMessageInstance()->beginProcessMsg("Removing empty detectors...");
            optNet->removeEmptyDetectors(detectors, flows, oc.getInt("begin"), oc.getInt("end"), 60);
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        } else  if (oc.getBool("report-empty-detectors")) {
            MsgHandler::getMessageInstance()->beginProcessMsg("Scanning for empty detectors...");
            optNet->reportEmptyDetectors(detectors, flows);
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        }
        // compute the detector types (optionally)
        if (!detectors.detectorsHaveCompleteTypes()||oc.getBool("revalidate-detectors")) {
            optNet->computeTypes(detectors, oc.getBool("strict-sources"));
        }
        // compute routes between the detectors (optionally)
        if (!detectors.detectorsHaveRoutes()||oc.getBool("revalidate-routes")||oc.getBool("guess-empty-flows")) {
            MsgHandler::getMessageInstance()->beginProcessMsg("Computing routes...");
            optNet->buildRoutes(detectors,
                                oc.getBool("all-end-follower"), oc.getBool("keep-unfound-ends"),
                                oc.getBool("routes-for-all"), !oc.getBool("keep-longer-routes"),
                                oc.getInt("max-nodet-follower"));
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        }
    }

    // check
    // whether the detectors are valid
    if (!detectors.detectorsHaveCompleteTypes()) {
        throw ProcessError("The detector types are not defined; use in combination with a network");
    }
    // whether the detectors have routes
    if (!detectors.detectorsHaveRoutes()) {
        throw ProcessError("The emitters have no routes; use in combination with a network");
    }

    // save the detectors if wished
    if (oc.isSet("detectors-output")) {
        detectors.save(oc.getString("detectors-output"));
    }
    // save their positions as POIs if wished
    if (oc.isSet("detectors-poi-output")) {
        detectors.saveAsPOIs(oc.getString("detectors-poi-output"));
    }

    // save the routes file if it was changed or it's wished
    if (detectors.detectorsHaveRoutes()&&oc.isSet("routes-output")) {
        detectors.saveRoutes(oc.getString("routes-output"));
    }

    // guess flows if wished
    if (oc.getBool("guess-empty-flows")) {
        optNet->buildDetectorDependencies(detectors);
        detectors.guessEmptyFlows(flows);
    }

    // save emitters if wished
    if (oc.isSet("emitters-output")||oc.isSet("emitters-poi-output")) {
        optNet->buildEdgeFlowMap(flows, detectors, oc.getInt("begin"), oc.getInt("end"), 60); // !!!
        if (oc.getBool("revalidate-flows")) {
            MsgHandler::getMessageInstance()->beginProcessMsg("Rechecking loaded flows...");
            optNet->revalidateFlows(detectors, flows, oc.getInt("begin"), oc.getInt("end"), 60);
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        }
        if (oc.isSet("emitters-output")) {
            MsgHandler::getMessageInstance()->beginProcessMsg("Writing emitters...");
            detectors.writeEmitters(oc.getString("emitters-output"), flows,
                                    oc.getInt("begin"), oc.getInt("end"), 60,
                                    *optNet,
                                    oc.getBool("write-calibrators"),
                                    oc.getBool("include-unused-routes"),
                                    oc.getFloat("scale"),
                                    oc.getInt("max-nodet-follower"),
                                    oc.getBool("emissions-only"));
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        }
        if (oc.isSet("emitters-poi-output")) {
            MsgHandler::getMessageInstance()->beginProcessMsg("Writing emitter pois...");
            detectors.writeEmitterPOIs(oc.getString("emitters-poi-output"), flows,
                                       oc.getInt("begin"), oc.getInt("end"), 60);
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        }
    }
    // save end speed trigger if wished
    if (oc.isSet("speed-trigger-output")) {
        MsgHandler::getMessageInstance()->beginProcessMsg("Writing speed triggers...");
        detectors.writeSpeedTrigger(optNet, oc.getString("speed-trigger-output"), flows,
                                    oc.getInt("begin"), oc.getInt("end"), 60);
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
    // save checking detectors if wished
    if (oc.isSet("validation-output")) {
        MsgHandler::getMessageInstance()->beginProcessMsg("Writing validation detectors...");
        detectors.writeValidationDetectors(oc.getString("validation-output"),
                                           oc.getBool("validation-output.add-sources"), true, true); // !!!
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
    // build global rerouter on end if wished
    if (oc.isSet("end-reroute-output")) {
        MsgHandler::getMessageInstance()->beginProcessMsg("Writing highway end rerouter...");
        detectors.writeEndRerouterDetectors(oc.getString("end-reroute-output")); // !!!
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
    /*
       // save the emission definitions
       if(oc.isSet("flow-definitions")) {
           buildVehicleEmissions(oc.getString("flow-definitions"));
       }
    */
    //
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv) {
    OptionsCont &oc = OptionsCont::getOptions();
    // give some application descriptions
    oc.setApplicationDescription("Builds vehicle routes for SUMO using detector values.");
#ifdef WIN32
    oc.setApplicationName("dfrouter.exe", "SUMO dfrouter Version " + (string)VERSION_STRING);
#else
    oc.setApplicationName("sumo-dfrouter", "SUMO dfrouter Version " + (string)VERSION_STRING);
#endif
    int ret = 0;
    RODFNet *net = 0;
    RODFDetectorCon *detectors = 0;
    RODFDetectorFlows *flows = 0;
    try {
        // initialise the application system (messaging, xml, options)
        XMLSubSys::init(false);
        RODFFrame::fillOptions();
        OptionsIO::getOptions(true, argc, argv);
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }
        MsgHandler::initOutputOptions();
        if (!RODFFrame::checkOptions()) throw ProcessError();
        RandHelper::initRandGlobal();
        // load data
        ROLoader loader(oc, false);
        net = new RODFNet(oc.getBool("highway-mode"));
        RODFEdgeBuilder builder;
        loader.loadNet(*net, builder);
        net->buildApproachList();
        // load detectors
        detectors = new RODFDetectorCon();
        readDetectors(*detectors, oc, net);
        // load detector values
        flows = new RODFDetectorFlows(oc.getInt("begin"), oc.getInt("end"), 60); // !!!
        readDetectorFlows(*flows, oc, *detectors);
        // build routes
        startComputation(net, *flows, *detectors, oc);
    } catch (ProcessError &e) {
        if (string(e.what())!=string("Process Error") && string(e.what())!=string("")) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
#ifndef _DEBUG
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on unknown error).", false);
        ret = 1;
#endif
    }
    delete net;
    delete flows;
    delete detectors;
    OutputDevice::closeAll();
    SystemFrame::close();
    if (ret==0) {
        cout << "Success." << endl;
    }
    return ret;
}



/****************************************************************************/

