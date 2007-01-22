#ifndef MSSaveState_h
#define MSSaveState_h

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


enum MSSaveState {
    SAVESTATE_EDGES = 1,
    SAVESTATE_EMITTER = 2,
    SAVESTATE_LOGICS = 4,
    SAVESTATE_ROUTES = 8,
    SAVESTATE_VEHICLES = 16
};

#endif
