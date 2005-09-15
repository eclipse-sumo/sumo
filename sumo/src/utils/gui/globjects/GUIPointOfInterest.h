#ifndef GUIPointOfInterest_h
#define GUIPointOfInterest_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/shapes/PointOfInterest.h>
#include <utils/gui/globjects/GUIGlObject.h>

class GUIGlObjectStorage;

class GUIPointOfInterest : public PointOfInterest, public GUIGlObject {
public:
    GUIPointOfInterest(GUIGlObjectStorage &idStorage,
        const std::string &id, const std::string &type,
        const Position2D &p, const RGBColor &c);

    virtual ~GUIPointOfInterest();


    /// Returns an own popup-menu
    GUIGLObjectPopupMenu *getPopUpMenu(
        GUIMainWindow &app, GUISUMOAbstractView &parent);

    /// Returns an own parameter window
    GUIParameterTableWindow *getParameterWindow(
        GUIMainWindow &app, GUISUMOAbstractView &parent);

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    /// returns the id of the object as known to microsim
    std::string microsimID() const;

    /// Returns the information whether this object is still active
    bool active() const;

	/// Returns the boundary to which the object shall be centered
	Boundary getCenteringBoundary() const;

};

#endif
