#ifndef GUIColorer_GradientByFunctionValue_h
#define GUIColorer_GradientByFunctionValue_h

#include "GUIBaseColorer.h"
#include <utils/gfx/RGBColor.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <utils/gui/div/GUIGlobalSelection.h>

template<class _T, class _P, class _PC>
class GUIColorer_GradientByFunctionValue : public GUIBaseColorer<_T> {
public:
    /// Type of the function to execute.
    typedef double ( _T::* Operation )(_PC) const;

    GUIColorer_GradientByFunctionValue(double min, double max,
        const std::vector<RGBColor> &gradient, Operation operation,
        _P param)
        : myMin(min), myMax(max), myGradient(gradient),
            myOperation(operation), myParameter(param)
    {
        myScale = 1.0 / (myMax-myMin);
    }

    virtual ~GUIColorer_GradientByFunctionValue() { }

    void setGlColor(const _T& i) const {
        double val = (i.*myOperation)((_PC) myParameter) - myMin;
        if(val==-1) {
            glColor3f(0.5, 0.5, 0.5);
        } else {
            if(val<myMin) {
                val = myMin; // !!! Aua!!!
            } else if(val>myMax) {
                val = myMax; // !!! Aua!!!
            }
            val = val * myScale;
            int idx = (int) (val * (double) (myGradient.size()-1));
            assert(idx<(int) myGradient.size());
            const RGBColor &c = myGradient[idx];
            glColor3d(c.red(), c.green(), c.blue());
        }
	}

	void setGlColor(double val) const {
        if(val<myMin) {
            val = myMin; // !!! Aua!!!
        } else if(val>myMax) {
            val = myMax; // !!! Aua!!!
        }
        val = val * myScale;
        int idx = (int) (val * (double) (myGradient.size()-1));
        assert(idx<(int) myGradient.size());
        const RGBColor &c = myGradient[idx];
        glColor3d(c.red(), c.green(), c.blue());
    }


protected:
    double myMin, myMax, myScale;
    std::vector<RGBColor> myGradient;

    /// The object's operation to perform.
    Operation myOperation;

    ///
    _P myParameter;

};


#endif
