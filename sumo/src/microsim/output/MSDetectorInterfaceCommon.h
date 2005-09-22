#ifndef MSDETECTORINTERFACECOMMON_H
#define MSDETECTORINTERFACECOMMON_H

///
/// @file    MSDetectorInterfaceCommon.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Mon Dec 01 2003 16:20 CET
/// @version
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

/* =========================================================================
 * included modules
 * ======================================================================= */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <microsim/MSUnit.h>

///
/// Commom interface for all TD, ED and LD detectors.
///
/// @see MSTDDetector.h
/// @see MSEDDetector.h
/// @see MSLDDetector.h
///
/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSDetectorInterfaceCommon
{
public:

    /// Get the id of a particular detector instance.
    ///
    /// @return The member idM.
    ///
    const std::string& getId( void ) const
        {
            return idM;
        }

    /// Get the name of the detector-type, e.g. "density"
    ///
    /// @return The member detNameM.
    ///
    const std::string& getName( void ) const
        {
            return detNameM;
        }

    /// Get the aggregated value of a detector sampled over the last
    /// lastNSeconds.
    ///
    /// @param lastNSeconds Length of the aggregation intervall
    /// (now-lastNSeconds, now].
    ///
    /// @return An aggregated value.
    ///
    virtual SUMOReal getAggregate( MSUnit::Seconds lastNSeconds ) = 0;

    /// Dtor.
    virtual ~MSDetectorInterfaceCommon( void )
        {}

protected:
    /// Ctor. Note that the member detNameM has to be set by the ctor
    /// of the inherited classes.
    ///
    /// @param id The detector's id.
    ///
    MSDetectorInterfaceCommon(
        std::string id
        //, std::string detName
        )
        :
        idM( id )
        , detNameM("")
        {}

    std::string idM;            ///< The detector's id.
    std::string detNameM;       ///< The name of the detector-type.

private:

};


// Local Variables:
// mode:C++
// End:

#endif // MSDETECTORINTERFACECOMMON_H
