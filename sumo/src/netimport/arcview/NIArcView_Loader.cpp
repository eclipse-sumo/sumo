#include <string>
#include <utils/common/FileErrorReporter.h>
#include <utils/common/SErrorHandler.h>
#include <utils/convert/ToString.h>
#include <utils/convert/TplConvert.h>
#include <utils/importio/LineHandler.h>
#include <utils/importio/LineReader.h>
#include <utils/importio/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeomHelper.h>
#include <netbuild/NBHelpers.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include "NIArcView_ShapeReader.h"
#include "NIArcView_Loader.h"

using namespace std;


NIArcView_Loader::NIArcView_Loader(const std::string &dbf_name,
                                   const std::string &shp_name)
    : FileErrorReporter("Navtech Edge description", dbf_name),
    myShapeReader(shp_name), myIsFirstLine(true),
    myLineReader(dbf_name)
{
}


NIArcView_Loader::~NIArcView_Loader()
{
}


void
NIArcView_Loader::load(OptionsCont &options)
{
    // read names from first line
    myIsFirstLine = true;
    myLineReader.readLine(*this);
    // read data
    myCurrentLink = 0;
    myIsFirstLine = false;
    myLineReader.readAll(*this);
}


bool
NIArcView_Loader::report(const std::string &line)
{
    // skip first line
    if(myIsFirstLine) {
        myColumnsParser.reinit(line, ";", ";", true);
        return false;
    }
    // parse following lines
    return parseLine(line);
}


bool
NIArcView_Loader::parseLine(const std::string &line)
{
    // omit empty lines
    if(StringUtils::prune(line)=="") {
        return false;
    }
    myColumnsParser.parseLine(line);
    // extract values
    string id = myColumnsParser.get("LINK_ID", true);
    string name = myColumnsParser.get("ST_NAME", true);
    string from_node = myColumnsParser.get("REF_IN_ID", true);
    string to_node = myColumnsParser.get("NREF_IN_ID", true);
    string type = myColumnsParser.get("ST_TYP_AFT", true);
    double speed = getSpeed(id);
    double nolanes = getLaneNo(id);
    int priority = getPriority(id);
    NBEdge::EdgeBasicFunction function = NBEdge::EDGEFUNCTION_NORMAL;
        // extract shape
    myShapeReader.readShape(myCurrentLink++);
    if(SErrorHandler::errorOccured()) {
        return false;
    }
    // build and check nodes
    NBNode *from = 0;
    NBNode *to = 0;
    Position2D from_pos = myShapeReader.getFromNodePosition();
    if(!NBNodeCont::insert(from_node, from_pos.x(), from_pos.y())) {
        addError(
            string("A false from-node occured (id='") + from_node
            + string("', pos=(") + toString(from_pos.x()) + string(", ")
            + toString(from_pos.y()) + string("))."));
        return false;
    } else {
        from = NBNodeCont::retrieve(from_pos.x(), from_pos.y());
    }
    Position2D to_pos = myShapeReader.getToNodePosition();
    if(!NBNodeCont::insert(to_node, to_pos.x(), to_pos.y())) {
        addError(
            string("A false to-node occured (id='") + from_node
            + string("', pos=(") + toString(to_pos.x()) + string(", ")
            + toString(to_pos.y()) + string("))."));
        return false;
    } else {
        to = NBNodeCont::retrieve(to_pos.x(), to_pos.y());
    }
        // retrieve length
    double length = getLength(id, from_pos, to_pos);

    // retrieve the information whether the street is bi-directional
    string dir = myColumnsParser.get("DIR_TRAVEL", true);
        // add positive direction if wanted
    if(dir=="B"||dir=="F") {
        if(NBEdgeCont::retrieve(id)==0) {
            NBEdge *edge = new NBEdge(id, name, from, to, type, speed, nolanes,
                length, priority, function);
            NBEdgeCont::insert(edge);
        }
    }
        // add negative direction if wanted
    if(dir=="B"||dir=="T") {
        id = "-" + id;
        if(NBEdgeCont::retrieve(id)==0) {
            NBEdge *edge = new NBEdge(id, name, to, from, type, speed, nolanes,
                length, priority, function);
            NBEdgeCont::insert(edge);
        }
    }
    return true;
}


double
NIArcView_Loader::getSpeed(const std::string &edgeid)
{
    try {
        int speedcat = TplConvert<char>::_2int(myColumnsParser.get("SPEED_CAT", true).c_str());
        switch(speedcat) {
        case 1:
            return 300 / 3.6;
        case 2:
            return 130 / 3.6;
        case 3:
            return 100 / 3.6;
        case 4:
            return 90 / 3.6;
        case 5:
            return 70 / 3.6;
        case 6:
            return 50 / 3.6;
        case 7:
            return 30 / 3.6;
        case 8:
            return 10 / 3.6;
        default:
            throw 1;
        }
    } catch (...) {
        if(myColumnsParser.get("SPEED_CAT", true)=="NA") {
            cout << "Warning: non-applicable speed definition found for edge '"
                << edgeid << "'" << endl; // !!! Warning-level
            cout << "Using 30km/h" << endl;
            return 30 / 3.6;
        }
        addError(
            string("Error on parsing edge speed definition for edge '")
            + edgeid + string("'."));
        return 0;
    }
}


size_t
NIArcView_Loader::getLaneNo(const std::string &edgeid)
{
    try {
        size_t lanecat = TplConvert<char>::_2int(myColumnsParser.get("LANE_CAT", true).c_str());
        switch(lanecat) {
        case 1:
            return 1;
        case 2:
            return 2;
        case 3:
            return 4;
        default:
            throw 1;
        }
    } catch (...) {
        if(myColumnsParser.get("LANE_CAT", true)=="NA") {
            cout << "Warning: non-applicable number of lanes found for edge '"
                << edgeid << "'" << endl; // !!! Warning-level
            cout << "Using '1'" << endl;
            return 1;
        }
        addError(
            string("Error on parsing edge's noumber of lanes information for edge '")
            + edgeid + string("'."));
        return 0;
    }
}


double
NIArcView_Loader::getLength(const std::string &edgeid,
                           const Position2D &from_pos, const Position2D &to_pos)
{
    return GeomHelper::distance(from_pos, to_pos);
}


int
NIArcView_Loader::getPriority(const std::string &edgeid)
{
    try {
        int prio = TplConvert<char>::_2int(myColumnsParser.get("FUNC_CLASS", true).c_str());
        return 5 - prio;
    } catch (...) {
        if(myColumnsParser.get("FUNC_CLASS", true)=="NA") {
            cout << "Warning: non-applicable priority found for edge '"
                << edgeid << "'" << endl; // !!! Warning-level
            cout << "Using '0'" << endl;
            return 0;
        }
        addError(
            string("Error on parsing edge priority information for edge '")
            + edgeid + string("'."));
        return 0;
    }
}
