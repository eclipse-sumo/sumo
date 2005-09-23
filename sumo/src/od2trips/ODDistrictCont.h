#ifndef ODDistrictCont_h
#define ODDistrictCont_h
//---------------------------------------------------------------------------//
//                        ODDistrictCont.h -
//  The container for districts
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.8  2005/09/23 06:04:23  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.7  2005/09/15 12:04:48  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/05/04 08:44:57  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.5  2004/07/02 09:38:21  dkrajzew
// coding style adaptations
//
// Revision 1.4  2003/08/04 11:37:37  dkrajzew
// added the generation of colors from districts
//
// Revision 1.3  2003/02/07 10:44:19  dkrajzew
// updated
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

#include "ODDistrict.h"
#include <utils/helpers/NamedObjectCont.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ODDistrictCont
 * A container for districts.
 */
class ODDistrictCont : public NamedObjectCont<ODDistrict*> {
public:
    /// Constructor
    ODDistrictCont();

    /// Destructor
    ~ODDistrictCont();

    /// Returns a source from the named district by random
    std::string getRandomSourceFromDistrict(const std::string &name) const;

    /// Returns a sink from the named district by random
    std::string getRandomSinkFromDistrict(const std::string &name) const;

    // Computes abstract colors for all districts
    void colorize();

    /// Returns an abstract (computed) color of the district
    SUMOReal getDistrictColor(const std::string &name) const;

private:
    /// Holds the information whether the districts were colored
    bool myHadColorized;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

