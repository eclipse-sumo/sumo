#ifndef NIArtemisParser_Nodes_h
#define NIArtemisParser_Nodes_h
/***************************************************************************
                          NIArtemisParser_Nodes.h
                             -------------------
    project              : SUMO
    begin                : Mon, 10 Feb 2003
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
// $Log$
// Revision 1.3  2005/04/27 12:24:25  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.2  2003/03/20 16:25:12  dkrajzew
// windows eol removed
//
// Revision 1.1  2003/03/03 15:00:34  dkrajzew
// initial commit for artemis-import files
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include "NIArtemisLoader.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBNodeCont;
class NBTrafficLightLogicCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIArtemisParser_Nodes
 */
class NIArtemisParser_Nodes :
        public NIArtemisLoader::NIArtemisSingleDataTypeParser {
public:
    /// Constructor
    NIArtemisParser_Nodes(NBNodeCont &nc, NBTrafficLightLogicCont &tlc,
        NIArtemisLoader &parent, const std::string &dataName);

    /// Destructor
    ~NIArtemisParser_Nodes();

protected:
    /** @brief Parses a single modality type name using data from the inherited NamedColumnsParser. */
    void myDependentReport();

protected:
    NBNodeCont &myNodeCont;
    NBTrafficLightLogicCont &myTLLogicCont;


};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
