#ifndef ROEdgeCont_h
#define ROEdgeCont_h

#include <map>
#include <string>
#include <utils/common/NamedObjectCont.h>

class ROEdge;

class ROEdgeCont : public NamedObjectCont<ROEdge*> {
public:
	ROEdgeCont();
	~ROEdgeCont();
    void postloadInit();
    void init();
private:
    /// we made the copy constructor invalid
    ROEdgeCont(const ROEdgeCont &src);
    /// we made the assignment operator invalid
    ROEdgeCont &operator=(const ROEdgeCont &src);
};

#endif
