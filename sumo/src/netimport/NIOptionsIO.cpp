/***************************************************************************
                          NIOptionsIO.cpp
              A class for the initialisation, input and veryfying of the
        programs options
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
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
// Revision 1.8  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/07/12 12:35:21  dkrajzew
// elmar2 importer included; debugging
//
// Revision 1.6  2005/04/27 12:24:24  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.5  2004/11/23 10:23:34  dkrajzew
// debugging
//
// Revision 1.4  2004/07/05 09:32:53  dkrajzew
// false check patched (must be reworked)
//
// Revision 1.3  2004/07/02 09:34:37  dkrajzew
// elmar and tiger import added
//
// Revision 1.2  2004/01/28 12:37:41  dkrajzew
// added the possibility to define vissims speed factor and a default speed
//  for vissim-networks
//
// Revision 1.1  2003/12/11 06:16:37  dkrajzew
// NBOptionsIO moved to netimport NIOptionsIO where they belong to
//
// Revision 1.20  2003/09/05 14:40:42  dkrajzew
// options for network building are now commoly set within NBNetBuilder
//
// Revision 1.19  2003/08/18 12:49:59  dkrajzew
// possibility to print node positions added
//
// Revision 1.18  2003/08/14 13:51:51  dkrajzew
// reshifting of networks added
//
// Revision 1.17  2003/07/22 15:22:05  dkrajzew
// duration of traffic lines may now be changed on command line
//
// Revision 1.16  2003/07/21 11:04:06  dkrajzew
// the default duration of green light phases may now be changed on startup
//
// Revision 1.15  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of
//  geometry information
//
// Revision 1.14  2003/06/24 14:38:46  dkrajzew
// false instantiation of option "log-file" as Option_Strng patched into
//  Option_FileName patched
//
// Revision 1.13  2003/06/24 08:09:29  dkrajzew
// implemented SystemFrame and applied the changes to all applications
//
// Revision 1.12  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a message, warning
//  or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.11  2003/06/05 11:43:35  dkrajzew
// class templates applied; documentation added
//
// Revision 1.10  2003/05/20 09:33:48  dkrajzew
// false computation of yielding on lane ends debugged; some debugging
//  on tl-import; further work on vissim-import
//
// Revision 1.9  2003/04/10 15:45:20  dkrajzew
// some lost changes reapplied
//
// Revision 1.8  2003/04/04 07:43:04  dkrajzew
// Yellow phases must be now explicetely given; comments added; order of edge
//  sorting (false lane connections) debugged
//
// Revision 1.7  2003/03/20 16:23:09  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.6  2003/03/18 13:07:23  dkrajzew
// usage of node position within xml-edge descriptions allowed
//
// Revision 1.5  2003/03/12 16:47:55  dkrajzew
// extension for artemis-import
//
// Revision 1.4  2003/02/13 15:53:15  dkrajzew
// some further processing options added
//
// Revision 1.3  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.2  2002/10/17 13:32:55  dkrajzew
// handling of connection specification files added
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.10  2002/07/25 08:40:46  dkrajzew
// Visum7.5 and Cell import added
//
// Revision 1.9  2002/07/18 07:22:26  dkrajzew
// Default usage of types removed
//
// Revision 1.8  2002/07/11 05:47:35  dkrajzew
// Options describing files transfered to Option_FileName to enable relative
//  path setting within the configuration files
//
// Revision 1.7  2002/06/17 15:19:29  dkrajzew
// unreferenced variable declarations removed
//
// Revision 1.6  2002/06/11 16:00:42  dkrajzew
// windows eol removed; template class definition inclusion depends now on
//  the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.5  2002/05/14 04:42:56  dkrajzew
// new computation flow
//
// Revision 1.4  2002/04/26 10:07:12  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.3  2002/04/24 06:52:01  dkrajzew
// Deprecated initialisation of the options container that used enviroment
//  variables removed
//
// Revision 1.2  2002/04/16 12:30:13  dkrajzew
// Usage of SUMO_DATA removed
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.6  2002/04/09 12:21:25  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.5  2002/03/22 10:50:03  dkrajzew
// Memory leaks debugging added (MSVC++)
//
// Revision 1.4  2002/03/20 08:30:33  dkrajzew
// Help output added
//
// Revision 1.3  2002/03/06 10:13:25  traffic
// Enviroment variable changef from SUMO to SUMO_DATA
//
// Revision 1.2  2002/03/05 14:55:33  traffic
// Error report on unset path changed
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream>
#include <fstream>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>
#include "NIOptionsIO.h"
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/RandHelper.h>
#include <utils/common/DevHelper.h>
#include <netbuild/NBNetBuilder.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection
   #define _INC_MALLOC       // exclude standard memory alloc procedures
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
NIOptionsIO::fillOptions(OptionsCont &oc)
{
    oc.doRegister("sumo-net", 's', new Option_FileName());
    oc.doRegister("xml-node-files", 'n', new Option_FileName());
    oc.doRegister("xml-edge-files", 'e', new Option_FileName());
    oc.doRegister("xml-connection-files", 'x', new Option_FileName());
    oc.doRegister("elmar", new Option_FileName());
    oc.doRegister("elmar2", new Option_FileName());
    oc.doRegister("tiger", new Option_FileName());
    oc.doRegister("cell-node-file", new Option_FileName());
    oc.doRegister("cell-edge-file", new Option_FileName());
    oc.doRegister("visum-file", new Option_FileName());
    oc.doRegister("vissim-file", new Option_FileName());
    oc.doRegister("artemis-path", new Option_FileName());
    oc.doRegister("arcview", new Option_FileName());
    oc.doRegister("arcview-shp", new Option_FileName());
    oc.doRegister("arcview-dbf", new Option_FileName());
    oc.doRegister("type-file", 't', new Option_FileName(/*"types.xml"*/));
    oc.doRegister("output-file", 'o', new Option_FileName("default.net.xml"));
    oc.doRegister("configuration-file", 'c',
        new Option_FileName());
    oc.addSynonyme("xml-node-files", "xml-nodes");
    oc.addSynonyme("xml-edge-files", "xml-edges");
    oc.addSynonyme("xml-connection-files", "xml-connections");
    oc.addSynonyme("cell-node-file", "cell-nodes");
    oc.addSynonyme("cell-edge-file", "cell-edges");
    oc.addSynonyme("visum-file", "visum");
    oc.addSynonyme("vissim-file", "vissim");
    oc.addSynonyme("artemis-path", "artemis");
    oc.addSynonyme("type-file", "types");
    oc.addSynonyme("output-file", "output");
    oc.addSynonyme("configuration-file", "configuration");
    // register computation variables
    oc.doRegister("capacity-norm", 'N', new Option_Float((float) 20000));
    // register further vissim-options
    oc.doRegister("vissim-offset", new Option_Float(5.0f));
    oc.doRegister("vissim-default-speed", new Option_Float(50.0f/3.6f));
    oc.doRegister("vissim-speed-norm", new Option_Float(1.0f));
    // register the data processing options
    oc.doRegister("speed-in-km", new Option_Bool(false));
    // add netbuilding options
    NBNetBuilder::insertNetBuildOptions(oc);
    // add rand and dev options
    DevHelper::insertDevOptions(oc);
    RandHelper::insertRandOptions(oc);
}


bool
NIOptionsIO::checkOptions(OptionsCont &oc)
{
    bool ok = true;
    try {
        if(!checkCompleteDescription(oc)) {
            if(!checkNodes(oc)) ok = false;
            if(!checkEdges(oc)) ok = false;
            if(!checkOutput(oc)) ok = false;
        }
    } catch (InvalidArgument &e) {
        MsgHandler::getErrorInstance()->inform(e.msg());
        return false;
    }
    return ok;
}


bool
NIOptionsIO::checkCompleteDescription(OptionsCont &oc)
{
   return false;
}


bool
NIOptionsIO::checkNodes(OptionsCont &oc)
{
    // check the existance of a name for the nodes file
    if( oc.isSet("n") ||
        oc.isSet("e") ||
        oc.isSet("cell-nodes") ||
        oc.isSet("visum") ||
        oc.isSet("vissim") ||
        oc.isSet("artemis") ||
        oc.isSet("tiger") ||
        oc.isSet("elmar") ||
        oc.isSet("arcview") ||
        oc.isSet("arcview-shp") ||
        oc.isSet("arcview-dbf") ||
        oc.isSet("sumo-net") ) {
        return true;
    }
    MsgHandler::getErrorInstance()->inform("The nodes must be supplied.");
    return false;
}


bool
NIOptionsIO::checkEdges(OptionsCont &oc)
{
    // check whether at least a sections or a edges file is supplied
    if( oc.isSet("e") ||
        oc.isSet("cell-edges") ||
        oc.isSet("visum") ||
        oc.isSet("vissim") ||
        oc.isSet("artemis") ||
        oc.isSet("arcview") ||
        oc.isSet("arcview-shp") ||
        oc.isSet("arcview-dbf") ||
        oc.isSet("sumo-net") ) {
        return true;
    }
    MsgHandler::getErrorInstance()->inform(
        "Either sections or edges must be supplied.");
    return false;
}


bool
NIOptionsIO::checkOutput(OptionsCont &oc)
{
    ofstream strm(oc.getString("o").c_str()); // !!! should be made when input are ok
    if(!strm.good()) {
        MsgHandler::getErrorInstance()->inform(
            string("The output file \"") + oc.getString("o")
            + string("\" can not be build."));
        return false;
    }
    return true;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

