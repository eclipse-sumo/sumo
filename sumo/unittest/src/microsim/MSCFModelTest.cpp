#include <gtest/gtest.h>
#include <utils/common/SUMOVTypeParameter.h>
#include <microsim/MSVehicleType.h>
#include <microsim/cfmodels/MSCFModel.h>
#include <microsim/cfmodels/MSCFModel_Krauss.h>


/*
Tests the cfmodel functions
*/

class MSCFModelTest : public testing::Test {
	protected :
		MSVehicleType* type;
		MSCFModel* m;
        SUMOReal accel;
        SUMOReal decel;
        SUMOReal dawdle;
        SUMOReal tau; // headway time

		virtual void SetUp(){
            accel = 2;
            decel = 4;
            dawdle = 0;
            tau = 1;
            type = new MSVehicleType(SUMOVTypeParameter());
            m = new MSCFModel_Krauss(type,
                    accel, decel, dawdle, tau);
		}

		virtual void TearDown(){
			delete m;
			delete type;
		}
};

/* Test the method 'brakeGap'.*/

TEST_F(MSCFModelTest, test_method_brakeGap) {	
    // discrete braking model. keep driving for 1 s
    const SUMOReal v = 3;
	EXPECT_DOUBLE_EQ(tau * v, m->brakeGap(v)); 
}



