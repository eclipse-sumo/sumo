#ifndef GUIMainWindow_h
#define GUIMainWindow_h

#include <fx.h>
#include <vector>
#include <utils/foxtools/FXMutex.h>

class GUIMainWindow : public FXMainWindow {
public:
    GUIMainWindow(FXApp* a, int glWidth, int glHeight);
    virtual ~GUIMainWindow();
    /// Adds a further child window to the list
    void addChild(FXMDIChild *child, bool updateOnSimStep=true);
    void addChild(FXMainWindow *child, bool updateOnSimStep=true);

    /// removes the given child window from the list
    void removeChild(FXMDIChild *child);
    void removeChild(FXMainWindow  *child);

    void updateChildren();

    FXFont *getBoldFont();

    /// Returns the maximum width of gl-windows
    int getMaxGLWidth() const;

    /// Returns the maximum height of gl-windows
    int getMaxGLHeight() const;

    FXGLVisual *getGLVisual() const;

    virtual FXGLCanvas *getBuildGLCanvas() const = 0;

    virtual size_t getCurrentSimTime() const = 0;

    virtual void loadSelection(const std::string &file) const = 0;

protected:
    std::vector<FXMDIChild*> mySubWindows;
    std::vector<FXMainWindow*> myTrackerWindows;
    /// A lock to make the removal and addition of trackers secure
    FXEX::FXMutex myTrackerLock;

    /// Font used for popup-menu titles
    FXFont *myBoldFont;

    /// The openGL-maximum screen sizes
    int myGLWidth, myGLHeight;

    /// The multi view panel
    FXMDIClient *myMDIClient;


    /// The gl-visual used
    FXGLVisual *myGLVisual;


protected:
    GUIMainWindow() { }

};

#endif

