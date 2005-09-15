/***************************************************************************
                          NIArtemisLoader.cpp
              A loader artemis-files
                             -------------------
    project              : SUMO
    begin                : Mon, 10 Feb 2003
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.7  2005/04/27 12:24:24  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.6  2004/11/23 10:23:51  dkrajzew
// debugging
//
// Revision 1.5  2003/06/18 11:14:13  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.4  2003/03/26 12:04:56  dkrajzew
// some debugging
//
// Revision 1.3  2003/03/17 14:18:57  dkrajzew
// Windows eol removed
//
// Revision 1.2  2003/03/12 16:44:46  dkrajzew
// further work on artemis-import
//
// Revision 1.1  2003/03/03 15:00:26  dkrajzew
// initial commit for artemis-import files
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
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/convert/TplConvert.h>
#include <utils/options/OptionsCont.h>
#include "NIArtemisLoader.h"
#include "NIArtemisParser_Nodes.h"
#include "NIArtemisParser_Links.h"
#include "NIArtemisParser_Signals.h"
#include "NIArtemisParser_SignalGroups.h"
#include "NIArtemisParser_SignalPhases.h"
#include "NIArtemisParser_Lanes.h"
#include "NIArtemisParser_Segments.h"
#include "NIArtemisParser_HVdests.h"
#include "NIArtemisTempSignal.h"
#include "NIArtemisTempEdgeLanes.h"

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
/* -------------------------------------------------------------------------
 * methods from NIArtemisLoader::PositionSetter
 * ----------------------------------------------------------------------- */

/* -------------------------------------------------------------------------
 * methods from NIArtemisLoader::NIArtemisSingleDataTypeParser
 * ----------------------------------------------------------------------- */
NIArtemisLoader::NIArtemisSingleDataTypeParser::NIArtemisSingleDataTypeParser(
        NIArtemisLoader &parent, const std::string &dataName)
    : FileErrorReporter::Child(parent), myDataName(dataName),
    myStep(0)
{
}


NIArtemisLoader::NIArtemisSingleDataTypeParser::~NIArtemisSingleDataTypeParser()
{
}


bool
NIArtemisLoader::NIArtemisSingleDataTypeParser::parse()
{
    WRITE_MESSAGE(string("Parsing ") + getDataName() + string("... "));
    string file = myParent.getFileName() + getDataName();
    LineReader reader(file);
    if(!reader.good()) {
        if(!amOptional()) {
            MsgHandler::getErrorInstance()->inform(
                string("Problems on parsing '") + file + string("'."));
            return false;
        } else {
            WRITE_MESSAGE("not supplied (no error).");
            return true;
        }
    }
    // parser-dependent init
    myInitialise();
    // skip/set names
    reader.readAll(*this);
    WRITE_MESSAGE("done.");
    // parser-dependent close
    myClose();
    return true;
}

bool
NIArtemisLoader::NIArtemisSingleDataTypeParser::amOptional() const
{
    return false;
}



const std::string &
NIArtemisLoader::NIArtemisSingleDataTypeParser::getDataName() const
{
    return myDataName;
}


void
NIArtemisLoader::NIArtemisSingleDataTypeParser::myInitialise()
{
}


void
NIArtemisLoader::NIArtemisSingleDataTypeParser::myClose()
{
}



bool
NIArtemisLoader::NIArtemisSingleDataTypeParser::report(
    const std::string &line)
{
    if(myStep==0) {
        myLineParser.reinit(line, "\t", "\t", true);
        myStep++;
    } else {
        if(StringUtils::prune(line).length()!=0) {
            myLineParser.parseLine(line);
            myDependentReport();
        }
    }
    return true;
}


void
NIArtemisLoader::NIArtemisSingleDataTypeParser::addError2(
    const std::string &type, const std::string &id,
    const std::string &exception)
{
    if(id.length()!=0) {
        if(exception.length()!=0) {
            addError(
                string("The definition of the ") + type + string(" '")
                + id
                + string("' is malicious (") + exception + string(")."));
        } else {
            addError(
                string("The definition of the ") + type + string(" '")
                + id
                + string("' is malicious."));
        }
    } else {
        addError(
            string("Something is wrong with a ") + type
            + string(" (unknown id)."));
    }
}





 /* -------------------------------------------------------------------------
 * methods from NIArtemisLoader
 * ----------------------------------------------------------------------- */
NIArtemisLoader::NIArtemisLoader(const std::string &file,
                                 NBDistrictCont &dc,
                                 NBNodeCont &nc, NBEdgeCont &ec,
                                 NBTrafficLightLogicCont &tlc)
    : FileErrorReporter("artemis-network", file)
{
    // the order of process is important!
    // set1
    mySingleDataParsers.push_back(
        new NIArtemisParser_Nodes(nc, tlc, *this, "Nodes.txt"));
    mySingleDataParsers.push_back(
        new NIArtemisParser_Links(nc, ec, *this, "Links.txt"));
    // signals must be loaded before the edges are splitted
    mySingleDataParsers.push_back(
        new NIArtemisParser_Signals(*this, "Signals.txt"));
    mySingleDataParsers.push_back(
        new NIArtemisParser_SignalPhases(*this, "Signal Phases.txt"));
    mySingleDataParsers.push_back(
        new NIArtemisParser_SignalGroups(nc, *this, "Signal Groups.txt"));
    // segments must be loaded before the edges are splitted
    mySingleDataParsers.push_back(
        new NIArtemisParser_Segments(ec, *this, "Segments.txt"));
    // edges are splitted when adding lane infomration
    mySingleDataParsers.push_back(
        new NIArtemisParser_Lanes(dc, nc, ec, *this, "Lanes.txt"));
    // needed for insertion of source and destination edges
    mySingleDataParsers.push_back(
        new NIArtemisParser_HVdests(nc, ec, *this, "HVdests.txt"));
}


NIArtemisLoader::~NIArtemisLoader()
{
    for( ParserVector::iterator i=mySingleDataParsers.begin();
         i!=mySingleDataParsers.end(); i++) {
        delete (*i);
    }
}


void NIArtemisLoader::load(OptionsCont &options)
{
    for( ParserVector::iterator i=mySingleDataParsers.begin();
         i!=mySingleDataParsers.end(); i++) {
        (*i)->parse();
    }
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

