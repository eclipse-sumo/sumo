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

private:
    class MyWrapper : public GUIDetectorWrapper {
    public:
        MyWrapper(GUIInductLoop &detector,
            GUIGlObjectStorage &idStorage, GUILaneWrapper &wrapper,
            double pos);
        ~MyWrapper();
        Boundery getBoundery() const;
        void drawGL(double scale) const;

        /// Returns an own popup-menu
        QGLObjectPopupMenu *getPopUpMenu(
            GUIApplicationWindow *app, GUISUMOAbstractView *parent);

        /// Returns the type of the object as coded in GUIGlObjectType
        GUIGlObjectType getType() const;

        /// returns the id of the object as known to microsim
        std::string microsimID() const;

        void insertTableParameter(GUIParameterTableWindow *window,
            QListView *table, double *parameter,
            QListViewItem **vitems);

        size_t getTableParameterNo() const;

        double getTableParameter(size_t pos) const;

        void fillTableParameter(double *parameter) const;

        /// Needed to set the id
        friend class GUIGlObjectStorage;

        const char * const getTableItem(size_t pos) const;

	    bool active() const;
    protected:

        TableType getTableType(size_t pos) const;

        const char *getTableBeginValue(size_t pos) const;


    private:
        GUIInductLoop &myDetector;
        Boundery myBoundery;
        Position2D myPosition;
        Position2D myBegin;
        Position2D myEnd;
        double myRotation;

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


