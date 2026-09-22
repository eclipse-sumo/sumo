#ifndef ROUTING_KIT_INVERSE_VECTOR_H
#define ROUTING_KIT_INVERSE_VECTOR_H

#include <routingkit/sort.h>
#include <routingkit/min_max.h>

#include <assert.h>
#include <algorithm>
#include <vector>

namespace RoutingKit{

//
// The inverse vector p of a vector v is a vector such that the elements
// v[p[i]], v[p[i]+1], v[p[i]+2], ..., v[p[i+1]-1] are exactly the elements
// with value i in v. If i does not occur in v, then p[i] == p[i+1]. v must be
// a sorted vector of unsigned integers.
//

inline
std::vector<unsigned>invert_vector(const std::vector<unsigned>&v, unsigned element_count){
	std::vector<unsigned>index(element_count+1);
	if(v.empty()){
		std::fill(index.begin(), index.end(), 0);
	}else{
		assert(is_sorted_using_less(v));
		assert(max_element_of(v) < element_count);

		index[0] = 0;

		unsigned pos = 0;
		for(unsigned i=0; i<element_count; ++i){
			while(pos < v.size() && v[pos] < i)
				++pos;
			index[i] = pos;
		}
		index[element_count] = v.size();
	}
	return index;
}

inline
std::vector<unsigned>invert_inverse_vector(const std::vector<unsigned>&sorted_index){
	assert(!sorted_index.empty());

	std::vector<unsigned>v(sorted_index.back());

	for(unsigned i=0; i<sorted_index.size()-1; ++i)
		for(unsigned j=sorted_index[i]; j<sorted_index[i+1]; ++j)
			v[j] = i;

	return v;
}

} // RoutingKit

#endif

