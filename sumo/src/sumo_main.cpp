/***************************************************************************
                          sumo_main.cpp
              The main procedure for the simulation module
                             -------------------
    project              : SUMO
    subproject           : simulation
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.26  2005/05/04 09:33:43  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.25  2005/02/17 10:33:29  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.24  2004/11/25 16:26:45  dkrajzew
// consolidated and debugged some detectors and their usage
//
// Revision 1.23  2004/11/23 10:43:29  dkrajzew
// debugging
//
// Revision 1.22  2004/07/02 09:52:22  dkrajzew
// changes due to the online-routing implementation
//
// Revision 1.21  2004/04/02 11:29:45  dkrajzew
// simulation-wide output files are now handled by MSNet directly
//
// Revision 1.20  2004/02/16 13:44:26  dkrajzew
// dump output generating function renamed in order to add vehicle dump ability in the future
//
// Revision 1.19  2003/12/11 07:14:04  dkrajzew
// new netload usage adapted
//
// Revision 1.18  2003/10/28 08:35:01  dkrajzew
// random number specification options added
//
// Revision 1.17  2003/08/21 13:02:22  dkrajzew
// cleaned up
//
// Revision 1.16  2003/07/22 15:40:40  dkrajzew
// false exception catch used during debugging removed
//
// Revision 1.15  2003/07/22 15:16:49  dkrajzew
// new detector usage applied
//
// Revision 1.14  2003/07/21 18:17:28  roessel
// Creation of SingletonDictionaries for Detectors added. Move to another place?
//
// Revision 1.13  2003/06/24 14:41:37  dkrajzew
// the correct step number is now prompted as the step the simulation ended at
//
// Revision 1.12  2003/06/24 08:06:36  dkrajzew
// implemented SystemFrame and applied the changes to all applications
//
// Revision 1.11  2003/06/19 11:03:57  dkrajzew
// debugging
//
// Revision 1.10  2003/06/19 07:07:52  dkrajzew
// false order of calling XML- and Options-subsystems patched
//
// Revision 1.9  2003/06/18 11:26:15  dkrajzew
// new message and error processing: output to user may be a message, warning
// or an error now; it is reported to a Singleton (MsgHandler); this handler
// puts it further to output instances. changes: no verbose-parameter needed;
// messages are exported to singleton
//
// Revision 1.8  2003/06/06 10:54:20  dkrajzew
// deletion of the MSLaneState-singleton map added
//
// Revision 1.7  2003/06/05 14:45:02  dkrajzew
// false usage of relative paths changed
//
// Revision 1.6  2003/05/21 16:20:43  dkrajzew
// further work detectors
//
// Revision 1.5  2003/05/20 09:54:45  dkrajzew
// configuration files are no longer set as default
//
// Revision 1.4  2003/03/03 15:09:30  dkrajzew
// unnecessary debug output removed
//
// Revision 1.3  2003/02/07 10:37:30  dkrajzew
// files updated
//
// Revision 1.2  2002/10/17 10:36:30  dkrajzew
// sources and detectors joined with triggers to additional-files
//
// Revision 1.1  2002/10/16 14:51:08  dkrajzew
// Moved from ROOT/sumo to ROOT/src; added further help and main files for
// netconvert, router, od2trips and gui version
//
// Revision 1.9  2002/07/31 17:42:10  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.12  2002/07/22 12:52:23  dkrajzew
// Source handling added
//
// Revision 1.11  2002/07/11 07:30:43  dkrajzew
// Option_FileName invented to allow relative path names within the
// configuration files; two not yet implemented parameter introduced
//
// Revision 1.10  2002/07/02 12:48:10  dkrajzew
// --help now does not require -c
//
// Revision 1.9  2002/07/02 08:16:19  dkrajzew
// Program flow changed to allow better options removal; return values
// corrected
//
// Revision 1.8  2002/06/17 15:57:43  dkrajzew
// unreferenced variable declarations removed
//
// Revision 1.7  2002/05/14 07:43:51  dkrajzew
// _SPEEDCHECK-methods moved completely to MSNet
//
// Revision 1.6  2002/04/29 13:52:07  dkrajzew
// the program flow, especially the handling of errors improved
//
// Revision 1.5  2002/04/18 06:04:53  dkrajzew
// Forgotten test switch removed
//
// Revision 1.4  2002/04/17 11:20:40  dkrajzew
// Windows-carriage returns removed
//
// Revision 1.3  2002/04/16 12:21:13  dkrajzew
// Usage of SUMO_DATA removed
//
// Revision 1.2  2002/04/15 06:55:47  dkrajzew
// new loading paradigm implemented
//
// Revision 2.6  2002/03/20 08:13:54  dkrajzew
// help-output added
//
// Revision 2.5  2002/03/15 12:45:49  dkrajzew
// Warning is set to true forever due to bugs in value testing when no
// warnings are used (will be fixed later)
//
// Revision 2.4  2002/03/14 08:09:13  traffic
// Option for no raw output added
//
// Revision 2.3  2002/03/07 07:54:43  traffic
// implemented the usage of stdout as the default raw output
//
// Revision 2.2  2002/03/06 10:12:17  traffic
// Enviroment variable changef from SUMO to SUMO_DATA
//
// Revision 2.1  2002/03/05 14:51:25  traffic
// SegViolation on unset path debugged
//
// Revision 2.0  2002/02/14 14:43:11  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:35:33  croessel
// Merging sourceForge with tesseraCVS.
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#ifdef DEBUB_ALLOC
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#ifdef WIN32
   #include <utils/dev/MemDiff.h>
   #include <crtdbg.h>
#endif
/*
#include <utils/dev/debug_new.h>
#define new debug_new
*/
#endif

