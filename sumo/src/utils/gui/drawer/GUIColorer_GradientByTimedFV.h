#ifndef GUIColorer_GradientByTimedFV_h
#define GUIColorer_GradientByTimedFV_h

#include "GUIBaseColorer.h"
#include <utils/gfx/RGBColor.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <utils/gui/div/GUIGlobalSelection.h>

template<class _T>
class GUIColorer_GradientByTimedFV : public GUIBaseColorer<_T> {
public:
    /// Type of the function to execute.
    typedef double ( _T::* Operation )() const;

    GUIColorer_GradientByTimedFV(double min, double max,
        const std::vector<RGBColor> &gradient, unsigned int timeGiver,
        Operation operation)
        : myMin(min), myMax(max), myGradient(gradient),
            myTimeGiver(timeGiver), myOperation(operation)
    {
        myScale = 1.0 / (myMax-myMin);
    }

    virtual ~GUIColorer_GradientByTimedFV() { }

    void setGlColor(const _T& i) const {
        double val = (i.*myOperation)() - myMin;
        val = val * myScale;
        if(val==-1) {
            glColor3f(0.5, 0.5, 0.5);
        } else {
            if(val<0) {
                val = 0; // !!! Aua!!!
            } else if(val>1) {
                val = 1; // !!! Aua!!!
            }
            int idx = (int) (val * (double) myGradient.size());
            assert(idx<myGradient.size());
            const RGBColor &c = myGradient[idx];
            glColor3f(c.red(), c.green(), c.blue());
    }

protected:
    double myMin, myMax, myScale;
    std::vector<RGBColor> myGradient;

    /// The object's operation to perform.
    Operation myOperation;


};


#endif
