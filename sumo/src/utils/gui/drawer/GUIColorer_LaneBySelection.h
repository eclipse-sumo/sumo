#ifndef GUIColorer_LaneBySelection_h
#define GUIColorer_LaneBySelection_h

#include "GUIBaseColorer.h"
#include <utils/gfx/RGBColor.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <utils/gui/div/GUIGlobalSelection.h>

template<class _T>
class GUIColorer_LaneBySelection : public GUIBaseColorer<_T> {
public:
    GUIColorer_LaneBySelection() { }

    virtual ~GUIColorer_LaneBySelection() { }

    void setGlColor(const _T& i) const {
        if(gSelected.isSelected(i.getType(), i.getGlID())) {
            glColor3f(0, .4f, .8f);
        } else {
            glColor3f(0, 0, 0);
        }
    }

};


#endif