#include <ctime>
#include <string>
#include <iostream>
#include <fstream>
#include <microsim/MSNet.h>
#include <microsim/MSRoute.h>
#include <microsim/MSEmitControl.h>
#include <microsim/MSVehicleControl.h>
#include <netload/NLNetBuilder.h>
#include <netload/NLEdgeControlBuilder.h>
#include <netload/NLJunctionControlBuilder.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/HelpPrinter.h>
#include <utils/common/StringTokenizer.h>
#include <utils/convert/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/options/OptionsSubSys.h>
#include <sumo_only/SUMOFrame.h>
#include "sumo_help.h"
#include <microsim/output/MSDetectorSubSys.h>
#include <utils/iodevices/SharedOutputDevices.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * functions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * data processing methods
 * ----------------------------------------------------------------------- */
/**
 * loads the net, additional routes and the detectors
 */
MSNet *
load(OptionsCont &oc)
{
    SharedOutputDevices::setInstance(new SharedOutputDevices());
    SUMOFrame::setMSGlobals(oc);
    NLEdgeControlBuilder eb;
    NLJunctionControlBuilder jb;
    NLNetBuilder builder(oc, eb, jb);
    MSNet *ret = builder.buildNet(new MSVehicleControl());
    if(ret==0) {
        throw ProcessError();
    }
    return ret;
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
#ifdef _DEBUG
#ifdef WIN32
    CMemDiff state1;
    // uncomment next line and insert the context of an undeleted
    //  allocation to break within it (MSVC++ only)
    // _CrtSetBreakAlloc(814107);
#endif
#endif

    size_t rand_init = 10551;
    int ret = 0;
    try {
        if(!SystemFrame::init(false, argc, argv,
            SUMOFrame::fillOptions, SUMOFrame::checkOptions, help)) {
            throw ProcessError();
        }
        // retrieve the options
        OptionsCont &oc = OptionsSubSys::getOptions();
        // load the net
        MSNet *net = load(oc);
        // report the begin when wished
        WRITE_MESSAGE(string("Simulation started with time: ") + toString<int>(oc.getInt("b")));
        // simulate
        net->simulate(oc.getInt("b"), oc.getInt("e"));
        // report the end when wished
        WRITE_MESSAGE(string("Simulation ended at time: ") + toString<int>(net->getCurrentTimeStep()));
        delete net;
        delete SharedOutputDevices::getInstance();
    } catch (...) {
        MSNet::clearAll();
        delete SharedOutputDevices::getInstance();
        MsgHandler::getMessageInstance()->inform(
            "Quitting (on error).");
        ret = 1;
    }
    SystemFrame::close();
    return ret;
}
