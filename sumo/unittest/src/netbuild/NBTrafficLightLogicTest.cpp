#include <gtest/gtest.h>
#include <netbuild/NBTrafficLightLogic.h>
#include "../utils/iodevices/OutputDeviceMock.h"

using namespace std;

/*
Tests the class NBTrafficLightLogic 
*/

/* Test the method 'getDuration'*/
TEST(NBTrafficLightLogic, test_method_getDuration) {	
	NBTrafficLightLogic *edge = new NBTrafficLightLogic("1","11",2);
	EXPECT_EQ(0, edge->getDuration());

	edge->addStep(3,"gr");
	edge->addStep(5,"rG");
	EXPECT_EQ(8, edge->getDuration());
	
}
