#include <string>
#include <utils/common/StringTokenizer.h>
#include <utils/convert/TplConvert.h>
#include "GfxConvHelper.h"


RGBColor
GfxConvHelper::parseColor(const std::string &coldef)
{
    StringTokenizer st(coldef, ",");
    double r = TplConvert<char>::_2float(st.next().c_str());
    double g = TplConvert<char>::_2float(st.next().c_str());
    double b = TplConvert<char>::_2float(st.next().c_str());
    return RGBColor(r, g, b);
}

