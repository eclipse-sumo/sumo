#ifndef GUI_E2_ZS_Collector_h
#define GUI_E2_ZS_Collector_h

#include <microsim/MS_E2_ZS_Collector.h>
#include <microsim/MSNet.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/common/DoubleVector.h>
#include "GUIDetectorWrapper.h"


class GUIGlObjectStorage;
class GUILaneWrapper;

class GUI_E2_ZS_Collector : public MS_E2_ZS_Collector
{
public:
    GUI_E2_ZS_Collector( std::string id, //daraus ergibt sich der Filename
                        MSLane* lane,
                        MSUnit::Meters startPos,
                        MSUnit::Meters detLength,
                        MSUnit::Seconds haltingTimeThreshold = 1,
                        MSUnit::MetersPerSecond haltingSpeedThreshold =5.0/3.6,
                        MSUnit::Meters jamDistThreshold = 10,
                        MSUnit::Seconds deleteDataAfterSeconds = 1800 );
    ~GUI_E2_ZS_Collector();
    // valid for gui-version only
    virtual GUIDetectorWrapper *buildDetectorWrapper(
        GUIGlObjectStorage &idStorage,
        GUILaneWrapper &lane);

    bool amVisible() const {
        return true;
    }

public:
    class MyWrapper : public GUIDetectorWrapper {
    public:
        MyWrapper(GUI_E2_ZS_Collector &detector,
            GUIGlObjectStorage &idStorage, GUILaneWrapper &wrapper);
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

        Position2D getPosition() const;

        GUI_E2_ZS_Collector &getLoop();
    protected:
        void myMkExistingItem(GUIParameterTableWindow &ret,
            const std::string &name, MS_E2_ZS_Collector::DetType type);

    private:
        GUI_E2_ZS_Collector &myDetector;
        Boundery myBoundery;
        Position2D mySGPosition;
        double mySGRotation;
		double mySGLength;
		Position2DVector myFullGeometry;
		DoubleVector myShapeLengths;
		DoubleVector myShapeRotations;

        class ValueRetriever : public DoubleValueSource {
        public:
            ValueRetriever(GUI_E2_ZS_Collector &det,
                MS_E2_ZS_Collector::DetType type, size_t nSec)
                : myDetector(det), myType(type), myNSec(nSec) { }
            ~ValueRetriever() { }
            double getValue() const
                {
                    return myDetector.getAggregate(myType, myNSec);
                }

            DoubleValueSource *copy() const {
                return new ValueRetriever(myDetector, myType, myNSec);
            }

        private:
            GUI_E2_ZS_Collector &myDetector;
            MS_E2_ZS_Collector::DetType myType;
            size_t myNSec;
        };

    };

private:
};


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

//#ifndef DISABLE_INLINE
//#include "GUI_E2_ZS_Collector.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


