/***************************************************************************
                          NetElements.cpp
                             -------------------
    project              : SUMO
    begin                : Mar, 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
    author               : Markus Hartinger
    email                : Markus.Hartinger@dlr.de
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
// Revision 1.2  2003/07/17 12:14:21  dkrajzew
// NBLoader moved to netimport; NBNetBuilder performs now all the building steps
//
// Revision 1.1  2003/07/16 15:41:17  dkrajzew
// network generator added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <fstream>
#include <string>
#include <netbuild/NBNetBuilder.h>
#include <netgen/NGNet.h>
#include <netgen/NGRandomNet.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/UtilExceptions.h>
#include "netgen_help.h"


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#ifdef WIN32
   #include <utils/dev/MemDiff.h>
#endif
#endif



/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
bool
checkOptions(OptionsCont &oc)
{
    // check whether the output is valid and can be build
    if(!oc.isSet("o")) {
        MsgHandler::getErrorInstance()->inform("No output specified.");
        return false;
    }
    std::ofstream tst(oc.getString("o").c_str());
    if(!tst.good()) {
        MsgHandler::getErrorInstance()->inform(
            string("The output file '") + oc.getString("o")
            + string("' can not be build."));
        return false;
    }
    // check whether exactly one type of a network to build was wished
    int no = 0;
    if(oc.getBool("s")) no++;
    if(oc.getBool("g")) no++;
    if(oc.getBool("r")) no++;
    if(no==0) {
        MsgHandler::getErrorInstance()->inform(
            "You have to specify the type of network to generate.");
        return false;
    }
    if(no>1) {
        MsgHandler::getErrorInstance()->inform(
            "You may specify only one type of network to generate at once.");
        return false;
    }
    //
    return true;
}


/** build and retrieve the options (settings) */
void
fillOptions(OptionsCont &oc)
{
    // register the file i/o options
    oc.doRegister("random-net", 'n', new Option_Bool(false));
    oc.doRegister("spider-net", 's', new Option_Bool(false));
    oc.doRegister("grid-net", 'g', new Option_Bool(false));
    oc.doRegister("output", 'o', new Option_FileName());
    oc.doRegister("configuration-file", 'c', new Option_FileName());
    oc.addSynonyme("random-net", "random");
    oc.addSynonyme("spider-net", "spider");
    oc.addSynonyme("grid-net", "grid");
    oc.addSynonyme("output-file", "output");
    oc.addSynonyme("configuration-file", "configuration");
    // regsister random-net options
    oc.doRegister("rand-max-distance", new Option_Float(250));
    oc.doRegister("rand-min-distance", new Option_Float(100));
    oc.doRegister("rand-min-angle", new Option_Float(45.0/180.0*PI));
    oc.doRegister("rand-num-tries", new Option_Float(50));
    oc.doRegister("rand-connectivity", new Option_Float(0.95));
    oc.doRegister("rand-neighbor-dist1", new Option_Float(0));
    oc.doRegister("rand-neighbor-dist2", new Option_Float(0));
    oc.doRegister("rand-neighbor-dist3", new Option_Float(10));
    oc.doRegister("rand-neighbor-dist4", new Option_Float(10));
    oc.doRegister("rand-neighbor-dist5", new Option_Float(2));
    oc.doRegister("rand-neighbor-dist6", new Option_Float(1));
    oc.doRegister("rand-iterations", new Option_Integer(2000));
    oc.addSynonyme("rand-max-distance", "max-dist");
    oc.addSynonyme("rand-min-distance", "min-dist");
    oc.addSynonyme("rand-min-angle", "min-angle");
    oc.addSynonyme("rand-num-tries", "num-tries");
    oc.addSynonyme("rand-connectivity", "connectivity");
    oc.addSynonyme("rand-neighbor-dist1", "dist1");
    oc.addSynonyme("rand-neighbor-dist2", "dist2");
    oc.addSynonyme("rand-neighbor-dist3", "dist3");
    oc.addSynonyme("rand-neighbor-dist4", "dist4");
    oc.addSynonyme("rand-neighbor-dist5", "dist5");
    oc.addSynonyme("rand-neighbor-dist6", "dist6");
    oc.addSynonyme("rand-iterations", "iterations");
    // register spider-net options
    oc.doRegister("spider-no-arms", new Option_Integer(13));
    oc.doRegister("spider-no-circles", new Option_Integer(20));
    oc.doRegister("spider-space-rad", new Option_Float(100));
    oc.addSynonyme("spider-no-arms", "no-arms");
    oc.addSynonyme("spider-no-circles", "no-circles");
    oc.addSynonyme("spider-space-rad", "space-rad");
    // register grid-net options
    oc.doRegister("grid-x-number", new Option_Integer(5));
    oc.doRegister("grid-y-number", new Option_Integer(5));
    oc.doRegister("grid-x-length", new Option_Float(100));
    oc.doRegister("grid-y-length", new Option_Float(100));
    oc.doRegister("grid-number", new Option_Integer(5));
    oc.doRegister("grid-length", new Option_Float(100));
    oc.addSynonyme("grid-x-number", "x-no");
    oc.addSynonyme("grid-y-number", "y-no");
    oc.addSynonyme("grid-x-length", "x-length");
    oc.addSynonyme("grid-y-length", "y-length");
    oc.addSynonyme("grid-length", "length");
    oc.addSynonyme("grid-number", "number");
    // register the report options
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.doRegister("continue-on-unbuild", new Option_Bool(false));
    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.doRegister("help", new Option_Bool(false));
    oc.doRegister("log-file", 'l', new Option_FileName());
}


