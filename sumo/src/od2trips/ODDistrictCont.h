/****************************************************************************/
/// @file    ODDistrictCont.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// The container for districts
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
#ifndef ODDistrictCont_h
#define ODDistrictCont_h
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

#include "ODDistrict.h"
#include <utils/helpers/NamedObjectCont.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ODDistrictCont
 * A container for districts.
 */
class ODDistrictCont : public NamedObjectCont<ODDistrict*>
{
public:
    /// Constructor
    ODDistrictCont();

    /// Destructor
    ~ODDistrictCont();

    /// Returns a source from the named district by random
    std::string getRandomSourceFromDistrict(const std::string &name) const;

    /// Returns a sink from the named district by random
    std::string getRandomSinkFromDistrict(const std::string &name) const;

    /// Computes abstract colors for all districts
    void colorize();

    /// Returns an abstract (computed) color of the district
    SUMOReal getDistrictColor(const std::string &name) const;

private:
    /// Holds the information whether the districts were colored
    bool myHadColorized;

};


#endif

/****************************************************************************/

