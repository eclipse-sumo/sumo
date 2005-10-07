#ifndef MSDebugHelper_h
#define MSDebugHelper_h

#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/common/SUMOTime.h>


/// The current simulation time for debugging purposes
extern SUMOTime debug_globaltime;

/// ----------------- debug variables -------------

#ifdef ABS_DEBUG
extern SUMOTime debug_searchedtime;
extern std::string debug_searched1, debug_searched2, debug_searchedJunction;
#endif


#endif
