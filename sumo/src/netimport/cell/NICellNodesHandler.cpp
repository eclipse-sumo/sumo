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
#include "NICellNodesHandler.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NICellNodesHandler::NICellNodesHandler(const std::string &file,
                                       bool warn, bool verbose)
    : FileErrorReporter("cell-nodes", file)
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
            double x, y;
            try {
                x = TplConvert<char>::_2float(st.next().c_str());
                y = TplConvert<char>::_2float(st.next().c_str());
                if(!NBNodeCont::insert(id, x, y)) {
                    SErrorHandler::add(
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
//#ifdef DISABLE_INLINE
//#include "NICellNodesHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:


