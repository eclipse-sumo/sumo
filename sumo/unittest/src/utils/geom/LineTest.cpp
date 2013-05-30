#include <gtest/gtest.h>
#include <utils/geom/Line.h>
#include <utils/geom/GeomHelper.h>

/*
Tests the class Line
*/

/* Test the method 'intersectsAt' with no intersection, expected thrown exception*/
//TODO must return NULL
/*
TEST(Line, test_method_intersectsAt_no_intersection) {
	Line line(Position(0,0),Position(2,2));
	Position pos = line.intersectsAt(Line(Position(0,3),Position(0,6)));
	EXPECT_EQUALS(NULL,pos);
}
*/

/* Test the method 'intersectsAt' with intersection on an end position, expected thrown exception*/
//TODO must return NULL
/*
TEST(Line, test_method_intersectsAt_at_end_position) {
	Line line(Position(0,0),Position(2,2));
	Position pos = line.intersectsAt(Line(Position(0,-1),Position(0,6)));
	EXPECT_EQUALS(NULL,pos);
}
*/

/* Test the method 'add' with positive numbers*/
TEST(Line, test_method_add_positive) {
	Line line(Position(0,0),Position(2,2));
	line.add(2,2);
	EXPECT_DOUBLE_EQ(2,(line.p1()).x());
	EXPECT_DOUBLE_EQ(2,(line.p1()).y());
	EXPECT_DOUBLE_EQ(4,(line.p2()).x());
	EXPECT_DOUBLE_EQ(4,(line.p2()).y());
}

/* Test the method 'add' with negative numbers*/
TEST(Line, test_method_add_negative) {
	Line line(Position(0,0),Position(2,2));
	line.add(Position(-2,-2));
	EXPECT_DOUBLE_EQ(-2,(line.p1()).x());
	EXPECT_DOUBLE_EQ(-2,(line.p1()).y());
	EXPECT_DOUBLE_EQ(0,(line.p2()).x());
	EXPECT_DOUBLE_EQ(0,(line.p2()).y());
}

/* Test the method 'sub' with negative numbers*/
TEST(Line, test_method_sub_negative) {
	Line line(Position(0,0),Position(2,2));
	line.sub(-2,-2);
	EXPECT_DOUBLE_EQ(2,(line.p1()).x());
	EXPECT_DOUBLE_EQ(2,(line.p1()).y());
	EXPECT_DOUBLE_EQ(4,(line.p2()).x());
	EXPECT_DOUBLE_EQ(4,(line.p2()).y());
}

/* Test the method 'sub' with positive numbers*/
TEST(Line, test_method_sub_positive) {
	Line line(Position(0,0),Position(2,2));
	line.sub(2,2);
	EXPECT_DOUBLE_EQ(-2,(line.p1()).x());
	EXPECT_DOUBLE_EQ(-2,(line.p1()).y());
	EXPECT_DOUBLE_EQ(0,(line.p2()).x());
	EXPECT_DOUBLE_EQ(0,(line.p2()).y());
}

/* Test the method 'reverse' */
TEST(Line, test_method_reverse) {
	Line line(Position(0,0),Position(2,2));
	Line lineReverse = line.reverse();
	EXPECT_DOUBLE_EQ(2,(lineReverse.p1()).x());
	EXPECT_DOUBLE_EQ(2,(lineReverse.p1()).y());
	EXPECT_DOUBLE_EQ(0,(lineReverse.p2()).x());
	EXPECT_DOUBLE_EQ(0,(lineReverse.p2()).y());
}

/* Test the method 'intersectsAtLength2D' */
TEST(Line, test_method_intersectsAtLength2D) {
	Line line(Position(0,0),Position(2,2));
	SUMOReal point = line.intersectsAtLength2D(Line(Position(0,2),Position(2,0)));
	EXPECT_DOUBLE_EQ(sqrt(SUMOReal(2)), point);	
}

/* Test the method 'intersectsAtLength' at the beginning */
TEST(Line, test_method_intersectsAtLength_zero) {
	Line line(Position(0,0),Position(2,2));
	SUMOReal point = line.intersectsAtLength2D(Line(Position(0,0),Position(2,0)));
	EXPECT_DOUBLE_EQ(0, point);	
}

/* Test the method 'intersectsAtLength' with no intersect. Must be -1*/
TEST(Line, test_method_intersectsAtLength_no_intersect) {
	Line line(Position(0,0),Position(2,2));
	SUMOReal point = line.intersectsAtLength2D(Line(Position(0,3),Position(0,6)));
	EXPECT_DOUBLE_EQ(-1, point);	
}

/* Test the method 'rotateAtP1' */
TEST(Line, test_method_rotateAtP1) {
	Line line(Position(0,0),Position(2,2));
	line.rotateAtP1(M_PI/2);
	EXPECT_DOUBLE_EQ(-2,(line.p2()).x());
	EXPECT_DOUBLE_EQ(2,(line.p2()).y());
}

/* Test the method 'rotateAtP1' with negative rotation */
TEST(Line, test_method_rotateAtP1_negative) {
	Line line(Position(0,0),Position(2,2));
	line.rotateAtP1(-3*M_PI/4);
	EXPECT_DOUBLE_EQ(1,SUMOReal((line.p2()).x())+1);
	EXPECT_DOUBLE_EQ(-2*sqrt(SUMOReal(2)),(line.p2()).y());
}

/* Test the method 'length' */
TEST(Line, test_method_length) {
	Line line(Position(0,0),Position(0,2));
	EXPECT_DOUBLE_EQ(2,line.length());
	Line line2(Position(0,0),Position(-1,0));
	EXPECT_DOUBLE_EQ(1,line2.length());
	Line line3(Position(2,2),Position(2,2));
	EXPECT_DOUBLE_EQ(0,line3.length());
}

/* Test the method 'intersects' */
TEST(Line, test_method_intersects) {
	Line line(Position(0,0),Position(2,2));
	EXPECT_TRUE(line.intersects(Line(Position(2,0),Position(0,2))));
	Line line2(Position(0,0),Position(2,2));
	EXPECT_FALSE(line.intersects(Line(Position(0,3),Position(0,6))));
	Line line3(Position(0,0),Position(2,2));
	EXPECT_TRUE(line.intersects(Line(Position(0,-1),Position(0,6))));
}

/* Test the method 'intersectsAt' */
TEST(Line, test_method_intersectsAt) {
	Line line(Position(0,0),Position(2,2));
	Position pos = line.intersectsAt(Line(Position(2,0),Position(0,2)));
	EXPECT_DOUBLE_EQ(1,pos.x());
	EXPECT_DOUBLE_EQ(1,pos.y());
}

/* Test the method 'move2side' */
TEST(Line, test_method_move2side) {
	Line line(Position(0,0),Position(2,2));
	line.move2side(SUMOReal(3));
	//TODO what do the method? correct the assertions.
	/*
	EXPECT_DOUBLE_EQ(0,(line.p1()).x());
	EXPECT_DOUBLE_EQ(3,(line.p1()).y());
	EXPECT_DOUBLE_EQ(2,(line.p2()).x());
	EXPECT_DOUBLE_EQ(5,(line.p2()).y());
	*/
}
