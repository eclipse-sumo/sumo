#ifndef MSVehicleQuitReminded_h
#define MSVehicleQuitReminded_h
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


class MSVehicleQuitReminded {
public:
    virtual ~MSVehicleQuitReminded() {}
    virtual void removeOnTripEnd( MSVehicle *veh ) = 0;
};

#endif

