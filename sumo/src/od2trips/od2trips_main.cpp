/****************************************************************************/
/// @file    od2trips_main.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Yun-Pang Floetteroed
/// @date    Thu, 12 September 2002
/// @version $Id$
///
// Main for OD2TRIPS
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <iostream>
#include <algorithm>
#include <math.h>
#include <cstdlib>
#include <string>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/RandHelper.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/StringUtils.h>
#include <od2trips/ODDistrictCont.h>
#include <od2trips/ODDistrictHandler.h>
#include <od2trips/ODMatrix.h>
#include <utils/common/TplConvert.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/FileHelpers.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/importio/LineReader.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// functions
// ===========================================================================
void
fillOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    oc.addCallExample("-c <CONFIGURATION>", "run with configuration file");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("Time");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Defaults");
    SystemFrame::addReportOptions(oc); // fill this subtopic, too


    // register the file input options
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addDescription("net-file", "Input", "Loads network (districts) from FILE");

    oc.doRegister("od-matrix-files", 'd', new Option_FileName());
    oc.addSynonyme("od-matrix-files", "od-files");
    oc.addSynonyme("od-matrix-files", "od");
    oc.addDescription("od-matrix-files", "Input", "Loads O/D-files from FILE(s)");

    oc.doRegister("od-amitran-files", new Option_FileName());
    oc.addSynonyme("od-amitran-files", "amitran-files");
    oc.addSynonyme("od-amitran-files", "amitran");
    oc.addDescription("od-amitran-files", "Input", "Loads O/D-matrix in Amitran format from FILE(s)");


    // register the file output options
    oc.doRegister("output-file", 'o', new Option_FileName());
    oc.addSynonyme("output-file", "output", true);
    oc.addDescription("output-file", "Output", "Writes trip definitions into FILE");

    oc.doRegister("flow-output", new Option_FileName());
    oc.addDescription("flow-output", "Output", "Writes flow definitions into FILE");

    oc.doRegister("ignore-vehicle-type", new Option_Bool(false));
    oc.addSynonyme("ignore-vehicle-type", "no-vtype", true);
    oc.addDescription("ignore-vehicle-type", "Output", "Does not save vtype information");


    // register the time settings
    oc.doRegister("begin", 'b', new Option_String("0", "TIME"));
    oc.addDescription("begin", "Time", "Defines the begin time; Previous trips will be discarded");

    oc.doRegister("end", 'e', new Option_String(SUMOTIME_MAXSTRING, "TIME"));
    oc.addDescription("end", "Time", "Defines the end time; Later trips will be discarded; Defaults to the maximum time that SUMO can represent");


    // register the data processing options
    oc.doRegister("scale", 's', new Option_Float(1));
    oc.addDescription("scale", "Processing", "Scales the loaded flows by FLOAT");

    oc.doRegister("spread.uniform", new Option_Bool(false));
    oc.addDescription("spread.uniform", "Processing", "Spreads trips uniformly over each time period");

    oc.doRegister("vtype", new Option_String(""));
    oc.addDescription("vtype", "Processing", "Defines the name of the vehicle type to use");

    oc.doRegister("prefix", new Option_String(""));
    oc.addDescription("prefix", "Processing", "Defines the prefix for vehicle names");

    oc.doRegister("timeline", new Option_String());
    oc.addDescription("timeline", "Processing", "Uses STR as a timeline definition");

    oc.doRegister("timeline.day-in-hours", new Option_Bool(false));
    oc.addDescription("timeline.day-in-hours", "Processing", "Uses STR as a 24h-timeline definition");

    oc.doRegister("ignore-errors", new Option_Bool(false)); // !!! describe, document
    oc.addSynonyme("ignore-errors", "dismiss-loading-errors", true);
    oc.addDescription("ignore-errors", "Processing", "Continue on broken input");

    oc.doRegister("no-step-log", new Option_Bool(false));
    oc.addDescription("no-step-log", "Processing", "Disable console output of current time step");


    // register defaults options
    oc.doRegister("departlane", new Option_String("free"));
    oc.addDescription("departlane", "Defaults", "Assigns a default depart lane");

    oc.doRegister("departpos", new Option_String());
    oc.addDescription("departpos", "Defaults", "Assigns a default depart position");

    oc.doRegister("departspeed", new Option_String("max"));
    oc.addDescription("departspeed", "Defaults", "Assigns a default depart speed");

    oc.doRegister("arrivallane", new Option_String());
    oc.addDescription("arrivallane", "Defaults", "Assigns a default arrival lane");

    oc.doRegister("arrivalpos", new Option_String());
    oc.addDescription("arrivalpos", "Defaults", "Assigns a default arrival position");

    oc.doRegister("arrivalspeed", new Option_String());
    oc.addDescription("arrivalspeed", "Defaults", "Assigns a default arrival speed");

    // add rand options
    RandHelper::insertRandOptions();
}

