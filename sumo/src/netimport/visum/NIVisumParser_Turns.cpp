/***************************************************************************
                          NIVisumParser_Turns.cpp
              Parser for visum-turn definitions
                             -------------------
    project              : SUMO
    begin                : Thu, 14 Nov 2002
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
// Revision 1.3  2004/11/23 10:23:51  dkrajzew
// debugging
//
// Revision 1.2  2004/01/12 15:36:08  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.1  2003/02/07 11:14:54  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <utils/common/UtilExceptions.h>
#include "NIVisumLoader.h"
#include "NIVisumParser_Turns.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NIVisumParser_Turns::NIVisumParser_Turns(NIVisumLoader &parent,
        const std::string &dataName, NIVisumLoader::VSysTypeNames &vsystypes)
    : NIVisumLoader::NIVisumSingleDataTypeParser(parent, dataName),
    usedVSysTypes(vsystypes)
{
}


NIVisumParser_Turns::~NIVisumParser_Turns()
{
}


void
NIVisumParser_Turns::myDependentReport()
{
    try {
        // retrieve the nodes
        NBNode *from =
            NBNodeCont::retrieve(
                NBHelpers::normalIDRepresentation(
                    myLineParser.get("VonKnot")));
        NBNode *via =
            NBNodeCont::retrieve(
                NBHelpers::normalIDRepresentation(
                    myLineParser.get("UeberKnot")));
        NBNode *to =
            NBNodeCont::retrieve(
                NBHelpers::normalIDRepresentation(
                    myLineParser.get("NachKnot")));
        // check the nodes
        bool ok = checkNode(from, "from", "VonKnot") &&
            checkNode(via, "via", "UeberKnot") &&
            checkNode(to, "to", "NachKnot");
        if(!ok) {
            return;
        }
        // all nodes are known
        if(isVehicleTurning()) {
            // try to set the turning definition
            via->setTurningDefinition(from, to);
        }
    } catch (OutOfBoundsException) {
        addError2("ABBIEGEBEZIEHUNG", "", "OutOfBounds");
    } catch (NumberFormatException) {
        addError2("ABBIEGEBEZIEHUNG", "", "NumberFormat");
    } catch (UnknownElement) {
        addError2("ABBIEGEBEZIEHUNG", "", "UnknownElement");
    }
}



bool
NIVisumParser_Turns::checkNode(NBNode *node, const std::string &type,
                               const std::string &nodeTypeName)
{
    if(node==0) {
        addError(
            string("The ") + type + ("-node '")
            + NBHelpers::normalIDRepresentation(myLineParser.get(nodeTypeName))
            + string("' is not known inside the net."));
        return false;
    }
    return true;
}


bool
NIVisumParser_Turns::isVehicleTurning() {
    string type = myLineParser.get("VSysCode");
    return usedVSysTypes.find(type)!=usedVSysTypes.end() &&
        usedVSysTypes.find(type)->second=="IV";
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NIVisumParser_Turns.icc"
//#endif

// Local Variables:
// mode:C++
// End:


