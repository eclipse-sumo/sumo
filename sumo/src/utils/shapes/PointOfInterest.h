#ifndef PointOfInterest_h
#define PointOfInterest_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2D.h>
#include <utils/common/Named.h>

class PointOfInterest :
    public RGBColor, public Position2D, public Named {
public:
    PointOfInterest(const std::string &id, const std::string &type,
        const Position2D &p, const RGBColor &c)
        : Named(id), RGBColor(c), Position2D(p), myType(type) { }
    virtual ~PointOfInterest() { }

    const std::string &getType() const { return myType; }

protected:
    std::string myType;

};

#endif
