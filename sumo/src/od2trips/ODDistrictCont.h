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
// Revision 1.4  2003/08/04 11:37:37  dkrajzew
// added the generation of colors from districts
//
// Revision 1.3  2003/02/07 10:44:19  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "ODDistrict.h"
#include <utils/common/NamedObjectCont.h>

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
    double getDistrictColor(const std::string &name) const;

private:
    /// Holds the information whether the districts were colored
    bool myHadColorized;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ODDistrictCont.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

