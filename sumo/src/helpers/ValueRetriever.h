#ifndef ValueRetriever_h
#define ValueRetriever_h

template<typename _T>
class ValueRetriever {
public:
    ValueRetriever() { }
    virtual ~ValueRetriever() { }
    virtual void addValue(_T value) = 0;

};


#endif

