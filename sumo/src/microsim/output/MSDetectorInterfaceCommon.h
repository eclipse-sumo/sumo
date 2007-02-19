/****************************************************************************/
/// @file    MSDetectorInterfaceCommon.h
/// @author  Christian Roessel
/// @date    Mon Dec 01 2003 16:20 CET
/// @version $Id$
///
// / @author  Christian Roessel <christian.roessel@dlr.de>
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
#ifndef MSDetectorInterfaceCommon_h
#define MSDetectorInterfaceCommon_h
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

#include <string>
#include <microsim/MSUnit.h>

///
/// Commom interface for all TD, ED and LD detectors.
///
/// @see MSTDDetector.h
/// @see MSEDDetector.h
/// @see MSLDDetector.h
///
// ===========================================================================
// class definitions
// ===========================================================================
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
    const std::string& getID(void) const
    {
        return idM;
    }

    /// Get the name of the detector-type, e.g. "density"
    ///
    /// @return The member detNameM.
    ///
    const std::string& getName(void) const
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
    virtual SUMOReal getAggregate(MSUnit::Seconds lastNSeconds) = 0;

    /// Dtor.
    virtual ~MSDetectorInterfaceCommon(void)
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
            idM(id)
            , detNameM("")
    {}

    std::string idM;            ///< The detector's id.
    std::string detNameM;       ///< The name of the detector-type.

private:

};


#endif

/****************************************************************************/

