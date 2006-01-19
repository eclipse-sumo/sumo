//---------------------------------------------------------------------------//
//                        DFDetFlowLoader.cpp -
//  A handler for Artemis-files
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 12 Mar 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.1  2006/01/19 17:42:59  ericnicolay
// base classes for the reading of the detectorflows
//
// Revision 1.8  2005/10/17 09:21:57  dkrajzew
// c4503 warning removed
//
// Revision 1.7  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2005/05/04 08:50:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.2  2004/02/16 13:47:07  dkrajzew
// Type-dependent loader/generator-"API" changed
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// ------------------------------------------------
// Revision 1.10  2003/10/31 08:00:31  dkrajzew
// hope to have patched false usage of RAND_MAX when using gcc
//
// Revision 1.9  2003/10/15 11:55:11  dkrajzew
// false usage of rand() patched
//
// Revision 1.8  2003/07/30 09:26:33  dkrajzew
// all vehicles, routes and vehicle types may now have specific colors
//
// Revision 1.7  2003/07/18 12:35:06  dkrajzew
// removed some warnings
//
// Revision 1.6  2003/07/16 15:36:50  dkrajzew
// vehicles and routes may now have colors
//
// Revision 1.5  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.4  2003/05/20 09:48:34  dkrajzew
// debugging
//
// Revision 1.3  2003/03/20 16:39:15  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.2  2003/03/17 14:26:38  dkrajzew
// debugging
//
// Revision 1.1  2003/03/12 16:39:17  dkrajzew
// artemis route support added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)
#pragma warning(disable: 4503)


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

#include <string>
#include <iostream> // !!! debug only
#include <fstream>
#include <sstream>
#include <utils/importio/LineHandler.h>
#include <utils/gfx/RGBColor.h>
#include <utils/options/OptionsCont.h>
#include <utils/router/IDSupplier.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/TplConvert.h>
#include <utils/common/UtilExceptions.h>
#include "DFDetFlowLoader.h"

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
DFDetFlowLoader::DFDetFlowLoader(std::string file, DFDetectorCon *DetCon)
{
	mydetFlows = new DFDetectorFlows();
	detcon = DetCon;
    myFirstLine = true;
    myReader.setFileName(file);
    myReader.readAll(*this);
}


DFDetFlowLoader::~DFDetFlowLoader()
{
}


bool
DFDetFlowLoader::report(const std::string &result)
{
    // parse the flows
    if(myFirstLine) {
		myLineHandler.reinit(result, "\t", "\t", true);
        myFirstLine = false;
    } else {
		myLineHandler.parseLine(result);
		FlowDef fd;
		fd.time = TplConvert<char>::_2int((myLineHandler.get("Time").c_str()));
		fd.det = myLineHandler.get("Detector");
		fd.isLKW = 0;
		fd.qPKW = TplConvert<char>::_2SUMOReal(myLineHandler.get("qPKW").c_str());
		fd.vPKW = TplConvert<char>::_2SUMOReal(myLineHandler.get("vPKW").c_str());
		fd.qLKW = TplConvert<char>::_2SUMOReal(myLineHandler.get("qLKW").c_str());
		fd.vLKW = TplConvert<char>::_2SUMOReal(myLineHandler.get("vLKW").c_str());
		if ( fd.qLKW!=0 && fd.qPKW!=0 ) 
			fd.fLKW = fd.qLKW / fd.qPKW ;
		else
			fd.fLKW = 0;
		if ( detcon->isDetector( fd.det ) )
			this->mydetFlows->addFlow( fd.det, fd.time, fd );
        }

    return true;
}

DFDetectorFlows *
DFDetFlowLoader::getFlows()
{
	return this->mydetFlows;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


