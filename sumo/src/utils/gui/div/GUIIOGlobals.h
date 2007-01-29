#ifndef GUIIOGlobals_h
#define GUIIOGlobals_h
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)



#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>

/// The folder used as last
extern std::string gCurrentFolder;

#endif
