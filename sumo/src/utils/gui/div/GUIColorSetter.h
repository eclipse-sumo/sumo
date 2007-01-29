#ifndef GUIColorSetter_h
#define GUIColorSetter_h

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


#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <utils/gfx/RGBColor.h>


inline
void
mglColor(const RGBColor &c) {
    glColor3d(c.red(), c.green(), c.blue());
}

#endif
