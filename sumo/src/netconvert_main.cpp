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
// Revision 1.34  2006/01/09 13:33:30  dkrajzew
// debugging error handling
//
// Revision 1.33  2005/11/30 08:56:49  dkrajzew
// final try/catch is now only used in the release version
//
// Revision 1.32  2005/11/15 10:15:49  dkrajzew
// debugging and beautifying for the next release
//
// Revision 1.31  2005/11/14 09:56:18  dkrajzew
// "speed-in-km" is now called "speed-in-kmh";
//  removed two files definition for arcview
//
// Revision 1.30  2005/11/09 06:48:16  dkrajzew
// removed some memory leaks
//
// Revision 1.29  2005/10/17 09:27:46  dkrajzew
// got rid of the old MSVC memory leak checker
//
// Revision 1.28  2005/10/07 11:48:01  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.27  2005/09/15 12:27:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.26  2005/07/12 12:55:27  dkrajzew
// build number output added
//
// Revision 1.25  2005/05/04 09:34:35  dkrajzew
// netbuilding containers are now non-static
//
// Revision 1.24  2004/12/16 12:17:29  dkrajzew
// a further network prune option added
//
// Revision 1.23  2004/11/23 10:43:28  dkrajzew
// debugging
//
// Revision 1.22  2004/02/06 08:54:28  dkrajzew
// _INC_MALLOC definition removed (does not work on MSVC7.0)
//
// Revision 1.21  2004/01/12 15:43:04  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.20  2003/12/11 06:13:32  dkrajzew
// NBOptionsIO moved to netimport NIOptionsIO where they belong to
//
// Revision 1.19  2003/09/05 15:29:48  dkrajzew
// a bug on comparing the return value patched
//
// Revision 1.18  2003/07/18 12:35:04  dkrajzew
// removed some warnings
//
// Revision 1.17  2003/07/17 12:14:21  dkrajzew
// NBLoader moved to netimport;
//  NBNetBuilder performs now all the building steps
//
// Revision 1.16  2003/07/07 08:41:12  dkrajzew
// the edges positions are now normalised, too; the edges are joined if
//  connecting the same node
//
// Revision 1.15  2003/06/24 14:38:46  dkrajzew
// false instantiation of option "log-file" as Option_Strng patched into
//  Option_FileName patched
//
// Revision 1.14  2003/06/24 08:06:36  dkrajzew
// implemented SystemFrame and applied the changes to all applications
//
// Revision 1.13  2003/06/19 07:07:52  dkrajzew
// false order of calling XML- and Options-subsystems patched
//
// Revision 1.12  2003/06/18 11:26:15  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
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
// Moved from ROOT/sumo to ROOT/src; added further help and main files for
//  netconvert, router, od2trips and gui version
//
// Revision 1.9  2002/07/25 08:24:10  dkrajzew
// Visum and Cell import added
//
// Revision 1.8  2002/07/02 12:51:51  dkrajzew
// removal of options on help printing added
//
// Revision 1.7  2002/07/02 08:54:50  dkrajzew
// XMLSubSys implemented; return values fixed; better flow and a better
//  documentation
//
// Revision 1.6  2002/06/17 15:15:28  dkrajzew
// unreferenced variable declarations removed
//
// Revision 1.5  2002/06/11 15:56:07  dkrajzew
// windows eol removed
//
// Revision 1.4  2002/06/07 14:58:45  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed;
//  Comments improved
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
#include <netbuild/NBNetBuilder.h>
#include <netimport/NIOptionsIO.h>
#include <netimport/NILoader.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/nodes/NBNodeCont.h>
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
#include "netconvert_build.h"
#include "sumo_version.h"
#include <utils/common/HelpPrinter.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
    int ret = 0;
#ifndef _DEBUG
    try {
#endif
        int init_ret = SystemFrame::init(false, argc, argv, NIOptionsIO::fillOptions);
        if(init_ret<0) {
            cout << "SUMO netconvert" << endl;
            cout << " (c) DLR/ZAIK 2000-2006; http://sumo.sourceforge.net" << endl;
            switch(init_ret) {
            case -1:
                cout << " Version " << version << endl;
                cout << " Build #" << NEXT_BUILD_NUMBER << endl;
                break;
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
        // retrieve the options
        OptionsCont &oc = OptionsSubSys::getOptions();
        NBNetBuilder nb;
        // initialise the (default) types
        nb.getTypeCont().setDefaults(
            oc.getString("T"), oc.getInt("L"), oc.getFloat("S"),
            oc.getInt("P"));
        // load data
        nb.preCheckOptions(oc);
        NILoader nl(nb);
        nl.load(oc);
        // check whether any errors occured
        if(MsgHandler::getErrorInstance()->wasInformed()) {
            throw ProcessError();
        }
        nb.buildLoaded();
#ifndef _DEBUG
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (conversion failed).", false);
        ret = 1;
    }
#endif
    NBDistribution::clear();
    SystemFrame::close();
    // report about ending
    if(ret==0) {
        cout << "Success." << endl;
    }
    return ret;
}


