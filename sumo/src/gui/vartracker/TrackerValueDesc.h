#ifndef TrackerValueDesc_h
#define TrackerValueDesc_h

#include <string>
#include <vector>
#include <utils/gfx/RGBColor.h>

class GUIGlObject;

class TrackerValueDesc {
public:
    TrackerValueDesc(const std::string &name, const RGBColor &col,
        GUIGlObject *o, size_t itemPos);
    ~TrackerValueDesc();
    void simStep();
    float getRange() const;
    float getMin() const;
    float getMax() const;
    float getYCenter() const;
    const RGBColor &getColor() const;
    const std::vector<float> &getValues() const;
    const std::string &getName() const;
private:
    std::string myName;
    GUIGlObject *myObject;
    size_t myItemPos;
    std::vector<float> myValues;
	RGBColor myActiveCol;
	RGBColor myInactiveCol;
	bool myAmActive;
	double myMin, myMax;
};

#endif
