#ifndef InstancePool_h
#define InstancePool_h

#include <vector>
#include <algorithm>
#include <cassert>

template<class T>
class InstancePool {
public:
    InstancePool(bool deleteOnQuit) { }
    ~InstancePool() {
	if(myDeleteOnQuit) {
	    typedef std::vector<T*>::iterator It;
	    for(It i=myFreeInstances.begin(); i!=myFreeInstances.end(); i++) {
		delete *i;
	    }
	}
    }

    T* getFreeInstance() {
	if(myFreeInstances.size()==0) {
	    return 0;
	} else {
	    T *instance = myFreeInstances.back();
	    myFreeInstances.pop_back();
	    return instance;
	}
    }

    void addFreeInstance(T *instance) {
	myFreeInstances.push_back(instance);
    }

    void addFreeInstances(const std::vector<T*> instances) {
	std::copy(instances.begin(), instances.end(),
		  std::back_inserter(myFreeInstances));
    }


private:
    std::vector<T*> myFreeInstances;
    bool myDeleteOnQuit;

};


#endif
