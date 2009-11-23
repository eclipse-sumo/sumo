#include <gtest/gtest.h>
#include <microsim/MSEventControl.h>
#include "../utils/common/CommandMock.h"

using namespace std;

/*
Tests the class MSEventControl 
*/

/* Test the method 'execute'. Tests if the execute method from the Command Class is called.*/
//Todo Create Instance of MSNet, because it is needed in addEvent

TEST(MSEventControl, test_method_execute) {	
	
	MSEventControl *edge = new MSEventControl();
	CommandMock *mock = new CommandMock(); 

	//edge->addEvent(mock,1,MSEventControl::ADAPT_AFTER_EXECUTION);
	
	EXPECT_FALSE(mock->isExecuteCalled());
	edge->execute(2);
	EXPECT_TRUE(mock->isExecuteCalled());
}