bool
checkOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    bool ok = true;
    if (!oc.isSet("net-file")) {
        WRITE_ERROR("No net input file (-n) specified.");
        ok = false;
    }
    if (!oc.isSet("od-matrix-files") && !oc.isSet("od-amitran-files")) {
        WRITE_ERROR("No input specified.");
        ok = false;
    }
    if (!oc.isSet("output-file")) {
        WRITE_ERROR("No trip table output file (-o) specified.");
        ok = false;
    }
    //
    SUMOVehicleParameter p;
    std::string error;
    if (oc.isSet("departlane") && !SUMOVehicleParameter::parseDepartLane(oc.getString("departlane"), "option", "departlane", p.departLane, p.departLaneProcedure, error)) {
        WRITE_ERROR(error);
        ok = false;
    }
    if (oc.isSet("departpos") && !SUMOVehicleParameter::parseDepartPos(oc.getString("departpos"), "option", "departpos", p.departPos, p.departPosProcedure, error)) {
        WRITE_ERROR(error);
        ok = false;
    }
    if (oc.isSet("departspeed") && !SUMOVehicleParameter::parseDepartSpeed(oc.getString("departspeed"), "option", "departspeed", p.departSpeed, p.departSpeedProcedure, error)) {
        WRITE_ERROR(error);
        ok = false;
    }
    if (oc.isSet("arrivallane") && !SUMOVehicleParameter::parseArrivalLane(oc.getString("arrivallane"), "option", "arrivallane", p.arrivalLane, p.arrivalLaneProcedure, error)) {
        WRITE_ERROR(error);
        ok = false;
    }
    if (oc.isSet("arrivalpos") && !SUMOVehicleParameter::parseArrivalPos(oc.getString("arrivalpos"), "option", "arrivalpos", p.arrivalPos, p.arrivalPosProcedure, error)) {
        WRITE_ERROR(error);
        ok = false;
    }
    if (oc.isSet("arrivalspeed") && !SUMOVehicleParameter::parseArrivalSpeed(oc.getString("arrivalspeed"), "option", "arrivalspeed", p.arrivalSpeed, p.arrivalSpeedProcedure, error)) {
        WRITE_ERROR(error);
        ok = false;
    }
    return ok;
}




/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char** argv) {
    OptionsCont& oc = OptionsCont::getOptions();
    // give some application descriptions
    oc.setApplicationDescription("Importer of O/D-matrices for the road traffic simulation SUMO.");
    oc.setApplicationName("od2trips", "SUMO od2trips Version " + getBuildName(VERSION_STRING));
    int ret = 0;
    try {
        // initialise subsystems
        XMLSubSys::init();
        fillOptions();
        OptionsIO::getOptions(true, argc, argv);
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }
        XMLSubSys::setValidation(oc.getString("xml-validation"), oc.getString("xml-validation.net"));
        MsgHandler::initOutputOptions();
        if (!checkOptions()) {
            throw ProcessError();
        }
        RandHelper::initRandGlobal();
        // load the districts
        // check whether the user gave a net filename
        if (!oc.isSet("net-file")) {
            throw ProcessError("You must supply a network or districts file ('-n').");
        }
        // get the file name and set it
        ODDistrictCont districts;
        districts.loadDistricts(oc.getString("net-file"));
        if (districts.size() == 0) {
            throw ProcessError("No districts loaded.");
        }
        // load the matrix
        ODMatrix matrix(districts);
        matrix.loadMatrix(oc);
        if (matrix.getNoLoaded() == 0) {
            throw ProcessError("No vehicles loaded.");
        }
        if (MsgHandler::getErrorInstance()->wasInformed() && !oc.getBool("ignore-errors")) {
            throw ProcessError("Loading failed.");
        }
        WRITE_MESSAGE(toString(matrix.getNoLoaded()) + " vehicles loaded.");
        // apply a curve if wished
        if (oc.isSet("timeline")) {
            matrix.applyCurve(matrix.parseTimeLine(oc.getStringVector("timeline"), oc.getBool("timeline.day-in-hours")));
        }
        // write
        bool haveOutput = false;
        if (OutputDevice::createDeviceByOption("output-file", "routes", "routes_file.xsd")) {
            matrix.write(string2time(oc.getString("begin")), string2time(oc.getString("end")),
                         OutputDevice::getDeviceByOption("output-file"),
                         oc.getBool("spread.uniform"), oc.getBool("ignore-vehicle-type"),
                         oc.getString("prefix"), !oc.getBool("no-step-log"));
            haveOutput = true;
        }
        if (OutputDevice::createDeviceByOption("flow-output", "routes", "routes_file.xsd")) {
            matrix.writeFlows(string2time(oc.getString("begin")), string2time(oc.getString("end")),
                              OutputDevice::getDeviceByOption("flow-output"),
                              oc.getBool("ignore-vehicle-type"), oc.getString("prefix"));
            haveOutput = true;
        }
        if (!haveOutput) {
            throw ProcessError("No output file given.");
        }
        WRITE_MESSAGE(toString(matrix.getNoDiscarded()) + " vehicles discarded.");
        WRITE_MESSAGE(toString(matrix.getNoWritten()) + " vehicles written.");
    } catch (const ProcessError& e) {
        if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string("")) {
            WRITE_ERROR(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
#ifndef _DEBUG
    } catch (const std::exception& e) {
        if (std::string(e.what()) != std::string("")) {
            WRITE_ERROR(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on unknown error).", false);
        ret = 1;
#endif
    }
    SystemFrame::close();
    if (ret == 0) {
        std::cout << "Success." << std::endl;
    }
    return ret;
}



/****************************************************************************/

