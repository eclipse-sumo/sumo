/***************************************************************************
                          MSLogicJunction.cpp  -  Base class for junctions
                          with one ore more logics.
                             -------------------
    begin                : Wed, 12 Dez 2001
    copyright            : (C) 2001 by Christian Roessel
    email                : roessel@zpr.uni-koeln.de
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
// Revision 1.4  2003/04/14 08:33:00  dkrajzew
// some further bugs removed
//
// Revision 1.3  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.2  2002/10/16 16:42:29  dkrajzew
// complete deletion within destructors implemented; clear-operator added for container; global file include; junction extended by position information (should be revalidated later)
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.2  2002/04/18 10:51:22  croessel
// Introduced new method "bool driveRequest()" in class DriveBrakeRequest
// to let findCompetitor check, if a first car set a request.
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:17  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.5  2002/02/05 13:51:52  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.4  2002/01/31 13:51:40  croessel
// Revision 1.3 Log-message corrected.
//
// Revision 1.3  2002/01/30 16:16:43  croessel
// Destructor defined.
//
// Revision 1.2  2001/12/20 14:30:23  croessel
// using namespace std added.
//
// Revision 1.1  2001/12/13 15:56:47  croessel
// Initial commit.
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSLinkCont.h"
#include "MSLogicJunction.h"
#include "MSLane.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * methods from MSLogicJunction
 * ----------------------------------------------------------------------- */
MSLogicJunction::MSLogicJunction( string id, double x, double y,
				  InLaneCont in )
    : MSJunction( id, x, y ), myInLanes(in),
    myRequest(false),
    myRespond(false)
{
}

//-------------------------------------------------------------------------//

MSLogicJunction::~MSLogicJunction()
{
}

//-------------------------------------------------------------------------//


void
MSLogicJunction::postloadInit()
{
    size_t requestPos = 0;
    size_t respondPos = 0;
    for(InLaneCont::iterator i=myInLanes.begin(); i!=myInLanes.end(); i++) {
        const MSLinkCont &links = (*i).myLane->getLinkCont();
/*        std::bitset<64> clearInfo((unsigned long) (0xffffffff));
        // set lanes to clear
        for(size_t k=0; k<links.size(); k++) {
            clearInfo.set(k+requestPos, false);
        }*/
        // set information for every link
        for(MSLinkCont::const_iterator j=links.begin(); j!=links.end(); j++) {
            (*j)->setRequestInformation(&myRequest, requestPos,
                &myRespond, respondPos/*, clearInfo*/);
            requestPos++;
        }
        respondPos++;
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSLogicJunction.icc"
//#endif

// Local Variables:
// mode:C++
// End:
