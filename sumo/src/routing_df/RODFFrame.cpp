/***************************************************************************
                          RODFFrame.cpp
    Some IO-initialisations for the DFROUTER
                             -------------------
    project              : SUMO
    begin                : Thu, 16.03.2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
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
// Revision 1.16  2006/09/18 10:15:40  dkrajzew
// code beautifying
//
// Revision 1.15  2006/08/01 11:30:21  dkrajzew
// patching building
//
// Revision 1.13  2006/05/08 11:14:53  dksumo
// further work on the dfrouter
//
// Revision 1.12  2006/04/11 11:07:58  dksumo
// debugging
//
// Revision 1.11  2006/04/07 10:44:17  dksumo
// multiple detector and flows definitions can be read
//
// Revision 1.10  2006/04/05 05:35:25  dksumo
// further work on the dfrouter
//
// Revision 1.9  2006/03/27 07:32:19  dksumo
// some further work...
//
// Revision 1.8  2006/03/17 09:04:18  dksumo
// class-documentation added/patched
//
// Revision 1.7  2006/03/08 12:51:06  dksumo
// further work on the dfrouter
//
// Revision 1.6  2006/02/13 07:26:56  dksumo
// current work on the DFROUTER added (unfinished)
//
// Revision 1.5  2006/02/02 12:16:13  dksumo
// saved to recheck methods
//
// Revision 1.4  2006/01/31 11:00:48  dksumo
// added the possibility to write detector positions as pois; debugging
//
// Revision 1.3  2006/01/16 13:45:21  dksumo
// initial work on the dfrouter
//
// Revision 1.2  2006/01/09 09:17:04  dksumo
// first work on dfrouter
//
// Revision 1.1  2005/12/12 11:52:31  dksumo
// dfrouter added
//
// Revision 1.1  2005/12/08 12:59:41  ericnicolay
// *** empty log message ***
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
#include <fstream>
#include <ctime>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include "RODFFrame.h"
#include <router/ROFrame.h>
#include <utils/common/RandHelper.h>


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
RODFFrame::fillOptions(OptionsCont &oc)
{
    // register input-options
    oc.doRegister("configuration-file", 'c', new Option_FileName());
    oc.addSynonyme("configuration-file", "configuration");
    oc.doRegister("routes-input", 'r', new Option_FileName());
    oc.doRegister("detector-files", 'd', new Option_FileName());
    oc.addSynonyme("detector-files", "detectors");
    oc.doRegister("detector-flow-files", 'f', new Option_FileName());
    oc.addSynonyme("detector-flow-files", "detflows");
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.addSynonyme("net-file", "net");

    // register output options
    oc.doRegister("routes-output", 'o', new Option_FileName());
    oc.doRegister("routes-for-all", new Option_Bool(false));
    oc.doRegister("detectors-output", new Option_FileName());
    oc.doRegister("detectors-poi-output", new Option_FileName());
	oc.doRegister("emitters-output", new Option_FileName());
	oc.doRegister("speed-trigger-output", new Option_FileName());
    oc.doRegister("end-reroute-output", new Option_FileName());
	oc.doRegister("validation-output", new Option_FileName());
	oc.doRegister("validation-output.add-sources", new Option_Bool(false));

    // register processing options
    oc.doRegister("highway-mode", 'h', new Option_Bool(false));
        // for detector type computation
    oc.doRegister("revalidate-detectors", new Option_Bool(false));
        // for route computation
    oc.doRegister("revalidate-routes", new Option_Bool(false));
    oc.doRegister("all-end-follower", new Option_Bool(false));
    oc.doRegister("keep-unfound-ends", new Option_Bool(false));
        // flow reading
    oc.doRegister("fast-flows", new Option_Bool(false));
    oc.doRegister("time-offset", new Option_Integer(0));
        // saving further structures
	oc.doRegister("write-calibrators", new Option_Bool(false)); // undescribed
    oc.doRegister("revalidate-flows", new Option_Bool(false));
    oc.doRegister("remove-empty-detectors", new Option_Bool(false));

    oc.doRegister("strict-sources", new Option_Bool(false)); // undescribed

#ifdef HAVE_MESOSIM
    oc.doRegister("mesosim", new Option_Bool(false));
#endif

    // register the simulation settings
    oc.doRegister("begin", 'b', new Option_Integer(0));
    oc.doRegister("end", 'e', new Option_Integer(864000));

    // register vehicle type defaults
    oc.doRegister("krauss-vmax", 'V', new Option_Float(SUMOReal(70)));
    oc.doRegister("krauss-a", 'A', new Option_Float(SUMOReal(2.6)));
    oc.doRegister("krauss-b", 'B', new Option_Float(SUMOReal(4.5)));
    oc.doRegister("krauss-length", 'L', new Option_Float(SUMOReal(5)));
    oc.doRegister("krauss-eps", 'E', new Option_Float(SUMOReal(0.5)));

    // register reporting options
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.doRegister("help", new Option_Bool(false));
    oc.doRegister("log-file", 'l', new Option_FileName());

    RandHelper::insertRandOptions(oc);
}


bool
RODFFrame::checkOptions(OptionsCont &oc)
{
    return true;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


