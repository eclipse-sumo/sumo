#ifndef NIVisumParser_TurnsToSignalGroups_h
#define NIVisumParser_TurnsToSignalGroups_h
/***************************************************************************
                          NIVisumParser_Nodes.h
			  Parser for visum TurnsToSignalGroups relation
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
// Revision 1.1  2003/05/20 09:39:14  dkrajzew
// Visum traffic light import added (by Markus Hartinger)
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "NIVisumLoader.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIVisumParser_TurnsToSignalGroups
 * This class parses TurnsToSignalGroups from visum-files.
 */
class NIVisumParser_TurnsToSignalGroups :
        public NIVisumLoader::NIVisumSingleDataTypeParser {
public:
    /// Constructor
    NIVisumParser_TurnsToSignalGroups(NIVisumLoader &parent,
        const std::string &dataName, NIVisumLoader::NIVisumTL_Map &NIVisumTLs);

    /// Destructor
    ~NIVisumParser_TurnsToSignalGroups();

protected:
    /** @brief Parses a single node using data from the inherited NamedColumnsParser. */
    void myDependentReport();
private:
	NIVisumLoader::NIVisumTL_Map &myNIVisumTLs;
	// return edge between FromNode and ToNode if connected
	NBEdge *getEdge(NBNode *FromNode, NBNode *ToNode);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NIVisumParser_TurnsToSignalGroups.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

