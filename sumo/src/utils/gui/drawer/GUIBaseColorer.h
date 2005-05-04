#ifndef GUIBaseColorer_h
#define GUIBaseColorer_h

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

template<class _T>
class GUIBaseColorer {
public:
	GUIBaseColorer() { }
	virtual ~GUIBaseColorer() { }
	virtual void setGlColor(const _T& i) const = 0;
    virtual void setGlColor(double val) const {
        glColor3d(val, val, val);
    }
};


#endif
