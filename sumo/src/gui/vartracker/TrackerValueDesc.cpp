#include <string>
#include <vector>
#include <utils/gfx/RGBColor.h>
#include <gui/GUIGlObject.h>
#include <guisim/GUINet.h>
#include "TrackerValueDesc.h"


TrackerValueDesc::TrackerValueDesc(const std::string &name,
                                   const RGBColor &col,
                                   GUIGlObject *o, size_t itemPos)
    : myObject(o), myItemPos(itemPos),
    myActiveCol(col), myInactiveCol(col),
    myAmActive(true),
    myMin(0), myMax(0)
{
}


TrackerValueDesc::~TrackerValueDesc()
{
}


void
TrackerValueDesc::simStep()
{
	if(!myObject->active()) {
		return;
	}
    double val = myObject->getTableParameter(myItemPos);
    if(myValues.size()==0) {
        myMin = val;
        myMax = val;
    } else {
        myMin = val < myMin ? val : myMin;
        myMax = val > myMax ? val : myMax;
    }
    myValues.push_back(val);
}


float
TrackerValueDesc::getRange() const
{
    getMin();
    getMax();
    return myMax - myMin;
}


float
TrackerValueDesc::getMin() const
{
    return myMin;
}


float
TrackerValueDesc::getMax() const
{
    return myMax;
}


float
TrackerValueDesc::getYCenter() const
{
    getMin();
    getMax();
    return (myMin + myMax) / 2.0;
}


const RGBColor &
TrackerValueDesc::getColor() const
{
    return myActiveCol;
}


const std::vector<float> &
TrackerValueDesc::getValues() const
{
    return myValues;
}


const std::string &
TrackerValueDesc::getName() const
{
    return myName;
}


