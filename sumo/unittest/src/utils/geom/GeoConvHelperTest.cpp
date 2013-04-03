#include <gtest/gtest.h>
#include <utils/geom/GeoConvHelper.h>

using namespace std;

/*
Tests the class GeoConvHelper
*/


/* Test the method 'move2side' */
TEST(GeoConvHelper, test_method_x2cartesian) {
    GeoConvHelper gch(
            "+proj=utm +zone=33 +ellps=WGS84 +datum=WGS84 +units=m +no_defs",
            Position(), Boundary(), Boundary(), 0, false);

    Position pos(13.5326994, 52.428098100000007);
    gch.x2cartesian(pos);
    
	EXPECT_FLOAT_EQ(400235.50494557252, pos.x());
	EXPECT_FLOAT_EQ(5809666.826070101, pos.y());
}
