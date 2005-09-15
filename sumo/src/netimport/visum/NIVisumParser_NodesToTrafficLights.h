#ifndef NIVisumParser_NodesToTrafficLights_h
#define NIVisumParser_NodesToTrafficLights_h
/***************************************************************************
                          NIVisumParser_Nodes.h
			  Parser for visum node to traffic lights relation
                             -------------------
    project              : SUMO
    begin                : Fri, 09 May 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
    author               : Markus Hartinger
    email                : Markus.Hartinger@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.3  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/04/27 12:24:41  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.1  2003/05/20 09:39:14  dkrajzew
// Visum traffic light import added (by Markus Hartinger)
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

#include "NIVisumLoader.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIVisumParser_NodesToTrafficLights
 * This class parses traffic lights from visum-files.
 */
class NIVisumParser_NodesToTrafficLights :
        public NIVisumLoader::NIVisumSingleDataTypeParser {
public:
    /// Constructor
    NIVisumParser_NodesToTrafficLights(NIVisumLoader &parent,
        NBNodeCont &nc,
        const std::string &dataName, NIVisumLoader::NIVisumTL_Map &NIVisumTLs);

    /// Destructor
    ~NIVisumParser_NodesToTrafficLights();

protected:
    /** @brief Parses a single node using data from the inherited NamedColumnsParser. */
    void myDependentReport();
private:
	NIVisumLoader::NIVisumTL_Map &myNIVisumTLs;

private:
    NBNodeCont &myNodeCont;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

