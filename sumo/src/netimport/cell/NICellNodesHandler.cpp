/***************************************************************************
                          NICellNodesHandler.cpp
             A LineHandler-derivate to load nodes form a cell-nodes-file
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
// Revision 1.9  2005/09/23 06:02:04  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.8  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/04/27 12:24:35  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.6  2004/08/02 12:44:12  dkrajzew
// using Position2D instead of two SUMOReals
//
// Revision 1.5  2004/01/12 15:31:18  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.4  2003/06/18 11:14:48  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.3  2003/03/20 16:25:59  dkrajzew
// windows eol removed
//
// Revision 1.2  2003/03/06 17:14:41  dkrajzew
// more stringent usage of insertion into containers; y-direction flipped
//
// Revision 1.1  2003/02/07 11:10:56  dkrajzew
// names changed
//
// Revision 1.1  2002/10/16 15:40:04  dkrajzew
// initial commit for cell importing classes
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/importio/LineHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include "NICellNodesHandler.h"

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
NICellNodesHandler::NICellNodesHandler(NBNodeCont &nc,
                                       const std::string &file)
    : FileErrorReporter("cell-nodes", file), myNodeCont(nc)
{
}

NICellNodesHandler::~NICellNodesHandler()
{
}

bool
NICellNodesHandler::report(const std::string &result)
{
    if(result.length()>0) {
        StringTokenizer st(result);
        if(st.size()!=3) {
            addError(
                string("The following cell-nodes - entry contains an error:")
                + '\n' + result.c_str());
        } else {
            string id = NBHelpers::normalIDRepresentation(st.next());
            SUMOReal x, y;
            try {
                x = TplConvert<char>::_2SUMOReal(st.next().c_str());
                y = TplConvert<char>::_2SUMOReal(st.next().c_str());
                if(!myNodeCont.insert(id, Position2D(x, y))) {
                    MsgHandler::getErrorInstance()->inform(
                        string("Could not build node '") + id + string("'."));
                }
            } catch (NumberFormatException) {
                addError(
                    string("The following cell-nodes - entry contains a non-digit position information:")
                    + '\n' + result.c_str());
                throw ProcessError();
            }
        }
    }
    return true;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


