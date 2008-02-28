/****************************************************************************/
/// @file    od2trips_main.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 12 September 2002
/// @version $Id$
///
// Main for OD2TRIPS
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
#include <utils/importio/LineReader.h>
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
void
fillOptions()
{
    OptionsCont &oc = OptionsCont::getOptions();
    oc.addCallExample("-c <CONFIGURATION>");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("Time");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Report");


    // register the file input options
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addDescription("net-file", "Input", "Loads network (districts) from FILE");

    oc.doRegister("od-files", 'd', new Option_FileName());
    oc.addSynonyme("od-files", "od");
    oc.addDescription("od-files", "Input", "Loads O/D-files from FILE(s)");

    oc.doRegister("vissim", new Option_FileName());
    oc.addDescription("vissim", "Input", "Uses FILE to determine which O/D-matrices to load");


    // register the file output options
    oc.doRegister("output-file", 'o', new Option_FileName());
    oc.addSynonyme("output-file", "output");
    oc.addDescription("output-file", "Output", "Writes trip definitions into FILE");


    // register the time settings
    oc.doRegister("begin", 'b', new Option_Integer(0));
    oc.addDescription("begin", "Time", "Defines the begin time; Previous trips will be discarded");

    oc.doRegister("end", 'e', new Option_Integer(86400));
    oc.addDescription("end", "Time", "Defines the end time; Later trips will be discarded");


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

    oc.doRegister("dismiss-loading-errors", new Option_Bool(false)); // !!! describe, document
    oc.addDescription("dismiss-loading-errors", "Processing", "Continue on broken input");


    // register report options
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.addDescription("verbose", "Report", "Switches to verbose output");

    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.addDescription("suppress-warnings", "Report", "Disables output of warnings");

    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.addDescription("print-options", "Report", "Prints option values before processing");

    oc.doRegister("help", '?', new Option_Bool(false));
    oc.addDescription("help", "Report", "Prints this screen");

    oc.doRegister("log-file", 'l', new Option_FileName());
    oc.addDescription("log-file", "Report", "Writes all messages to FILE");


    // add rand options
    RandHelper::insertRandOptions();
}


Distribution_Points
parseTimeLine(const std::vector<std::string> &def, bool timelineDayInHours)
{
    bool interpolating = !timelineDayInHours;
    Position2DVector points;
    SUMOReal prob;
    if (timelineDayInHours) {
        if (def.size()!=24) {
            throw ProcessError("Assuming 24 entries for a day timeline, but got " + toString(def.size()) + ".");
        }
        for (int chour=0; chour<24; ++chour) {
            prob = TplConvert<char>::_2SUMOReal(def[chour].c_str());
            points.push_back(Position2D((SUMOReal)(chour*3600), prob));
        }
        points.push_back(Position2D((SUMOReal)(24 * 3600), prob));
    } else {
        size_t i = 0;
        while (i<def.size()) {
            StringTokenizer st2(def[i++], ":");
            if (st2.size()!=2) {
                throw ProcessError("Broken time line definition: missing a value in '" + def[i-1] + "'.");
            }
            int time = TplConvert<char>::_2int(st2.next().c_str());
            prob = TplConvert<char>::_2SUMOReal(st2.next().c_str());
            points.push_back(Position2D((SUMOReal) time, prob));
        }
    }
    return Distribution_Points("N/A", points, interpolating);
}


bool
checkOptions()
{
    OptionsCont &oc = OptionsCont::getOptions();
    bool ok = true;
    if (!oc.isSet("net-file")) {
        MsgHandler::getErrorInstance()->inform("No net input file (-n) specified.");
        ok = false;
    }
    if (!oc.isSet("od-files")&&!oc.isSet("vissim")) {
        MsgHandler::getErrorInstance()->inform("No input specified.");
        ok = false;
    }
    if (!oc.isSet("o")) {
        MsgHandler::getErrorInstance()->inform("No trip table output file (-o) specified.");
        ok = false;
    }
    //
    return ok;
}


void
loadDistricts(ODDistrictCont &districts, OptionsCont &oc)
{
    // check whether the user gave a net filename
    if (!oc.isSet("net-file")) {
        MsgHandler::getErrorInstance()->inform("You must supply a network ('-n').");
        return;
    }
    // get the file name and set it
    string file = oc.getString("net-file");
    if (!FileHelpers::exists(file)) {
        throw ProcessError("Could not find network '" + file + "' to load.");
    }
    MsgHandler::getMessageInstance()->beginProcessMsg("Loading districts from '" + file + "'...");
    // build the xml-parser and handler
    ODDistrictHandler handler(districts, file);
    if (!XMLSubSys::runParser(handler, file)) {
        MsgHandler::getMessageInstance()->endProcessMsg("failed.");
    } else {
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
}


std::vector<std::string>
getVissimDynUMLMatrices(const std::string file)
{
    std::vector<std::string> ret;
    LineReader lr(file);
    if (!lr.good()) {
        throw ProcessError("Could not open vissim-file '" + file + "'.");
    }
    bool haveAll = false;
    while (!haveAll&&lr.hasMore()) {
        string line = lr.readLine();
        if (line.find("MATRIXDATEI")!=string::npos) {
            string name = line.substr(line.find("MATRIXDATEI"));
            name = name.substr(name.find('"')+1);
            name = name.substr(0, name.find('"'));
            ret.push_back(name);
        } else {
            // do not process the whole file if we have seen all matrices
            if (ret.size()!=0&&line.find("--------------------------")!=string::npos) {
                haveAll = true;
            }
        }
    }
    return ret;
}


string
getNextNonCommentLine(LineReader &lr)
{
    string line;
    do {
        line = lr.readLine();
        if (line[0]!='*') {
            return StringUtils::prune(line);
        }
    } while (lr.good());
    throw ProcessError();
}


SUMOTime
parseSingleTime(const std::string &time)
{
    if (time.find('.')==string::npos) {
        throw OutOfBoundsException();
    }
    string hours = time.substr(0, time.find('.'));
    string minutes = time.substr(time.find('.')+1);
    return (SUMOTime) TplConvert<char>::_2int(hours.c_str()) * 3600 + TplConvert<char>::_2int(minutes.c_str());
}


std::pair<SUMOTime, SUMOTime>
readTime(LineReader &lr)
{
    string line = getNextNonCommentLine(lr);
    try {
        StringTokenizer st(line, StringTokenizer::WHITECHARS);
        SUMOTime begin = parseSingleTime(st.next());
        SUMOTime end = parseSingleTime(st.next());
        if (begin>=end) {
            throw ProcessError("Begin time is larger than end time.");
        }
        return make_pair(begin, end);
    } catch (OutOfBoundsException &) {
        throw ProcessError("Broken period definition '" + line + "'.");
    } catch (NumberFormatException &) {
        throw ProcessError("Broken period definition '" + line + "'.");
    }
}


SUMOReal
readFactor(LineReader &lr, SUMOReal scale)
{
    string line = getNextNonCommentLine(lr);
    SUMOReal factor = -1;
    try {
        factor = TplConvert<char>::_2SUMOReal(line.c_str()) * scale;
    } catch (NumberFormatException &) {
        throw ProcessError("Broken factor: '" + line + "'.");
    }
    return factor;
}



void
readV(LineReader &lr, ODMatrix &into, SUMOReal scale,
      std::string vehType, bool matrixHasVehType)
{
    MsgHandler::getMessageInstance()->beginProcessMsg("Reading matrix '" + lr.getFileName() + "' stored as VMR...");
    // parse first defs
    string line;
    if (matrixHasVehType) {
        line = getNextNonCommentLine(lr);
        if (vehType=="") {
            vehType = StringUtils::prune(line);
        }
    }

    // parse time
    pair<SUMOTime, SUMOTime> times = readTime(lr);
    SUMOTime begin = times.first;
    SUMOTime end = times.second;

    // factor
    SUMOReal factor = readFactor(lr, scale);

    // districts
    line = getNextNonCommentLine(lr);
    int districtNo = TplConvert<char>::_2int(StringUtils::prune(line).c_str());
    // parse district names (normally ints)
    std::vector<std::string> names;
    line = getNextNonCommentLine(lr);
    do {
        StringTokenizer st2(line, StringTokenizer::WHITECHARS);
        while (st2.hasNext()) {
            names.push_back(st2.next());
        }
        line = lr.readLine();
    } while (line[0]!='*');
    assert((int) names.size()==districtNo);

    // parse the cells
    for (std::vector<std::string>::iterator si=names.begin(); si!=names.end(); ++si) {
        std::vector<std::string>::iterator di = names.begin();
        //
        line = getNextNonCommentLine(lr);
        do {
            try {
                StringTokenizer st2(line, StringTokenizer::WHITECHARS);
                while (st2.hasNext()) {
                    assert(di!=names.end());
                    SUMOReal vehNumber = TplConvert<char>::_2SUMOReal(st2.next().c_str()) * factor;
                    if (vehNumber!=0) {
                        into.add(vehNumber, begin, end, *si, *di, vehType);
                    }
                    ++di;
                }
            } catch (NumberFormatException &) {
                throw ProcessError("Not numeric vehicle number in line '" + line + "'.");
            }
            if (!lr.hasMore()) {
                break;
            }
            line = lr.readLine();
        } while (line[0]!='*');
    }
    MsgHandler::getMessageInstance()->endProcessMsg("done.");
}


void
readO(LineReader &lr, ODMatrix &into, SUMOReal scale,
      std::string vehType, bool matrixHasVehType)
{
    MsgHandler::getMessageInstance()->beginProcessMsg("Reading matrix '" + lr.getFileName() + "' stored as OR...");
    // parse first defs
    string line;
    if (matrixHasVehType) {
        line = getNextNonCommentLine(lr);
        int type = TplConvert<char>::_2int(StringUtils::prune(line).c_str());
        if (vehType=="") {
            vehType = toString(type);
        }
    }

    // parse time
    pair<SUMOTime, SUMOTime> times = readTime(lr);
    SUMOTime begin = times.first;
    SUMOTime end = times.second;

    // factor
    SUMOReal factor = readFactor(lr, scale);

    // parse the cells
    while (lr.hasMore()) {
        line = getNextNonCommentLine(lr);
        StringTokenizer st2(line, StringTokenizer::WHITECHARS);
        if (st2.size()==0) {
            continue;
        }
        try {
            string sourceD = st2.next();
            string destD = st2.next();
            SUMOReal vehNumber = TplConvert<char>::_2SUMOReal(st2.next().c_str()) * factor;
            if (vehNumber!=0) {
                into.add(vehNumber, begin, end, sourceD, destD, vehType);
            }
        } catch (OutOfBoundsException &) {
            throw ProcessError("Missing at least one information in line '" + line + "'.");
        } catch (NumberFormatException &) {
            throw ProcessError("Not numeric vehicle number in line '" + line + "'.");
        }
    }
    MsgHandler::getMessageInstance()->endProcessMsg("done.");
}


void
loadMatrix(OptionsCont &oc, ODMatrix &into)
{
    std::vector<std::string> files;
    // check whether the filenames shall be read from a vissim file
    if (oc.isSet("vissim")) {
        files = getVissimDynUMLMatrices(oc.getString("vissim"));
    } else {
        files = oc.getStringVector("od-files");
    }

    // ok, we now should have a list of files to parse
    //  check
    if (files.size()==0) {
        throw ProcessError("No files to parse are given.");
    }
    //  parse
    for (std::vector<std::string>::iterator i=files.begin(); i!=files.end(); ++i) {
        LineReader lr(*i);
        if (!lr.good()) {
            throw ProcessError("Could not open '" + (*i) + "'.");
        }
        string type = lr.readLine();
        // get the type only
        if (type.find(';')!=string::npos) {
            type = type.substr(0, type.find(';'));
        }
        // parse type-dependant
        if (type.length()>1 && type[1]=='V') {
            // process ptv's 'V'-matrices
            if (type.find('N')!=string::npos) {
                throw ProcessError("'" + *i + "' does not contain the needed information about the time described.");
            }
            readV(lr, into, oc.getFloat("scale"), oc.getString("vtype"), type.find('M')!=string::npos);
        } else if (type.length()>1 && type[1]=='O') {
            // process ptv's 'O'-matrices
            if (type.find('N')!=string::npos) {
                throw ProcessError("'" + *i + "' does not contain the needed information about the time described.");
            }
            readO(lr, into, oc.getFloat("scale"), oc.getString("vtype"), type.find('M')!=string::npos);
        } else {
            throw ProcessError("'" + *i + "' uses an unknown matrix type '" + type + "'.");
        }
    }
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
    OptionsCont &oc = OptionsCont::getOptions();
    // give some application descriptions
    oc.setApplicationDescription("Importer of O/D-matrices for the road traffic simulation SUMO.");
#ifdef WIN32
    oc.setApplicationName("od2trips.exe", "SUMO od2trips Version " + (string)VERSION_STRING);
#else
    oc.setApplicationName("sumo-od2trips", "SUMO od2trips Version " + (string)VERSION_STRING);
#endif
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
        MsgHandler::initOutputOptions();
        if (!checkOptions()) throw ProcessError();
        RandHelper::initRandGlobal();
        // load the districts
        ODDistrictCont districts;
        loadDistricts(districts, oc);
        if (districts.size()==0) {
            throw ProcessError("No districts loaded...");
        }
        // load the matrix
        ODMatrix matrix(districts);
        loadMatrix(oc, matrix);
        if (matrix.getNoLoaded()==0) {
            throw ProcessError("No vehicles loaded...");
        }
        if (MsgHandler::getErrorInstance()->wasInformed()&&!oc.getBool("dismiss-loading-errors")) {
            throw ProcessError("Loading failed...");
        }
        MsgHandler::getMessageInstance()->inform(toString(matrix.getNoLoaded()) + " vehicles loaded.");
        // apply a curve if wished
        if (oc.isSet("timeline")) {
            matrix.applyCurve(parseTimeLine(oc.getStringVector("timeline"), oc.getBool("timeline.day-in-hours")));
        }
        // write
        if (!OutputDevice::createDeviceByOption("output", "tripdefs")) {
            throw ProcessError("No output name is given.");
        }
        OutputDevice& dev = OutputDevice::getDeviceByOption("output");
        matrix.write((SUMOTime) oc.getInt("begin"), (SUMOTime) oc.getInt("end"),
                     dev, oc.getBool("spread.uniform"), oc.getString("prefix"));
        MsgHandler::getMessageInstance()->inform(toString(matrix.getNoDiscarded()) + " vehicles discarded.");
        MsgHandler::getMessageInstance()->inform(toString(matrix.getNoWritten()) + " vehicles written.");
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
    SystemFrame::close();
    OutputDevice::closeAll();
    if (ret==0) {
        cout << "Success." << endl;
    }
    return ret;
}



/****************************************************************************/

