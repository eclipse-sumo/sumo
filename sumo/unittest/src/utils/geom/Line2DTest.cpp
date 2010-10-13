#include <gtest/gtest.h>
#include <utils/geom/Line2D.h>
#include <utils/geom/GeomHelper.h>

using namespace std;

/*
Tests the class Line2D
*/

/* Test the method 'intersectsAt' with no intersection, expected thrown exception*/
//TODO must return NULL
/*
TEST(Line2D, test_method_intersectsAt_no_intersection) {
	Line2D line(Position2D(0,0),Position2D(2,2));
	Position2D pos = line.intersectsAt(Line2D(Position2D(0,3),Position2D(0,6)));
	EXPECT_EQUALS(NULL,pos);
}
*/

/* Test the method 'intersectsAt' with intersection on an end position, expected thrown exception*/
//TODO must return NULL
/*
TEST(Line2D, test_method_intersectsAt_at_end_position) {
	Line2D line(Position2D(0,0),Position2D(2,2));
	Position2D pos = line.intersectsAt(Line2D(Position2D(0,-1),Position2D(0,6)));
	EXPECT_EQUALS(NULL,pos);
}
*/

/* Test the method 'add' with positive numbers*/
TEST(Line2D, test_method_add_positive) {
	Line2D line(Position2D(0,0),Position2D(2,2));
	line.add(2,2);
	EXPECT_FLOAT_EQ(2,(line.p1()).x());
	EXPECT_FLOAT_EQ(2,(line.p1()).y());
	EXPECT_FLOAT_EQ(4,(line.p2()).x());
	EXPECT_FLOAT_EQ(4,(line.p2()).y());
}

/* Test the method 'add' with negative numbers*/
TEST(Line2D, test_method_add_negative) {
	Line2D line(Position2D(0,0),Position2D(2,2));
	line.add(Position2D(-2,-2));
	EXPECT_FLOAT_EQ(-2,(line.p1()).x());
	EXPECT_FLOAT_EQ(-2,(line.p1()).y());
	EXPECT_FLOAT_EQ(0,(line.p2()).x());
	EXPECT_FLOAT_EQ(0,(line.p2()).y());
}

/* Test the method 'sub' with negative numbers*/
TEST(Line2D, test_method_sub_negative) {
	Line2D line(Position2D(0,0),Position2D(2,2));
	line.sub(-2,-2);
	EXPECT_FLOAT_EQ(2,(line.p1()).x());
	EXPECT_FLOAT_EQ(2,(line.p1()).y());
	EXPECT_FLOAT_EQ(4,(line.p2()).x());
	EXPECT_FLOAT_EQ(4,(line.p2()).y());
}

/* Test the method 'sub' with positive numbers*/
TEST(Line2D, test_method_sub_positive) {
	Line2D line(Position2D(0,0),Position2D(2,2));
	line.sub(2,2);
	EXPECT_FLOAT_EQ(-2,(line.p1()).x());
	EXPECT_FLOAT_EQ(-2,(line.p1()).y());
	EXPECT_FLOAT_EQ(0,(line.p2()).x());
	EXPECT_FLOAT_EQ(0,(line.p2()).y());
}

/* Test the method 'reverse' */
TEST(Line2D, test_method_reverse) {
	Line2D line(Position2D(0,0),Position2D(2,2));
	Line2D lineReverse = line.reverse();
	EXPECT_FLOAT_EQ(2,(lineReverse.p1()).x());
	EXPECT_FLOAT_EQ(2,(lineReverse.p1()).y());
	EXPECT_FLOAT_EQ(0,(lineReverse.p2()).x());
	EXPECT_FLOAT_EQ(0,(lineReverse.p2()).y());
}

/* Test the method 'intersectsAtLength' */
TEST(Line2D, test_method_intersectsAtLength) {
	Line2D line(Position2D(0,0),Position2D(2,2));
	SUMOReal point = line.intersectsAtLength(Line2D(Position2D(0,2),Position2D(2,0)));
	EXPECT_FLOAT_EQ(sqrt(SUMOReal(2)), point);	
}

/* Test the method 'intersectsAtLength' at the beginning */
TEST(Line2D, test_method_intersectsAtLength_zero) {
	Line2D line(Position2D(0,0),Position2D(2,2));
	SUMOReal point = line.intersectsAtLength(Line2D(Position2D(0,0),Position2D(2,0)));
	EXPECT_FLOAT_EQ(0, point);	
}

/* Test the method 'intersectsAtLength' with no intersect. Must be -1*/
TEST(Line2D, test_method_intersectsAtLength_no_intersect) {
	Line2D line(Position2D(0,0),Position2D(2,2));
	SUMOReal point = line.intersectsAtLength(Line2D(Position2D(0,3),Position2D(0,6)));
	EXPECT_FLOAT_EQ(-1, point);	
}

/* Test the method 'rotateAtP1' */
TEST(Line2D, test_method_rotateAtP1) {
	Line2D line(Position2D(0,0),Position2D(2,2));
	line.rotateAtP1(PI/2);
	EXPECT_FLOAT_EQ(-2,(line.p2()).x());
	EXPECT_FLOAT_EQ(2,(line.p2()).y());
}

/* Test the method 'rotateAtP1' with negative rotation */
TEST(Line2D, test_method_rotateAtP1_negative) {
	Line2D line(Position2D(0,0),Position2D(2,2));
	line.rotateAtP1(-3*PI/4);
	EXPECT_FLOAT_EQ(1,SUMOReal((line.p2()).x())+1);
	EXPECT_FLOAT_EQ(-2*sqrt(SUMOReal(2)),(line.p2()).y());
}

/* Test the method 'length' */
TEST(Line2D, test_method_length) {
	Line2D line(Position2D(0,0),Position2D(0,2));
	EXPECT_FLOAT_EQ(2,line.length());
	Line2D line2(Position2D(0,0),Position2D(-1,0));
	EXPECT_FLOAT_EQ(1,line2.length());
	Line2D line3(Position2D(2,2),Position2D(2,2));
	EXPECT_FLOAT_EQ(0,line3.length());
}

/* Test the method 'intersects' */
TEST(Line2D, test_method_intersects) {
	Line2D line(Position2D(0,0),Position2D(2,2));
	EXPECT_TRUE(line.intersects(Line2D(Position2D(2,0),Position2D(0,2))));
	Line2D line2(Position2D(0,0),Position2D(2,2));
	EXPECT_FALSE(line.intersects(Line2D(Position2D(0,3),Position2D(0,6))));
	Line2D line3(Position2D(0,0),Position2D(2,2));
	EXPECT_TRUE(line.intersects(Line2D(Position2D(0,-1),Position2D(0,6))));
}

/* Test the method 'intersectsAt' */
TEST(Line2D, test_method_intersectsAt) {
	Line2D line(Position2D(0,0),Position2D(2,2));
	Position2D pos = line.intersectsAt(Line2D(Position2D(2,0),Position2D(0,2)));
	EXPECT_FLOAT_EQ(1,pos.x());
	EXPECT_FLOAT_EQ(1,pos.y());
}

/* Test the method 'move2side' */
TEST(Line2D, test_method_move2side) {
	Line2D line(Position2D(0,0),Position2D(2,2));
	line.move2side(SUMOReal(3));
	//TODO what do the method? correct the assertions.
	/*
	EXPECT_FLOAT_EQ(0,(line.p1()).x());
	EXPECT_FLOAT_EQ(3,(line.p1()).y());
	EXPECT_FLOAT_EQ(2,(line.p2()).x());
	EXPECT_FLOAT_EQ(5,(line.p2()).y());
	*/
}
