/***************************************************************************
                          main.cpp
			  The main procedure for the conversion /
			  building of networks
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
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
// Revision 1.18  2003/07/18 12:35:04  dkrajzew
// removed some warnings
//
// Revision 1.17  2003/07/17 12:14:21  dkrajzew
// NBLoader moved to netimport; NBNetBuilder performs now all the building steps
//
// Revision 1.16  2003/07/07 08:41:12  dkrajzew
// the edges positions are now normalised, too; the edges are joined if connecting the same node
//
// Revision 1.15  2003/06/24 14:38:46  dkrajzew
// false instantiation of option "log-file" as Option_Strng patched into Option_FileName patched
//
// Revision 1.14  2003/06/24 08:06:36  dkrajzew
// implemented SystemFrame and applied the changes to all applications
//
// Revision 1.13  2003/06/19 07:07:52  dkrajzew
// false order of calling XML- and Options-subsystems patched
//
// Revision 1.12  2003/06/18 11:26:15  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.11  2003/06/05 14:41:53  dkrajzew
// further parameter contraining the area of connectors to join (VIssim) added
//
// Revision 1.10  2003/05/20 09:54:45  dkrajzew
// configuration files are no longer set as default
//
// Revision 1.9  2003/04/10 16:13:51  dkrajzew
// recent changes
//
// Revision 1.8  2003/04/09 15:53:16  dkrajzew
// netconvert-changes: further work on Vissim-import, documentation added
//
// Revision 1.7  2003/04/04 08:41:48  dkrajzew
// help screen updated; min-decel usage added
//
// Revision 1.6  2003/03/18 13:19:38  dkrajzew
// memory leak debugging functions included (MSVC)
//
// Revision 1.5  2003/03/06 17:19:11  dkrajzew
// debugging
//
// Revision 1.4  2003/03/03 15:08:48  dkrajzew
// vissim requires more steps on building
//
// Revision 1.3  2003/02/07 10:37:30  dkrajzew
// files updated
//
// Revision 1.1  2002/10/16 14:51:08  dkrajzew
// Moved from ROOT/sumo to ROOT/src; added further help and main files for netconvert, router, od2trips and gui version
//
// Revision 1.9  2002/07/25 08:24:10  dkrajzew
// Visum and Cell import added
//
// Revision 1.8  2002/07/02 12:51:51  dkrajzew
// removal of options on help printing added
//
// Revision 1.7  2002/07/02 08:54:50  dkrajzew
// XMLSubSys implemented; return values fixed; better flow and a better documentation
//
// Revision 1.6  2002/06/17 15:15:28  dkrajzew
// unreferenced variable declarations removed
//
// Revision 1.5  2002/06/11 15:56:07  dkrajzew
// windows eol removed
//
// Revision 1.4  2002/06/07 14:58:45  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed; Comments improved
//
// Revision 1.3  2002/05/14 04:39:47  dkrajzew
// new computation steps
//
// Revision 1.2  2002/04/26 10:04:04  dkrajzew
// Windows eol removed
//
// Revision 1.1.1.1  2002/04/09 14:18:26  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.4  2002/04/09 12:19:51  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.3  2002/03/22 10:40:25  dkrajzew
// Memory leak finder added
//
// Revision 1.2  2002/03/20 08:28:02  dkrajzew
// Help output added
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// -------------------------------------------
// moved to an own project
// -------------------------------------------
// Revision 1.1  2001/12/06 13:03:15  traffic
// added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <string>
#include <fstream>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBOptionsIO.h>
#include <netimport/NILoader.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBJunctionLogicCont.h>
#include <netbuild/NBDistrictCont.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBDistribution.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/MsgHandler.h>
#include "netconvert_help.h"


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#ifdef WIN32
   #include <utils/dev/MemDiff.h>
   #include <crtdbg.h>
#endif
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
void
clearAll()
{
    NBEdgeCont::clear();
    NBNodeCont::clear();
    NBTypeCont::clear();
    NBJunctionLogicCont::clear();
    NBDistrictCont::clear();
    NBTrafficLightLogicCont::clear();
    NBDistribution::clear();
}


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

    int ret = 0;
    try {
        if(!SystemFrame::init(false, argc, argv,
            NBOptionsIO::fillOptions, 0, help)) {
            throw ProcessError();
        }
        // retrieve the options
        OptionsCont &oc = OptionsSubSys::getOptions();
        // initialise the (default) types
        NBTypeCont::setDefaults(
            oc.getString("T"),
            oc.getInt("L"),
            oc.getFloat("S"),
            oc.getInt("P"));
        // load data
        NILoader::load(oc);
        // check whether any errors occured
        if(MsgHandler::getErrorInstance()->wasInformed()) {
            throw ProcessError();
        }
        NBNetBuilder nb;
        nb.buildLoaded();
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (conversion failed).");
        ret = 1;
    }
    clearAll();
    SystemFrame::close();
    // report about ending
    if(ret=0) {
        MsgHandler::getMessageInstance()->inform("Success.");
    }
    return ret;
}


