//---------------------------------------------------------------------------//
//                        ROJTRRouter.cpp -
//      The junction-percentage router
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 20 Jan 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.1  2005/10/10 12:09:36  dkrajzew
// renamed ROJP*-classes to ROJTR*
//
// Revision 1.6  2005/10/07 11:42:39  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/23 06:04:58  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.4  2005/09/15 12:05:34  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/05/04 08:57:12  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.2  2004/11/23 10:26:59  dkrajzew
// debugging
//
// Revision 1.1  2004/02/06 08:43:46  dkrajzew
// new naming applied to the folders (jp-router is now called jtr-router)
//
// Revision 1.4  2004/02/02 16:20:16  dkrajzew
// catched the problems with dead end edges
//
// Revision 1.3  2004/01/28 14:19:16  dkrajzew
// allowed to specify the maximum edge number in a route by a factor
//
// Revision 1.2  2004/01/26 09:58:15  dkrajzew
// sinks are now simply marked as these instead of the usage of a further container
//
// Revision 1.1  2004/01/26 06:09:11  dkrajzew
// initial commit for jp-classes
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

#include <router/RONet.h>
#include "ROJTRRouter.h"
#include "ROJTREdge.h"
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>

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
ROJTRRouter::ROJTRRouter(RONet &net)
    : myNet(net)
{
    myMaxEdges = (int) (
        ((SUMOReal) net.getEdgeNo()) *
         OptionsSubSys::getOptions().getFloat("max-edges-factor"));
}


ROJTRRouter::~ROJTRRouter()
{
}


ROEdgeVector
ROJTRRouter::compute(ROEdge *from, ROEdge *to, SUMOTime time,
                    bool continueOnUnbuild,
					ROAbstractEdgeEffortRetriever * const retriever)
{
    ROEdgeVector ret;
    ROJTREdge *current = static_cast<ROJTREdge*>(from);
    // route until a sinks has been found
    while(  current!=0
            &&
            current->getType()!=ROEdge::ET_SINK
            &&
            (int) ret.size()<myMaxEdges) {
        ret.add(current);
        time += (SUMOTime) current->getDuration(time);
        current = current->chooseNext(time);
    }
    // check whether no valid ending edge was found
    if((int) ret.size()>=myMaxEdges) {
        MsgHandler *mh = 0;
        if(continueOnUnbuild) {
            mh = MsgHandler::getWarningInstance();
        } else {
            mh = MsgHandler::getErrorInstance();
        }
        mh->inform(string("The route starting at edge '") + from->getID()
            + string("' could not be closed."));
    }
    // append the sink
    if(current!=0) {
        ret.add(current);
    }
    return ret;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


