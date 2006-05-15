/***************************************************************************
                         od2trips_main.cpp

     The main procedure for converting OD an NET files to trip tables

    project              : SUMO      :
    subproject           : OD2TRIPS
    begin                : Thu, 12 September 2002
    modified             : Thu, 25 March 2003, INVENT purposes
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
    authors              : Daniel Krajzewicz, Peter Mieth
    emails               : Daniel.Krajzewicz@dlr.de, Peter.Mieth@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
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
#include <algorithm>
#include <math.h>
#include <cstdlib>
#include <string>
#include <parsers/SAXParser.hpp>
#include <sax2/SAX2XMLReader.hpp>

#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/HelpPrinter.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/RandHelper.h>
#include <utils/common/DevHelper.h>
#include <utils/common/ToString.h>
#include <utils/common/XMLHelpers.h>
#include <od2trips/ODDistrictCont.h>
#include <od2trips/ODDistrictHandler.h>
#include "od2trips_help.h"
#include "od2trips_build.h"
#include "sumo_version.h"
#include <od2trips/ODmatrix.h>
#include <utils/common/TplConvert.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/HelpPrinter.h>
#include <utils/importio/LineReader.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * functions
 * ======================================================================= */
void
fillOptions(OptionsCont &oc)
{
    oc.doRegister("configuration-file", 'c', new Option_FileName());
    oc.addSynonyme("configuration-file", "configuration");

    // register the file i/o options
    oc.doRegister("net-file", 'n', new Option_FileName());
	oc.addSynonyme("net-file", "net");

    oc.doRegister("od-files", 'd', new Option_FileName());
	oc.addSynonyme("od-files", "od");

	oc.doRegister("vissim", new Option_FileName());

    oc.doRegister("output-file", 'o', new Option_FileName());
	oc.addSynonyme("output-file", "output");

    // register the report options
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.doRegister("help", '?', new Option_Bool(false));
    oc.doRegister("log-file", 'l', new Option_FileName());

    // register the data processing options
    oc.doRegister("begin", 'b', new Option_Integer(0));
    oc.doRegister("end", 'e', new Option_Integer(86400));
    oc.doRegister("scale", 's', new Option_Float(1));
    oc.doRegister("no-color", new Option_Bool(false));
    oc.doRegister("spread.uniform", new Option_Bool(false));
    oc.doRegister("vtype", new Option_String(""));
    oc.doRegister("prefix", new Option_String(""));
    oc.doRegister("timeline", new Option_String(""));
    oc.doRegister("timeline.day-in-hours", new Option_Bool(false));

    // add rand and dev options
    DevHelper::insertDevOptions(oc);
    RandHelper::insertRandOptions(oc);
}


Distribution_Points
parseTimeLine(const std::string &def, bool timelineDayInHours)
{
    bool interpolating = !timelineDayInHours;
    Position2DVector points;
    StringTokenizer st(def, ";");
    if(timelineDayInHours&&st.size()!=24) {
        MsgHandler::getErrorInstance()->inform("Assuming 24 entries for a day timeline, but got " + toString(st.size()) + ".");
        throw ProcessError();
    }
    int chour = 0;
    SUMOReal prob;
    while(st.hasNext()) {
        string hourval = st.next();
        StringTokenizer st2(hourval, ",");
        int time = chour * 3600;
        // parse time if the time line is assumed to contain this
        if(!timelineDayInHours) {
            if(st2.size()!=2) {
                MsgHandler::getErrorInstance()->inform("Broken time line definition: missing a value in '" + hourval + "'.");
                throw ProcessError();
            }
            time = TplConvert<char>::_2int(st2.next().c_str());
        }
        // check otherwise
        if(timelineDayInHours&&st2.size()!=1) {
            MsgHandler::getErrorInstance()->inform("Broken time line definition: missing a value in '" + hourval + "'.");
            throw ProcessError();
        }
        // get the distribution value
        prob = TplConvert<char>::_2SUMOReal(st2.next().c_str());
        points.push_back(Position2D(time, prob));
        chour++;
    }
    if(timelineDayInHours) {
        points.push_back(Position2D(chour*3600, prob));
    }
    return Distribution_Points("N/A", points, interpolating);
}


bool
checkOptions(OptionsCont &oc)
{
    bool ok = true;
    if(!oc.isSet("net-file")) {
        MsgHandler::getErrorInstance()->inform("No net input file (-n) specified.");
        ok = false;
    }
    if(!oc.isSet("od-files")&&!oc.isSet("vissim")) {
        MsgHandler::getErrorInstance()->inform("No input specified.");
        ok = false;
    }
    if(!oc.isSet("o")) {
        MsgHandler::getErrorInstance()->inform("No trip table output file (-o) specified.");
        ok = false;
    }
    //
    return ok;
}


ODDistrictCont *
loadDistricts(OptionsCont &oc)
{
    // check whether the user gave a net filename
    if(!oc.isSet("n")) {
        MsgHandler::getErrorInstance()->inform("You must supply a network ('-n').");
        return 0;
    }
    // build the container
    ODDistrictCont *ret = new ODDistrictCont();
    // get the file name and set it
    string file = oc.getString("n");
    MsgHandler::getMessageInstance()->beginProcessMsg("Loading districts from '" + file + "'...");
    // build the xml-parser and handler
    ODDistrictHandler handler(*ret);
    handler.setFileName(file);
	XMLHelpers::runParser(handler, file);
    // check whether the loading was ok
	if(ret->size()==0||MsgHandler::getErrorInstance()->wasInformed()) {
        delete ret;
        MsgHandler::getMessageInstance()->endProcessMsg("failed.");
        return 0;
    }
    ret->colorize();
    MsgHandler::getMessageInstance()->endProcessMsg("done.");
    return ret;
}


std::vector<std::string>
getVissimDynUMLMatrices(const std::string file)
{
    std::vector<std::string> ret;
    LineReader lr(file);
    if(!lr.good()) {
        MsgHandler::getErrorInstance()->inform("Could not open vissim-file '" + file + "'.");
        throw ProcessError();
    }
    bool haveAll = false;
    while(!haveAll&&lr.hasMore()) {
        string line = lr.readLine();
        if(line.find("MATRIXDATEI")!=string::npos) {
            string name = line.substr(line.find("MATRIXDATEI"));
            name = name.substr(name.find('"')+1);
            name = name.substr(0, name.find('"'));
            ret.push_back(name);
        } else {
            // do not process the whole file if we have seen all matrices
            if(ret.size()!=0&&line.find("--------------------------")!=string::npos) {
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
        if(line[0]!='*') {
            return line;
        }
    } while(lr.good());
    throw ProcessError();
}


SUMOTime
parseTime(const std::string &time)
{
    string hours = time.substr(0, time.find('.'));
    string minutes = time.substr(time.find('.')+1);
    return TplConvert<char>::_2int(hours.c_str()) * 3600 + TplConvert<char>::_2int(minutes.c_str());
}


void
readVMR(LineReader &lr, ODMatrix &into, float scale,
        std::string vehType)
{
    MsgHandler::getMessageInstance()->beginProcessMsg("Reading matrix '" + lr.getFileName() + "' stored as VMR...");
    // parse first defs
    string line = getNextNonCommentLine(lr);
    int type = TplConvert<char>::_2int(line.c_str());
    if(vehType=="") {
        vehType = toString(type);
    }
    line = getNextNonCommentLine(lr);
    StringTokenizer st(line, StringTokenizer::WHITECHARS);
    int begin = parseTime(st.next());
    int end = parseTime(st.next());
    line = getNextNonCommentLine(lr);
    SUMOReal factor = TplConvert<char>::_2SUMOReal(line.c_str()) * scale;
    line = getNextNonCommentLine(lr);
    int districtNo = TplConvert<char>::_2int(line.c_str());
    // parse district names (normally ints)
    std::vector<std::string> names;
    line = getNextNonCommentLine(lr);
    do {
        StringTokenizer st2(line, StringTokenizer::WHITECHARS);
        while(st2.hasNext()) {
            names.push_back(st2.next());
        }
        line = lr.readLine();
    } while(line[0]!='*');
    assert(names.size()==districtNo);
    // parse the cells
    for(std::vector<std::string>::iterator si=names.begin(); si!=names.end(); ++si) {
        std::vector<std::string>::iterator di = names.begin();
        //
        line = getNextNonCommentLine(lr);
        do {
            StringTokenizer st2(line, StringTokenizer::WHITECHARS);
            while(st2.hasNext()) {
                assert(di!=names.end());
                float vehNumber = TplConvert<char>::_2SUMOReal(st2.next().c_str()) * factor;
                if(vehNumber!=0) {
                    ODCell *cell = new ODCell();
                    cell->begin = begin;
                    cell->end = end;
                    cell->origin = *si;
                    cell->destination = *di;
                    cell->vehicleType = vehType;
                    cell->vehicleNumber = vehNumber;
                    into.add(cell);
                }
                di++;
            }
            line = lr.readLine();
        } while(line[0]!='*'&&lr.hasMore());
    }
    MsgHandler::getMessageInstance()->endProcessMsg("done.");
}


void
readOR(LineReader &lr, ODMatrix &into, float scale,
       std::string vehType)
{
    MsgHandler::getMessageInstance()->beginProcessMsg("Reading matrix '" + lr.getFileName() + "' stored as OR...");
    // parse first defs
    string line = getNextNonCommentLine(lr);
//    int type = TplConvert<char>::_2int(line.c_str());
//    line = getNextNonCommentLine(lr);
    StringTokenizer st(line, StringTokenizer::WHITECHARS);
    int begin = parseTime(st.next());
    int end = parseTime(st.next());
    line = getNextNonCommentLine(lr);
    SUMOReal factor = TplConvert<char>::_2SUMOReal(line.c_str()) * scale;
    // parse the cells
    while(lr.hasMore()) {
        line = getNextNonCommentLine(lr);
        StringTokenizer st2(line, StringTokenizer::WHITECHARS);
        string sourceD = st2.next();
        string destD = st2.next();
        float vehNumber = TplConvert<char>::_2SUMOReal(st2.next().c_str()) * factor;
        if(vehNumber!=0) {
            ODCell *cell = new ODCell();
            cell->begin = begin;
            cell->end = end;
            cell->origin = sourceD;
            cell->destination = destD;
            cell->vehicleType = vehType;
            cell->vehicleNumber = vehNumber;
            into.add(cell);
        }
    }
    MsgHandler::getMessageInstance()->endProcessMsg("done.");
}


void
loadMatrix(OptionsCont &oc, ODMatrix &into)
{
    std::vector<std::string> files;
    // check whether the filenames shall be read from a vissim file
    if(oc.isSet("vissim")) {
        files = getVissimDynUMLMatrices(oc.getString("vissim"));
    } else {
        string fileS = oc.getString("od-files");
        StringTokenizer st(fileS, ";");
        files = st.getVector();
    }

    // ok, we now should have a list of files to parse
    //  check
    if(files.size()==0) {
        MsgHandler::getErrorInstance()->inform("No files to parse are given.");
        throw ProcessError();
    }
    //  parse
    for(std::vector<std::string>::iterator i=files.begin(); i!=files.end(); ++i) {
        LineReader lr(*i);
        if(!lr.good()) {
            MsgHandler::getErrorInstance()->inform("Could not open '" + (*i) + "'.");
            throw ProcessError();
        }
        string type = lr.readLine();
        // get the type only
        if(type.find(';')!=string::npos) {
            type = type.substr(0, type.find(';'));
        }
        // parse type-dependant
        if(type=="$VMR") {
            readVMR(lr, into, oc.getFloat("scale"), oc.getString("vtype"));
        } else if(type=="$OR") {
            readOR(lr, into, oc.getFloat("scale"), oc.getString("vtype"));
        } else {
            MsgHandler::getErrorInstance()->inform("Unknown matrix type '" + type + "'.");
            throw ProcessError();
        }
    }
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
    int ret = 0;
#ifndef _DEBUG
    try {
#endif
        // initialise subsystems
        int init_ret = SystemFrame::init(false, argc, argv, fillOptions);
        if(init_ret<0) {
            cout << "SUMO od2trips" << endl;
            cout << " (c) DLR/ZAIK 2000-2006; http://sumo.sourceforge.net" << endl;
            switch(init_ret) {
            case -1:
                cout << " Version " << version << endl;
                cout << " Build #" << NEXT_BUILD_NUMBER << endl;
                break;
            case -2:
                HelpPrinter::print(help);
                break;
            default:
                cout << " Use --help to get the list of options." << endl;
            }
            SystemFrame::close();
            return 0;
        } else if(init_ret!=0||!checkOptions(OptionsSubSys::getOptions())) {
            throw ProcessError();
        }
        // retrieve the options
        OptionsCont &oc = OptionsSubSys::getOptions();
        // load the districts
		ODDistrictCont *districts = loadDistricts(oc);
        if(districts==0) {
            MsgHandler::getErrorInstance()->inform("No districts loaded...");
            throw ProcessError();
        }
        // load the matrix
        ODMatrix matrix;
        loadMatrix(oc, matrix);
        MsgHandler::getMessageInstance()->inform(toString(matrix.getNoLoaded()) + " vehicles loaded.");
        // apply a curve if wished
        if(oc.isSet("timeline")) {
            matrix.applyCurve(parseTimeLine(oc.getString("timeline"), oc.getBool("timeline.day-in-hours")));
        }
        // write
        ofstream ostrm(oc.getString("output").c_str());
        if(!ostrm.good()) {
            MsgHandler::getErrorInstance()->inform("Could not open output file '" + oc.getString("output") + "'.");
            throw ProcessError();
        }
        ostrm << "<tripdefs>" << endl;
        matrix.write(oc.getInt("begin"), oc.getInt("end"),
            ostrm, *districts, oc.getBool("spread.uniform"), oc.getString("prefix"));
        ostrm << "</tripdefs>" << endl;
        MsgHandler::getMessageInstance()->inform(toString(matrix.getNoWritten()) + " vehicles written.");
#ifndef _DEBUG
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    }
#endif
    SystemFrame::close();
    if(ret==0) {
        cout << "Success." << endl;
    }
    return ret;
}
