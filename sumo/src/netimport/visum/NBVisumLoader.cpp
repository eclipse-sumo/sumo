/***************************************************************************
                          NBVisumLoader.cpp
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
#include <utils/options/OptionsCont.h>
#include <utils/importio/LineReader.h>
#include <utils/importio/NamedColumnsParser.h>
#include <utils/common/SErrorHandler.h>
#include <utils/convert/TplConvert.h>
#include <utils/convert/TplConvertSec.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/NBType.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBCapacity2Lanes.h>
#include <netbuild/NBDistrict.h>
#include <netbuild/NBDistrictCont.h>
#include "NBVisumLoader.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NBVisumLoader::NBVisumLoader(const std::string &file,
                             NBCapacity2Lanes capacity2Lanes)
    : FileErrorReporter(file),
    _nowReading(NOTHING),
    _vsystypePos(-1),
    _nodePos(-1), _edgePos(-1), _typePos(-1), _districtPos(-1),
    _connectorPos(-1), _turnPos(-1),
    _vsystypesRead(false), _nodesRead(false), _edgesRead(false), _typesRead(false),
    _districtsRead(false), _connectorsRead(false), _turnsRead(false),
    _capacity2Lanes(capacity2Lanes)
{
}

NBVisumLoader::~NBVisumLoader()
{
}

void NBVisumLoader::load(OptionsCont &options)
{
    _verbose = options.getBool("v");
    _edgesRead = false;
    // do not load the types when other are supplied
    _loadTypes = !options.isSet("t");
    _typesRead = !_loadTypes;
    // read
    _nowReading = NOTHING;
    _step = 0;
    if(!_lineReader.setFileName(options.getString("visum"))) {
        SErrorHandler::add(
            string("Can not open visum-file '")
            + options.getString("visum")
            + string("'."), true);
        throw ProcessError();
    }
    _lineReader.readAll(*this);
    // process the second step when the edges and the connectors
    //  were at the begin of the file (should not be)
    if(_edgesRead && _connectorsRead) {
        return;
    }
    _step = 1;
    _lineReader.reinit();
    if(_edgePos!=-1) {
        _lineReader.setPos(_edgePos);
        _nowReading = EDGES;
    }
    _lineReader.readAll(*this);
    // process the third step when the turning definitions were at
    //  at the begin of the file (should not be
    if(_turnsRead) {
        return;
    }
    _step = 2;
    _lineReader.reinit();
    if(_turnPos!=-1) {
        _lineReader.setPos(_turnPos);
        _nowReading = TURNS;
    }
    _lineReader.readAll(*this);
}



bool
NBVisumLoader::report(const std::string &msg)
{
    // process nodes and types
    switch(_nowReading) {
    case VSYSTYPES:
        return addVSysType(msg);
    case NODES:
        return addNode(msg);
    case TYPES:
        return addType(msg);
    case EDGES:
        return addEdge(msg);
    case DISTRICTS:
        return addDistrict(msg);
    case CONNECTORS:
        return addConnector(msg);
    case TURNS:
        return addTurningDefinition(msg);
    default:
        break;
    }
    // check for the next type
    if(_step==0&&msg.substr(0, 8)=="$KNOTEN:") {
        initKnoten(msg);
    } else if(msg.substr(0, 10)=="$STRECKEN:") {
        initStrecken(msg);
    } else if(_step==0&&msg.substr(0, 13)=="$STRECKENTYP:") {
        initTypen(msg);
    } else if(_step==0&&msg.substr(0, 8)=="$BEZIRK:") {
        initBezirk(msg);
    } else if(msg.substr(0, 11)=="$ANBINDUNG:") {
        initAnbindung(msg);
    } else if(msg.substr(0, 18)=="$ABBIEGEBEZIEHUNG:") {
        initAbbiegeBeziehung(msg);
    } else if(_step==0&&msg.substr(0, 6)=="$VSYS:") {
        initVSysType(msg);
    }
    _oldPos = _lineReader.getPosition();
    return continueStep();
}


void
NBVisumLoader::initKnoten(const std::string &msg)
{
    _nowReading = NODES;
    _nodePos = _oldPos;
    _lineParser.reinit(msg.substr(8));
    if(_verbose) {
        cout << "Reading the nodes... ";
    }
}


void
NBVisumLoader::initVSysType(const std::string &msg)
{
    _nowReading = VSYSTYPES;
    _vsystypePos = _oldPos;
    _lineParser.reinit(msg.substr(6));
    if(_verbose) {
        cout << "Reading the vsystypes... ";
    }
}


void
NBVisumLoader::initBezirk(const std::string &msg)
{
    _nowReading = DISTRICTS;
    _districtPos = _oldPos;
    _lineParser.reinit(msg.substr(8));
    if(_verbose) {
        cout << "Reading the districts... ";
    }
}


void
NBVisumLoader::initTypen(const std::string &msg)
{
    if(_loadTypes==true) {
        _nowReading = TYPES;
        _lineParser.reinit(msg.substr(13));
        if(_verbose) {
            cout << "Reading the types... ";
        }
    }
    _typePos = _lineReader.getPosition();
}


void
NBVisumLoader::initStrecken(const std::string &msg)
{
    if(_step==1||(_step==0&&(_nodePos>0&&_typePos>0))) {
        _nowReading = EDGES;
        _lineParser.reinit(msg.substr(10));
        if(_verbose) {
            cout << "Reading the edges... ";
        }
    }
    _edgePos = _lineReader.getPosition();
}


void
NBVisumLoader::initAnbindung(const std::string &msg)
{
    if(_step==1||(_step==0&&(_districtPos>0&&_nodePos>0))) {
        _nowReading = CONNECTORS;
        _lineParser.reinit(msg.substr(11));
        if(_verbose) {
            cout << "Reading the connectors... ";
        }
    }
    _connectorPos = _lineReader.getPosition();
}


void
NBVisumLoader::initAbbiegeBeziehung(const std::string &msg)
{
    if(_step==2||(_edgePos>0&&_nodePos>0&&_vsystypePos>0)) {
        _nowReading = TURNS;
        _lineParser.reinit(msg.substr(18));
        if(_verbose) {
            cout << "Reading the turning directions... ";
        }
    }
    _turnPos = _lineReader.getPosition();
}


bool
NBVisumLoader::addType(const std::string &msg) {
    // check whether there are further data to read
    if(!dataTypeEnded(msg)) {
        // parse the data line
        _lineParser.parseLine(msg);
        string id;
        try {
            // get the id
            id = NBHelpers::normalIDRepresentation(_lineParser.get("Nr"));
            // get the maximum speed
            float speed = TplConvert<char>::_2float(_lineParser.get("v0-IV").c_str());
            // get the priority
            int priority = TplConvert<char>::_2int(_lineParser.get("Rang").c_str());
            // try to retrieve the number of lanes
            int nolanes = _capacity2Lanes.get(
                TplConvert<char>::_2float(_lineParser.get("Kap-IV").c_str()));
            // insert the type
            NBType *type = new NBType(id, nolanes, speed/3.6, 100-priority);
            if(!NBTypeCont::insert(type)) {
                addError("visum-file",
                    string(" Duplicate type occured ('") + id + string("')."));
                delete type;
            }
        } catch (OutOfBoundsException) {
            myAddError("STRECKENTYP", id, "OutOfBounds");
        } catch (NumberFormatException) {
            myAddError("STRECKENTYP", id, "NumberFormat");
        } catch (UnknownElement) {
            myAddError("STRECKENTYP", id, "UnknownElement");
        }
    } else {
        _typesRead = true;
    }
    return continueStep();
}

bool
NBVisumLoader::addEdge(const std::string &msg) {
    // check whether there are further data to read
    if(!dataTypeEnded(msg)) {
        // parse the data line
        _lineParser.parseLine(msg);
        string id;
        try {
            // get the id
            id = NBHelpers::normalIDRepresentation(_lineParser.get("Nr"));
            // get the from- & to-node and validate them
            NBNode *from = NBNodeCont::retrieve(
                NBHelpers::normalIDRepresentation(_lineParser.get("VonKnot")));
            NBNode *to = NBNodeCont::retrieve(
                NBHelpers::normalIDRepresentation(_lineParser.get("NachKnot")));
            if(from==0||to==0) {
                if(from==0) {
                    addError("visum-file",
                        " The from-node was not found within the net");
                } else {
                    addError("visum-file",
                        " The to-node was not found within the net");
                }
            }
            // get the type
            string type = _lineParser.get("Typ");
            // get the street length
            float length = 
                TplConvertSec<char>::_2floatSec(
                _lineParser.get("Laenge").c_str(), 0);
                // compute when the street's length is not available
            if(length==0) {
                double xb = from->getXCoordinate();
                double xe = to->getXCoordinate();
                double yb = from->getYCoordinate();
                double ye = to->getYCoordinate();
                length = sqrt((xb-xe)*(xb-xe) + (yb-ye)*(yb-ye));
            }
            // get the speed
            float speed = TplConvertSec<char>::_2floatSec(_lineParser.get("v0-IV").c_str(), -1);
            if(speed<0) {
                speed = NBTypeCont::getSpeed(type);
            } else {
                speed = speed / 3.6;
            }
            // get the information whether the edge is a one-way
            bool oneway = TplConvert<char>::_2bool(_lineParser.get("Einbahn").c_str());
            // check whether the id is already used
            //  (should be the opposite direction)
            if(NBEdgeCont::retrieve(id)!=0) {
                id = '-' + id;
            }
            // add the edge
            if(!NBEdgeCont::insert(new NBEdge(id, id, from, to, type,
                speed, NBTypeCont::getNoLanes(type), length,
                NBTypeCont::getPriority(type)))) {
                addError("visum-file",
                    string(" Duplicate edge occured ('")
                    + id + string("')."));
            }
            // nothing more to do, when the edge is a one-way street
            if(oneway) {
                return continueStep();
            }
            // add the opposite edge
            id = '-' + id;
            NBEdge *edge = new NBEdge(id, id, from, to, type,
                speed, NBTypeCont::getNoLanes(type), length,
                NBTypeCont::getPriority(type));
            if(!NBEdgeCont::insert(edge)) {
                addError("visum-file",
                    string(" Duplicate edge occured ('")
                    + id + string("')."));
                delete edge;
            }
        } catch (OutOfBoundsException) {
            myAddError("STRECKE", id, "OutOfBounds");
        } catch (NumberFormatException) {
            myAddError("STRECKE", id, "NumberFormat");
        } catch (UnknownElement) {
            myAddError("STRECKE", id, "UnknownElement");
        }
    } else {
        _edgesRead = true;
    }
    return continueStep();
}

bool
NBVisumLoader::addNode(const std::string &msg) {
    // check whether there are further data to read
    if(!dataTypeEnded(msg)) {
        string id;
        // parse the data line
        _lineParser.parseLine(msg);
        try {
            // get the id
            id = NBHelpers::normalIDRepresentation(_lineParser.get("Nr"));
            // get the position
            double x = TplConvert<char>::_2float(_lineParser.get("XKoord").c_str());
            double y = TplConvert<char>::_2float(_lineParser.get("YKoord").c_str());
            // add to the list
            if(!NBNodeCont::insert(id, x, y)) {
                addError("visum-file",
                    string(" Duplicate node occured ('")
                    + id + string("')."));
            }
        } catch (OutOfBoundsException) {
            myAddError("KNOTEN", id, "OutOfBounds");
        } catch (NumberFormatException) {
            myAddError("KNOTEN", id, "NumberFormat");
        } catch (UnknownElement) {
            myAddError("KNOTEN", id, "UnknownElement");
        }
    } else {
        _nodesRead = true;
    }
    return continueStep();
}


bool
NBVisumLoader::addDistrict(const std::string &msg)
{
    // check whether there are further data to read
    if(!dataTypeEnded(msg)) {
        string id;
        // parse the data line
        _lineParser.parseLine(msg);
        try {
            // get the id
            id = NBHelpers::normalIDRepresentation(_lineParser.get("Nr"));
            // get the name
            string name = _lineParser.get("NAME");
            // get the information whether the source and the destination
            //  connections are weighted
            bool sourcesWeighted = getWeightedBool("Proz_Q");
            bool destWeighted = getWeightedBool("Proz_Z");
            // build the district
            NBDistrict *district = new NBDistrict(id, name,
                sourcesWeighted, destWeighted);
            if(!NBDistrictCont::insert(district)) {
                addError("visum-file",
                    string(" Duplicate district occured ('")
                    + id + string("')."));
                delete district;
            }
            // get the node information
            double x =
                TplConvert<char>::_2float(_lineParser.get("XKoord").c_str());
            double y =
                TplConvert<char>::_2float(_lineParser.get("YKoord").c_str());
            // use a special name for the node
            id = string("DistrictCenter_") + id;
            // try to add the node
            if(!NBNodeCont::insert(id, x, y)) {
                addError("visum-file",
                    "Ups, this should not happen: A district lies on a node.");
            }
        } catch (OutOfBoundsException) {
            myAddError("BEZIRK", id, "OutOfBounds");
        } catch (NumberFormatException) {
            myAddError("BEZIRK", id, "NumberFormat");
        } catch (UnknownElement) {
            myAddError("BEZIRK", id, "UnknownElement");
        }
    } else {
        _districtsRead = true;
    }
    return continueStep();
}


bool
NBVisumLoader::addTurningDefinition(const std::string &msg)
{
    // check whether there are further data to read
    if(!dataTypeEnded(msg)) {
        // parse the data line
        _lineParser.parseLine(msg);
        try {
            // retrieve the nodes
            NBNode *from =
                NBNodeCont::retrieve(
                    NBHelpers::normalIDRepresentation(
                    _lineParser.get("VonKnot")));
            NBNode *via =
                NBNodeCont::retrieve(
                    NBHelpers::normalIDRepresentation(
                    _lineParser.get("UeberKnot")));
            NBNode *to =
                NBNodeCont::retrieve(
                    NBHelpers::normalIDRepresentation(
                    _lineParser.get("NachKnot")));
            // check the nodes
            bool ok = checkNode(from, "from", "VonKnot") &&
                checkNode(via, "via", "UeberKnot") &&
                checkNode(to, "to", "NachKnot");
            if(!ok) {
                return continueStep();
            }
            // all nodes are known
            if(isVehicleTurning()) {
                // try to set the turning definition
                string error = via->setTurningDefinition(from, to);
                if(error.length()>0) {
                    addError("visum-file", error);
                }
            }
        } catch (OutOfBoundsException) {
            myAddError("ABBIEGEBEZIEHUNG", "", "OutOfBounds");
        } catch (NumberFormatException) {
            myAddError("ABBIEGEBEZIEHUNG", "", "NumberFormat");
        } catch (UnknownElement) {
            myAddError("ABBIEGEBEZIEHUNG", "", "UnknownElement");
        }
    } else {
        _turnsRead = true;
    }
    return continueStep();
}


bool
NBVisumLoader::checkNode(NBNode *node, const std::string &type,
                         const std::string &nodeTypeName) {
    if(node==0) {
        addError("visum-file",
            string("The ") + type + ("-node '")
            + NBHelpers::normalIDRepresentation(_lineParser.get(nodeTypeName))
            + string("' is not known inside the net."));
        return false;
    }
    return true;
}


bool
NBVisumLoader::isVehicleTurning() {
    string type = _lineParser.get("VSysCode");
    return _vsystypes.find(type)!=_vsystypes.end() &&
        _vsystypes.find(type)->second=="IV";
}


bool
NBVisumLoader::addVSysType(const std::string &msg)
{
    if(!dataTypeEnded(msg)) {
        // parse the data line
        _lineParser.parseLine(msg);
        try {
            string name = _lineParser.get("VSysCode");
            string type = _lineParser.get("VSysMode");
            _vsystypes[name] = type;
        } catch (OutOfBoundsException) {
            myAddError("VSYS", "", "OutOfBounds");
        } catch (NumberFormatException) {
            myAddError("VSYS", "", "NumberFormat");
        } catch (UnknownElement) {
            myAddError("VSYS", "", "UnknownElement");
        }
    } else {
        _vsystypesRead = true;
    }
    return continueStep();
}


bool
NBVisumLoader::addConnector(const std::string &msg) {
    // check whether there are further data to read
    if(!dataTypeEnded(msg)) {
        // parse the data line
        _lineParser.parseLine(msg);
        try {
            // get the source district
            string bez =
                NBHelpers::normalIDRepresentation(_lineParser.get("BezNr"));
            NBNode *src = NBNodeCont::retrieve(string("DistrictCenter_") + bez);
            if(src==0) {
                addError("visum-file",
                    string("The district '") + bez + string("' is not known."));
                return false;
            }
            // get the destination node
            string node =
                NBHelpers::normalIDRepresentation(_lineParser.get("KnotNr"));
            NBNode *dest =
                NBNodeCont::retrieve(node);
            if(dest==0) {
                addError("visum-file",
                    string("The node '") + bez + string("' is not known."));
                return false;
            }
            // get the weight of the connection
            double proz = getWeightedFloat("Proz");
            if(proz>0) {
                proz /= 100;
            }
            // get the duration to wait
            double retard =
                TplConvertSec<char>::_2floatSec(
                _lineParser.get("t0-IV").c_str(), -1);
            // get the length
            double length =
                TplConvertSec<char>::_2floatSec(
                _lineParser.get("Laenge").c_str(), 0);
                // compute the length when it was not given explicitely
            if(length==0) {
                length = NBHelpers::distance(src, dest);
            }
            // get the type;
            //  use a standard type with a large speed when a type is not given
            string type =
                NBHelpers::normalIDRepresentation(_lineParser.get("Typ"));

            double speed;
            int prio, nolanes;
            if(type.length()==0) {
                speed = 10000;
                nolanes = 1;
                prio = 0;
            } else {
                speed = NBTypeCont::getSpeed(type);
                nolanes = NBTypeCont::getNoLanes(type);
                prio = NBTypeCont::getPriority(type);
            }
            // add the connectors as an edge
            string id = bez + string("-") + node;
            // get the information whether this is a sink or a source
            string dir = _lineParser.get("Richtung");
            if(dir.length()==0) {
                dir = "QZ";
            }
            // build the source when needed
            if(dir.find('Q')!=string::npos) {
                NBEdge *edge = new NBEdge(id, id, src, dest, "VisumConnector",
                    speed, nolanes, length, prio, NBEdge::EDGEFUNCTION_SOURCE);
                if(!NBEdgeCont::insert(edge)) {
                    addError("visum-file",
                        string("A duplicate edge id occured (ID='") + id +
                        string("')."));
                } else {
                    NBDistrictCont::addSource(bez, edge, proz);
                }
            }
            // build the sink when needed
            if(dir.find('Z')!=string::npos) {
                id = string("-") + id;
                NBEdge *edge = new NBEdge(id, id, dest, src, "VisumConnector",
                    speed, nolanes, length, prio, NBEdge::EDGEFUNCTION_SINK);
                if(!NBEdgeCont::insert(edge)) {
                    addError("visum-file",
                        string("A duplicate edge id occured (ID='") + id +
                        string("')."));
                } else {
                    NBDistrictCont::addSink(bez, edge, proz);
                }
            }
        } catch (OutOfBoundsException) {
            myAddError("ANBINDUNG", "", "OutOfBounds");
        } catch (NumberFormatException) {
            myAddError("ANBINDUNG", "", "NumberFormat");
        } catch (UnknownElement) {
            myAddError("ANBINDUNG", "", "UnknownElement");
        }
    } else {
        _connectorsRead = true;
    }
}



bool
NBVisumLoader::getWeightedBool(const std::string &name)
{
    try {
        return TplConvert<char>::_2bool(_lineParser.get(name).c_str());
    } catch (...) {
    }
    try {
        return
            TplConvert<char>::_2bool(_lineParser.get(
            (name+string("(IV)"))).c_str());
    } catch (...) {
    }
    return false;
}


float
NBVisumLoader::getWeightedFloat(const std::string &name)
{
    try {
        return TplConvert<char>::_2float(_lineParser.get(name).c_str());
    } catch (...) {
    }
    try {
        return
            TplConvert<char>::_2float(_lineParser.get(
              (name+string("(IV)"))).c_str());
    } catch (...) {
    }
    return -1;
}


bool
NBVisumLoader::dataTypeEnded(const std::string &msg)
{
    if(msg.length()==0||msg.at(0)=='*'||msg.at(0)=='$') {
        _nowReading = NOTHING;
        if(_verbose) {
            cout << "done." << endl;
        }
        return true;
    }
    return false;
}

bool
NBVisumLoader::continueStep() const {
    return
        (_step==0 &&
                (!_nodesRead || !_typesRead ||
                !_districtsRead || !_vsystypesRead) ) ||
        (_step==1&&(!_edgesRead||!_connectorsRead)) ||
        (_step==2&&!_turnsRead);
}

void
NBVisumLoader::myAddError(const std::string &type, const std::string &id,
                          const std::string &exception)
{
    if(id.length()!=0) {
        if(exception.length()!=0) {
            addError("visum-file",
                string("The definition of the ") + type + string("'") + id
                + string("' is malicious (") + exception + string(")."));
        } else {
            addError("visum-file",
                string("The definition of the ") + type + string("'") + id
                + string("' is malicious."));
        }
    } else {
        addError("visum-file",
            string("Something is wrong with a ") + type + string(" (unknown id)."));
    }
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBVisumLoader.icc"
//#endif

// Local Variables:
// mode:C++
// End:

