#ifndef GUIColorer_LaneByPurpose_h
#define GUIColorer_LaneByPurpose_h

#include <utils/gui/drawer/GUIBaseColorer.h>
#include <utils/gfx/RGBColor.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <microsim/MSEdge.h>
#include <guisim/GUILaneWrapper.h>
#include <GL/gl.h>


template<class _T>
class GUIColorer_LaneByPurpose : public GUIBaseColorer<_T> {
public:
    GUIColorer_LaneByPurpose() { }

    virtual ~GUIColorer_LaneByPurpose() { }

    void setGlColor(const _T& i) const {
        switch(i.getPurpose()) {
        case MSEdge::EDGEFUNCTION_NORMAL:
            glColor3f(0, 0, 0);
            return;
        case MSEdge::EDGEFUNCTION_SOURCE:
            glColor3f(0, 1, 0);
            return;
        case MSEdge::EDGEFUNCTION_SINK:
            glColor3f(1, 0, 0);
            return;
        case MSEdge::EDGEFUNCTION_INTERNAL:
            glColor3f(0, 0, 1);
            return;
        default:
            throw 1;
        }
    }

	void setGlColor(double val) const {
        glColor3d(val, val, val);
    }

};


#endif
