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
// Revision 1.18  2006/07/06 06:16:38  dkrajzew
// further debugging of VISUM-import (unfinished)
//
// Revision 1.17  2006/06/13 13:16:00  dkrajzew
// patching problems on loading split lanes and tls
//
// Revision 1.16  2006/05/15 05:55:26  dkrajzew
// added consective process messages
//
// Revision 1.16  2006/05/08 11:13:50  dkrajzew
// added consective process messages
//
// Revision 1.15  2006/04/11 11:01:37  dkrajzew
// extended the message-API to (re)allow process output
//
// Revision 1.14  2006/03/28 09:12:43  dkrajzew
// lane connections for unsplitted lanes implemented, further refactoring
//
// Revision 1.13  2006/03/28 06:15:48  dkrajzew
// refactoring and extending the Visum-import
//
// Revision 1.12  2006/03/08 13:02:27  dkrajzew
// some further work on converting geo-coordinates
//
// Revision 1.11  2006/02/23 11:23:53  dkrajzew
// VISION import added
//
// Revision 1.10  2005/10/07 11:41:01  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.9  2005/09/23 06:03:50  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.8  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/04/27 12:24:41  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.6  2004/11/23 10:23:51  dkrajzew
// debugging
//
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

#include <string>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <netbuild/NBNetBuilder.h>
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
#include "NIVisumParser_EdgePolys.h"
#include "NIVisumParser_Phases.h"
#include "NIVisumParser_LanesConnections.h"
#include "NIVisumParser_Lanes.h"

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
    MsgHandler::getMessageInstance()->beginProcessMsg("Parsing " + getDataName() + "...");
    reader.reinit();
    reader.setPos(myPosition);
    reader.readAll(*this);
    MsgHandler::getMessageInstance()->endProcessMsg("done.");
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
            addError("The definition of the " + type + " '" + id + "' is malicious (" + exception + ").");
        } else {
            addError("The definition of the " + type + " '" + id + "' is malicious.");
        }
    } else {
        addError("Something is wrong with a " + type + " (unknown id).");
    }
}



SUMOReal
NIVisumLoader::NIVisumSingleDataTypeParser::getWeightedFloat(
    const std::string &name)
{
    try {
        return TplConvert<char>::_2SUMOReal(myLineParser.get(name).c_str());
    } catch (...) {
    }
    try {
        return TplConvert<char>::_2SUMOReal(myLineParser.get((name+"(IV)")).c_str());
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
        return TplConvert<char>::_2bool(myLineParser.get((name+"(IV)")).c_str());
    } catch (...) {
    }
    return false;
}


NBNode *
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedNode(NBNodeCont &nc,
                                                         const std::string &dataName,
                                                         const std::string &fieldName)
{
    try {
        string nodeS = NBHelpers::normalIDRepresentation(myLineParser.get(fieldName));
        NBNode *node = nc.retrieve(nodeS);
        if(node==0) {
            addError("The node '" + nodeS + "' is not known.");
        }
        return node;
    } catch (OutOfBoundsException) {
        addError2(dataName, "", "OutOfBounds");
    } catch (NumberFormatException) {
        addError2(dataName, "", "NumberFormat");
    } catch (UnknownElement) {
        addError2(dataName, "", "UnknownElement");
    }
    return 0;
}


NBNode *
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedNode(NBNodeCont &nc,
                                                         const std::string &dataName,
                                                         const std::string &fieldName1,
                                                         const std::string &fieldName2)
{
    if(myLineParser.know(fieldName1)) {
        return getNamedNode(nc, dataName, fieldName1);
    } else {
        return getNamedNode(nc, dataName, fieldName2);
    }
}


NBEdge *
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedEdge(NBEdgeCont &nc,
                                                         const std::string &dataName,
                                                         const std::string &fieldName)
{
    try {
        string edgeS = NBHelpers::normalIDRepresentation(myLineParser.get(fieldName));
        NBEdge *edge = nc.retrieve(edgeS);
        if(edge==0) {
            addError("The edge '" + edgeS + "' is not known.");
        }
        return edge;
    } catch (OutOfBoundsException) {
        addError2(dataName, "", "OutOfBounds");
    } catch (NumberFormatException) {
        addError2(dataName, "", "NumberFormat");
    } catch (UnknownElement) {
        addError2(dataName, "", "UnknownElement");
    }
    return 0;
}


