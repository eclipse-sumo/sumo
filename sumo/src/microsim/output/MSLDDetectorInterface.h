#ifndef MSLDDETECTORINTERFACE_H
#define MSLDDETECTORINTERFACE_H

///
/// @file    MSLDDetectorInterface.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Tue Dec 02 2003 20:04 CET
/// @version
///
/// @brief
///
///

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)

/* =========================================================================
 * included modules
 * ======================================================================= */

#include "MSDetectorInterfaceCommon.h"
#include <string>
/* =========================================================================
 * class declarations
 * ======================================================================= */

class MSVehicle;

/// @namespace LD The namespace LD contains the specifics of the LD
/// (leave data) detectors. These detector update their value if a
/// vehicles leaves the detector.
/// @see namespace ED, namespace TD

namespace LD
{
    /// This abstract class declares the particular methods for the LD
    /// (leave data) detectors.
    class MSDetectorInterface
        :
        public MSDetectorInterfaceCommon
    {
    public:

        /// Perform detector update if vehicle leaves the detector.
        ///
        /// @param veh The leaving vehicle.
        ///
        virtual void leave( MSVehicle& veh ) = 0;

        /// Dtor
        virtual ~MSDetectorInterface( void )
            {}

    protected:

        /// Ctor. Passed the id to base class.
        ///
        /// @param id The detector's id.
        MSDetectorInterface( std::string id )
            : MSDetectorInterfaceCommon( id )
            {}

    private:

    };

} // end namespace LD


// Local Variables:
// mode:C++
// End:

#endif // MSLDDETECTORINTERFACE_H
