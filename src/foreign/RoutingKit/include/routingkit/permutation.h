#ifndef ROUTING_KIT_PERMUTATION_H
#define ROUTING_KIT_PERMUTATION_H

#include <routingkit/constants.h>

#include <vector>
#include <assert.h>
#include <algorithm>

namespace RoutingKit{

//
//  Applying a permutation p to a vector v consists of computing the vector {v[p[0]], v[p[1]], v[p[2]], ... , v[p[n]]}
//
//  Applying a permutation p to the elements of a vector v consists of computing the vector {p[v[0]], p[v[1]], p[v[2]], ... , p[v[n]]}
//

inline
bool is_permutation(const std::vector<unsigned>&p){
	std::vector<bool>found(p.size(), false);
	for(unsigned x:p){
		if(x >= p.size())
			return false;
		if(found[x])
			return false;
		found[x] = true;
	}
	return true;
}

inline
std::vector<unsigned>chain_permutation_first_left_then_right(const std::vector<unsigned>&p, const std::vector<unsigned>&q){
	assert(is_permutation(p) && "p must be a permutation");
	assert(is_permutation(q) && "q must be a permutation");
	assert(p.size() == q.size() && "p and q must permute the same number of objects");
	std::vector<unsigned>r(p.size());
	for(unsigned i=0; i<r.size(); ++i)
		r[i] = p[q[i]];
	return r;
}

inline
std::vector<unsigned>chain_permutation_first_right_then_left(const std::vector<unsigned>&p, const std::vector<unsigned>&q){
	return chain_permutation_first_left_then_right(q, p);
}

template<class T>
std::vector<T> apply_permutation(const std::vector<unsigned>&p, const std::vector<T>&v){
	assert(is_permutation(p) && "p must be a permutation");
	assert(p.size() == v.size() && "permutation and vector must have the same size");

	std::vector<T>r(v.size());
	for(unsigned i = 0; i<v.size(); ++i)
		r[i] = v[p[i]];

	return r; // NVRO
}

template<class T>
std::vector<T> apply_permutation(const std::vector<unsigned>&p, std::vector<T>&&v){
	assert(is_permutation(p) && "p must be a permutation");
	assert(p.size() == v.size() && "permutation and vector must have the same size");

	std::vector<T>r(v.size());
	for(unsigned i = 0; i<v.size(); ++i)
		r[i] = std::move(v[p[i]]);

	return r; // NVRO
}

template<class T>
std::vector<T> apply_inverse_permutation(const std::vector<unsigned>&p, const std::vector<T>&v){
	assert(is_permutation(p) && "p must be a permutation");
	assert(p.size() == v.size() && "permutation and vector must have the same size");

	std::vector<T>r(v.size());
	for(unsigned i = 0; i<v.size(); ++i)
		r[p[i]] = v[i];

	return r; // NVRO
}

template<class T>
std::vector<T> apply_inverse_permutation(const std::vector<unsigned>&p, std::vector<T>&&v){
	assert(is_permutation(p) && "p must be a permutation");
	assert(p.size() == v.size() && "permutation and vector must have the same size");

	std::vector<T>r(v.size());
	for(unsigned i = 0; i<v.size(); ++i)
		r[p[i]] = std::move(v[i]);

	return r; // NVRO
}

inline
void inplace_apply_permutation_to_elements_of(const std::vector<unsigned>&p, std::vector<unsigned>&v){
	assert(is_permutation(p) && "p must be a permutation");
	assert(std::all_of(v.begin(), v.end(), [&](unsigned x){return x < p.size();}) && "v has an out of bounds element");
	
	for(unsigned i=0; i<v.size(); ++i)
		v[i] = p[v[i]];
}

inline
std::vector<unsigned> apply_permutation_to_elements_of(const std::vector<unsigned>&p, const std::vector<unsigned>&v){
	assert(is_permutation(p) && "p must be a permutation");
	assert(std::all_of(v.begin(), v.end(), [&](unsigned x){return x < p.size();}) && "v has an out of bounds element");
	
	std::vector<unsigned> r = v;
	inplace_apply_permutation_to_elements_of(p, r);
	return r; // NVRO
}


inline
void inplace_apply_permutation_to_possibly_invalid_elements_of(const std::vector<unsigned>&p, std::vector<unsigned>&v){
	assert(is_permutation(p) && "p must be a permutation");
	assert(std::all_of(v.begin(), v.end(), [&](unsigned x){return x < p.size() || x == invalid_id;}) && "v has an out of bounds element");
	
	for(unsigned i=0; i<v.size(); ++i)
		if(v[i] != invalid_id)
			v[i] = p[v[i]];
}

inline
std::vector<unsigned> apply_permutation_to_possibly_invalid_elements_of(const std::vector<unsigned>&p, const std::vector<unsigned>&v){
	assert(is_permutation(p) && "p must be a permutation");
	assert(std::all_of(v.begin(), v.end(), [&](unsigned x){return x < p.size() || x == invalid_id;}) && "v has an out of bounds element");
	
	std::vector<unsigned> r = v;
	inplace_apply_permutation_to_elements_of(p, r);
	return r; // NVRO
}


inline
std::vector<unsigned> invert_permutation(const std::vector<unsigned>&p){
	assert(is_permutation(p) && "p must be a permutation");
	
	std::vector<unsigned> inv_p(p.size());
	for(unsigned i=0; i<p.size(); ++i)
		inv_p[p[i]] = i;
	
	return inv_p; // NVRO
}

inline
std::vector<unsigned> identity_permutation(unsigned n){
	std::vector<unsigned> p(n);
	for(unsigned i=0; i<n; ++i)
		p[i] = i;
	return p; // NVRO
}

template<class RandomGenerator>
std::vector<unsigned> random_permutation(unsigned n, RandomGenerator&&gen){
	auto r = identity_permutation(n);
	std::shuffle(r.begin(), r.end(), std::forward<RandomGenerator>(gen));
	return r;
}

} // RoutingKit

#endif