NBEdge *
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedEdge(NBEdgeCont &nc,
                                                         const std::string &dataName,
                                                         const std::string &fieldName1,
                                                         const std::string &fieldName2)
{
    if(myLineParser.know(fieldName1)) {
        return getNamedEdge(nc, dataName, fieldName1);
    } else {
        return getNamedEdge(nc, dataName, fieldName2);
    }
}



NBEdge *
NIVisumLoader::NIVisumSingleDataTypeParser::getReversedContinuating(
        NBEdgeCont &nc, NBEdge *edge, NBNode *node)
{
    string sid;
    if(edge->getID()[0]=='-') {
        sid = edge->getID().substr(1);
    } else {
        sid = "-" + edge->getID();
    }
    if(sid.find('_')!=string::npos) {
        sid = sid.substr(0, sid.find('_'));
    }
    return getNamedEdgeContinuating(nc, sid,  node);
}


NBEdge *
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedEdgeContinuating(
        NBEdge *begin, NBNode *node)
{
    NBEdge *ret = begin;
    string edgeID = ret->getID();
    // hangle forward
    while(ret!=0) {
        // ok, this is the edge we are looking for
        if(ret->getToNode()==node) {
            return ret;
        }
        const EdgeVector &nedges = ret->getToNode()->getOutgoingEdges();
        if(nedges.size()!=1) {
            // too many edges follow
            ret = 0;
            continue;
        }
        NBEdge *next = nedges[0];
        if(ret->getID().substr(0, edgeID.length())!=next->getID().substr(0, edgeID.length())) {
            // ok, another edge is next...
            ret = 0;
            continue;
        }
        if(next->getID().substr(next->getID().length()-node->getID().length())!=node->getID()) {
            ret = 0;
            continue;
        }
        ret = next;
    }

    ret = begin;
    // hangle backward
    while(ret!=0) {
        // ok, this is the edge we are looking for
        if(ret->getFromNode()==node) {
            return ret;
        }
        const EdgeVector &nedges = ret->getFromNode()->getIncomingEdges();
        if(nedges.size()!=1) {
            // too many edges follow
            ret = 0;
            continue;
        }
        NBEdge *next = nedges[0];
        if(ret->getID().substr(0, edgeID.length())!=next->getID().substr(0, edgeID.length())) {
            // ok, another edge is next...
            ret = 0;
            continue;
        }
        if(next->getID().substr(next->getID().length()-node->getID().length())!=node->getID()) {
            ret = 0;
            continue;
        }
        ret = next;
    }
    return 0;
}


NBEdge *
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedEdgeContinuating(
        NBEdgeCont &nc, const std::string &name, NBNode *node)
{
    return getNamedEdgeContinuating(nc.retrieve(name), node);
}


NBEdge *
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedEdgeContinuating(
        NBEdgeCont &nc, const std::string &dataName, const std::string &fieldName,
        NBNode *node)
{
    try {
        string edgeS = NBHelpers::normalIDRepresentation(myLineParser.get(fieldName));
        NBEdge *edge = nc.retrieve(edgeS);
        if(edge==0) {
            addError("The edge '" + edgeS + "' is not known.");
        }
        return getNamedEdgeContinuating(edge, node);
    } catch (OutOfBoundsException) {
        addError2(dataName, "", "OutOfBounds");
    } catch (NumberFormatException) {
        addError2(dataName, "", "NumberFormat");
    } catch (UnknownElement) {
        addError2(dataName, "", "UnknownElement");
    }
    return 0;
}


NBEdge *
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedEdgeContinuating(
        NBEdgeCont &nc, const std::string &dataName,
        const std::string &fieldName1, const std::string &fieldName2,
        NBNode *node)
{
    if(myLineParser.know(fieldName1)) {
        return getNamedEdgeContinuating(nc, dataName, fieldName1, node);
    } else {
        return getNamedEdgeContinuating(nc, dataName, fieldName2, node);
    }
}


SUMOReal
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedFloat(const std::string &fieldName)
{
    string valS = NBHelpers::normalIDRepresentation(myLineParser.get(fieldName));
    return TplConvert<char>::_2SUMOReal(valS.c_str());
}


SUMOReal
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedFloat(const std::string &fieldName,
                                                          SUMOReal defaultValue)
{
    try {
        string valS = NBHelpers::normalIDRepresentation(myLineParser.get(fieldName));
        return TplConvert<char>::_2SUMOReal(valS.c_str());
    } catch(...) {
        return defaultValue;
    }
}


SUMOReal
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedFloat(const std::string &fieldName1,
                                                          const std::string &fieldName2)
{
    if(myLineParser.know(fieldName1)) {
        return getNamedFloat(fieldName1);
    } else {
        return getNamedFloat(fieldName2);
    }
}


SUMOReal
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedFloat(const std::string &fieldName1,
                                                          const std::string &fieldName2,
                                                          SUMOReal defaultValue)
{
    if(myLineParser.know(fieldName1)) {
        return getNamedFloat(fieldName1, defaultValue);
    } else {
        return getNamedFloat(fieldName2, defaultValue);
    }
}


std::string
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedString(const std::string &fieldName)
{
    return NBHelpers::normalIDRepresentation(myLineParser.get(fieldName));
}


std::string
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedString(const std::string &fieldName1,
                                                           const std::string &fieldName2)
{
    if(myLineParser.know(fieldName1)) {
        return getNamedString(fieldName1);
    } else {
        return getNamedString(fieldName2);
    }
}



 /* -------------------------------------------------------------------------
 * methods from NIVisumLoader
 * ----------------------------------------------------------------------- */
