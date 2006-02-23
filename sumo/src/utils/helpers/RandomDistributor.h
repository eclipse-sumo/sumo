#ifndef RandomDistributor_h
#define RandomDistributor_h

#include <cassert>

template<class T>
class RandomDistributor
{
public:
    RandomDistributor() : myProb(0) { }
    ~RandomDistributor() { }

    void add(SUMOReal prob, T val) {
        assert(prob>=0);
        myVals.push_back(val);
        myProbs.push_back(prob);
        myProb += prob;
    }

    T get() const {
        SUMOReal prob = (SUMOReal) (((SUMOReal) rand()/(SUMOReal) RAND_MAX) * myProb);
        for(int i=0; i<(int) myVals.size(); i++) {
            if(prob<myProbs[i]) {
                return myVals[i];
            }
            prob -= myProbs[i];
        }
        return myVals[myVals.size()-1];
    }

    SUMOReal getOverallProb() const {
        return myProb;
    }

    void clear() {
        myProb = 0;
        myVals.clear();
        myProbs.clear();
    }

	const std::vector<T> &getVals() const {
		return myVals;
	}

	const std::vector<SUMOReal> &getProbs() const {
		return myProbs;
	}

private:
    SUMOReal myProb;
    std::vector<T> myVals;
    std::vector<SUMOReal> myProbs;

};



#endif