TNGNet *
buildNetwork()
{
	TNGNet *net = new TNGNet();
    OptionsCont &oc = OptionsSubSys::getOptions();
    // spider-net
    if(oc.getBool("s")) {
        net->CreateSpiderWeb(
            oc.getInt("no-circles"),
            oc.getInt("no-arns"),
            oc.getFloat("space-rad"));
        return net;
    }
    // grid-net
    if(oc.getBool("g")) {
        int xNo = oc.getInt("x-no");
        int yNo = oc.getInt("y-no");
        float xLength = oc.getFloat("x-length");
        float yLength = oc.getFloat("y-length");
        if(oc.isDefault("x-no")&&!oc.isDefault("number")) {
            xNo = oc.getInt("number");
        }
        if(oc.isDefault("y-no")&&!oc.isDefault("number")) {
            yNo = oc.getInt("number");
        }
        if(oc.isDefault("x-length")&&!oc.isDefault("length")) {
            xLength = oc.getInt("length");
        }
        if(oc.isDefault("y-length")&&!oc.isDefault("length")) {
            yLength = oc.getInt("length");
        }
        net->CreateChequerBoard(xNo, yNo, xLength, yLength);
        return net;
    }
    // random net
	TNGRandomNet RandomNet(net);
	RandomNet.SetMaxDistance(oc.getFloat("max-distance"));
	RandomNet.SetMinDistance(oc.getFloat("min-distance"));
	RandomNet.SetMinLinkAngle(oc.getFloat("min-angle"));
	RandomNet.SetNumTries(oc.getFloat("num-tries"));
	RandomNet.SetConnectivity(oc.getFloat("connectivity"));
    RandomNet.NeighbourDistribution.Add(1, oc.getFloat("dist1"));
    RandomNet.NeighbourDistribution.Add(2, oc.getFloat("dist2"));
    RandomNet.NeighbourDistribution.Add(3, oc.getFloat("dist3"));
	RandomNet.NeighbourDistribution.Add(4, oc.getFloat("dist4"));
	RandomNet.NeighbourDistribution.Add(5, oc.getFloat("dist5"));
	RandomNet.NeighbourDistribution.Add(6, oc.getFloat("dist6"));
	RandomNet.CreateNet(oc.getInt("iterations"));
	return net;
}



int
main(int argc, char **argv)
{
#ifdef _DEBUG
#ifdef WIN32
    CMemDiff state1;
    // uncomment next line and insert the context of an undeleted
    //  allocation to break within it (MSVC++ only)
    // _CrtSetBreakAlloc(434490);
#endif
#endif
    int ret = 0;
    try {
        // initialise the application system (messaging, xml, options)
        if(!SystemFrame::init(false, argc, argv,
            fillOptions, checkOptions, help)) {
            throw ProcessError();
        }
        // build the netgen-network description
        TNGNet *net = buildNetwork();
//     	net->SaveNet("test");
        delete net;
        NBNetBuilder nb;
        nb.buildLoaded();
    } catch (...) {
        MsgHandler::getErrorInstance()->inform(
            "Quitting (building failed).");
        ret = 1;
    }
    SystemFrame::close();
    if(ret==0) {
        MsgHandler::getMessageInstance()->inform("Success.");
    }
    return ret;
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NetGen.icc"
//#endif

// Local Variables:
// mode:C++
// End:


