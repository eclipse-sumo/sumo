#ifndef MSTDDETECTORINTERFACE_H
#define MSTDDETECTORINTERFACE_H

///
/// @file    MSTDDetectorInterface.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Tue Dec 02 2003 20:03 CET
/// @version $Id$
///
/// @brief
///
///

/* Copyright (C) 2003, 2004 by German Aerospace Center (http://www.dlr.de) */

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


#include "MSDetectorInterfaceCommon.h"
#include <microsim/MSUpdateEachTimestep.h>

/// @namespace TD The namespace TD contains the specifics of the TD
/// (timestep data) detectors. These detectors update their value
/// every timestep and provide access to this timestep data.
/// @see namespace ED, namespace LD

namespace TD
{
    /// This abstract class declares the particular methods for the TD
    /// (timestep data) detectors.
    class MSDetectorInterface
        :
        public MSDetectorInterfaceCommon
        , public MSUpdateEachTimestep< MSDetectorInterface >
    {
    public:

        /// Get the detector's value compiled in the last/current
        /// (depending on the time of the query) timestep.
        ///
        /// @return The detector's value compiled in one timestep.
        ///
        virtual double getCurrent( void ) const = 0;

        /// Dtor.
        virtual ~MSDetectorInterface( void )
            {}

    protected:

        /// Ctor. Passed the id to base class.
        ///
        /// @param id The detector's id.
        MSDetectorInterface( std::string id )
            :
            MSDetectorInterfaceCommon( id )
            {}

    private:

    };

} // end namespace TD

namespace Detector
{
    /// Alias for easier access to the MSUpdateEachTimestep interface
    /// for TD::MSDetectorInterface detectors.
    typedef MSUpdateEachTimestep< TD::MSDetectorInterface > UpdateE2Detectors;
}


// Local Variables:
// mode:C++
// End:

#endif // MSTDDETECTORINTERFACE_H
