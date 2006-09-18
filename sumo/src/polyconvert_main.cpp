/***************************************************************************
                          polyconvert_main.cpp
			  The main procedure for the conversion /
			  building of polygons
                             -------------------
    project              : SUMO
    subproject           : PolyConvert
    begin                : Mon, 05 Dec 2005
    copyright            : (C) 2005 by DLR http://ivf.dlr.de/
    author               : Danilo Boyom
    email                : Danilot.Tete-Boyom@dlr.de
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
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.2  2006/09/18 10:19:29  dkrajzew
// changed the way geocoordinates are processed
//
// Revision 1.1  2006/08/01 07:20:32  dkrajzew
// polyconvert added
//
// Revision 1.6  2006/02/13 07:34:16  dksumo
// ending work on polyconvert
//
// Revision 1.5  2006/01/09 09:23:11  dksumo
// patched error handling
//
// Revision 1.4  2005/12/12 11:58:14  dksumo
// help output patched; cheanges due to correct the output
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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
#include <string>
#include <fstream>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/HelpPrinter.h>
#include <utils/importio/LineReader.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/geom/Boundary.h>
#include "polyconvert_help.h"
#include "sumo_version.h"
#include <polyconvert/PCVisum.h>
#include <polyconvert/PCElmar.h>
#include <polyconvert/PCTypeMap.h>
#include <polyconvert/PCTypeDefHandler.h>
#include <utils/common/XMLHelpers.h>
#include <utils/common/RandHelper.h>
#include <utils/geoconv/GeoConvHelper.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
fillOptions(OptionsCont &oc)
{
    // add process options
    oc.doRegister("configuration-file", 'c', new Option_FileName());
    oc.addSynonyme("configuration-file", "configuration");
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.doRegister("help", new Option_Bool(false));
    oc.doRegister("log-file", 'l', new Option_FileName());

    // add i/o options
        // original network
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
        // elmar import
	oc.doRegister("elmar", new Option_FileName());
        // visum import
    oc.doRegister("visum-file", new Option_FileName());
    oc.addSynonyme("visum-file", "visum");
        // typemap reading
    oc.doRegister("typemap", new Option_FileName());
        // output
    oc.doRegister("output", 'o', new Option_FileName("polygons.xml"));

    // projection options
    oc.doRegister("use-projection", new Option_Bool(false));
    oc.doRegister("proj.simple", new Option_Bool(false));
    oc.doRegister("proj", new Option_String());

    // default values
    oc.doRegister("color", new Option_String("0.2,0.5,1."));
    oc.doRegister("prefix", new Option_String(""));
    oc.doRegister("type", new Option_String("unknown"));
    oc.doRegister("layer", new Option_Integer(-1));

    // random initialisation (not used!!!)
    RandHelper::insertRandOptions(oc);
}


Boundary
getNetworkOrigBoundary(const std::string &file)
{
    LineReader lr(file);
    if(!lr.good()) {
        MsgHandler::getErrorInstance()->inform("Could not open net '" + file + "'.");
        throw ProcessError();
    }
    while(lr.hasMore()) {
        string line = lr.readLine();
        if(line.find("<orig-boundary>")!=string::npos) {
            size_t beg = line.find('>');
            size_t end = line.find('<', beg);
            string my = line.substr(beg+1, end-beg-1);
            return GeomConvHelper::parseBoundary(my);
        }
    }
    MsgHandler::getErrorInstance()->inform("Could not find projection description in net.");
    throw ProcessError();
}


Position2D
getNetworkOffset(const std::string &file)
{
    LineReader lr(file);
    if(!lr.good()) {
        MsgHandler::getErrorInstance()->inform("Could not open net '" + file + "'.");
        throw ProcessError();
    }
    while(lr.hasMore()) {
        string line = lr.readLine();
        if(line.find("<net-offset>")!=string::npos) {
            size_t beg = line.find('>');
            size_t end = line.find('<', beg);
            string my = line.substr(beg+1, end-beg-1);
            return GeomConvHelper::parseShape(my)[0];
        }
    }
    MsgHandler::getErrorInstance()->inform("Could not find projection description in net.");
    throw ProcessError();
}


std::string
getOrigProj(const std::string &file)
{
    LineReader lr(file);
    if(!lr.good()) {
        MsgHandler::getErrorInstance()->inform("Could not open net '" + file + "'.");
        throw ProcessError();
    }
    while(lr.hasMore()) {
        string line = lr.readLine();
        if(line.find("<orig-proj>")!=string::npos) {
            size_t beg = line.find('>');
            size_t end = line.find('<', beg);
            return line.substr(beg+1, end-beg-1);
        }
    }
    MsgHandler::getErrorInstance()->inform("Could not find projection description in net.");
    throw ProcessError();
}


int
main(int argc, char **argv)
{
    int ret = 0;
#ifndef _DEBUG
    try {
#endif
        int init_ret = SystemFrame::init(false, argc, argv, fillOptions, 0);
        if(init_ret<0) {
            cout << "SUMO polyconvert" << endl;
            cout << " (c) DLR/ZAIK 2000-2006; http://sumo.sourceforge.net" << endl;
            cout << " Version " << version << endl;
            switch(init_ret) {
            case -2:
                HelpPrinter::print(help);
                break;
            default:
                cout << " Use --help to get the list of options." << endl;
            }
            SystemFrame::close();
            return 0;
        } else if(init_ret!=0) {
            throw ProcessError();
        }
        // retrieve the options // gibt die Options aus der container zurück
        OptionsCont &oc = OptionsSubSys::getOptions();

		PCPolyContainer toFill;

        // build the projection
        Boundary origNetBoundary;
        Position2D netOffset;
        string proj;

        if(!oc.getBool("use-projection")) {
            GeoConvHelper::init("!", Position2D());
        } else if(oc.getBool("proj.simple")) {
            GeoConvHelper::init("-", Position2D());
        } else {
            if(oc.isSet("net")) {
                origNetBoundary = getNetworkOrigBoundary(oc.getString("net"));
                netOffset = getNetworkOffset(oc.getString("net"));
                proj = getOrigProj(oc.getString("net"));
            }
            if(oc.isSet("proj")) {
                proj = oc.getString("proj");
            }
            if(!GeoConvHelper::init(proj, netOffset)) {
                MsgHandler::getErrorInstance()->inform("Could not build projection!");
                throw ProcessError();
            }
        }

        // read in the type defaults
        PCTypeMap tm;
        if(oc.isSet("typemap")) {
            PCTypeDefHandler handler(oc, tm);
            XMLHelpers::runParser(handler, oc.getString("typemap"));
        }

        // read in the data
        if(oc.isSet("elmar")) {
            // elmars
            PCElmar pce(toFill, origNetBoundary, netOffset, tm);
            pce.loadElmar(oc);
        }
        if(oc.isSet("visum-file")) {
            // visum
		    PCVisum pcv(toFill);
            pcv.loadVisum(oc);
        }

        // check whether any errors occured
        if(!MsgHandler::getErrorInstance()->wasInformed()) {
            // no? ok, save
            toFill.save(oc.getString("output"), oc.getInt("layer"));
        } else {
            throw ProcessError();
        }
#ifndef _DEBUG
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (conversion failed).", false);
        ret = 1;
    }
#endif
    SystemFrame::close();
    // report about ending
    if(ret==0) {
        cout << "Success." << endl;
    }
    return ret;
}


