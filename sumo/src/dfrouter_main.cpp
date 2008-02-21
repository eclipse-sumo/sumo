/****************************************************************************/
/// @file    dfrouter_main.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// Main for the DFROUTER
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
#include <router/ROVehicleBuilder.h>
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
#include <routing_df/RODFLoader.h>
#include <routing_df/RODFFrame.h>
#include <routing_df/RODFNet.h>
#include <routing_df/RODFEdge.h>
#include <routing_df/RODFDetector.h>
#include <routing_df/RODFDetectorHandler.h>
#include <routing_df/DFRORouteCont.h>
#include <routing_df/RODFDetectorFlow.h>
#include <routing_df/DFDetFlowLoader.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/FileHelpers.h>

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
/**
 * loads the net
 * The net is in this meaning made up by the net itself and the dynamic
 * weights which may be supplied in a separate file
 */
RODFNet *
loadNet(OptionsCont &oc)
{
    // load the network if wished
    if (!oc.isSet("net-file")) {
        return 0;
    }
    ROVehicleBuilder vb;
    RODFLoader loader(oc, vb, false);
    // load the net
    RODFEdgeBuilder builder;
    return loader.loadNet(builder, oc.getBool("highway-mode"));
}


RODFDetectorCon *
readDetectors(OptionsCont &oc, RODFNet *optNet)
{
    if (!oc.isSet("detector-files")&&!oc.isSet("elmar-detector-files")) {
        throw ProcessError("No detector file given (use --detector-files <FILE>).");
    }
    RODFDetectorCon *cont = new RODFDetectorCon();
    // read definitions stored in XML-format
    {
        vector<string> files = oc.getStringVector("detector-files");
        for (vector<string>::const_iterator fileIt=files.begin(); fileIt!=files.end(); ++fileIt) {
            if (!FileHelpers::exists(*fileIt)) {
                delete cont;
                throw ProcessError("Could not open detector file '" + *fileIt + "'");
            }
            MsgHandler::getMessageInstance()->beginProcessMsg("Loading detector definitions from '" + *fileIt + "'... ");
            RODFDetectorHandler handler(oc, *cont, *fileIt);
            if (XMLSubSys::runParser(handler, *fileIt)) {
                MsgHandler::getMessageInstance()->endProcessMsg("done.");
            } else {
                MsgHandler::getMessageInstance()->endProcessMsg("failed.");
                delete cont;
                throw ProcessError();
            }
        }
    }
    // read definitions from Elmar-format
    {
        vector<string> files = oc.getStringVector("elmar-detector-files");
        if (files.size()!=0 && optNet==0) {
            delete cont;
            throw ProcessError("You need a network in order to read elmar definitions.");
        }
        for (vector<string>::const_iterator fileIt=files.begin(); fileIt!=files.end(); ++fileIt) {
            if (!FileHelpers::exists(*fileIt)) {
                delete cont;
                throw ProcessError("Could not open elmar detector file '" + *fileIt + "'");
            }
            MsgHandler::getMessageInstance()->beginProcessMsg("Loading detector definitions from '" + *fileIt + "'... ");
            LineReader lr(*fileIt);
            while (lr.hasMore()) {
                string line = lr.readLine();
                // skip comments and empty lines
                if (line.length()==0||line[0]=='#') {
                    continue;
                }
                // parse entries
                StringTokenizer st(line, "\t");
                vector<string> values = st.getVector();
                // false number of values (error?)
                if (values.size()<2) {
                    continue;
                }
                // process detectors only
                if (values[1]!="5") {
                    continue;
                }
                // check
                if (values.size()<6) {
                    delete cont;
                    throw ProcessError("Something is false with the following detector definition:\n " + line);
                }
                // parse
                string edge = values[5];
                string defs = values[2];
                StringTokenizer st2(defs, ";");
                if (st2.size()<3) {
                    delete cont;
                    throw ProcessError("Something is false with the following detector definition:\n " + line);
                }
                vector<string> values2 = st2.getVector();
                string id = values2[0];
                string dist = values2[2];
                dist = dist.substr(8);
                SUMOReal d = TplConvert<char>::_2SUMOReal(dist.c_str());
                ROEdge *e = optNet->getEdge(edge);
                if (e==0) {
                    MsgHandler::getWarningInstance()->inform("Detector " + id + " lies on an edge not inside the network (" + edge + ").");
                    continue;
                }
                for (int i=0; i<e->getLaneNo(); ++i) {
                    string lane = edge + "_" + toString(i);
                    string did = id + "_" + toString(i);
                    RODFDetector *detector = new RODFDetector(did, lane, d, TYPE_NOT_DEFINED);
                    if (!cont->addDetector(detector)) {
                        MsgHandler::getErrorInstance()->inform("Could not add detector '" + id + "' (probably the id is already used).");
                        delete detector;
                    }
                }
            }
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        }
    }
    return cont;

}


