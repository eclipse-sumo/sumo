#ifndef NIArtemisParser_Signals_h
#define NIArtemisParser_Signals_h
/***************************************************************************
                          NIArtemisParser_Signals.h
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
// Revision 1.1  2003/03/03 15:00:38  dkrajzew
// initial commit for artemis-import files
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "NIArtemisLoader.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIArtemisParser_Signals
 */
class NIArtemisParser_Signals :
        public NIArtemisLoader::NIArtemisSingleDataTypeParser {
public:
    /// Constructor
    NIArtemisParser_Signals(NIArtemisLoader &parent,
        const std::string &dataName);

    /// Destructor
    ~NIArtemisParser_Signals();

protected:
    /** @brief Parses a single modality type name using data from the inherited NamedColumnsParser. */
    void myDependentReport();

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NIArtemisParser_Signals.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
