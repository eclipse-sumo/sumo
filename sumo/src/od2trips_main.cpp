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
#include <utils/convert/ToString.h>
#include <utils/common/XMLHelpers.h>
#include <od2trips/ODDistrictCont.h>
#include <od2trips/ODDistrictHandler.h>
#include "od2trips_help.h"
#include <od2trips/ODmatrix.h>
#include <od2trips/ODsubroutines.h>


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
    // register the file i/o options
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.doRegister("od-file", 'd', new Option_FileName());
    oc.addSynonyme("od-file", "od");
    oc.doRegister("output-file", 'o', new Option_FileName());
    oc.addSynonyme("output-file", "output");
    oc.doRegister("od-path", 'i', new Option_FileName());
    oc.addSynonyme("od-path", "path");
    // register the report options
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.doRegister("help", '?', new Option_Bool(false));
    oc.doRegister("log-file", 'l', new Option_FileName());
    // register the data processing options
    oc.doRegister("begin", 'b', new Option_Long(0));
    oc.doRegister("end", 'e', new Option_Long(86400));
    oc.doRegister("scale", 's', new Option_Float(1));
    oc.doRegister("no-color", new Option_Bool(false));
    RandHelper::insertRandOptions(oc);
}


bool
checkOptions(OptionsCont &oc)
{
    bool ok = true;
    if(!oc.isSet("n")) {
        MsgHandler::getErrorInstance()->inform(
            "No net input file (-n) specified.");
        ok = false;
    }
    if(!oc.isSet("d")) {
        MsgHandler::getErrorInstance()->inform(
            "No OD input file (-d) specified.");
        ok = false;
    }
    if(!oc.isSet("o")) {
        MsgHandler::getErrorInstance()->inform(
            "No trip table output file (-o) specified.");
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
    // build the xml-parser and handler
    ODDistrictHandler handler(*ret);
    handler.setFileName(file);
    XMLHelpers::runParser(handler, file);
    // check whether the loading was ok
    if(ret->size()==0||MsgHandler::getErrorInstance()->wasInformed()) {
        delete ret;
        return 0;
    }
    ret->colorize();
    return ret;
}


int
cmpfun(long int a, long int b)
{
    if (a > b)
        return 1;
    else if (a < b)
        return -1;
    else
        return 0;
}


int
getFMatType(const std::string &file)
{
    if (file.find(".inp") != string::npos) {
        return 1; // new format: vissim type
    }
    if (file.find(".fma") != string::npos) {
        return 2; // old fma format
    }
    MsgHandler::getErrorInstance()->inform("Error: Wrong od-file in *.cfg!");
    throw ProcessError();
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */

int
main(int argc, char **argv)
{
    int ret = 0;
    try {
        // initialise subsystems
        if(!SystemFrame::init(false, argc, argv,
            fillOptions, checkOptions, help)) {
            throw ProcessError();
        }
        // retrieve the options
        OptionsCont &oc = OptionsSubSys::getOptions();
        string OD_filename = oc.getString("d");
        //
        int fmatype = getFMatType(OD_filename);
        string OD_outfile = oc.getString("o");
        string OD_path = oc.getString("i");
        std::vector<ODContent> content;
        std::vector<std::string> infiles;
        // reads out some metadata from *.inp
        if( fmatype == 1) {
            WRITE_MESSAGE("**** VISSIM input data format **** ");
            ODInpread (OD_filename, infiles, content/*,&maxfiles*/);
        } else {
            infiles.push_back(OD_filename);
            WRITE_MESSAGE("**** Original input data format **** ");
        }
        long maxele=50000; // initial number of ODs, finally derived from OD-inputfile
        long int total_cars=0;  // total number of cars, finally derived from OD-inputfile
        long int i, j;
        float factor;
        long int  start, finish;
        bool ok_begin = oc.isDefault("begin");
        bool ok_end = oc.isDefault("end");
        // load districts
        ODDistrictCont *districts = loadDistricts(oc);
        if(districts==0) {
            MsgHandler::getErrorInstance()->inform("No districts loaded...");
            throw ProcessError();
        }
        int od_next=0;
        // define dimensions
        long int max_cars=3000000; // maximum number of cars
        // temporary storage space
        long int *when_all = new long int [max_cars];
        string *source = new string [max_cars];
        string *sink =  new string [max_cars];
        int *cartype =  new int [max_cars];
        long int *old_index= new long int [max_cars]; // holds the old index after sorting
        int index, tmpk, k;
        long int begin, end, period;
        bool ini;
        double tmprand, maxrand;
        float scale, rest;
        ini=true; // initialize random numbers with time, only first call
        for (size_t ifile=0;ifile<infiles.size();ifile++) { // proceed for all *.fma files
            // OD list
            vector<OD_IN> od_in;
            vector<OD_IN>::iterator it1;
            if (fmatype == 1) {
                OD_filename = OD_path + infiles[ifile];
                WRITE_MESSAGE("Processing " + OD_filename);
                // Reads the OD Matrix
                ODPtvread ( OD_filename, od_in, &maxele, &total_cars, &start, &finish, &factor );
                // use settings if provided
            } else {
                ODPtvread ( OD_filename, od_in, &maxele, &total_cars, &start, &finish, &factor );
//              ODread ( OD_filename, od_in, &maxele, &total_cars, &start, &finish, &factor );
            }
            //
            if(ok_begin) {
                if(oc.getLong("begin") > start) start = oc.getLong("begin");
            }
            if(ok_end) {
                if(oc.getLong("end") < finish) finish = oc.getLong("end");
            }
            // check permitted range
            if( (start>finish) || (start<0) || (finish>86400) ) {
                MsgHandler::getErrorInstance()->inform(
                    "Wrong time range (begin/end)");
                throw ProcessError();
            }
            period = finish - start;
            // scale input
            scale = oc.getFloat("scale");
            scale = scale / factor;
            total_cars=0;
            for(it1=od_in.begin(); it1!=od_in.end(); it1++) {
                rest = (float) (fmod ( double((*it1).how_many), double(scale)) / double(scale));
                if(rest <= 0.5)
                    (*it1).how_many = int(double((*it1).how_many) / scale);
                else
                    (*it1).how_many = int(double((*it1).how_many) / scale)+1;
            }
            // recompute total_cars
            total_cars = 0;
            for(it1=od_in.begin(); it1!=od_in.end(); it1++) {
                total_cars += (*it1).how_many;
            }
            if(fmatype == 2) {
                WRITE_MESSAGE(string("Total number of cars computed: ")+ toString<long int>(total_cars));
            }
            int *when = new int [period];
            int *elements = new int [period];
            // temporary variables with initial values
            begin=0;
            end=0;
            for(i=0;i<period;i++) *(when+i)=0;
            // loop over all ODs
            for(i=0;i<maxele;i++) {
                //OD_IN tmp = od_in[i];
                end = od_in[i].how_many;
                if(end!=0) {
                    // Gets random numbers which are the release times for the cars
                    end = Get_rand( end, period, start, elements, when, ini);
                    ini = false;
                    end = begin+end;
                    for(j=begin;j<end;j++)
                    {
                        *(old_index+j+od_next)=j+od_next; // initial order for index
                        // find dsource, dsink
                        //string pp=od_in[i].from;
                        //string p2=od_in[i].to;
                        *(source+j+od_next) = od_in[i].from;//
                        *(sink+j+od_next) = od_in[i].to;//
                        *(when_all+j+od_next) = *(when+j-begin);
                        // determine car type for VISSIM format
                        if (fmatype == 1) {
                            maxrand=0.;
                            for (k=0;k<content[ifile].max;k++) {
                                tmprand= rand() * content[ifile].fraction[k];
                                    if (maxrand<tmprand) {
                                        maxrand=tmprand;
                                        tmpk=k;
                                    }
                            }
                            *(cartype+j+od_next) = content[ifile].cartype[tmpk];
                        } else *(cartype+j+od_next) = 0; // cartype=0 for old format
                    }
                    begin=end;
                }
            }
            total_cars = begin;
            od_next = od_next + total_cars;
            delete [] when; delete [] elements;
        }
        total_cars = od_next;
        WRITE_MESSAGE(string("Total number of released cars: ") + toString<int>(total_cars));
        // sorts the time
        WRITE_MESSAGE("Sorting ...");
        IndexSort(when_all, old_index, cmpfun, total_cars);
        std::vector<OD_OUT> source_sink(max_cars); // output OD data
        for(i=0;i<total_cars;i++)
        {
            index = old_index[i];
            source_sink[i].from = *(source+index);
            source_sink[i].to = *(sink+index);
            source_sink[i].time = *(when_all+i);
            source_sink[i].type = *(cartype+index);
        }
        // writes output to file
        ODWrite( OD_outfile, source_sink, total_cars, *districts );
        delete [] source; delete [] sink;
        delete [] when_all; delete [] cartype;
        WRITE_MESSAGE("Success.");
    } catch (...) {
        MsgHandler::getMessageInstance()->inform(
            "Quitting (on error).");
        ret = 1;
    }
    SystemFrame::close();
    if(ret==0) {
        WRITE_MESSAGE("Success.");
    }
    return ret;
}
