#ifndef ROUTING_KIT_FILTER_H
#define ROUTING_KIT_FILTER_H

#include <routingkit/constants.h>
#include <routingkit/bit_vector.h>

#include <vector>
#include <assert.h>

namespace RoutingKit{

template<class T>
void inplace_keep_element_of_vector_if(const BitVector&keep_filter, std::vector<T>&vec){
	assert(keep_filter.size() == vec.size());

	uint64_t in = 0, out = 0, s = vec.size();
	while(in != s){
		if(keep_filter.is_set(in)){
			vec[out] = std::move(vec[in]);
			++out;
		}
		++in;
	}
	vec.erase(vec.begin() + out, vec.end());
}

template<class T>
void inplace_remove_element_from_vector_if(const BitVector&remove_filter, std::vector<T>&vec){
	assert(remove_filter.size() == vec.size());

	uint64_t in = 0, out = 0, s = vec.size();
	while(in != s){
		if(!remove_filter.is_set(in)){
			vec[out] = std::move(vec[in]);
			++out;
		}
		++in;
	}
	vec.erase(vec.begin() + out, vec.end());
}

template<class T>
std::vector<T> keep_element_of_vector_if(const BitVector&keep_filter, std::vector<T>vec){
	inplace_keep_element_of_vector_if(keep_filter, vec);
	return vec; // NVRO
}

template<class T>
std::vector<T> remove_element_from_vector_if(const BitVector&keep_filter, std::vector<T>vec){
	inplace_remove_element_from_vector_if(keep_filter, vec);
	return vec; // NVRO
}

} // RoutingKit

#endif
