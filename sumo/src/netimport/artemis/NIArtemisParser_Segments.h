/****************************************************************************/
/// @file    NIArtemisParser_Segments.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 10 Feb 2003
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIArtemisParser_Segments_h
#define NIArtemisParser_Segments_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "NIArtemisLoader.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIArtemisParser_Segments
 */
class NIArtemisParser_Segments :
            public NIArtemisLoader::NIArtemisSingleDataTypeParser
{
public:
    /// Constructor
    NIArtemisParser_Segments(NBEdgeCont &ec, NIArtemisLoader &parent,
                             const std::string &dataName);

    /// Destructor
    ~NIArtemisParser_Segments();

    /// Segments are not mandatory
    bool amOptional() const
    {
        return true;
    }


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


#endif

/****************************************************************************/