NIVisumLoader::NIVisumLoader(NBNetBuilder &nb,
                             const std::string &file,
                             NBCapacity2Lanes capacity2Lanes, projPJ pj)
    : FileErrorReporter("visum-network", file),
    _capacity2Lanes(capacity2Lanes),
    myTLLogicCont(nb.getTLLogicCont()), myEdgeCont(nb.getEdgeCont()),
    myProjection(pj)
{
    // the order of process is important!
    // set1
    mySingleDataParsers.push_back(
        new NIVisumParser_VSysTypes(*this, "VSYS", myVSysTypes));
    mySingleDataParsers.push_back(
        new NIVisumParser_Types(*this, nb.getTypeCont(), "STRECKENTYP", _capacity2Lanes));
    mySingleDataParsers.push_back(
        new NIVisumParser_Nodes(*this, nb.getNodeCont(), pj, "KNOTEN"));
    mySingleDataParsers.push_back(
        new NIVisumParser_Districts(*this, nb.getDistrictCont(), pj, "BEZIRK"));


    // set2
        // two types of "strecke"
    mySingleDataParsers.push_back(
        new NIVisumParser_Edges(*this, nb.getNodeCont(), nb.getEdgeCont(), nb.getTypeCont(), "STRECKE"));
    mySingleDataParsers.push_back(
        new NIVisumParser_Edges(*this, nb.getNodeCont(), nb.getEdgeCont(), nb.getTypeCont(), "STRECKEN"));


    // set3
    mySingleDataParsers.push_back(
        new NIVisumParser_Connectors(*this, nb.getNodeCont(), nb.getEdgeCont(), nb.getTypeCont(), nb.getDistrictCont(), "ANBINDUNG"));
        // two types of "abbieger"
    mySingleDataParsers.push_back(
        new NIVisumParser_Turns(*this, nb.getNodeCont(), "ABBIEGEBEZIEHUNG", myVSysTypes));
    mySingleDataParsers.push_back(
        new NIVisumParser_Turns(*this, nb.getNodeCont(), "ABBIEGER", myVSysTypes));

    mySingleDataParsers.push_back(
        new NIVisumParser_EdgePolys(*this, nb.getNodeCont(), pj, "STRECKENPOLY"));
    mySingleDataParsers.push_back(
        new NIVisumParser_Lanes(*this, nb.getNodeCont(), nb.getEdgeCont(), nb.getDistrictCont(), "FAHRSTREIFEN"));


	// set4
        // two types of lsa
	mySingleDataParsers.push_back(
		new NIVisumParser_TrafficLights(*this, "LSA", myNIVisumTLs));
	mySingleDataParsers.push_back(
		new NIVisumParser_TrafficLights(*this, "SIGNALANLAGE", myNIVisumTLs));
        // two types of knotenzulsa
	mySingleDataParsers.push_back(
		new NIVisumParser_NodesToTrafficLights(*this, nb.getNodeCont(), "KNOTENZULSA", myNIVisumTLs));
	mySingleDataParsers.push_back(
		new NIVisumParser_NodesToTrafficLights(*this, nb.getNodeCont(), "SIGNALANLAGEZUKNOTEN", myNIVisumTLs));
        // two types of signalgruppe
	mySingleDataParsers.push_back(
		new NIVisumParser_SignalGroups(*this, "LSASIGNALGRUPPE", myNIVisumTLs));
	mySingleDataParsers.push_back(
		new NIVisumParser_SignalGroups(*this, "SIGNALGRUPPE", myNIVisumTLs));
        // two types of ABBZULSASIGNALGRUPPE
    mySingleDataParsers.push_back(
		new NIVisumParser_TurnsToSignalGroups(*this, nb.getNodeCont(), nb.getEdgeCont(), "ABBZULSASIGNALGRUPPE", myNIVisumTLs));
    mySingleDataParsers.push_back(
		new NIVisumParser_TurnsToSignalGroups(*this, nb.getNodeCont(), nb.getEdgeCont(), "SIGNALGRUPPEZUABBIEGER", myNIVisumTLs));
        // two types of LSAPHASE
	mySingleDataParsers.push_back(
		new NIVisumParser_Phases(*this, "LSAPHASE", myNIVisumTLs));
	mySingleDataParsers.push_back(
		new NIVisumParser_Phases(*this, "PHASE", myNIVisumTLs));

	mySingleDataParsers.push_back(
		new NIVisumParser_SignalGroupsToPhases(*this, "LSASIGNALGRUPPEZULSAPHASE", myNIVisumTLs));
    mySingleDataParsers.push_back(
        new NIVisumParser_LanesConnections(*this, nb.getNodeCont(), nb.getEdgeCont(), "FAHRSTREIFENABBIEGER"));

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
    // open the file
    if(!myLineReader.setFileName(options.getString("visum"))) {
        MsgHandler::getErrorInstance()->inform("Can not open visum-file '" + options.getString("visum") + "'.");
        throw ProcessError();
    }
    // scan the file for data positions
    PositionSetter posSet(*this);
    myLineReader.readAll(posSet);
    // go through the parsers and process all entries
    for( ParserVector::iterator i=mySingleDataParsers.begin(); i!=mySingleDataParsers.end(); i++) {
        (*i)->readUsing(myLineReader);
    }
    // build traffic lights
	for(NIVisumTL_Map::iterator j=myNIVisumTLs.begin(); j!=myNIVisumTLs.end(); j++) {
		j->second->build(myTLLogicCont);
	}
    // recheck all edge shapes
    myEdgeCont.recheckEdgeGeomsForDoublePositions();
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
        string dataName = "$" + parser->getDataName() + ":";
        if(line.substr(0, dataName.length())==dataName) {
            parser->setStreamPosition(myLineReader.getPosition());
            parser->initLineParser(line.substr(dataName.length()));
            WRITE_MESSAGE("Found: " + dataName + " at " + toString<int>(myLineReader.getPosition()));
        }
    }
    // it is not necessary to read the whole file
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

// Local Variables:
// mode:C++
// End:

