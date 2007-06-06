/****************************************************************************/
/// @file    NIArtemisLoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10 Feb 2003
/// @version $Id$
///
// A loader artemis-files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/TplConvert.h>
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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
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
{}


NIArtemisLoader::NIArtemisSingleDataTypeParser::~NIArtemisSingleDataTypeParser()
{}


bool
NIArtemisLoader::NIArtemisSingleDataTypeParser::parse()
{
    MsgHandler::getMessageInstance()->beginProcessMsg("Parsing " + getDataName() + "...");
    string file = myParent.getFileName() + getDataName();
    LineReader reader(file);
    if (!reader.good()) {
        if (!amOptional()) {
            MsgHandler::getErrorInstance()->inform("Problems on parsing '" + file + "'.");
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
    MsgHandler::getMessageInstance()->endProcessMsg("done.");
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
{}


void
NIArtemisLoader::NIArtemisSingleDataTypeParser::myClose()
{}



bool
NIArtemisLoader::NIArtemisSingleDataTypeParser::report(
    const std::string &line)
{
    if (myStep==0) {
        myLineParser.reinit(line, "\t", "\t", true);
        myStep++;
    } else {
        if (StringUtils::prune(line).length()!=0) {
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
    if (id.length()!=0) {
        if (exception.length()!=0) {
            addError("The definition of the " + type + " '" + id + "' is malicious (" + exception + ").");
        } else {
            addError("The definition of the " + type + " '" + id + "' is malicious.");
        }
    } else {
        addError("Something is wrong with a " + type + " (unknown id).");
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
    for (ParserVector::iterator i=mySingleDataParsers.begin();
            i!=mySingleDataParsers.end(); i++) {
        delete(*i);
    }
}


void NIArtemisLoader::load(OptionsCont &)
{
    for (ParserVector::iterator i=mySingleDataParsers.begin(); i!=mySingleDataParsers.end(); i++) {
        (*i)->parse();
    }
}



/****************************************************************************/

