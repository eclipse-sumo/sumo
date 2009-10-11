#include <gtest/gtest.h>
#include <utils/common/RGBColor.h>
#include <utils/common/UtilExceptions.h>
#include <iostream>
#include <fstream>

using namespace std;

/*
Tests the class RGBColor
*/

/* Test the method 'parseColor' with a wrong String. An exception must occur.*/
//TODO must throw an invalid argumgent exception
/*
TEST(RGBColor, test_parseColor_with_wrong_string) {
	ASSERT_THROW(RGBColor::parseColor("a,b,y"),InvalidArgument) << "Expect an InvalidArgument exception.";
}
*/

/* Test the method 'parseColor'*/
TEST(RGBColor, test_parseColor) {
	RGBColor color = RGBColor::parseColor("1,2,3");
	EXPECT_EQ(SUMOReal(1), color.red());
	EXPECT_EQ(SUMOReal(2), color.green());
	EXPECT_EQ(SUMOReal(3), color.blue());
}

/* Test the method 'parseColor' with a longer String*/
TEST(RGBColor, test_parseColor_with_a_long_string) {
	RGBColor color = RGBColor::parseColor("1,2,3,5,432test");
	EXPECT_EQ(SUMOReal(1), color.red());
	EXPECT_EQ(SUMOReal(2), color.green());
	EXPECT_EQ(SUMOReal(3), color.blue());
}

/* Test the method 'parseColor' with an empty String*/
TEST(RGBColor, test_parseColor_with_empty_string) {
	std::string s;	
	ASSERT_THROW(RGBColor::parseColor(s),EmptyData) << "Expect an EmptyData exception.";	
}

/* Test the method 'parseColor' with thrown EmptyData Exception*/
TEST(RGBColor, test_parseColor_with_a_short_string) {	
	ASSERT_THROW(RGBColor::parseColor("1,2"),EmptyData) << "Expect an EmptyData exception.";
	ASSERT_THROW(RGBColor::parseColor("test"),EmptyData) << "Expect an EmptyData exception.";
}

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
TEST(RGBColor, test_interpolate_weight_1) {	
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
TEST(RGBColor, test_interpolate_weight_0) {	
	RGBColor color1 = RGBColor(1,2,3);
	RGBColor color2 = RGBColor(2,4,2);
	RGBColor colorResult = RGBColor::interpolate(color1, color2, 0);
	RGBColor colorResult2 = RGBColor::interpolate(color1, color2, -1000);
	EXPECT_TRUE(colorResult==colorResult2);
	EXPECT_EQ(SUMOReal(1), colorResult.red());
	EXPECT_EQ(SUMOReal(2), colorResult.green());
	EXPECT_EQ(SUMOReal(3), colorResult.blue());
}

/* Test the operator '=='. It has an tolerance of 0.1*/
TEST(RGBColor, test_operator_equal) {	
	RGBColor color1 = RGBColor(1,2,3);
	RGBColor color2 = RGBColor(1.09,1.901,3);	
	EXPECT_TRUE(color1==color2 );	
	color2 = RGBColor(1.19,2,3);	
	EXPECT_FALSE(color1==color2 );
}
