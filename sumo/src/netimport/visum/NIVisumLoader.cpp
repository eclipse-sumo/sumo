/***************************************************************************
                          NIVisumLoader.cpp
			  A loader visum-files
                             -------------------
    project              : SUMO
    begin                : Fri, 19 Jul 2002
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
// Revision 1.5  2003/07/22 15:11:25  dkrajzew
// removed warnings
//
// Revision 1.4  2003/06/18 11:15:58  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.3  2003/05/20 09:39:14  dkrajzew
// Visum traffic light import added (by Markus Hartinger)
//
// Revision 1.2  2003/03/26 12:04:04  dkrajzew
// debugging for Vissim and Visum-imports
//
// Revision 1.1  2003/02/07 11:14:53  dkrajzew
// updated
//
// Revision 1.2  2002/10/22 10:06:22  dkrajzew
// unclosed loading of one of the types patched
//
// Revision 1.1  2002/10/16 15:44:01  dkrajzew
// initial commit for visum importing classes
//
// Revision 1.1  2002/07/25 08:41:45  dkrajzew
// Visum7.5 and Cell import added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/convert/TplConvert.h>
#include <utils/convert/ToString.h>
#include <utils/options/OptionsCont.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_VSysTypes.h"
#include "NIVisumParser_Types.h"
#include "NIVisumParser_Nodes.h"
#include "NIVisumParser_Districts.h"
#include "NIVisumParser_Edges.h"
#include "NIVisumParser_Connectors.h"
#include "NIVisumParser_Turns.h"
#include "NIVisumParser_TrafficLights.h"
#include "NIVisumParser_NodesToTrafficLights.h"
#include "NIVisumParser_SignalGroups.h"
#include "NIVisumParser_SignalGroupsToPhases.h"
#include "NIVisumParser_TurnsToSignalGroups.h"
#include "NIVisumParser_Phases.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * methods from NIVisumLoader::PositionSetter
 * ----------------------------------------------------------------------- */
NIVisumLoader::PositionSetter::PositionSetter(NIVisumLoader &parent)
    : myParent(parent)
{
}


NIVisumLoader::PositionSetter::~PositionSetter()
{
}


bool
NIVisumLoader::PositionSetter::report(const std::string &result)
{
    return myParent.checkForPosition(result);
}


/* -------------------------------------------------------------------------
 * methods from NIVisumLoader::NIVisumSingleDataTypeParser
 * ----------------------------------------------------------------------- */
NIVisumLoader::NIVisumSingleDataTypeParser::NIVisumSingleDataTypeParser(
        NIVisumLoader &parent, const std::string &dataName)
    : FileErrorReporter::Child(parent),
    myDataName(dataName), myPosition(-1)
{
}


NIVisumLoader::NIVisumSingleDataTypeParser::~NIVisumSingleDataTypeParser()
{
}


void
NIVisumLoader::NIVisumSingleDataTypeParser::setStreamPosition(long pos)
{
    myPosition = pos;
}


bool
NIVisumLoader::NIVisumSingleDataTypeParser::positionKnown() const
{
    return myPosition != -1;
}


bool
NIVisumLoader::NIVisumSingleDataTypeParser::readUsing(LineReader &reader)
{
    if(myPosition==-1) {
        return false;
    }
    MsgHandler::getMessageInstance()->inform(
        string("Parsing ") + getDataName() + string("... "));
    reader.reinit();
    reader.setPos(myPosition);
    reader.readAll(*this);
    MsgHandler::getMessageInstance()->inform("done.");
    return true;
}


const std::string &
NIVisumLoader::NIVisumSingleDataTypeParser::getDataName() const
{
    return myDataName;
}


bool
NIVisumLoader::NIVisumSingleDataTypeParser::report(
    const std::string &line)
{
    // check whether there are further data to read
    if(dataTypeEnded(line)) {
        return false;
    }
    myLineParser.parseLine(line);
    myDependentReport();
    return true;
}


bool
NIVisumLoader::NIVisumSingleDataTypeParser::dataTypeEnded(
    const std::string &msg)
{
    return (msg.length()==0||msg.at(0)=='*'||msg.at(0)=='$');
}


void
NIVisumLoader::NIVisumSingleDataTypeParser::initLineParser(
    const std::string &pattern)
{
    myLineParser.reinit(pattern);
}


void
NIVisumLoader::NIVisumSingleDataTypeParser::addError2(
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



float
NIVisumLoader::NIVisumSingleDataTypeParser::getWeightedFloat(
    const std::string &name)
{
    try {
        return TplConvert<char>::_2float(myLineParser.get(name).c_str());
    } catch (...) {
    }
    try {
        return
            TplConvert<char>::_2float(myLineParser.get(
              (name+string("(IV)"))).c_str());
    } catch (...) {
    }
    return -1;
}


bool
NIVisumLoader::NIVisumSingleDataTypeParser::getWeightedBool(
    const std::string &name)
{
    try {
        return TplConvert<char>::_2bool(myLineParser.get(name).c_str());
    } catch (...) {
    }
    try {
        return
            TplConvert<char>::_2bool(myLineParser.get(
            (name+string("(IV)"))).c_str());
    } catch (...) {
    }
    return false;
}



 /* -------------------------------------------------------------------------
 * methods from NIVisumLoader
 * ----------------------------------------------------------------------- */
NIVisumLoader::NIVisumLoader(const std::string &file,
                             NBCapacity2Lanes capacity2Lanes)
    : FileErrorReporter("visum-network", file),
    _capacity2Lanes(capacity2Lanes)
{
    // the order of process is important!
    // set1
    mySingleDataParsers.push_back(
        new NIVisumParser_VSysTypes(*this, "VSYS", myVSysTypes));
    mySingleDataParsers.push_back(
        new NIVisumParser_Types(*this, "STRECKENTYP", _capacity2Lanes));
    mySingleDataParsers.push_back(
        new NIVisumParser_Nodes(*this, "KNOTEN"));
    mySingleDataParsers.push_back(
        new NIVisumParser_Districts(*this, "BEZIRK"));
    // set2
    mySingleDataParsers.push_back(
        new NIVisumParser_Edges(*this, "STRECKEN"));
    // set3
    mySingleDataParsers.push_back(
        new NIVisumParser_Connectors(*this, "ANBINDUNG"));
    mySingleDataParsers.push_back(
        new NIVisumParser_Turns(*this, "ABBIEGEBEZIEHUNG", myVSysTypes));
	// set4
	mySingleDataParsers.push_back(
		new NIVisumParser_TrafficLights(*this, "LSA", myNIVisumTLs));
	mySingleDataParsers.push_back(
		new NIVisumParser_NodesToTrafficLights(*this, "KNOTENZULSA", myNIVisumTLs));
	mySingleDataParsers.push_back(
		new NIVisumParser_SignalGroups(*this, "LSASIGNALGRUPPE", myNIVisumTLs));
	mySingleDataParsers.push_back(
		new NIVisumParser_TurnsToSignalGroups(*this, "ABBZULSASIGNALGRUPPE", myNIVisumTLs));
	mySingleDataParsers.push_back(
		new NIVisumParser_Phases(*this, "LSAPHASE", myNIVisumTLs));
	mySingleDataParsers.push_back(
		new NIVisumParser_SignalGroupsToPhases(*this, "LSASIGNALGRUPPEZULSAPHASE", myNIVisumTLs));
}


NIVisumLoader::~NIVisumLoader()
{
    for( ParserVector::iterator i=mySingleDataParsers.begin();
         i!=mySingleDataParsers.end(); i++) {
        delete (*i);
    }
    for( NIVisumTL_Map::iterator j=myNIVisumTLs.begin();
         j!=myNIVisumTLs.end(); j++) {
        delete (j->second);
    }
}


void NIVisumLoader::load(OptionsCont &options)
{
//    bool verbose = options.getBool("v");
    // open the file
    if(!myLineReader.setFileName(options.getString("visum"))) {
        MsgHandler::getErrorInstance()->inform(
            string("Can not open visum-file '")
            + options.getString("visum")
            + string("'."));
        throw ProcessError();
    }
    // scan the file for data positions
    PositionSetter posSet(*this);
    myLineReader.readAll(posSet);
    // go through the parsers and process every entry
    for( ParserVector::iterator i=mySingleDataParsers.begin();
         i!=mySingleDataParsers.end(); i++) {
        (*i)->readUsing(myLineReader);
    }
    // build traffic lights
	for(NIVisumTL_Map::iterator j=myNIVisumTLs.begin();
		j!=myNIVisumTLs.end(); j++) {
		j->second->build();
	}
}


bool
NIVisumLoader::checkForPosition(const std::string &line)
{
    // check whether a new data type started here
    if(line[0]!='$') {
        return true;
    }
    // check whether the data type is needed
    ParserVector::iterator i;
    for( i=mySingleDataParsers.begin();
         i!=mySingleDataParsers.end(); i++) {
        NIVisumSingleDataTypeParser *parser = (*i);
        string dataName = string("$") + parser->getDataName() + string(":");
        if(line.substr(0, dataName.length())==dataName) {
            parser->setStreamPosition(myLineReader.getPosition());
            parser->initLineParser(line.substr(dataName.length()));
            MsgHandler::getMessageInstance()->inform(
                string("Found: ") + dataName + string(" at ")
                + toString<int>(myLineReader.getPosition()));
        }
    }
    // it is not necessary to rea the whole file
    //  only the position of all needed types must be known
    // mark all are known
    size_t noUnknown = 0;
    for( i=mySingleDataParsers.begin();
         i!=mySingleDataParsers.end(); i++) {
        NIVisumSingleDataTypeParser *parser = (*i);
        // check whether the parser must be
        if(!parser->positionKnown()) {
            noUnknown++;
        }
    }
    return noUnknown!=0;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVisumLoader.icc"
//#endif

// Local Variables:
// mode:C++
// End:

