#ifndef NIArtemisParser_HVdests_h
#define NIArtemisParser_HVdests_h
/***************************************************************************
                          NIArtemisParser_HVdests.h
                             -------------------
    project              : SUMO
    begin                : Wed, 13 Feb 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.1  2003/03/12 16:44:45  dkrajzew
// further work on artemis-import
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
 * @class NIArtemisParser_HVdests
 */
class NIArtemisParser_HVdests :
        public NIArtemisLoader::NIArtemisSingleDataTypeParser {
public:
    /// Constructor
    NIArtemisParser_HVdests(NIArtemisLoader &parent,
        const std::string &dataName);

    /// Destructor
    ~NIArtemisParser_HVdests();

    /// HVdests are not mandatory
    bool amOptional() const { return true; }

protected:
    /** @brief Parses a single modality type name using data from the inherited NamedColumnsParser. */
    void myDependentReport();

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NIArtemisParser_HVdests.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
