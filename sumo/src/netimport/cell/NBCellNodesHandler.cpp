/***************************************************************************
                          NBCellNodesHandler.cpp
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
// Revision 1.1  2002/10/16 15:40:04  dkrajzew
// initial commit for cell importing classes
//
// Revision 1.1  2002/07/25 08:41:45  dkrajzew
// Visum7.5 and Cell import added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <utils/importio/LineHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/SErrorHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/convert/TplConvert.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include "NBCellNodesHandler.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NBCellNodesHandler::NBCellNodesHandler(const std::string &file,
                                       bool warn, bool verbose)
    : _file(file)
{
}

NBCellNodesHandler::~NBCellNodesHandler()
{
}

bool
NBCellNodesHandler::report(const std::string &result)
{
    if(result.length()>0) {
        StringTokenizer st(result);
        if(st.size()!=3) {
            SErrorHandler::add(string("Within file '") + _file + string("':"));
            SErrorHandler::add("The following cell-nodes - entry contains an error:", true);
            SErrorHandler::add(result.c_str(), true);
            SErrorHandler::add("The net may be corrupt.", true);
        } else {
            string id = NBHelpers::normalIDRepresentation(st.next());
            double x, y;
            try {
                x = TplConvert<char>::_2float(st.next().c_str());
                y = TplConvert<char>::_2float(st.next().c_str());
                NBNodeCont::insert(id, x, y);
            } catch (NumberFormatException) {
                SErrorHandler::add(string("Within file '") + _file + string("':"));
                SErrorHandler::add(
                    "The following cell-nodes - entry contains a non-digit position information:",
                    true);
                SErrorHandler::add(result.c_str(), true);
                throw ProcessError();
            }
        }
    }
    return true;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBCellNodesHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:


