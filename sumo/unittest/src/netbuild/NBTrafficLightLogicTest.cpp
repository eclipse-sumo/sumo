#include <gtest/gtest.h>
#include <netbuild/NBTrafficLightLogic.h>
#include <utils/iodevices/OutputDevice_COUT.h>

using namespace std;

/*
Tests the class NBTrafficLightLogic 
*/

/* Test the method 'writeXML'*/
TEST(NBTrafficLightLogic, test_method_writeXML) {	
	
	NBTrafficLightLogic *edge = new NBTrafficLightLogic("1","11",2);
	//cout << edge->getDuration()<<endl;
	OutputDevice_COUT *d = new OutputDevice_COUT();
	//edge->writeXML(d);

	//EXPECT_EQ(3, 2);
}
