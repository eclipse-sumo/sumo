/****************************************************************************/
/// @file    NIVisumLoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 19 Jul 2002
/// @version $Id$
///
// A loader visum-files
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
 * methods from NIVisumLoader::PositionSetter
 * ----------------------------------------------------------------------- */
NIVisumLoader::PositionSetter::PositionSetter(NIVisumLoader &parent)
        : myParent(parent)
{}


NIVisumLoader::PositionSetter::~PositionSetter()
{}


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
{}


NIVisumLoader::NIVisumSingleDataTypeParser::~NIVisumSingleDataTypeParser()
{}


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
    if (myPosition==-1) {
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
    if (dataTypeEnded(line)) {
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



SUMOReal
NIVisumLoader::NIVisumSingleDataTypeParser::getWeightedFloat(
    const std::string &name)
{
    try {
        return TplConvert<char>::my2SUMOReal(myLineParser.get(name).c_str());
    } catch (...) {}
    try {
        return TplConvert<char>::my2SUMOReal(myLineParser.get((name+"(IV)")).c_str());
    } catch (...) {}
    return -1;
}


bool
NIVisumLoader::NIVisumSingleDataTypeParser::getWeightedBool(
    const std::string &name)
{
    try {
        return TplConvert<char>::my2bool(myLineParser.get(name).c_str());
    } catch (...) {}
    try {
        return TplConvert<char>::my2bool(myLineParser.get((name+"(IV)")).c_str());
    } catch (...) {}
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
        if (node==0) {
            addError("The node '" + nodeS + "' is not known.");
        }
        return node;
    } catch (OutOfBoundsException &) {
        addError2(dataName, "", "OutOfBounds");
    } catch (NumberFormatException &) {
        addError2(dataName, "", "NumberFormat");
    } catch (UnknownElement &) {
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
    if (myLineParser.know(fieldName1)) {
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
        if (edge==0) {
            addError("The edge '" + edgeS + "' is not known.");
        }
        return edge;
    } catch (OutOfBoundsException &) {
        addError2(dataName, "", "OutOfBounds");
    } catch (NumberFormatException &) {
        addError2(dataName, "", "NumberFormat");
    } catch (UnknownElement &) {
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
    if (myLineParser.know(fieldName1)) {
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
    if (edge->getID()[0]=='-') {
        sid = edge->getID().substr(1);
    } else {
        sid = "-" + edge->getID();
    }
    if (sid.find('_')!=string::npos) {
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
    while (ret!=0) {
        // ok, this is the edge we are looking for
        if (ret->getToNode()==node) {
            return ret;
        }
        const EdgeVector &nedges = ret->getToNode()->getOutgoingEdges();
        if (nedges.size()!=1) {
            // too many edges follow
            ret = 0;
            continue;
        }
        NBEdge *next = nedges[0];
        if (ret->getID().substr(0, edgeID.length())!=next->getID().substr(0, edgeID.length())) {
            // ok, another edge is next...
            ret = 0;
            continue;
        }
        if (next->getID().substr(next->getID().length()-node->getID().length())!=node->getID()) {
            ret = 0;
            continue;
        }
        ret = next;
    }

    ret = begin;
    // hangle backward
    while (ret!=0) {
        // ok, this is the edge we are looking for
        if (ret->getFromNode()==node) {
            return ret;
        }
        const EdgeVector &nedges = ret->getFromNode()->getIncomingEdges();
        if (nedges.size()!=1) {
            // too many edges follow
            ret = 0;
            continue;
        }
        NBEdge *next = nedges[0];
        if (ret->getID().substr(0, edgeID.length())!=next->getID().substr(0, edgeID.length())) {
            // ok, another edge is next...
            ret = 0;
            continue;
        }
        if (next->getID().substr(next->getID().length()-node->getID().length())!=node->getID()) {
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
        if (edge==0) {
            addError("The edge '" + edgeS + "' is not known.");
        }
        return getNamedEdgeContinuating(edge, node);
    } catch (OutOfBoundsException &) {
        addError2(dataName, "", "OutOfBounds");
    } catch (NumberFormatException &) {
        addError2(dataName, "", "NumberFormat");
    } catch (UnknownElement &) {
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
    if (myLineParser.know(fieldName1)) {
        return getNamedEdgeContinuating(nc, dataName, fieldName1, node);
    } else {
        return getNamedEdgeContinuating(nc, dataName, fieldName2, node);
    }
}


SUMOReal
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedFloat(const std::string &fieldName)
{
    string valS = NBHelpers::normalIDRepresentation(myLineParser.get(fieldName));
    return TplConvert<char>::my2SUMOReal(valS.c_str());
}


SUMOReal
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedFloat(const std::string &fieldName,
        SUMOReal defaultValue)
{
    try {
        string valS = NBHelpers::normalIDRepresentation(myLineParser.get(fieldName));
        return TplConvert<char>::my2SUMOReal(valS.c_str());
    } catch (...) {
        return defaultValue;
    }
}


SUMOReal
NIVisumLoader::NIVisumSingleDataTypeParser::getNamedFloat(const std::string &fieldName1,
        const std::string &fieldName2)
{
    if (myLineParser.know(fieldName1)) {
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
    if (myLineParser.know(fieldName1)) {
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
    if (myLineParser.know(fieldName1)) {
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
                             NBCapacity2Lanes capacity2Lanes,
                             bool useVisumPrio)
        : FileErrorReporter("visum-network", file),
        myCapacity2Lanes(capacity2Lanes),
        myTLLogicCont(nb.getTLLogicCont()), myEdgeCont(nb.getEdgeCont())
{
    // the order of process is important!
    // set1
    mySingleDataParsers.push_back(
        new NIVisumParser_VSysTypes(*this, "VSYS", myVSysTypes));
    mySingleDataParsers.push_back(
        new NIVisumParser_Types(*this, nb.getTypeCont(), "STRECKENTYP", myCapacity2Lanes));
    mySingleDataParsers.push_back(
        new NIVisumParser_Nodes(*this, nb.getNodeCont(), "KNOTEN"));
    mySingleDataParsers.push_back(
        new NIVisumParser_Districts(*this, nb.getDistrictCont(), "BEZIRK"));


    // set2
    // two types of "strecke"
    mySingleDataParsers.push_back(
        new NIVisumParser_Edges(*this, nb.getNodeCont(), nb.getEdgeCont(), nb.getTypeCont(), "STRECKE", useVisumPrio));
    mySingleDataParsers.push_back(
        new NIVisumParser_Edges(*this, nb.getNodeCont(), nb.getEdgeCont(), nb.getTypeCont(), "STRECKEN", useVisumPrio));


    // set3
    mySingleDataParsers.push_back(
        new NIVisumParser_Connectors(*this, nb.getNodeCont(), nb.getEdgeCont(), nb.getTypeCont(), nb.getDistrictCont(), "ANBINDUNG"));
    // two types of "abbieger"
    mySingleDataParsers.push_back(
        new NIVisumParser_Turns(*this, nb.getNodeCont(), "ABBIEGEBEZIEHUNG", myVSysTypes));
    mySingleDataParsers.push_back(
        new NIVisumParser_Turns(*this, nb.getNodeCont(), "ABBIEGER", myVSysTypes));

    mySingleDataParsers.push_back(
        new NIVisumParser_EdgePolys(*this, nb.getNodeCont(), "STRECKENPOLY"));
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
    for (ParserVector::iterator i=mySingleDataParsers.begin();
            i!=mySingleDataParsers.end(); i++) {
        delete(*i);
    }
    for (NIVisumTL_Map::iterator j=myNIVisumTLs.begin();
            j!=myNIVisumTLs.end(); j++) {
        delete(j->second);
    }
}


void NIVisumLoader::load(OptionsCont &options)
{
    // open the file
    if (!myLineReader.setFileName(options.getString("visum"))) {
        throw ProcessError("Can not open visum-file '" + options.getString("visum") + "'.");

    }
    // scan the file for data positions
    PositionSetter posSet(*this);
    myLineReader.readAll(posSet);
    // go through the parsers and process all entries
    for (ParserVector::iterator i=mySingleDataParsers.begin(); i!=mySingleDataParsers.end(); i++) {
        (*i)->readUsing(myLineReader);
    }
    // build traffic lights
    for (NIVisumTL_Map::iterator j=myNIVisumTLs.begin(); j!=myNIVisumTLs.end(); j++) {
        j->second->build(myTLLogicCont);
    }
    // recheck all edge shapes
    myEdgeCont.recheckEdgeGeomsForDoublePositions();
}


bool
NIVisumLoader::checkForPosition(const std::string &line)
{
    // check whether a new data type started here
    if (line[0]!='$') {
        return true;
    }
    // check whether the data type is needed
    ParserVector::iterator i;
    for (i=mySingleDataParsers.begin();
            i!=mySingleDataParsers.end(); i++) {
        NIVisumSingleDataTypeParser *parser = (*i);
        string dataName = "$" + parser->getDataName() + ":";
        if (line.substr(0, dataName.length())==dataName) {
            parser->setStreamPosition(myLineReader.getPosition());
            parser->initLineParser(line.substr(dataName.length()));
            WRITE_MESSAGE("Found: " + dataName + " at " + toString<int>(myLineReader.getPosition()));
        }
    }
    // it is not necessary to read the whole file
    //  only the position of all needed types must be known
    // mark all are known
    size_t noUnknown = 0;
    for (i=mySingleDataParsers.begin();
            i!=mySingleDataParsers.end(); i++) {
        NIVisumSingleDataTypeParser *parser = (*i);
        // check whether the parser must be
        if (!parser->positionKnown()) {
            noUnknown++;
        }
    }
    return noUnknown!=0;
}



/****************************************************************************/

