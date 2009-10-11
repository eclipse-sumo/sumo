#include <gtest/gtest.h>
#include <utils/geom/Boundary.h>

using namespace std;

/*
Tests the class Boundary
*/

/* Test the method 'add'*/
TEST(Boundary, test_method_add) {
	Boundary *bound = new Boundary();
    bound->add(1,2);
	EXPECT_EQ(bound->xmax(), 1);
	EXPECT_EQ(bound->xmin(), 1);
	EXPECT_EQ(bound->ymax(), 2);
	EXPECT_EQ(bound->ymin(), 2);
}

/* Test the method 'add' with multiple calls*/
TEST(Boundary, test_method_add_multiple) {
	Boundary *bound = new Boundary();
    bound->add(-1,-2);
	bound->add(3,5);
	bound->add(5,8);
	EXPECT_EQ(bound->xmax(), 5);
	EXPECT_EQ(bound->xmin(), -1);
	EXPECT_EQ(bound->ymax(), 8);
	EXPECT_EQ(bound->ymin(), -2);
}

/* Test the method 'getCenter'*/
TEST(Boundary, test_method_getCenter) {
	Boundary *bound = new Boundary(-2,-4,4,8);
	Position2D pos = bound->getCenter();
	EXPECT_EQ(pos.x(), 1);
	EXPECT_EQ(pos.y(), 2);
}

/* Test the method 'getWidth' and getHeight*/
TEST(Boundary, test_method_getWidthHeight) {
	Boundary *bound = new Boundary(-2,-4,4,8);
	EXPECT_EQ(bound->getHeight(), 12);
	EXPECT_EQ(bound->getWidth(), 6);
}

/* Test the method 'around'*/
TEST(Boundary, test_method_around) {
	Boundary *bound = new Boundary(1,2,3,6);
	EXPECT_TRUE(bound->around(Position2D(2,4)));
	EXPECT_FALSE(bound->around(Position2D(0,4)));
	EXPECT_FALSE(bound->around(Position2D(2,7)));
	EXPECT_TRUE(bound->around(Position2D(0,7),2));
}

/* Test the method 'overlapsWith'*/
TEST(Boundary, test_method_overlapsWith) {
	Boundary *bound = new Boundary(1,2,3,6);
	EXPECT_FALSE(bound->overlapsWith(Boundary(10,17,13,16)));
	EXPECT_TRUE(bound->overlapsWith(Boundary(-1,-7,2,4)));
	EXPECT_TRUE(bound->overlapsWith(Boundary(1,2,3,6)));	
	EXPECT_TRUE(bound->overlapsWith(Boundary(4,2,5,7),1));
}

/* Test the method 'crosses'*/
TEST(Boundary, test_method_crosses) {
	Boundary *bound = new Boundary(1,2,3,6);
	EXPECT_TRUE(bound->crosses(Position2D(3,2),Position2D(4,2)));
	EXPECT_TRUE(bound->crosses(Position2D(2,1),Position2D(0,3)));
	EXPECT_TRUE(bound->crosses(Position2D(1,2),Position2D(3,6)));
	EXPECT_FALSE(bound->crosses(Position2D(0,0),Position2D(0,8)));
}

/* Test the method 'partialWithin'*/
TEST(Boundary, test_method_partialWithin) {
	Boundary *bound = new Boundary(1,2,3,6);
	EXPECT_TRUE(bound->partialWithin(Boundary(1,2,1,2)));
	EXPECT_FALSE(bound->partialWithin(Boundary(10,17,13,16)));
	EXPECT_TRUE(bound->partialWithin(Boundary(1,2,3,6)));	
	EXPECT_TRUE(bound->partialWithin(Boundary(4,2,5,7),1));
}

/* Test the method 'flipY'*/
TEST(Boundary, test_method_flipY) {
	Boundary *bound = new Boundary(1,2,3,6);
	bound->flipY();
	EXPECT_EQ(bound->xmax(), 3);
	EXPECT_EQ(bound->xmin(), 1);
	EXPECT_EQ(bound->ymax(), -2);
	EXPECT_EQ(bound->ymin(), -6);
}

/* Test the method 'moveby'*/
TEST(Boundary, test_method_moveby) {
	Boundary *bound = new Boundary(1,2,3,6);
	bound->moveby(2.5,-3.5);
	EXPECT_EQ(bound->xmax(), 5.5);
	EXPECT_EQ(bound->xmin(), 3.5);
	EXPECT_EQ(bound->ymax(), 2.5);
	EXPECT_EQ(bound->ymin(), -1.5);
}