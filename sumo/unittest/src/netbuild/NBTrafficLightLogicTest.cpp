#include <gtest/gtest.h>
#include <netbuild/NBTrafficLightLogic.h>
#include "../utils/iodevices/OutputDeviceMock.h"

using namespace std;

/*
Tests the class NBTrafficLightLogic 
*/

/* Test the method 'writeXML'*/
TEST(NBTrafficLightLogic, test_method_writeXML) {	
	
	NBTrafficLightLogic *edge = new NBTrafficLightLogic("1","11",2);
	edge->addStep(3,"a");
	OutputDeviceMock *mock = new OutputDeviceMock();
	edge->writeXML(*mock);
	string expect = "   <tl-logic id=\"1\" type=\"static\" programID=\"11\" offset=\"0\">\n      <phase duration=\"3\" state=\"a\"/>\n   </tl-logic>\n\n";
	EXPECT_EQ(expect, mock->getString());
}

/* Test the method 'getDuration'*/
TEST(NBTrafficLightLogic, test_method_getDuration) {	
	NBTrafficLightLogic *edge = new NBTrafficLightLogic("1","11",2);
	EXPECT_EQ(0, edge->getDuration());

	edge->addStep(3,"a");
	edge->addStep(5,"b");
	EXPECT_EQ(8, edge->getDuration());
	
}
