#ifndef GeoConvHelper_h
#define GeoConvHelper_h

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <map>
#include <string>
#include <proj_api.h>
#include <utils/geom/Position2D.h>

class GeoConvHelper {
public:
    static bool init(const std::string &proj,
        const Position2D &offset);
    static bool initialised();
    static void close();
    static void cartesian2geo(Position2D &cartesian);
    static void remap(Position2D &from);

private:
    static projPJ myProjection;
    static Position2D myOffset;
    static bool myDisableProjection;
    static SUMOReal myInitX, myInitY;

};


#endif
