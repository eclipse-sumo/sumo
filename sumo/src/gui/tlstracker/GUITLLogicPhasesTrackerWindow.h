#ifndef GUITLLogicPhasesTrackerWindow_h
#define GUITLLogicPhasesTrackerWindow_h

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <bitset>
#include <utility>
#include <qgl.h>
#include <qdialog.h>
#include <qmainwindow.h>
#include <helpers/ValueRetriever.h>
#include <utils/qutils/NewQMutex.h>
#include <utils/glutils/lfontrenderer.h>

class GUIApplicationWindow;
class MSTrafficLightLogic;

typedef std::pair<std::bitset<64>, std::bitset<64> > SimplePhaseDef;

class GUITLLogicPhasesTrackerWindow : public QMainWindow,
    public ValueRetriever<SimplePhaseDef> {
    Q_OBJECT
public:
    GUITLLogicPhasesTrackerWindow(GUIApplicationWindow &app, 
        MSTrafficLightLogic &logic);
    ~GUITLLogicPhasesTrackerWindow();
    /// Returns the information about the largest width allowed for openGL-windows
    int getMaxGLWidth() const;

    /// Returns the information about the largest height allowed for openGL-windows
    int getMaxGLHeight() const;

    void addValue(SimplePhaseDef def);

protected:
    /// Callback for events
    bool event ( QEvent *e );

    /// Callback for the resize-event
    void resizeEvent ( QResizeEvent * );

    /// Callback for the paint-event
    void paintEvent ( QPaintEvent * );

    typedef std::vector<SimplePhaseDef> PhasesVector;
    typedef std::vector<size_t> DurationsVector;

    class GUITLLogicPhasesTrackerPanel : public QGLWidget {
    public:
        /// Constructor
        GUITLLogicPhasesTrackerPanel(GUIApplicationWindow &app,
            GUITLLogicPhasesTrackerWindow &parent);

        /// Destructor
        ~GUITLLogicPhasesTrackerPanel();

        size_t getHeightInPixels() const;
        size_t getWidthInPixels() const;

        /// needed to update
        friend class GUITLLogicPhasesTrackerWindow;

    protected:
        /// derived from QGLWidget, this method initialises the openGL canvas
        void initializeGL();

        /// called when the canvas has been resized
        void resizeGL( int, int );

        /// performs the painting of the simulation
        void paintGL();

    private:
        /// The parent window
        GUITLLogicPhasesTrackerWindow &myParent;

        /// A lock for drawing operations
        NewQMutex _lock; // !!! (same as in abstract view)

        /// Information how many times the drawing method was called at once
        size_t _noDrawing;

        /// the sizes of the window
        int _widthInPixels, _heightInPixels;

        /// The main application
        GUIApplicationWindow &myApplication;
    };

public:
    /// Draws all values
    void drawValues(GUITLLogicPhasesTrackerPanel &caller);

    void setFontRenderer(GUITLLogicPhasesTrackerPanel &caller);


private:
    /// The main application
    GUIApplicationWindow &myApplication;
    MSTrafficLightLogic &myTLLogic;
    PhasesVector myPhases;
    DurationsVector myDurations;
    GUITLLogicPhasesTrackerPanel *myPanel;
    NewQMutex myLock;
    std::vector<std::string> myLinkNames;
    /// The openGL-font drawer
    LFontRenderer myFontRenderer;

};

#endif
