#ifndef GUIGlObject_AbstractAdd_h
#define GUIGlObject_AbstractAdd_h

#include "GUIGlObjectTypes.h"
#include "GUIGlObject.h"
#include <utils/geom/HaveBoundery.h>
#include <utils/geom/Position2D.h>
#include <vector>
#include <map>

class GUIGlObject_AbstractAdd :
    public GUIGlObject, public HaveBoundery {
public:
    GUIGlObject_AbstractAdd(GUIGlObjectStorage &idStorage,
        std::string fullName, GUIGlObjectType type);

    /// Constructor for objects joining gl-objects
    GUIGlObject_AbstractAdd(GUIGlObjectStorage &idStorage,
        std::string fullName, size_t glID, GUIGlObjectType type);

    ~GUIGlObject_AbstractAdd();

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    /// Draws the detector in full-geometry mode
    virtual void drawGL_FG(double scale) = 0;

    /// Draws the detector in simple-geometry mode
    virtual void drawGL_SG(double scale) = 0;

    /// Returns the detector's coordinates
    virtual Position2D getPosition() const = 0;

    static void clearDictionary();

    static GUIGlObject_AbstractAdd *get(const std::string &name);

    static const std::vector<GUIGlObject_AbstractAdd*> &getObjectList();

protected:
    GUIGlObjectType myGlType;

    static std::map<std::string, GUIGlObject_AbstractAdd*> myObjects;

    static std::vector<GUIGlObject_AbstractAdd*> myObjectList;

};

#endif
