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
// Revision 1.2  2006/02/13 07:27:06  dkrajzew
// current work on the DFROUTER added (unfinished)
//
// Revision 1.1  2006/01/19 17:42:59  ericnicolay
// base classes for the reading of the detectorflows
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
DFDetFlowLoader::DFDetFlowLoader(DFDetectorCon *DetCon)
{
	mydetFlows = new DFDetectorFlows();
	detcon = DetCon;
}



DFDetFlowLoader::~DFDetFlowLoader()
{
}


DFDetectorFlows *
DFDetFlowLoader::read(const std::string &file)
{
    myFirstLine = true;
    myReader.setFileName(file);
    myReader.readAll(*this);
	return mydetFlows;
}


bool
DFDetFlowLoader::report(const std::string &result)
{
    // parse the flows
    if(myFirstLine) {
		myLineHandler.reinit(result, ";", ";", true); // !!!
        myFirstLine = false;
    } else {
		myLineHandler.parseLine(result);
		string detName = myLineHandler.get("Detector");
		FlowDef fd;
		fd.time = TplConvert<char>::_2int((myLineHandler.get("Time").c_str()));
//		fd.det = myLineHandler.get("Detector");
		fd.isLKW = 0;
		fd.qPKW = TplConvert<char>::_2SUMOReal(myLineHandler.get("qPKW").c_str());
		fd.vPKW = TplConvert<char>::_2SUMOReal(myLineHandler.get("vPKW").c_str());
		fd.qLKW = TplConvert<char>::_2SUMOReal(myLineHandler.get("qLKW").c_str());
		fd.vLKW = TplConvert<char>::_2SUMOReal(myLineHandler.get("vLKW").c_str());
        if ( fd.qLKW!=0 && fd.qPKW!=0 ) {
			fd.fLKW = fd.qLKW / fd.qPKW ;
        } else {
			fd.fLKW = 0;
		}
		mydetFlows->addFlow( detName, fd.time, fd );
	}

    return true;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


