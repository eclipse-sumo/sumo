//---------------------------------------------------------------------------//
//                        NIArcView_ShapeReader.cpp -
//  The reader of shape-files
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.5  2003/06/19 15:20:51  dkrajzew
// the parsing of positions is now less conservative
//
// Revision 1.4  2003/06/18 11:34:25  dkrajzew
// the arcview-import should be more stable nw when dealing with false tables
//
// Revision 1.3  2003/06/05 11:44:14  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <cassert>
#include <sstream>
#include <utils/convert/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/FileErrorReporter.h>
#include <utils/common/StringTokenizer.h>
#include <utils/importio/LineHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/geom/Position2DVector.h>
#include "NIArcView_ShapeReader.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIArcView_ShapeReader::NIArcView_ShapeReader(const std::string &file)
    : FileErrorReporter("Navtech shape", file), myLineReader(file)
{
}


NIArcView_ShapeReader::~NIArcView_ShapeReader()
{
}


void
NIArcView_ShapeReader::readShape(size_t no)
{
    // the shape was not found, yet
    myFoundAndProcessing = false;
    myShapeRead = false;
    // set the searched shape number
    mySearchedShape = no;
    // clear points
    myPoints.clear();
    // try to find and parse the shape
    while(!myShapeRead&&!MsgHandler::getErrorInstance()->wasInformed()) {
        myLineReader.readLine(*this);
    }
}


bool
NIArcView_ShapeReader::report(const std::string &line)
{
    // when a shape was read, check whether it is over
    if(myFoundAndProcessing&&line.length()==0) {
        assert(myPoints.size()==myVertexNo);
        myShapeRead = true;
        // stop reading from file
        return false;
    }
    // read further points when the shape began already
    if(myFoundAndProcessing) {
        switch(myParseStep) {
        case 0:
            myULCorner = parsePoint(line);
            myParseStep++;
            break;
        case 1:
            myDRCorner = parsePoint(line);
            myParseStep++;
            break;
        case 2:
            myPoints.push_back(parsePoint(line));
            break;
        default:
            throw 1;
        }
    }
    // try to find the shape's begin
    if(line.find("Shape:")!=string::npos) {
        // check whether a false steps begun
        int no = parseShapeNumber(line);
        if(no!=mySearchedShape) {
            stringstream s;
            s << "The searched shape '" << mySearchedShape << "' was not found."
                << endl << " Found '" << no << "' instead...";
            addError(s.str());
            return true;
        }
        myVertexNo = getNamedParam(line, "nVertices");
        if(getNamedParam(line, "nParts")!=1) {
            throw 1; // !!! don't know what the parameter is for
        }
        // initialise infomration parsing
        myParseStep = 0;
        myFoundAndProcessing = true;
    }
    return true;
}


Position2D
NIArcView_ShapeReader::parsePoint(const std::string &line)
{
    // get the begin of the point
    size_t idx1 = line.find('(');
    if(idx1==string::npos) {
        addError("No opening bracket found in shape description (should be).");
        return Position2D();
    }
    // get the end of the point
    size_t idx2 = line.find(')', idx1);
    if(idx2==string::npos) {
        addError("No closing bracket found in shape description (should be).");
        return Position2D();
    }
    // get the point description only
    string pd = StringUtils::prune(line.substr(idx1+1, idx2-idx1-2));
    // split at ", "
    StringTokenizer st(pd, ",");
    if(st.size()<2) {
        addError("Too few information in a position description");
        return Position2D();
    }
    // extract points
    try {
        return Position2D(
            TplConvert<char>::_2float(StringUtils::prune(st.get(0)).c_str()),
            TplConvert<char>::_2float(StringUtils::prune(st.get(1)).c_str()));
    } catch (NumberFormatException) {
        addError("Not numerical position entry.");
    } catch (EmptyData) {
        addError("Position value not given.");
    }
    return Position2D();
}


int
NIArcView_ShapeReader::parseShapeNumber(const std::string &line)
{
    size_t idx = line.find(' ');
    if(idx==string::npos) {
        addError("False shape number description");
    }
    string from = line.substr(6, idx-6);
    try {
        return TplConvert<char>::_2int(from.c_str());
    } catch (NumberFormatException) {
        addError("Not numerical shape number.");
    } catch (EmptyData) {
        addError("Shape number not given.");
    }
    return -1;
}


int
NIArcView_ShapeReader::getNamedParam(const std::string &from, const std::string &name)
{
    size_t idx = from.find(name);
    if(idx==string::npos) {
        addError(
            string("Wished parameter '") + name +
            string("' was not found within the line."));
        return -1;
    }
    string rfrom = from.substr(idx+name.length()+1);
    idx = rfrom.find(',');
    if(idx!=string::npos) {
        rfrom = rfrom.substr(0, idx);
    }
    try {
        return TplConvert<char>::_2int(rfrom.c_str());
    } catch (NumberFormatException) {
        addError(
            string("Wished parameter '") + name +
            string("' is not numeric."));
    } catch (EmptyData) {
        addError(
            string("Wished parameter '") + name +
            string("' is empty."));
    }
    return -1;
}



const Position2DVector &
NIArcView_ShapeReader::getShape() const
{
    return myPoints;
}


const Position2D &
NIArcView_ShapeReader::getFromNodePosition() const
{
    return myPoints.at(0);
}


const Position2D &
NIArcView_ShapeReader::getToNodePosition() const
{
    return myPoints.at(myPoints.size()-1);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIArcView_ShapeReader.icc"
//#endif

// Local Variables:
// mode:C++
// End:


