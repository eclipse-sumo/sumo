/****************************************************************************/
/// @file    NIImporter_Vissim.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
#include <fstream>
#include <utils/common/StringUtils.h>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <netbuild/NBNetBuilder.h>
#include "NIImporter_Vissim.h"
#include "typeloader/NIVissimSingleTypeParser_Simdauer.h"
#include "typeloader/NIVissimSingleTypeParser_Startuhrzeit.h"
#include "typeloader/NIVissimSingleTypeParser_DynUml.h"
#include "typeloader/NIVissimSingleTypeParser_Streckendefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Verbindungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Richtungsentscheidungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Routenentscheidungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_VWunschentscheidungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Langsamfahrbereichdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Zuflussdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Fahrzeugtypdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Fahrzeugklassendefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Verkehrszusammensetzungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Laengenverteilungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Zeitenverteilungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Lichtsignalanlagendefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Signalgruppendefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Stopschilddefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Knotendefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Signalgeberdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Detektordefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Liniendefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Haltestellendefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Reisezeitmessungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Querschnittsmessungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Messungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Verlustzeitmessungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Stauzaehlerdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Richtungspfeildefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Parkplatzdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Fahrverhaltendefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Streckentypdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Kennungszeile.h"
#include "typeloader/NIVissimSingleTypeParser_Fensterdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Auswertungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Zusammensetzungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Startzufallszahl.h"
#include "typeloader/NIVissimSingleTypeParser_SimRate.h"
#include "typeloader/NIVissimSingleTypeParser_Zeitschrittfaktor.h"
#include "typeloader/NIVissimSingleTypeParser_Linksverkehr.h"
#include "typeloader/NIVissimSingleTypeParser_Stauparameterdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Gelbverhaltendefinition.h"
#include "typeloader/NIVissimSingleTypeParser_LSAKopplungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Gefahrwarnungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_TEAPACDefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Netzobjektdefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Fahrtverlaufdateien.h"
#include "typeloader/NIVissimSingleTypeParser_Emission.h"
#include "typeloader/NIVissimSingleTypeParser_Einheitendefinition.h"
#include "typeloader/NIVissimSingleTypeParser__XVerteilungsdefinition.h"
#include "typeloader/NIVissimSingleTypeParser__XKurvedefinition.h"
#include "typeloader/NIVissimSingleTypeParser_Kantensperrung.h"
#include "typeloader/NIVissimSingleTypeParser_Rautedefinition.h"


#include "tempstructs/NIVissimTL.h"
#include "tempstructs/NIVissimClosures.h"
#include "tempstructs/NIVissimSource.h"
#include "tempstructs/NIVissimTrafficDescription.h"
#include "tempstructs/NIVissimVehTypeClass.h"
#include "tempstructs/NIVissimConnection.h"
#include "tempstructs/NIVissimDisturbance.h"
#include "tempstructs/NIVissimConnectionCluster.h"
#include "tempstructs/NIVissimNodeDef.h"
#include "tempstructs/NIVissimEdge.h"
#include "tempstructs/NIVissimDistrictConnection.h"
#include "tempstructs/NIVissimVehicleType.h"

#include <netbuild/NBEdgeCont.h> // !!! only for debugging purposes

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods (interface in this case)
// ---------------------------------------------------------------------------
void
NIImporter_Vissim::loadNetwork(const OptionsCont &oc, NBNetBuilder &nb) {
    if (!oc.isSet("vissim")) {
        return;
    }
    // load the visum network
    NIImporter_Vissim loader(nb, oc.getString("vissim"));
    loader.load(oc);
}


/* -------------------------------------------------------------------------
 * NIImporter_Vissim::VissimSingleTypeParser-methods
 * ----------------------------------------------------------------------- */
NIImporter_Vissim::VissimSingleTypeParser::VissimSingleTypeParser(NIImporter_Vissim &parent)
        : myVissimParent(parent) {}


NIImporter_Vissim::VissimSingleTypeParser::~VissimSingleTypeParser() {}


std::string
NIImporter_Vissim::VissimSingleTypeParser::myRead(std::istream &from) {
    std::string tmp;
    from >> tmp;
    return StringUtils::to_lower_case(tmp);
}



