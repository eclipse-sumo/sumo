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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <utils/common/SErrorHandler.h>
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
NIArtemisLoader::NIArtemisSingleDataTypeParser::parse(bool verbose)
{
    myWorkVerbose = verbose;
    if(myWorkVerbose) {
        cout << "Parsing " << getDataName() << "... ";
    }
    string file = myParent.getFileName() + getDataName();
    LineReader reader(file);
    if(!reader.good()) {
        if(!amOptional()) {
            SErrorHandler::add(
                string("Problems on parsing '") + file + string("'."));
            return false;
        } else {
            if(myWorkVerbose) {
                cout << "not supplied (no error)." << endl;
            }
            return true;
        }
    }
    // parser-dependent init
    myInitialise();
    // skip/set names
    reader.readAll(*this);
    if(myWorkVerbose) {
        cout << "done." << endl;
    }
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
NIArtemisLoader::NIArtemisLoader(const std::string &file)
    : FileErrorReporter("artemis-network", file)
{
    // the order of process is important!
    // set1
    mySingleDataParsers.push_back(
        new NIArtemisParser_Nodes(*this, "Nodes.txt"));
    mySingleDataParsers.push_back(
        new NIArtemisParser_Links(*this, "Links.txt"));
    // signals must be loaded before the edges are splitted
    mySingleDataParsers.push_back(
        new NIArtemisParser_Signals(*this, "Signals.txt"));
    mySingleDataParsers.push_back(
        new NIArtemisParser_SignalPhases(*this, "Signal Phases.txt"));
    mySingleDataParsers.push_back(
        new NIArtemisParser_SignalGroups(*this, "Signal Groups.txt"));
    // segments must be loaded before the edges are splitted
    mySingleDataParsers.push_back(
        new NIArtemisParser_Segments(*this, "Segments.txt"));
    // edges are splitted when adding lane infomration
    mySingleDataParsers.push_back(
        new NIArtemisParser_Lanes(*this, "Lanes.txt"));
    // needed for insertion of source and destination edges
    mySingleDataParsers.push_back(
        new NIArtemisParser_HVdests(*this, "HVdests.txt"));
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
        (*i)->parse(options.getBool("v"));
    }
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIArtemisLoader.icc"
//#endif

// Local Variables:
// mode:C++
// End:

