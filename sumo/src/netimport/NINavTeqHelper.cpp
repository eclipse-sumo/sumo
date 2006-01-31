#include "NINavTeqHelper.h"
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>

SUMOReal
NINavTeqHelper::getSpeed(const std::string &id, const std::string &speedClassS)
{
	try {
		int speedClass = TplConvert<char>::_2int(speedClassS.c_str());
		switch(speedClass) {
		case -1:
			return (SUMOReal) 1.0 / (SUMOReal) 3.6;
		case 1:
			return (SUMOReal) 200 / (SUMOReal) 3.6; //> 130 KPH / > 80 MPH
		case 2:
			return (SUMOReal) 115 / (SUMOReal) 3.6; //101-130 KPH / 65-80 MPH
		case 3:
			return (SUMOReal) 95 / (SUMOReal) 3.6; // 91-100 KPH / 55-64 MPH
		case 4:
			return (SUMOReal) 80 / (SUMOReal) 3.6; // 71-90 KPH / 41-54 MPH
		case 5:
			return (SUMOReal) 60 / (SUMOReal) 3.6; // 51-70 KPH / 31-40 MPH
		case 6:
			return (SUMOReal) 40 / (SUMOReal) 3.6; // 31-50 KPH / 21-30 MPH
		case 7:
			return (SUMOReal) 20 / (SUMOReal) 3.6; // 11-30 KPH / 6-20 MPH
		case 8:
			return (SUMOReal) 5 / (SUMOReal) 3.6; //< 11 KPH / < 6 MPH
		default:
			MsgHandler::getErrorInstance()->inform("Invalid speed code (edge '" + id + "'.");
			throw ProcessError();
		}
	} catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform(
            "Non-numerical value for an edge's speed type occured (edge '" + id + "').");
        throw ProcessError();
    }
	throw ProcessError();
}


size_t
NINavTeqHelper::getLaneNumber(const std::string &id, const std::string &laneNoS,
							  SUMOReal speed, bool useNewLaneNumberInfoPlain)
{
    try {
        int nolanes = TplConvert<char>::_2int(laneNoS.c_str());
        if(nolanes<0) {
            return 1;
        } else if(nolanes/10>0&&(useNewLaneNumberInfoPlain||abs((nolanes%10)-(nolanes/10))<2)) {
			return nolanes / 10;
        } else {
            switch(nolanes%10) {
            case 1:
                return 1;
            case 2:
                nolanes = 2;
                if(speed>78.0/3.6) {
                    nolanes = 3;
                }
				return nolanes;
            case 3:
                return 4;
            default:
                MsgHandler::getErrorInstance()->inform("Invalid lane number (edge '" + id + "').");
                throw ProcessError();
            }
        }
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform(
            "Non-numerical value for an edge's lane number occured (edge '" + id + "'.");
        throw ProcessError();
    }
	throw ProcessError();
}


