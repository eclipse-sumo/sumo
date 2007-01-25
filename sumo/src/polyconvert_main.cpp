/***************************************************************************
                          polyconvert_main.cpp
    The main procedure for the conversion building of polygons
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
// $Log: polyconvert_main.cpp,v $
// Revision 1.6  2007/01/09 14:43:06  dkrajzew
// added missing changes (was: Visum point import)
//
// Revision 1.5  2006/11/28 14:51:48  dkrajzew
// possibility to prune the plygons to import on a bounding box added
//
// Revision 1.4  2006/11/20 11:11:33  dkrajzew
// bug [ 1598346 ] (Versioning information in many places) patched - Version number is now read from windows_config.h/config.h
//
// Revision 1.3  2006/11/02 12:19:50  dkrajzew
// added parsing of Elmar's pointcollections
//
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
#include <utils/importio/LineReader.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/geom/Boundary.h>
#include <polyconvert/PCVisum.h>
#include <polyconvert/PCVisumPoints.h>
#include <polyconvert/PCElmar.h>
#include <polyconvert/PCElmarPoints.h>
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
    oc.setApplicationDescription("Importer of polygons and POIs for the road traffic simulation SUMO.");
#ifdef WIN32
    oc.setApplicationName("polyconvert.exe");
#else
    oc.setApplicationName("sumo-polyconvert");
#endif
    oc.addCallExample("-c <CONFIGURATION>");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("Projection");
    oc.addOptionSubTopic("Prunning");
    oc.addOptionSubTopic("Building Defaults");
    oc.addOptionSubTopic("Report");


    // register options
        // add i/o options
            // original network
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");
    oc.addDescription("net-file", "Input", "Loads SUMO-network FILE as reference to offset and projection");

            // elmar import
	oc.doRegister("elmar", new Option_FileName());
    oc.addDescription("elmar", "Input", "Reads polygons from FILE assuming they're coded in Elmar-format");

    oc.doRegister("elmar-points", new Option_FileName());
    oc.addDescription("elmar-points", "Input", "Reads pois from FILE assuming they're coded in Elmar-format");

            // visum import
    oc.doRegister("visum-file", new Option_FileName());
    oc.addSynonyme("visum-file", "visum");
    oc.addDescription("visum-file", "Input", "Reads polygons from FILE assuming it's a Visum-net");

    oc.doRegister("visum-points", new Option_FileName());
    oc.addDescription("visum-points", "Input", "Reads pois from FILE assuming it's a Visum-net");

            // typemap reading
    oc.doRegister("typemap", new Option_FileName());
    oc.addDescription("typemap", "Input", "Reads types from FILE");

        // output
    oc.doRegister("output", 'o', new Option_FileName("polygons.xml"));
    oc.addDescription("output", "Output", "Write generated polygons/pois to FILE");


        // projection options
    oc.addOptionSubTopic("Projection");

    oc.doRegister("use-projection", new Option_Bool(false));
    oc.addDescription("use-projection", "Projection", "Enables reprojection from geo to cartesian");

    oc.doRegister("proj.simple", new Option_Bool(false));
    oc.addDescription("proj.simple", "Projection", "Uses a simple method for projection");

    oc.doRegister("proj", new Option_String());
    oc.addDescription("proj", "Projection", "Uses STR as proj.4 definition for projection");


        // prunning options
    oc.addOptionSubTopic("Prunning");

    oc.doRegister("prune.on-net", new Option_Bool(false));
    oc.addDescription("prune.on-net", "Prunning", "Enables prunning on net boundaries");

    oc.doRegister("prune.on-net.offsets", new Option_String("0;0;0;0"));
    oc.addDescription("prune.on-net.offsets", "Prunning", "Uses STR as offset definition added to the net boundaries");

    oc.doRegister("prune.boundary", new Option_String());
    oc.addDescription("prune.boundary", "Prunning", "Uses STR as prunning boundary");


        // building defaults options
    oc.addOptionSubTopic("Building Defaults");

    oc.doRegister("color", new Option_String("0.2,0.5,1."));
    oc.addDescription("color", "Building Defaults", "Sets STR as default color");

    oc.doRegister("prefix", new Option_String(""));
    oc.addDescription("prefix", "Building Defaults", "Sets STR as default prefix");
    
    oc.doRegister("type", new Option_String("unknown"));
    oc.addDescription("type", "Building Defaults", "Sets STR as default type");
    
    oc.doRegister("layer", new Option_Integer(-1));
    oc.addDescription("layer", "Building Defaults", "Sets INT as default layer");


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


    // random initialisation (not used!!!)
    RandHelper::insertRandOptions(oc);
}


Boundary
getNamedNetworkBoundary(const std::string &file, const std::string &name)
{
    LineReader lr(file);
    if(!lr.good()) {
        MsgHandler::getErrorInstance()->inform("Could not open net '" + file + "'.");
        throw ProcessError();
    }
    while(lr.hasMore()) {
        string line = lr.readLine();
        if(line.find("<" + name + ">")!=string::npos) {
            size_t beg = line.find('>');
            size_t end = line.find('<', beg);
            string my = line.substr(beg+1, end-beg-1);
            return GeomConvHelper::parseBoundary(my);
        }
    }
    throw ProcessError();
}


Boundary
getNetworkOrigBoundary(const std::string &file)
{
    try {
        return getNamedNetworkBoundary(file, "orig-boundary");
    } catch (ProcessError &) {
    }
    MsgHandler::getErrorInstance()->inform("Could not find the original boundary in net.");
    throw ProcessError();
}


Boundary
getNetworkConvBoundary(const std::string &file)
{
    try {
        return getNamedNetworkBoundary(file, "conv-boundary");
    } catch (ProcessError &) {
    }
    MsgHandler::getErrorInstance()->inform("Could not find the converted boundary in net.");
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
            cout << " (c) DLR/ZAIK 2000-2007; http://sumo.sourceforge.net" << endl;
            cout << " Version " << VERSION << endl;
            switch(init_ret) {
            case -2:
                OptionsSubSys::getOptions().printHelp(cout);
                break;
            case -4:
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

        // check whether the input shall be prunned
        bool prune = false;
        Boundary prunningBoundary;
        if(oc.getBool("prune.on-net")) {
            if(!oc.isSet("net")) {
                MsgHandler::getErrorInstance()->inform("In order to prune the input on the net, you have to supply a network.");
                throw ProcessError();
            }
            prunningBoundary = getNetworkConvBoundary(oc.getString("net"));
            Boundary offsets = GeomConvHelper::parseBoundary(oc.getString("prune.on-net.offsets"));
            prunningBoundary = Boundary(
                prunningBoundary.xmin()+offsets.xmin(),
                prunningBoundary.ymin()+offsets.ymin(),
                prunningBoundary.xmax()+offsets.xmax(),
                prunningBoundary.ymax()+offsets.ymax());
            prune = true;
        }
        if(oc.isSet("prune.boundary")) {
            prunningBoundary = GeomConvHelper::parseBoundary(oc.getString("prune.boundary"));
            prune = true;
        }

		PCPolyContainer toFill(prune, prunningBoundary);

        // read in the type defaults
        PCTypeMap tm;
        if(oc.isSet("typemap")) {
            PCTypeDefHandler handler(oc, tm);
            XMLHelpers::runParser(handler, oc.getString("typemap"));
        }

        // read in the data
            // elmar's polygons
        if(oc.isSet("elmar")) {
            PCElmar pce(toFill, origNetBoundary, tm);
            pce.load(oc);
        }
            // elmar's points
        if(oc.isSet("elmar-points")) {
            PCElmarPoints pce(toFill, origNetBoundary, tm);
            pce.load(oc);
        }
            // visum
        if(oc.isSet("visum-file")) {
		    PCVisum pcv(toFill);
            pcv.load(oc);
        }
            // visum-points
        if(oc.isSet("visum-points")) {
		    PCVisumPoints pcv(toFill, origNetBoundary, tm);
            pcv.load(oc);
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


