#include <string>
#include <utils/geom/Position2DVector.h>
#include <utils/common/StringTokenizer.h>
#include <utils/convert/TplConvert.h>
#include "GeomConvHelper.h"



Position2DVector
GeomConvHelper::parseShape(const std::string &shpdef)
{
    StringTokenizer st(shpdef, " ");
    Position2DVector shape;
    while(st.hasNext()) {
        StringTokenizer pos(st.next(), ",");
        float x = TplConvert<char>::_2float(pos.next().c_str());
        float y = TplConvert<char>::_2float(pos.next().c_str());
        shape.push_back(
            Position2D(x, y));
    }
    return shape;
}


