#ifndef GUITrafficLightLogicWrapper_h
#define GUITrafficLightLogicWrapper_h

#include <gui/GUIGlObject.h>
#include <gui/tlstracker/GUITLLogicPhasesTrackerWindow.h>

class GUIGlObjectStorage;
class MSTrafficLightLogic;

class GUITrafficLightLogicWrapper
    : public GUIGlObject {
public:
    /// Constructor
    GUITrafficLightLogicWrapper(GUIGlObjectStorage &idStorage,
        MSTrafficLightLogic &tll);

    /// Destructor
    ~GUITrafficLightLogicWrapper();

    /// Returns an own popup-menu
    QGLObjectPopupMenu *getPopUpMenu(GUIApplicationWindow &app,
        GUISUMOAbstractView &parent);

    /// Returns an own parameter window
    GUIParameterTableWindow *getParameterWindow(GUIApplicationWindow &app,
        GUISUMOAbstractView &parent);

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    /// returns the id of the object as known to microsim
    std::string microsimID() const;

    /// Returns the information whether this object is still active
	bool active() const;

    SimplePhaseDef getPhaseDef() const;

    void showPhases();

private:
    /// The wrapped tl-logic
    MSTrafficLightLogic &myTLLogic;

    GUIApplicationWindow *myApp;

};


#endif
