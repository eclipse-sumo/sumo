#include <gtest/gtest.h>
#include <utils/geom/Position2DVector.h>

using namespace std;

/*
Tests the class Position2DVector
*/
class Position2DVectorTest : public testing::Test {
	protected :
		Position2DVector *vectorPolygon;
		Position2DVector *vectorLine;

		virtual void SetUp(){
			vectorPolygon = new Position2DVector();
			vectorPolygon->push_back(Position2D(0,0));
			vectorPolygon->push_back(Position2D(0,2));
			vectorPolygon->push_back(Position2D(2,4));
			vectorPolygon->push_back(Position2D(4,2));
			vectorPolygon->push_back(Position2D(4,0));	

			vectorLine = new Position2DVector();
			vectorLine->push_back(Position2D(0,0));
			vectorLine->push_back(Position2D(2,2));
		}

		virtual void TearDown(){
			delete vectorPolygon;
			delete vectorLine;
		}
};

/* Test the method 'around'*/
TEST_F(Position2DVectorTest, test_method_around) {

	EXPECT_TRUE(vectorPolygon->around(Position2D(1,1)));
	EXPECT_TRUE(vectorPolygon->around(Position2D(1,2)));
	EXPECT_FALSE(vectorPolygon->around(Position2D(4,4)));
	EXPECT_FALSE(vectorPolygon->around(Position2D(0,0)));

	EXPECT_FALSE(vectorLine->around(Position2D(1,1)));
	EXPECT_FALSE(vectorLine->around(Position2D(0,2)));
}

/* Test the method 'getPolygonCenter'.*/
TEST_F(Position2DVectorTest, test_method_getPolygonCenter) {
	Position2D pos = vectorPolygon->getPolygonCenter();
	EXPECT_FLOAT_EQ(2, pos.x());
	EXPECT_FLOAT_EQ(1.6, pos.y());
	Position2D pos2 = vectorLine->getPolygonCenter();
	EXPECT_FLOAT_EQ(1, pos2.x());
	EXPECT_FLOAT_EQ(1, pos2.y());
	
}

/* Test the method 'getBoxBoundary'*/
TEST_F(Position2DVectorTest, test_method_getBoxBoundary) {	
	Boundary bound = vectorPolygon->getBoxBoundary();
	EXPECT_FLOAT_EQ(bound.xmax(), 4);
	EXPECT_FLOAT_EQ(bound.xmin(), 0);
	EXPECT_FLOAT_EQ(bound.ymax(), 4);
	EXPECT_FLOAT_EQ(bound.ymin(), 0);
}

