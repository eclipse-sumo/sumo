#ifndef bezier_h
#define bezier_h
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


/* Bezier curve subroutine */
void
bezier(int npts, SUMOReal b[], int cpts, SUMOReal p[]);

#endif