RODFDetectorFlows *
readDetectorFlows(OptionsCont &oc, RODFDetectorCon &dc)
{
    RODFDetectorFlows *ret = new RODFDetectorFlows(oc.getInt("begin"), oc.getInt("end"), 60); // !!!
    if (!oc.isSet("detector-flow-files")) {
        // ok, not given, return an empty container
        return ret;
    }
    // check whether the file exists
    vector<string> files = oc.getStringVector("detector-flow-files");
    for (vector<string>::const_iterator fileIt=files.begin(); fileIt!=files.end(); ++fileIt) {
        if (!FileHelpers::exists(*fileIt)) {
            throw ProcessError("The detector-flow-file '" + *fileIt + "' can not be opened.");
        }
        // parse
        MsgHandler::getMessageInstance()->beginProcessMsg("Loading flows from '" + *fileIt + "'... ");
        DFDetFlowLoader dfl(dc, *ret, oc.getInt("begin"), oc.getInt("end"), oc.getInt("time-offset"));
        dfl.read(*fileIt, oc.getBool("fast-flows"));
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
    return ret;
}


void
startComputation(RODFNet *optNet, OptionsCont &oc)
{
    // read the detector definitions (mandatory)
    RODFDetectorCon *detectors = readDetectors(oc, optNet);
    RODFDetectorFlows *flows = readDetectorFlows(oc, *detectors);
    if (flows!=0&&oc.getBool("print-absolute-flows")) {
        flows->printAbsolute();
    }

    // if a network was loaded... (mode1)
    if (optNet!=0) {
        if (oc.getBool("remove-empty-detectors")) {
            MsgHandler::getMessageInstance()->beginProcessMsg("Removing empty detectors...");
            optNet->removeEmptyDetectors(*detectors, *flows, oc.getInt("begin"), oc.getInt("end"), 60);
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        } else  if (oc.getBool("report-empty-detectors")) {
            MsgHandler::getMessageInstance()->beginProcessMsg("Scanning for empty detectors...");
            optNet->reportEmptyDetectors(*detectors, *flows);
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        }
        // compute the detector types (optionally)
        if (!detectors->detectorsHaveCompleteTypes()||oc.getBool("revalidate-detectors")) {
            optNet->computeTypes(*detectors, oc.getBool("strict-sources"));
        }
        // compute routes between the detectors (optionally)
        if (!detectors->detectorsHaveRoutes()||oc.getBool("revalidate-routes")||oc.getBool("guess-empty-flows")) {
            MsgHandler::getMessageInstance()->beginProcessMsg("Computing routes...");
            optNet->buildRoutes(*detectors,
                                oc.getBool("all-end-follower"), oc.getBool("keep-unfound-ends"),
                                oc.getBool("routes-for-all"), !oc.getBool("keep-longer-routes"),
                                oc.getInt("max-nodet-follower"));
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        }
    }

    // check
    // whether the detectors are valid
    if (!detectors->detectorsHaveCompleteTypes()) {
        throw ProcessError("The detector types are not defined; use in combination with a network");
    }
    // whether the detectors have routes
    if (!detectors->detectorsHaveRoutes()) {
        throw ProcessError("The emitters have no routes; use in combination with a network");
    }

    // save the detectors if wished
    if (oc.isSet("detectors-output")) {
        detectors->save(oc.getString("detectors-output"));
    }
    // save their positions as POIs if wished
    if (oc.isSet("detectors-poi-output")) {
        detectors->saveAsPOIs(oc.getString("detectors-poi-output"));
    }

    // save the routes file if it was changed or it's wished
    if (detectors->detectorsHaveRoutes()&&oc.isSet("routes-output")) {
        detectors->saveRoutes(oc.getString("routes-output"));
    }

    // guess flows if wished
    if (oc.getBool("guess-empty-flows")) {
        optNet->buildDetectorDependencies(*detectors);
        detectors->guessEmptyFlows(*flows);
    }

    // save emitters if wished
    if (oc.isSet("emitters-output")||oc.isSet("emitters-poi-output")) {
        optNet->buildEdgeFlowMap(*flows, *detectors, oc.getInt("begin"), oc.getInt("end"), 60); // !!!
        if (oc.getBool("revalidate-flows")) {
            MsgHandler::getMessageInstance()->beginProcessMsg("Rechecking loaded flows...");
            optNet->revalidateFlows(*detectors, *flows, oc.getInt("begin"), oc.getInt("end"), 60);
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        }
        if (oc.isSet("emitters-output")) {
            MsgHandler::getMessageInstance()->beginProcessMsg("Writing emitters...");
            detectors->writeEmitters(oc.getString("emitters-output"), *flows,
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
            detectors->writeEmitterPOIs(oc.getString("emitters-poi-output"), *flows,
                                        oc.getInt("begin"), oc.getInt("end"), 60);
            MsgHandler::getMessageInstance()->endProcessMsg("done.");
        }
    }
    // save end speed trigger if wished
    if (oc.isSet("speed-trigger-output")) {
        MsgHandler::getMessageInstance()->beginProcessMsg("Writing speed triggers...");
        detectors->writeSpeedTrigger(oc.getString("speed-trigger-output"), *flows,
                                     oc.getInt("begin"), oc.getInt("end"), 60);
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
    // save checking detectors if wished
    if (oc.isSet("validation-output")) {
        MsgHandler::getMessageInstance()->beginProcessMsg("Writing validation detectors...");
        detectors->writeValidationDetectors(oc.getString("validation-output"),
                                            oc.getBool("validation-output.add-sources"), true, true); // !!!
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
    // build global rerouter on end if wished
    if (oc.isSet("end-reroute-output")) {
        MsgHandler::getMessageInstance()->beginProcessMsg("Writing highway end rerouter...");
        detectors->writeEndRerouterDetectors(oc.getString("end-reroute-output")); // !!!
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
    /*
       // save the emission definitions
       if(oc.isSet("flow-definitions")) {
           buildVehicleEmissions(oc.getString("flow-definitions"));
       }
    */
    //
    delete flows;
    delete detectors;
//!!!    delete routes;
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
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
    try {
        // initialise the application system (messaging, xml, options)
        XMLSubSys::init();
        RODFFrame::fillOptions();
        OptionsIO::getOptions(true, argc, argv);
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }
        MsgHandler::initOutputOptions();
        if (!RODFFrame::checkOptions()) throw ProcessError();
        RandHelper::initRandGlobal();
        // retrieve the options
        net = loadNet(oc);
        if (net==0) {
            throw ProcessError();
        }
        // build routes
        startComputation(net, oc);
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
    SystemFrame::close();
    if (ret==0) {
        cout << "Success." << endl;
    }
    return ret;
}



/****************************************************************************/

