/****************************************************************************/
/// @file    emissionsDrivingCycle_main.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 21.08.2013
/// @version $Id$
///
// Main for an emissions calculator
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2017 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/TplConvert.h>
#include <iostream>
#include <string>
#include <ctime>
#include <utils/common/MsgHandler.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/TplConvert.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/importio/LineReader.h>
#include "TrajectoriesHandler.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// functions
// ===========================================================================


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char** argv) {
    // build options
    OptionsCont& oc = OptionsCont::getOptions();
    //  give some application descriptions
    oc.setApplicationDescription("Computes emissions by driving a time line.");
    oc.setApplicationName("emissionsDrivingCycle", "SUMO emissionsDrivingCycle Version " VERSION_STRING);
    //  add options

    SystemFrame::addConfigurationOptions(oc);
    oc.addOptionSubTopic("Input");
    oc.doRegister("timeline-file", 't', new Option_FileName());
    oc.addSynonyme("timeline", "timeline-file");
    oc.addDescription("timeline-file", "Input", "Defines the file to read the driving cycle from.");

    oc.doRegister("timeline-file.skip", new Option_Integer(0));
    oc.addSynonyme("timeline.skip", "timeline-file.skip");
    oc.addDescription("timeline-file.skip", "Input", "Skips the firs NUM lines.");

    oc.doRegister("timeline-file.separator", new Option_String(";"));
    oc.addSynonyme("timeline.separator", "timeline-file.separator");
    oc.addDescription("timeline-file.separator", "Input", "Defines the entry separator.");

    oc.doRegister("netstate-file", 'n', new Option_FileName());
    oc.addSynonyme("netstate", "netstate-file");
    oc.addSynonyme("amitran", "netstate-file");
    oc.addDescription("netstate-file", "Input", "Defines the netstate, route and trajectory files to read the driving cycles from.");

    oc.doRegister("emission-class", 'e', new Option_String("unknown"));
    oc.addDescription("emission-class", "Input", "Defines for which emission class the emissions shall be generated. ");


    oc.addOptionSubTopic("Processing");
    oc.doRegister("compute-a", 'a', new Option_Bool(false));
    oc.addDescription("compute-a", "Processing", "If set, the acceleration is computed instead of being read from the file. ");

    oc.doRegister("compute-a.forward", new Option_Bool(false));
    oc.addDescription("compute-a.forward", "Processing", "If set, the acceleration for time t is computed from v(t+1) - v(t) instead of v(t) - v(t-1). ");

    oc.doRegister("compute-a.zero-correction", new Option_Bool(false));
    oc.addDescription("compute-a.zero-correction", "Processing", "If set, the acceleration for time t is set to 0 if the speed is 0. ");

    oc.doRegister("skip-first", 's', new Option_Bool(false));
    oc.addDescription("skip-first", "Processing", "If set, the first line of the read file is skipped.");

    oc.doRegister("kmh", new Option_Bool(false));
    oc.addDescription("kmh", "Processing", "If set, the given speed is interpreted as being given in km/h.");

    oc.doRegister("have-slope", new Option_Bool(false));
    oc.addDescription("have-slope", "Processing", "If set, the fourth column is read and used as slope (in deg).");

    oc.doRegister("slope", new Option_Float(0));
    oc.addDescription("slope", "Processing", "Sets a global slope (in deg) that is used if the file does not contain slope information.");

    oc.addOptionSubTopic("Output");
    oc.doRegister("output-file", 'o', new Option_String());
    oc.addSynonyme("output", "output-file");
    oc.addDescription("output", "Output", "Defines the file to write the emission cycle results into. ");

    oc.doRegister("emission-output", new Option_FileName());
    oc.addDescription("emission-output", "Output", "Save the emission values of each vehicle in XML");

    oc.doRegister("sum-output", new Option_FileName());
    oc.addSynonyme("sum", "sum-output");
    oc.addDescription("sum-output", "Output", "Save the aggregated and normed emission values of each vehicle in CSV");

    oc.addOptionSubTopic("Emissions");
    oc.doRegister("phemlight-path", new Option_FileName("./PHEMlight/"));
    oc.addDescription("phemlight-path", "Emissions", "Determines where to load PHEMlight definitions from.");

    SystemFrame::addReportOptions(oc);
    oc.doRegister("quiet", 'q', new Option_Bool(false));
    oc.addDescription("quiet", "Report", "Not writing anything.");

    // run
    int ret = 0;
    bool quiet = false;
    try {
        // initialise the application system (messaging, xml, options)
        XMLSubSys::init();
        OptionsIO::setArgs(argc, argv);
        OptionsIO::getOptions();
        OptionsCont& oc = OptionsCont::getOptions();
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }

        quiet = oc.getBool("quiet");
        if (!oc.isSet("timeline-file") && !oc.isSet("netstate-file")) {
            throw ProcessError("Either a timeline or a netstate / amitran file must be given.");
        }
        if (!oc.isSet("output-file") && (oc.isSet("timeline-file") || !oc.isSet("emission-output"))) {
            throw ProcessError("The output file must be given.");
        }
        std::ostream* out = 0;
        if (oc.isSet("output-file")) {
            out = new std::ofstream(oc.getString("output-file").c_str());
        }
        OutputDevice::createDeviceByOption("emission-output", "emission-export", "emission_file.xsd");
        OutputDevice* xmlOut = 0;
        if (oc.isSet("emission-output")) {
            xmlOut = &OutputDevice::getDeviceByOption("emission-output");
        } else if (out == 0) {
            out = &std::cout;
        }
        std::ostream* sumOut = 0;
        if (oc.isSet("sum-output")) {
            sumOut = new std::ofstream(oc.getString("sum-output").c_str());
            (*sumOut) << "Vehicle,Cycle,Time,Speed,Gradient,Acceleration,FC,FCel,CO2,NOx,CO,HC,PM" << std::endl;
        }

        const SUMOEmissionClass defaultClass = PollutantsInterface::getClassByName(oc.getString("emission-class"));
        const bool computeA = oc.getBool("compute-a") || oc.getBool("compute-a.forward");
        TrajectoriesHandler handler(computeA, oc.getBool("compute-a.forward"), oc.getBool("compute-a.zero-correction"), defaultClass, oc.getFloat("slope"), out, xmlOut);

        if (oc.isSet("timeline-file")) {
            int skip = oc.getBool("skip-first") ? 1 : oc.getInt("timeline-file.skip");
            const bool inKMH = oc.getBool("kmh");
            const bool haveSlope = oc.getBool("have-slope");
            SUMOReal l = 0;
            SUMOReal totalA = 0;
            SUMOReal totalS = 0;
            int time = 0;

            LineReader lr(oc.getString("timeline-file"));
            while (lr.hasMore()) {
                std::string line = lr.readLine();
                if (skip > 0) {
                    skip--;
                    continue;
                }
                StringTokenizer st(StringUtils::prune(line), oc.getString("timeline-file.separator"));
                if (st.hasNext()) {
                    try {
                        SUMOReal t = TplConvert::_2SUMOReal<char>(st.next().c_str());
                        SUMOReal v = 0;
                        if (st.hasNext()) {
                            v = TplConvert::_2SUMOReal<char>(st.next().c_str());
                        } else {
                            v = t;
                            t = time;
                        }
                        if (inKMH) {
                            v /= 3.6;
                        }
                        SUMOReal a = !computeA && st.hasNext() ? TplConvert::_2SUMOReal<char>(st.next().c_str()) : TrajectoriesHandler::INVALID_VALUE;
                        SUMOReal s = haveSlope && st.hasNext() ? TplConvert::_2SUMOReal<char>(st.next().c_str()) : TrajectoriesHandler::INVALID_VALUE;
                        if (handler.writeEmissions(*out, "", defaultClass, t, v, a, s)) {
                            l += v;
                            totalA += a;
                            totalS += s;
                            time++;
                        }
                    } catch (EmptyData&) {
                        throw ProcessError("Missing an entry in line '" + line + "'.");
                    } catch (NumberFormatException&) {
                        throw ProcessError("Not numeric entry in line '" + line + "'.");
                    }
                }
            }
            if (!quiet) {
                std::cout << "sums" << std::endl
                          << "length:" << l << std::endl;
            }
            if (sumOut != 0) {
                (*sumOut) << oc.getString("emission-class") << "," << lr.getFileName() << "," << time << ","
                          << (l / time * 3.6) << "," << (totalS / time) << "," << (totalA / time) << ",";
                handler.writeNormedSums(*sumOut, "", l);
            }
        }
        if (oc.isSet("netstate-file")) {
            XMLSubSys::runParser(handler, oc.getString("netstate-file"));
        }
        if (!quiet) {
            handler.writeSums(std::cout, "");
        }
    } catch (InvalidArgument& e) {
        MsgHandler::getErrorInstance()->inform(e.what());
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    } catch (ProcessError& e) {
        if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string("")) {
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
    SystemFrame::close();
    if (ret == 0 && !quiet) {
        std::cout << "Success." << std::endl;
    }
    return ret;
}



/****************************************************************************/

