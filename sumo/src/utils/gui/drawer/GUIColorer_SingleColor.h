#ifndef GUIColorer_SingleColor_h
#define GUIColorer_SingleColor_h

#include "GUIBaseColorer.h"
#include <utils/gfx/RGBColor.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


template<class _T>
class GUIColorer_SingleColor : public GUIBaseColorer<_T> {
public:
    GUIColorer_SingleColor(const RGBColor &c) : myColor(c) { }
    virtual ~GUIColorer_SingleColor() { }
    void setGlColor(const _T& i) const {
        glColor3d(myColor.red(), myColor.green(), myColor.blue());
    }
private:
    RGBColor myColor;

};


#endif
