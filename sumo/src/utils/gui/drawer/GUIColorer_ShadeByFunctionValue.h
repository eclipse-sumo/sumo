#ifndef GUIColorer_ShadeByFunctionValue_h
#define GUIColorer_ShadeByFunctionValue_h

#include "GUIBaseColorer.h"
#include <utils/gfx/RGBColor.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <utils/gui/div/GUIGlobalSelection.h>

template<class _T>
class GUIColorer_ShadeByFunctionValue : public GUIBaseColorer<_T> {
public:
    /// Type of the function to execute.
    typedef double ( _T::* Operation )() const;

    GUIColorer_ShadeByFunctionValue(double min, double max,
        const RGBColor &minC, const RGBColor &maxC,
        Operation operation)
        : myMin(min), myMax(max), myMinColor(minC), myMaxColor(maxC),
            myOperation(operation)
    {
        myScale = 1.0 / (myMax-myMin);
    }

    virtual ~GUIColorer_ShadeByFunctionValue() { }

    void setGlColor(const _T& i) const {
        double val = (i.*myOperation)() - myMin;
        if(val==-1) {
            glColor3f(0.5, 0.5, 0.5);
        } else {
            if(val<myMin) {
                val = myMin; // !!! Aua!!!
            } else if(val>myMax) {
                val = myMax; // !!! Aua!!!
            }
            val = val * myScale;
            RGBColor c =
                (myMinColor * (1.0 - val)) + (myMaxColor * val);
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
        RGBColor c =
            (myMinColor * (1.0 - val)) + (myMaxColor * val);
        glColor3d(c.red(), c.green(), c.blue());
    }

protected:
    double myMin, myMax, myScale;
    RGBColor myMinColor, myMaxColor;

    /// The object's operation to perform.
    Operation myOperation;


};


#endif
