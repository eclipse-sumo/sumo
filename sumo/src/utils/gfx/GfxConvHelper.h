#ifndef GfxConvHelper_h
#define GfxConvHelper_h

#include <string>
#include <utils/gfx/RGBColor.h>

class GfxConvHelper {
public:
    static RGBColor parseColor(const std::string &coldef);
};


#endif