std::string
NIImporter_Vissim::VissimSingleTypeParser::readEndSecure(std::istream &from,
        const std::string &excl) {
    std::string myExcl = StringUtils::to_lower_case(excl);
    std::string tmp = myRead(from);
    if (tmp=="") {
        return "DATAEND";
    }
    if (tmp!=myExcl
            &&
            (tmp.substr(0, 2)=="--"||!myVissimParent.admitContinue(tmp))
       ) {
        return "DATAEND";
    }
    return StringUtils::to_lower_case(tmp);
}


std::string
NIImporter_Vissim::VissimSingleTypeParser::readEndSecure(std::istream &from,
        const std::vector<std::string> &excl) {
    std::vector<std::string> myExcl;
    std::vector<std::string>::const_iterator i;
    for (i=excl.begin(); i!=excl.end(); i++) {
        std::string mes = StringUtils::to_lower_case(*i);
        myExcl.push_back(mes);
    }
    std::string tmp = myRead(from);
    if (tmp=="") {
        return "DATAEND";
    }

    bool equals = false;
    for (i=myExcl.begin(); i!=myExcl.end()&&!equals; i++) {
        if ((*i)==tmp) {
            equals = true;
        }
    }
    if (!equals
            &&
            (tmp.substr(0, 2)=="--"||!myVissimParent.admitContinue(tmp))
       ) {
        return "DATAEND";
    }
    return StringUtils::to_lower_case(tmp);
}


std::string
NIImporter_Vissim::VissimSingleTypeParser::overrideOptionalLabel(std::istream &from,
        const std::string &tag) {
    std::string tmp;
    if (tag=="") {
        tmp = myRead(from);
    } else {
        tmp = tag;
    }
    if (tmp=="beschriftung") {
        tmp = myRead(from);
        if (tmp=="keine") {
            from >> tmp;
        }
        tmp = myRead(from);
        tmp = myRead(from);
    }
    return tmp;
}


Position2D
NIImporter_Vissim::VissimSingleTypeParser::getPosition2D(std::istream &from) {
    SUMOReal x, y;
    from >> x; // type-checking is missing!
    from >> y; // type-checking is missing!
    return Position2D(x, y);
}


IntVector
NIImporter_Vissim::VissimSingleTypeParser::parseAssignedVehicleTypes(
    std::istream &from, const std::string &next) {
    std::string tmp = readEndSecure(from);
    IntVector ret;
    if (tmp=="alle") {
        ret.push_back(-1);
        return ret;
    }
    while (tmp!="DATAEND"&&tmp!=next) {
        ret.push_back(TplConvert<char>::_2int(tmp.c_str()));
        tmp = readEndSecure(from);
    }
    return ret;
}


NIVissimExtendedEdgePoint
NIImporter_Vissim::VissimSingleTypeParser::readExtEdgePointDef(
    std::istream &from) {
    std::string tag;
    from >> tag; // "Strecke"
    int edgeid;
    from >> edgeid; // type-checking is missing!
    from >> tag; // "Spuren"
    IntVector lanes;
    while (tag!="bei") {
        tag = readEndSecure(from);
        if (tag!="bei") {
            int lane = TplConvert<char>::_2int(tag.c_str());
            lanes.push_back(lane-1);
        }
    }
    SUMOReal position;
    from >> position;
    IntVector dummy;
    return NIVissimExtendedEdgePoint(edgeid, lanes, position, dummy);
}


std::string
NIImporter_Vissim::VissimSingleTypeParser::readName(std::istream &from) {
    std::string name;
    from >> name;
    if (name[0]=='"') {
        while (name[name.length()-1]!='"') {
            std::string tmp;
            from >> tmp;
            name = name + " " + tmp;
        }
        name = name.substr(1, name.length()-2);
    }
    return StringUtils::convertUmlaute(name);
}


void
NIImporter_Vissim::VissimSingleTypeParser::readUntil(std::istream &from,
        const std::string &name) {
    std::string tag;
    while (tag!=name) {
        tag = myRead(from);
    }
}

bool
NIImporter_Vissim::VissimSingleTypeParser::skipOverreading(std::istream &from,
        const std::string &name) {
    std::string tag;
    while (tag!=name) {
        tag = myRead(from);
    }
    while (tag!="DATAEND") {
        tag = readEndSecure(from);
    }
    return true;
}



/* -------------------------------------------------------------------------
 * NIImporter_Vissim-methods
 * ----------------------------------------------------------------------- */
NIImporter_Vissim::NIImporter_Vissim(NBNetBuilder &nb, const std::string &file)
        : myNetBuilder(nb) {
    insertKnownElements();
    buildParsers();
    myColorMap["blau"] = RGBColor((SUMOReal) .3, (SUMOReal) 0.3, (SUMOReal) 1);
    myColorMap["gelb"] = RGBColor(1, 1, 0);
    myColorMap["grau"] = RGBColor((SUMOReal) .5, (SUMOReal) 0.5, (SUMOReal) .5);
    myColorMap["lila"] = RGBColor(1, 0, 1);
    myColorMap["gruen"] = RGBColor(0, 1, 0);
    myColorMap["rot"] = RGBColor(1, 0, 0);
    myColorMap["schwarz"] = RGBColor(0, 0, 0);
    myColorMap["tuerkis"] = RGBColor(0, 1, 1);
    myColorMap["weiss"] = RGBColor(1, 1, 1);
    myColorMap["keine"] = RGBColor(1, 1, 1);
}




NIImporter_Vissim::~NIImporter_Vissim() {
    NIVissimAbstractEdge::clearDict();
    NIVissimClosures::clearDict();
    NIVissimDistrictConnection::clearDict();
    NIVissimDisturbance::clearDict();
    NIVissimNodeCluster::clearDict();
    NIVissimNodeDef::clearDict();
    NIVissimSource::clearDict();
    NIVissimTL::clearDict();
    NIVissimTL::NIVissimTLSignal::clearDict();
    NIVissimTL::NIVissimTLSignalGroup::clearDict();
    NIVissimTrafficDescription::clearDict();
    NIVissimVehTypeClass::clearDict();
    NIVissimVehicleType::clearDict();
    NIVissimConnectionCluster::clearDict();
    NIVissimEdge::clearDict();
    NIVissimAbstractEdge::clearDict();
    NIVissimConnection::clearDict();
    for (ToParserMap::iterator i=myParsers.begin(); i!=myParsers.end(); i++) {
        delete(*i).second;
    }
}


void
NIImporter_Vissim::load(const OptionsCont &options) {
    // load file contents
    // try to open the file
    std::ifstream strm(options.getString("vissim").c_str());
    if (!strm.good()) {
        MsgHandler::getErrorInstance()->inform("The vissim-file '" + options.getString("vissim") + "' was not found.");
        return;
    }
    if (!readContents(strm)) {
        return;
    }
    postLoadBuild(options.getFloat("vissim.offset"));
}


bool
NIImporter_Vissim::admitContinue(const std::string &tag) {
    ToElemIDMap::const_iterator i=myKnownElements.find(tag);
    if (i==myKnownElements.end()) {
        return true;
    }
    myLastSecure = tag;
    return false;
}


bool
NIImporter_Vissim::readContents(std::istream &strm) {
    // read contents
    bool ok = true;
    while (strm.good()&&ok) {
        std::string tag;
        if (myLastSecure!="") {
            tag = myLastSecure;
        } else {
            strm >> tag;
        }
        myLastSecure = "";
        bool parsed = false;
        while (!parsed&&strm.good()&&ok) {
            ToElemIDMap::iterator i=myKnownElements.find(StringUtils::to_lower_case(tag));
            if (i!=myKnownElements.end()) {
                ToParserMap::iterator j=myParsers.find((*i).second);
                if (j!=myParsers.end()) {
                    VissimSingleTypeParser *parser = (*j).second;
                    ok = parser->parse(strm);
                    parsed = true;
                }
            }
            if (!parsed) {
                std::string line;
                size_t pos;
                do {
                    pos = strm.tellg();
                    getline(strm, line);
                } while (strm.good()&&(line==""||line[0]==' '||line[0]=='-'));
                if (!strm.good()) {
                    return true;
                }
                strm.seekg(pos);
                strm >> tag;
            }
        }
    }
    return ok;
}


void
NIImporter_Vissim::postLoadBuild(SUMOReal offset) {
    // close the loading process
    NIVissimBoundedClusterObject::closeLoading();
    NIVissimConnection::dict_assignToEdges();
    NIVissimDisturbance::dict_SetDisturbances();
    // build district->connections map
    NIVissimDistrictConnection::dict_BuildDistrictConnections();
    // build clusters around nodes
//    NIVissimNodeDef::buildNodeClusters();
    // build node clusters around traffic lights
//    NIVissimTL::buildNodeClusters();

    // when connections or disturbances are left, build nodes around them

    // try to assign connection clusters to nodes
    //  only left connections will be processed in
    //   buildConnectionClusters & join
//30.4. brauchen wir noch!    NIVissimNodeDef::dict_assignConnectionsToNodes();

    // build clusters of connections with the same direction and a similar position along the streets
    NIVissimEdge::buildConnectionClusters();
    // check whether further nodes (connection clusters by now) must be added
    NIVissimDistrictConnection::dict_CheckEdgeEnds();

    // join clusters when overlapping (different streets are possible)
    NIVissimEdge::dict_checkEdges2Join();
    NIVissimConnectionCluster::joinBySameEdges(offset);
//    NIVissimConnectionCluster::joinByDisturbances(offset);

//    NIVissimConnectionCluster::addTLs(offset);

    // build nodes from clusters
    NIVissimNodeCluster::setCurrentVirtID(NIVissimNodeDef::getMaxID());
    NIVissimConnectionCluster::buildNodeClusters();

//    NIVissimNodeCluster::dict_recheckEdgeChanges();

    NIVissimNodeCluster::buildNBNodes(myNetBuilder.getNodeCont());
    NIVissimDistrictConnection::dict_BuildDistrictNodes(
        myNetBuilder.getDistrictCont(), myNetBuilder.getNodeCont());
    NIVissimEdge::dict_propagateSpeeds();
    NIVissimEdge::dict_buildNBEdges(myNetBuilder.getDistrictCont(),
                                    myNetBuilder.getNodeCont(), myNetBuilder.getEdgeCont(),
                                    offset);
    if (OptionsCont::getOptions().getBool("vissim.report-unset-speeds")) {
        NIVissimEdge::reportUnsetSpeeds();
    }
    NIVissimDistrictConnection::dict_BuildDistricts(myNetBuilder.getDistrictCont(),
            myNetBuilder.getEdgeCont(), myNetBuilder.getNodeCont());
    NIVissimConnection::dict_buildNBEdgeConnections(myNetBuilder.getEdgeCont());
    NIVissimNodeCluster::dict_addDisturbances(myNetBuilder.getDistrictCont(),
            myNetBuilder.getNodeCont(), myNetBuilder.getEdgeCont());
    NIVissimTL::dict_SetSignals(myNetBuilder.getTLLogicCont(),
                                myNetBuilder.getEdgeCont());

}


void
NIImporter_Vissim::insertKnownElements() {
    myKnownElements["kennung"] = VE_Kennungszeile;
    myKnownElements["zufallszahl"] = VE_Startzufallszahl;
    myKnownElements["simulationsdauer"] = VE_Simdauer;
    myKnownElements["startuhrzeit"] = VE_Startuhrzeit;
    myKnownElements["simulationsrate"] = VE_SimRate;
    myKnownElements["zeitschritt"] = VE_Zeitschrittfaktor;
    myKnownElements["linksverkehr"] = VE_Linksverkehr;
    myKnownElements["dynuml"] = VE_DynUml;
    myKnownElements["stau"] = VE_Stauparameterdefinition;
    myKnownElements["gelbverhalten"] = VE_Gelbverhaltendefinition;
    myKnownElements["strecke"] = VE_Streckendefinition;
    myKnownElements["verbindung"] = VE_Verbindungsdefinition;
    myKnownElements["richtungsentscheidung"] = VE_Richtungsentscheidungsdefinition;
    myKnownElements["routenentscheidung"] = VE_Routenentscheidungsdefinition;
    myKnownElements["vwunschentscheidung"] = VE_VWunschentscheidungsdefinition;
    myKnownElements["langsamfahrbereich"] = VE_Langsamfahrbereichdefinition;
    myKnownElements["zufluss"] = VE_Zuflussdefinition;
    myKnownElements["fahrzeugtyp"] = VE_Fahrzeugtypdefinition;
    myKnownElements["fahrzeugklasse"] = VE_Fahrzeugklassendefinition;
    myKnownElements["zusammensetzung"] = VE_Verkehrszusammensetzungsdefinition;
    myKnownElements["vwunsch"] = VE_Geschwindigkeitsverteilungsdefinition;
    myKnownElements["laengen"] = VE_Laengenverteilungsdefinition;
    myKnownElements["zeiten"] = VE_Zeitenverteilungsdefinition;
    myKnownElements["baujahre"] = VE_Baujahrverteilungsdefinition;
    myKnownElements["leistungen"] = VE_Laufleistungsverteilungsdefinition;
    myKnownElements["massen"] = VE_Massenverteilungsdefinition;
    myKnownElements["leistungen"] = VE_Leistungsverteilungsdefinition;
    myKnownElements["maxbeschleunigung"] = VE_Maxbeschleunigungskurvedefinition;
    myKnownElements["wunschbeschleunigung"] = VE_Wunschbeschleunigungskurvedefinition;
    myKnownElements["maxverzoegerung"] = VE_Maxverzoegerungskurvedefinition;
    myKnownElements["wunschverzoegerung"] = VE_Wunschverzoegerungskurvedefinition;
    myKnownElements["querverkehrsstoerung"] = VE_Querverkehrsstoerungsdefinition;
    myKnownElements["lsa"] = VE_Lichtsignalanlagendefinition;
    myKnownElements["signalgruppe"] = VE_Signalgruppendefinition;
    myKnownElements["signalgeber"] = VE_Signalgeberdefinition;
    myKnownElements["lsakopplung"] = VE_LSAKopplungdefinition;
    myKnownElements["detektor"] = VE_Detektorendefinition;
    myKnownElements["haltestelle"] = VE_Haltestellendefinition;
    myKnownElements["linie"] = VE_Liniendefinition;
    myKnownElements["stopschild"] = VE_Stopschilddefinition;
    myKnownElements["messung"] = VE_Messungsdefinition;
    myKnownElements["reisezeit"] = VE_Reisezeitmessungsdefinition;
    myKnownElements["verlustzeit"] = VE_Verlustzeitmessungsdefinition;
    myKnownElements["querschnittsmessung"] = VE_Querschnittsmessungsdefinition;
    myKnownElements["stauzaehler"] = VE_Stauzaehlerdefinition;
    myKnownElements["auswertung"] = VE_Auswertungsdefinition;
    myKnownElements["fenster"] = VE_Fensterdefinition;
    myKnownElements["motiv"] = VE_Gefahrenwarnsystemdefinition;
    myKnownElements["parkplatz"] = VE_Parkplatzdefinition;
    myKnownElements["knoten"] = VE_Knotendefinition;
    myKnownElements["teapac"] = VE_TEAPACdefinition;
    myKnownElements["netzobjekt"] = VE_Netzobjektdefinition;
    myKnownElements["richtungspfeil"] = VE_Richtungspfeildefinition;
    myKnownElements["raute"] = VE_Rautedefinition;
    myKnownElements["fahrverhalten"] = VE_Fahrverhaltendefinition;
    myKnownElements["fahrtverlaufdateien"] = VE_Fahrtverlaufdateien;
    myKnownElements["emission"] = VE_Emission;
    myKnownElements["einheit"] = VE_Einheitendefinition;
    myKnownElements["streckentyp"] = VE_Streckentypdefinition;
    myKnownElements["kantensperrung"] = VE_Kantensperrung;
    myKnownElements["kante"] = VE_Kantensperrung;


    myKnownElements["advance"] = VE_DUMMY;
    myKnownElements["temperatur"] = VE_DUMMY;

}



void
NIImporter_Vissim::buildParsers() {
    myParsers[VE_Simdauer] =
        new NIVissimSingleTypeParser_Simdauer(*this);
    myParsers[VE_Startuhrzeit] =
        new NIVissimSingleTypeParser_Startuhrzeit(*this);
    myParsers[VE_DynUml] =
        new NIVissimSingleTypeParser_DynUml(*this);
    myParsers[VE_Streckendefinition] =
        new NIVissimSingleTypeParser_Streckendefinition(*this);
    myParsers[VE_Verbindungsdefinition] =
        new NIVissimSingleTypeParser_Verbindungsdefinition(*this);
    myParsers[VE_Richtungsentscheidungsdefinition] =
        new NIVissimSingleTypeParser_Richtungsentscheidungsdefinition(*this);
    myParsers[VE_Routenentscheidungsdefinition] =
        new NIVissimSingleTypeParser_Routenentscheidungsdefinition(*this);
    myParsers[VE_VWunschentscheidungsdefinition] =
        new NIVissimSingleTypeParser_VWunschentscheidungsdefinition(*this);
    myParsers[VE_Langsamfahrbereichdefinition] =
        new NIVissimSingleTypeParser_Langsamfahrbereichdefinition(*this);
    myParsers[VE_Zuflussdefinition] =
        new NIVissimSingleTypeParser_Zuflussdefinition(*this);
    myParsers[VE_Fahrzeugtypdefinition] =
        new NIVissimSingleTypeParser_Fahrzeugtypdefinition(*this, myColorMap);
    myParsers[VE_Fahrzeugklassendefinition] =
        new NIVissimSingleTypeParser_Fahrzeugklassendefinition(*this, myColorMap);
    myParsers[VE_Geschwindigkeitsverteilungsdefinition] =
        new NIVissimSingleTypeParser_Geschwindigkeitsverteilungsdefinition(*this);
    myParsers[VE_Laengenverteilungsdefinition] =
        new NIVissimSingleTypeParser_Laengenverteilungsdefinition(*this);
    myParsers[VE_Zeitenverteilungsdefinition] =
        new NIVissimSingleTypeParser_Zeitenverteilungsdefinition(*this);
    myParsers[VE_Querverkehrsstoerungsdefinition] =
        new NIVissimSingleTypeParser_Querverkehrsstoerungsdefinition(*this);
    myParsers[VE_Lichtsignalanlagendefinition] =
        new NIVissimSingleTypeParser_Lichtsignalanlagendefinition(*this);
    myParsers[VE_Signalgruppendefinition] =
        new NIVissimSingleTypeParser_Signalgruppendefinition(*this);
    myParsers[VE_Stopschilddefinition] =
        new NIVissimSingleTypeParser_Stopschilddefinition(*this);
    myParsers[VE_Knotendefinition] =
        new NIVissimSingleTypeParser_Knotendefinition(*this);
    myParsers[VE_Signalgeberdefinition] =
        new NIVissimSingleTypeParser_Signalgeberdefinition(*this);
    myParsers[VE_Detektorendefinition] =
        new NIVissimSingleTypeParser_Detektordefinition(*this);
    myParsers[VE_Haltestellendefinition] =
        new NIVissimSingleTypeParser_Haltestellendefinition(*this);
    myParsers[VE_Liniendefinition] =
        new NIVissimSingleTypeParser_Liniendefinition(*this);
    myParsers[VE_Reisezeitmessungsdefinition] =
        new NIVissimSingleTypeParser_Reisezeitmessungsdefinition(*this);
    myParsers[VE_Querschnittsmessungsdefinition] =
        new NIVissimSingleTypeParser_Querschnittsmessungsdefinition(*this);
    myParsers[VE_Messungsdefinition] =
        new NIVissimSingleTypeParser_Messungsdefinition(*this);
    myParsers[VE_Verlustzeitmessungsdefinition] =
        new NIVissimSingleTypeParser_Verlustzeitmessungsdefinition(*this);
    myParsers[VE_Stauzaehlerdefinition] =
        new NIVissimSingleTypeParser_Stauzaehlerdefinition(*this);
    myParsers[VE_Rautedefinition] =
        new NIVissimSingleTypeParser_Rautedefinition(*this);
    myParsers[VE_Richtungspfeildefinition] =
        new NIVissimSingleTypeParser_Richtungspfeildefinition(*this);
    myParsers[VE_Parkplatzdefinition] =
        new NIVissimSingleTypeParser_Parkplatzdefinition(*this);
    myParsers[VE_Fahrverhaltendefinition] =
        new NIVissimSingleTypeParser_Fahrverhaltendefinition(*this);
    myParsers[VE_Streckentypdefinition] =
        new NIVissimSingleTypeParser_Streckentypdefinition(*this);
    myParsers[VE_Kennungszeile] =
        new NIVissimSingleTypeParser_Kennungszeile(*this);
    myParsers[VE_Fensterdefinition] =
        new NIVissimSingleTypeParser_Fensterdefinition(*this);
    myParsers[VE_Auswertungsdefinition] =
        new NIVissimSingleTypeParser_Auswertungsdefinition(*this);
    myParsers[VE_Verkehrszusammensetzungsdefinition] =
        new NIVissimSingleTypeParser_Zusammensetzungsdefinition(*this);
    myParsers[VE_Kantensperrung] =
        new NIVissimSingleTypeParser_Kantensperrung(*this);

    myParsers[VE_Startzufallszahl] =
        new NIVissimSingleTypeParser_Startzufallszahl(*this);
    myParsers[VE_SimRate] =
        new NIVissimSingleTypeParser_SimRate(*this);
    myParsers[VE_Zeitschrittfaktor] =
        new NIVissimSingleTypeParser_Zeitschrittfaktor(*this);
    myParsers[VE_Linksverkehr] =
        new NIVissimSingleTypeParser_Linksverkehr(*this);
    myParsers[VE_Stauparameterdefinition] =
        new NIVissimSingleTypeParser_Stauparameterdefinition(*this);
    myParsers[VE_Gelbverhaltendefinition] =
        new NIVissimSingleTypeParser_Gelbverhaltendefinition(*this);
    myParsers[VE_LSAKopplungdefinition] =
        new NIVissimSingleTypeParser_LSAKopplungsdefinition(*this);
    myParsers[VE_Gefahrenwarnsystemdefinition] =
        new NIVissimSingleTypeParser_Gefahrwarnungsdefinition(*this);
    myParsers[VE_TEAPACdefinition] =
        new NIVissimSingleTypeParser_TEAPACDefinition(*this);
    myParsers[VE_Netzobjektdefinition] =
        new NIVissimSingleTypeParser_Netzobjektdefinition(*this);
    myParsers[VE_Fahrtverlaufdateien] =
        new NIVissimSingleTypeParser_Fahrtverlaufdateien(*this);
    myParsers[VE_Emission] =
        new NIVissimSingleTypeParser_Emission(*this);
    myParsers[VE_Einheitendefinition] =
        new NIVissimSingleTypeParser_Einheitendefinition(*this);
    myParsers[VE_Baujahrverteilungsdefinition] =
        new NIVissimSingleTypeParser__XVerteilungsdefinition(*this);
    myParsers[VE_Laufleistungsverteilungsdefinition] =
        new NIVissimSingleTypeParser__XVerteilungsdefinition(*this);
    myParsers[VE_Massenverteilungsdefinition] =
        new NIVissimSingleTypeParser__XVerteilungsdefinition(*this);
    myParsers[VE_Leistungsverteilungsdefinition] =
        new NIVissimSingleTypeParser__XVerteilungsdefinition(*this);
    myParsers[VE_Maxbeschleunigungskurvedefinition] =
        new NIVissimSingleTypeParser__XKurvedefinition(*this);
    myParsers[VE_Wunschbeschleunigungskurvedefinition] =
        new NIVissimSingleTypeParser__XKurvedefinition(*this);
    myParsers[VE_Maxverzoegerungskurvedefinition] =
        new NIVissimSingleTypeParser__XKurvedefinition(*this);
    myParsers[VE_Wunschverzoegerungskurvedefinition] =
        new NIVissimSingleTypeParser__XKurvedefinition(*this);

}



/****************************************************************************/

