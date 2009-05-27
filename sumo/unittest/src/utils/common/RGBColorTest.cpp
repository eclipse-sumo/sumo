#include <gtest/gtest.h>
#include <utils/common/RGBColor.h>

using namespace std;

/*
Tests the class RGBColor
*/

/* Test the method 'parseColor'*/
TEST(RGBColor, test_parseColor) {
	RGBColor color = RGBColor::parseColor("1,2,3");
	EXPECT_EQ(SUMOReal(1), color.red());
	EXPECT_EQ(SUMOReal(2), color.green());
	EXPECT_EQ(SUMOReal(3), color.blue());
}

/* Test the method 'parseColor' with longer String*/
TEST(RGBColor, test_parseColorLongString) {
	RGBColor color = RGBColor::parseColor("1,2,3,5,432test");
	EXPECT_EQ(SUMOReal(1), color.red());
	EXPECT_EQ(SUMOReal(2), color.green());
	EXPECT_EQ(SUMOReal(3), color.blue());
}

/* Test the method 'parseColor' with wrong String*/
//TODO 
/*
TEST(RGBColor, test_parseColor) {
	RGBColor color = RGBColor::parseColor("a,b,y");		
}
*/

/* Test the method 'parseColor' with NULL String*/
//TODO 
/*
TEST(RGBColor, test_parseColor) {
	EXPECT_THROW(RGBColor::parseColor(NULL),runtime_error);	
}
*/

/* Test the method 'parseColor' with thrown Exception*/
/*
TEST(RGBColor, test_parseColor_Exception) {	
	EXPECT_THROW(RGBColor::parseColor("1,2"),runtime_error);
	EXPECT_THROW(RGBColor::parseColor("test"),runtime_error);
}
*/

/* Test the method 'interpolate'*/
TEST(RGBColor, test_interpolate) {	
	RGBColor color1 = RGBColor(1,2,3);
	RGBColor color2 = RGBColor(2,4,2);
	RGBColor colorResult = RGBColor::interpolate(color1, color2, 0.5);
	EXPECT_EQ(SUMOReal(1.5), colorResult.red());
	EXPECT_EQ(SUMOReal(3), colorResult.green());
	EXPECT_EQ(SUMOReal(2.5), colorResult.blue());
}

/* Test the method 'interpolate' with a weight of 1 and higher*/
TEST(RGBColor, test_interpolate_weight1) {	
	RGBColor color1 = RGBColor(1,2,3);
	RGBColor color2 = RGBColor(2,4,2);
	RGBColor colorResult = RGBColor::interpolate(color1, color2, 1);
	RGBColor colorResult2 = RGBColor::interpolate(color1, color2, 1000);
	EXPECT_TRUE(colorResult==colorResult2);
	EXPECT_EQ(SUMOReal(2), colorResult.red());
	EXPECT_EQ(SUMOReal(4), colorResult.green());
	EXPECT_EQ(SUMOReal(2), colorResult.blue());
}

/* Test the method 'interpolate' with a weight of 0 and lower*/
TEST(RGBColor, test_interpolate_weight0) {	
	RGBColor color1 = RGBColor(1,2,3);
	RGBColor color2 = RGBColor(2,4,2);
	RGBColor colorResult = RGBColor::interpolate(color1, color2, 0);
	RGBColor colorResult2 = RGBColor::interpolate(color1, color2, -1000);
	EXPECT_TRUE(colorResult==colorResult2);
	EXPECT_EQ(SUMOReal(1), colorResult.red());
	EXPECT_EQ(SUMOReal(2), colorResult.green());
	EXPECT_EQ(SUMOReal(3), colorResult.blue());
}






