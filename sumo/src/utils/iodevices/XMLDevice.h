#ifndef XMLDevice_h
#define XMLDevice_h
//---------------------------------------------------------------------------//
//                        XMLDevice.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//


/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)



/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class XMLDevice {
public:
    XMLDevice() { }
    virtual ~XMLDevice() { }
    virtual XMLDevice &writeString(const std::string &str) = 0;
    virtual void closeInfo() = 0;
    virtual bool needsDetectorName() const = 0;
};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
