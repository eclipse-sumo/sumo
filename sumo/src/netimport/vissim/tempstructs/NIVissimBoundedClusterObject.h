#ifndef NIVissimBoundedClusterObject_h
#define NIVissimBoundedClusterObject_h

#include <set>
#include <string>

class Boundery;

class NIVissimBoundedClusterObject {
public:
    NIVissimBoundedClusterObject();
    virtual ~NIVissimBoundedClusterObject();
    virtual void computeBounding() = 0;
    bool crosses(const AbstractPoly &poly, double offset=0) const;
    void inCluster(int id);
    bool clustered() const;
    const Boundery &getBoundery() const;
public:
    static void closeLoading();
protected:
    typedef std::set<NIVissimBoundedClusterObject*> ContType;
    static ContType myDict;
    Boundery *myBoundery;
    int myClusterID;
};


#endif

