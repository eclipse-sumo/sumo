#ifndef GUIInductLoop_h
#define GUIInductLoop_h

#include <microsim/MSInductLoop.h>
#include <microsim/MSNet.h>
#include <utils/geom/Position2D.h>
#include "GUIDetectorWrapper.h"


class GUIGlObjectStorage;
class GUILaneWrapper;

class GUIInductLoop : public MSInductLoop
{
public:
    GUIInductLoop(const std::string &id, MSLane* lane, double position,
        MSNet::Time deleteDataAfterSeconds=900);
    ~GUIInductLoop();
    // valid for gui-version only
    virtual GUIDetectorWrapper *buildDetectorWrapper(
        GUIGlObjectStorage &idStorage,
        GUILaneWrapper &wrapper);

public:
    class MyWrapper : public GUIDetectorWrapper {
    public:
        MyWrapper(GUIInductLoop &detector,
            GUIGlObjectStorage &idStorage, GUILaneWrapper &wrapper,
            double pos);
        ~MyWrapper();
        Boundery getBoundery() const;
        void drawGL_FG(double scale) const;
        void drawGL_SG(double scale) const;

        GUIParameterTableWindow *getParameterWindow(
            GUIApplicationWindow &app, GUISUMOAbstractView &parent);

        /// Returns the type of the object as coded in GUIGlObjectType
        GUIGlObjectType getType() const;

        /// returns the id of the object as known to microsim
        std::string microsimID() const;

        /// Needed to set the id
        friend class GUIGlObjectStorage;

        bool active() const;

        double getXCoordinate() const;
        double getYCoordinate() const;

        GUIInductLoop &getLoop();

    private:
        GUIInductLoop &myDetector;
        Boundery myBoundery;
        Position2D myFGPosition;
        Position2D mySGPosition;
//        Position2D myBegin;
//        Position2D myEnd;
        double myFGRotation, mySGRotation;

    };

private:
};


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

//#ifndef DISABLE_INLINE
//#include "GUIInductLoop.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


