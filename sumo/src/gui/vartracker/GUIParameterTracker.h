#ifndef GUIParameterTracker_h
#define GUIParameterTracker_h

#include <vector>
#include <qgl.h>
#include <qdialog.h>
#include <gui/GUIGlObject.h>
#include <utils/qutils/NewQMutex.h>
#include <utils/glutils/lfontrenderer.h>
#include "TrackerValueDesc.h"

class QPaintEvent;

class GUIParameterTracker : public QDialog
{
    Q_OBJECT
public:
    GUIParameterTracker( GUIApplicationWindow *app,
        QWidget * parent, GUIGlObject *o, size_t itemNo );

    GUIParameterTracker( GUIApplicationWindow *app,
        QWidget * parent );

    ~GUIParameterTracker();

    int getMaxGLWidth() const;

    int getMaxGLHeight() const;

protected:
    void addVariable( GUIGlObject *o, size_t itemNo );

    bool event ( QEvent *e );

    void resizeEvent ( QResizeEvent * );

    void paintEvent ( QPaintEvent * );

private:
    class GUIParameterTrackerPanel : public QGLWidget {
    public:
        GUIParameterTrackerPanel(GUIApplicationWindow *app,
            GUIParameterTracker *parent);
        ~GUIParameterTrackerPanel();
        friend class GUIParameterTracker;
    protected:
        /// derived from QGLWidget, this method initialises the openGL canvas
        void initializeGL();

        /// called when the canvas has been resized
        void resizeGL( int, int );

        /// performs the painting of the simulation
        void paintGL();

    private:
        void drawValues();
        void drawValue(TrackerValueDesc &desc, float namePos);
        float patchHeightVal(TrackerValueDesc &desc, double d);


    private:
        GUIParameterTracker &myParent;

        /// A lock for drawing operations
        NewQMutex _lock; // !!! (same as in abstract view)

        /// Information how many times the drawing method was called at once
        size_t _noDrawing;

        /// the sizes of the window
        int _widthInPixels, _heightInPixels;

        LFontRenderer myFontRenderer;

        GUIApplicationWindow *myApplication;

    };

public:
    friend class GUIParameterTrackerPanel;

protected:
    GUIApplicationWindow *myApplication;
    typedef std::vector<TrackerValueDesc*> TrackedVarsVector;
    TrackedVarsVector myTracked;
    GUIParameterTrackerPanel *myPanel;
};


#endif
