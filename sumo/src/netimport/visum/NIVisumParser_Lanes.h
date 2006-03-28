#ifndef NIVisumParser_Lanes_h
#define NIVisumParser_Lanes_h
/***************************************************************************
                          NIVisumParser_Lanes.h
			  Parser for visum-lanes
                             -------------------
    project              : SUMO
    begin                : Thu, 23 Mar 2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
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
// $Log$
// Revision 1.2  2006/03/28 09:12:43  dkrajzew
// lane connections for unsplitted lanes implemented, further refactoring
//
// Revision 1.1  2006/03/28 06:15:49  dkrajzew
// refactoring and extending the Visum-import
//
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

#include "NIVisumLoader.h"
#include <netbuild/NBEdge.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIVisumParser_Lanes
 * Parses connectors from a visum-file. Connectors are used to connect districts
 * and the "real" road network. Both types (ingoing and outgoing) are supported.
 */
class NIVisumParser_Lanes :
        public NIVisumLoader::NIVisumSingleDataTypeParser {
public:
    /// Constructor
    NIVisumParser_Lanes(NIVisumLoader &parent,
        NBNodeCont &nc, NBEdgeCont &ec, NBDistrictCont &dc,
        const std::string &dataName);

    /// Destructor
    ~NIVisumParser_Lanes();

protected:
    /** @brief Parses data of a single connector;
        Values are stored within the inherited NamedColumnsParser */
    void myDependentReport();

private:
    NBNodeCont &myNodeCont;
    NBEdgeCont &myEdgeCont;
    NBDistrictCont &myDistrictCont;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

