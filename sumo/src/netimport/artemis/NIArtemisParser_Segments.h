#ifndef NIArtemisParser_Segments_h
#define NIArtemisParser_Segments_h
/***************************************************************************
                          NIArtemisParser_Segments.h
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
// Revision 1.4  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/04/27 12:24:25  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.2  2003/03/17 14:18:58  dkrajzew
// Windows eol removed
//
// Revision 1.1  2003/03/12 16:44:45  dkrajzew
// further work on artemis-import
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

#include "NIArtemisLoader.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIArtemisParser_Segments
 */
class NIArtemisParser_Segments :
        public NIArtemisLoader::NIArtemisSingleDataTypeParser {
public:
    /// Constructor
    NIArtemisParser_Segments(NBEdgeCont &ec, NIArtemisLoader &parent,
        const std::string &dataName);

    /// Destructor
    ~NIArtemisParser_Segments();

    /// Segments are not mandatory
    bool amOptional() const { return true; }


protected:
    /** @brief Parses a single modality type name using data from the inherited NamedColumnsParser. */
    void myDependentReport();

    /** @brief Called after loading all segment definitions
        Sets the geometrical information of the edges.
        Speed definitions are not yet supported */
    void myClose();

protected:
    NBEdgeCont &myEdgeCont;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:
