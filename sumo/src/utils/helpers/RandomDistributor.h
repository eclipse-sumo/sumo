#ifndef RandomDistributor_h
#define RandomDistributor_h

template<class T>
class RandomDistributor
{
public:
    RandomDistributor() : myProb(0) { }
    ~RandomDistributor() { }

    void add(float prob, T val) {
        assert(prob>=0);
        myVals.push_back(val);
        myProbs.push_back(prob);
        myProb += prob;
    }

    T get() const {
        float prob = (float) (((double) rand()/(double) RAND_MAX) * myProb);
        for(int i=0; i<myVals.size(); i++) {
            if(prob<myProbs[i]) {
                return myVals[i];
            }
            prob -= myProbs[i];
        }
        return myVals[myVals.size()-1];
    }

    float getOverallProb() const {
        return myProb;
    }

private:
    float myProb;
    std::vector<T> myVals;
    std::vector<float> myProbs;

};



#endif
