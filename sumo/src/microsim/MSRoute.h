#ifndef MSRoute_h
#define MSRoute_h

#include <string>
#include <map>
#include <vector>
#include <utils/common/Named.h>

class MSEdge;

typedef std::vector<MSEdge*> MSEdgeVector;
typedef MSEdgeVector::const_iterator MSRouteIterator;

class MSRoute : public Named {
private:
	typedef std::map<std::string, MSRoute*> RouteDict;
	static RouteDict myDict;
	MSEdgeVector _edges;
    bool _multipleReferenced;
public:
	MSRoute(const std::string &id,
		const MSEdgeVector &edges,
        bool multipleReferenced);
	~MSRoute();
	MSRouteIterator begin() const;
	MSRouteIterator end() const;
	size_t size() const;
	MSEdge *getLastEdge() const;
	static bool dictionary(const std::string &id, MSRoute *route);
	static MSRoute *dictionary(const std::string &id);
	static void clear();
    static void remove(const std::string &id);
    bool inFurtherUse() const;

};

#endif
