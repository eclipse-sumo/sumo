/***************************************************************************
                         od2trips_main.cpp

	 The main procedure for converting OD an NET files to trip tables

    project              : SUMO		 :
	subproject           : OD2TRIPS
    begin                : Thu, 12 September 2002
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
#include <sax2/XMLReaderFactory.hpp>

#include <sax2/DefaultHandler.hpp>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/SErrorHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/HelpPrinter.h>
#include <utils/xml/XMLSubSys.h>
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
OptionsCont *
getSettings(int argc, char **argv)
{
    OptionsCont *oc = new OptionsCont();
	oc->doRegister("configuration-file", 'c', new Option_FileName("sumo-od2trips.cfg"));
    // register the file i/o options
    oc->doRegister("net-file", 'n', new Option_FileName());
	oc->addSynonyme("net-file", "net");
    oc->doRegister("od-file", 'd', new Option_FileName());
	oc->addSynonyme("od-file", "od");
    oc->doRegister("output-file", 'o', new Option_FileName());
	oc->addSynonyme("output-file", "output");
    // register the report options
    oc->doRegister("verbose", 'v', new Option_Bool(false));
    oc->doRegister("warn", 'w', new Option_Bool(true));
    oc->doRegister("print-options", 'p', new Option_Bool(false));
    oc->doRegister("help", new Option_Bool(false));
	oc->addSynonyme("help", "h");
    // register the data processing options
    oc->doRegister("no-config", 'C', new Option_Bool(false));
    oc->addSynonyme("no-config", "no-configuration");
    oc->doRegister("begin", 'b', new Option_Long(0));
    oc->doRegister("end", 'e', new Option_Long(86400));
    oc->doRegister("scale", 's', new Option_Float(1));
    // parse the command line arguments and configuration the file
    bool ok = OptionsIO::getOptions(oc, argc, argv);
    if(!ok||SErrorHandler::errorOccured()) {
        delete oc;
        return 0;
    }
    return oc;
}


ODDistrictCont *
loadDistricts(OptionsCont &oc)
{
    // check whether the user gave a net filename
    if(!oc.isSet("n")) {
        SErrorHandler::add("You must supply a network ('-n').");
        return 0;
    }
    // build the container
    ODDistrictCont *ret = new ODDistrictCont();
    // build the xml-parser and handler
    SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
    parser->setFeature(
        XMLString::transcode("http://xml.org/sax/features/validation"),
        false);
    ODDistrictHandler *handler = new ODDistrictHandler(*ret,
        oc.getBool("warn"), oc.getBool("verbose"));
    // initialise parser with the handler
    parser->setContentHandler(handler);
    parser->setErrorHandler(handler);
    // get the file name and set it
    string file = oc.getString("n");
    handler->setFileName(file);
    parser->parse(file.c_str());
    // delete alocated structures
    delete parser;
    delete handler;
    // check whether the loading was ok
	if(ret->size()==0||SErrorHandler::errorOccured()) {
        delete ret;
        return 0;
    }

    return ret;
}

int cmpfun(int a, int b)
{
	if (a > b)
		return 1;
	else if (a < b)
		return -1;
	else
		return 0;
}

/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */

int
main(int argc, char **argv)
{
    int ret = 0;
    try {
        // try to initialise the XML-subsystem
        if(!XMLSubSys::init()) {
            return 1;
        }
        // parse the settings
        OptionsCont *oc = getSettings(argc, argv);
        if(oc==0) {
            cout << "Quitting." << endl;
            return 1;
        }
        // check whether only the help shall be printed
        if(oc->getBool("help")) {
            HelpPrinter::print(help);
            delete oc;
            return 0;
        }
		if(oc->getBool("p"))
            cout << *oc;
	    bool ok = true;
		if(!oc->isSet("n")) {
			cerr << "Error: No net input file (-n) specified." << endl;
			ok = false;
		}
		if(!oc->isSet("d")) {
			cerr << "Error: No OD input file (-d) specified." << endl;
			ok = false;
		}
		string OD_filename = oc->getString("d");
		if(!oc->isSet("o")) {
			cerr << "Error: No trip table output file (-o) specified." << endl;
			ok = false;
		}
        if(!ok) {
            return 1;
        }
		string OD_outfile = oc->getString("o");
		bool verbose = oc->getBool("v");
		int maxele=50000; // initial number of ODs, finally derived from OD-inputfile
		int total_cars=0;  // total number of cars, finally derived from OD-inputfile
		int i, j;
		// OD list
		vector<OD_IN> od_in;
        od_in.reserve(maxele);
		float factor;
		int start, finish;
		bool ok_begin = oc->isDefault("begin");
		bool ok_end = oc->isDefault("end");
		// Reads the OD Matrix
	    ODread ( OD_filename, od_in, &maxele, &total_cars, &start, &finish, &factor );
		// use settings if provided
		if(ok_begin) start = oc->getLong("begin");
		if(ok_end) finish = oc->getLong("end");
		// check permitted range
		if( (start>finish) || (start<0) || (finish>86400) ) {
			std::cout << "Wrong time range (begin/end)" << endl;
			throw ProcessError();
		}
		const int period = finish - start;
        // scale input
        double scale = oc->getFloat("scale");
		scale = scale / factor;
        vector<OD_IN>::iterator it1;
		total_cars = 0;
        for(it1=od_in.begin(); it1!=od_in.end(); it1++) {
            total_cars += (*it1).how_many;
        }
		if(verbose) {
			std::cout << "Total number of cars desired: "<< int(total_cars/scale) << endl;
		}
		total_cars=0;
		double rest;
        for(it1=od_in.begin(); it1!=od_in.end(); it1++) {
			rest = fmod ( double((*it1).how_many), scale) / scale;
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
		if(verbose) {
			std::cout << "Total number of cars computed: "<< total_cars << endl;
		}
		// define dimensions
		std::vector<OD_OUT> source_sink(total_cars); // output OD data
		// temporary storage space
		int *when_all = new int [total_cars];
		string *source = new string [total_cars];
		string *sink = 	new string [total_cars];
		int *old_index= new int [total_cars]; // holds the old index after sorting
		int *when = new int [period];
		int *elements = new int [period];
		// initial order for index
		for(i=0;i<total_cars;i++) *(old_index+i)=i;
		if(verbose) {
			std::cout << "Number of OD elements: "<< maxele << "; Total number of cars: ";
			std::cout << total_cars << endl;
		}
		// temporary variables with initial values
		int begin=0;
		int end=0;
		int index;
		bool ini=true; // initialize random numbers with time, only first call
		for(i=0;i<period;i++) *(when+i)=0;
		// load districts
		ODDistrictCont *districts = loadDistricts(*oc);
		 // loop over all ODs
		for(i=0;i<maxele;i++) {
            OD_IN tmp = od_in[i];
			end = od_in[i].how_many;
            if(end!=0) {
			    // Gets random numbers which are the release times
			    // for the cars
			    end = Get_rand( end, period, elements, when, ini);
			    ini = false;
			    end = begin+end;
			    for(j=begin;j<end;j++)
			    {
				    // find dsource, dsink
				    *(source+j) = districts->getRandomSourceFromDistrict(od_in[i].from);
				    *(sink+j) = districts->getRandomSinkFromDistrict(od_in[i].to);
				    *(when_all+j) = *(when+j-begin);
			    }
			    begin=end;
            }
		}
        total_cars = begin;
		if(verbose) {
			std::cout << "Final total number of cars: "<< begin << endl;
		}
		// sorts the time
		IndexSort(when_all, old_index, cmpfun, total_cars);
		for(i=0;i<total_cars;i++)
		{
			index = old_index[i];
			source_sink[i].from = *(source+index);
			source_sink[i].to = *(sink+index);
			source_sink[i].time = *(when_all+i);
		}
		// writes output to file
		ODwrite( OD_outfile, source_sink, total_cars );
		delete [] source; delete [] sink;
		delete [] when_all; delete [] elements;
		delete [] when; delete [] old_index;
		delete oc;
        if(verbose) {
            cout << "Success." <<endl;
        }
    } catch (ProcessError) {
        ret = 1;
    }
    XMLSubSys::close();
    return ret;
}


