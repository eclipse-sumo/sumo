#ifndef ROUTING_KIT_SORT_H
#define ROUTING_KIT_SORT_H

#include <routingkit/permutation.h>

#include <vector>
#include <assert.h>
#include <algorithm>
#include <functional>

namespace RoutingKit{

//
// A sort permutation p of a vector v is a permutation such that
// v[p[0]] <= v[p[1]] <= v[p[2]] <= ... . Applying a sort permutation p of v to
// v yields a sorted vector.
//
// The functions in this header follow the following naming sheme:
//
//   1) compute_[inverse_][stable_]sort_using_[(key|less|comparator](vector, ...order...)
//   2) [stable_]sort_using_[key|less|comparator](vector, ...order...)
//   3) is_sorted_using_[key|less|comparator](vector, ...order...)
//
//
// The functionality is explained as following:
//
//   * The functions of category 1 compute sort permutations but do not actually
//     sort any array. The sorting is done by applying the resulting permutation
//     to one or more vectors.
//   * The functions of category 2 copy their argument vector, sort the copy and
//     return it. If an rvalue vector is given as parameter then the copy is
//     omitted.
//   * The functions of category 3 sort their argument array inplace.
//   * The functions of category 4 do not sort anything but check whether their
//     argument array is sorted.
//   * "inverse" indicates that not a sort permutation but its inverse is
//     computed. In combination with the _using_key function it is slightly
//     faster to compute an inverse sort permutation and then to use
//     apply_inverse_permutation then to compute a sort permutation and to use
//     apply_permutation.
//   * "stable" indicates that the relative order of values that compare equally
//     is not changed.
//   * "key" indicates that elements are sorted using bucket sort. All functions
//     take a (key_count, get_key) pair of parameters to specify the order.
//     get_key is a function  object that must map every element onto a value in
//     [0,key_count).  get_key is assumed to have a quick execution and is
//     called several times per sort operation. However, get_key is never
//     copied.
//   * "comparator" indicates that an STL like comparator is used and needs to
//     be passed to every function.
//   * "less" indicates that < is used for comparision. No order paramters need
//     to be passed to the functions.
//

template<class T, class C>
std::vector<unsigned> compute_stable_sort_permutation_using_comparator(const std::vector<T>&v, const C&is_less){
	std::vector<unsigned>p = identity_permutation(v.size());
	std::stable_sort(
		p.begin(), p.end(),
		[&](unsigned l, unsigned r){
			return is_less(v[l], v[r]);
		}
	);
	return p; // NVRO
}

template<class T, class C>
std::vector<T>stable_sort_using_comparator(const std::vector<T>&v, const C&is_less){
	std::vector<T>r = v;
	std::stable_sort(r.begin(), r.end(), is_less);
	return r; // NVRO
}

template<class T, class C>
std::vector<T>stable_sort_using_comparator(std::vector<T>&&v, const C&is_less){
	std::stable_sort(v.begin(), v.end(), is_less);
	return std::move(v); // NVRO
}

template<class T, class C>
std::vector<unsigned> compute_sort_permutation_using_comparator(const std::vector<T>&v, const C&is_less){
	std::vector<unsigned>p = identity_permutation(v.size());
	std::sort(
		p.begin(), p.end(),
		[&](unsigned l, unsigned r){
			return is_less(v[l], v[r]);
		}
	);
	return p; // NVRO
}

template<class T, class C>
std::vector<T>sort_using_comparator(const std::vector<T>&v, const C&is_less){
	std::vector<T>r = v;
	std::sort(r.begin(), r.end(), is_less);
	return r; // NVRO
}

template<class T, class C>
std::vector<T>sort_using_comparator(std::vector<T>&&v, const C&is_less){
	std::sort(v.begin(), v.end(), is_less);
	return std::move(v); // NVRO
}


template<class T, class C>
bool is_sorted_using_comparator(const std::vector<T>&v, const C&is_less){
	for(unsigned i=1; i<v.size(); ++i)
		if(is_less(v[i], v[i-1]))
			return false;
	return true;
}


template<class T, class C>
std::vector<unsigned> compute_inverse_sort_permutation_using_comparator(const std::vector<T>&v, const C&is_less){
	return invert_permutation(compute_sort_permutation_using_comparator(v, is_less));
}

template<class T, class C>
std::vector<unsigned> compute_inverse_stable_sort_permutation_using_comparator(const std::vector<T>&v, const C&is_less){
	return invert_permutation(compute_stable_sort_permutation_using_comparator(v, is_less));
}



namespace detail{
	const unsigned bucket_sort_min_key_to_element_ratio = 16;

	template<class T, class K>
	std::vector<unsigned>compute_key_pos(const std::vector<T>&v, unsigned key_count, const K&get_key){
		std::vector<unsigned>key_pos(key_count, 0);
		for(unsigned i=0; i<v.size(); ++i){
			unsigned k = get_key(v[i]);
			assert(k <= key_count && "key is too large");
			++key_pos[k];
		}
		unsigned sum = 0;
		for(unsigned i=0; i<key_count; ++i){
			unsigned tmp = sum + key_pos[i];
			key_pos[i] = sum;
			sum = tmp;
		}
		return key_pos; // NVRO
	}

	template<class K>
	struct CompareByKey{
		explicit CompareByKey(unsigned n, const K&k):get_key(k){
			#ifndef NDEBUG
			key_count = n;
			#else
			(void)n;
			#endif
		}

		template<class T>
		bool operator()(const T&l, const T&r)const{
			unsigned l_key = get_key(l);
			assert(l_key < key_count);
			unsigned r_key = get_key(r);
			assert(r_key < key_count);
			return l_key < r_key;
		}

		#ifndef NDEBUG
		unsigned key_count;
		#endif
		const K&get_key;
	};

	template<class K>
	CompareByKey<K> make_compare_by_key(unsigned n, const K&k){
		return CompareByKey<K>(n, k);
	}
}

namespace detail{
	// Sorting by key uses bucket sort if the number of elements is
	// not significantly smaller than the number of keys. Otherwise
	// we use the sort by comparator function, which (at the time
	// of writing this comment) forward to std::sort and
	// std::stable_sort. Bucket sort is always stable whereas
	// comparator-based sort is not. To avoid code dublication, we
	// therefore have maybe_stable functions that take a compile time
	// boolean which inidcates whether the fallback function should
	// be stable.

	template<bool is_stable, class T, class K>
	std::vector<unsigned> compute_maybe_stable_sort_permutation_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
		std::vector<unsigned>p;

		if(v.size() >= key_count / bucket_sort_min_key_to_element_ratio){
			p.resize(v.size());
			std::vector<unsigned>key_pos = detail::compute_key_pos(v, key_count, get_key);
			for(unsigned i=0; i<v.size(); ++i){
				unsigned k = get_key(v[i]);
				assert(k <= key_count && "key is too large");
				p[key_pos[k]] = i;
				++key_pos[k];
			}
		}else if(is_stable){
			p = compute_stable_sort_permutation_using_comparator(v, detail::make_compare_by_key(key_count, get_key));
		}else{
			p = compute_sort_permutation_using_comparator(v, detail::make_compare_by_key(key_count, get_key));
		}
		return p; // NVRO
	}

}

template<class T, class K>
std::vector<unsigned> compute_sort_permutation_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	return detail::compute_maybe_stable_sort_permutation_using_key<false>(v, key_count, get_key);
}

template<class T, class K>
std::vector<unsigned> compute_stable_sort_permutation_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	return detail::compute_maybe_stable_sort_permutation_using_key<true>(v, key_count, get_key);
}

namespace detail{
	template<bool is_stable, class T, class K>
	std::vector<unsigned> compute_inverse_maybe_stable_sort_permutation_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
		std::vector<unsigned>p;
		if(v.size() >= key_count / bucket_sort_min_key_to_element_ratio){
			p.resize(v.size());
			std::vector<unsigned>key_pos = detail::compute_key_pos(v, key_count, get_key);
			for(unsigned i=0; i<v.size(); ++i){
				unsigned k = get_key(v[i]);
				assert(k <= key_count && "key is too large");
				p[i] = key_pos[k];
				++key_pos[k];
			}
		} else if(is_stable){
			p = compute_inverse_stable_sort_permutation_using_comparator(v, detail::make_compare_by_key(key_count, get_key));
		}else{
			p = compute_inverse_sort_permutation_using_comparator(v, detail::make_compare_by_key(key_count, get_key));
		}
		return p; // NVRO
	}
}

template<class T, class K>
std::vector<unsigned> compute_inverse_sort_permutation_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	return detail::compute_inverse_maybe_stable_sort_permutation_using_key<false>(v, key_count, get_key);
}

template<class T, class K>
std::vector<unsigned> compute_inverse_stable_sort_permutation_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	return detail::compute_inverse_maybe_stable_sort_permutation_using_key<true>(v, key_count, get_key);
}

namespace detail{
	template<bool is_stable, class T, class K>
	std::vector<T>maybe_stable_sort_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
		std::vector<T>r;

		if(v.size() >= key_count / bucket_sort_min_key_to_element_ratio){
			std::vector<unsigned>key_pos = detail::compute_key_pos(v, key_count, get_key);
			r.resize(v.size());

			for(unsigned i=0; i<v.size(); ++i){
				unsigned k = get_key(v[i]);
				assert(k <= key_count && "key is too large");
				r[key_pos[k]] = v[i];
				++key_pos[k];
			}
		} else if(is_stable) {
			r = stable_sort_using_comparator(v, detail::make_compare_by_key(key_count, get_key));
		} else {
			r = sort_using_comparator(v, detail::make_compare_by_key(key_count, get_key));
		}
		return r; // NVRO
	}

	template<bool is_stable, class T, class K>
	std::vector<T>maybe_stable_sort_using_key(std::vector<T>&&v, unsigned key_count, const K&get_key){
		std::vector<T>r;
		if(v.size() >= key_count / bucket_sort_min_key_to_element_ratio){
			std::vector<unsigned>key_pos = detail::compute_key_pos(v, key_count, get_key);
			r.resize(v.size());

			for(unsigned i=0; i<v.size(); ++i){
				unsigned k = get_key(v[i]);
				assert(k <= key_count && "key is too large");
				r[key_pos[k]] = std::move(v[i]);
				++key_pos[k];
			}
		} else if(is_stable) {
			r = stable_sort_using_comparator(v, detail::make_compare_by_key(key_count, get_key));
		} else {
			r = sort_using_comparator(v, detail::make_compare_by_key(key_count, get_key));
		}
		return r; // NVRO
	}
}

template<class T, class K>
std::vector<T>sort_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	return detail::maybe_stable_sort_using_key<false>(v, key_count, get_key); // NVRO
}

template<class T, class K>
std::vector<T>sort_using_key(std::vector<T>&&v, unsigned key_count, const K&get_key){
	return detail::maybe_stable_sort_using_key<false>(std::move(v), key_count, get_key); // NVRO
}

template<class T, class K>
std::vector<T>stable_sort_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	return detail::maybe_stable_sort_using_key<true>(v, key_count, get_key); // NVRO
}

template<class T, class K>
std::vector<T>stable_sort_using_key(std::vector<T>&&v, unsigned key_count, const K&get_key){
	return detail::maybe_stable_sort_using_key<true>(std::move(v), key_count, get_key); // NVRO
}


template<class T, class K>
bool is_sorted_using_key(const std::vector<T>&v, unsigned key_count, const K&get_key){
	for(unsigned i=1; i<v.size(); ++i)
		if(get_key(v[i]) < get_key(v[i-1]))
			return false;
	return true;
}






template<class T>
std::vector<unsigned> compute_sort_permutation_using_less(const std::vector<T>&v){
	return compute_sort_permutation_using_comparator(v, [](const T&l, const T&r){return l < r;});
}

template<class T>
std::vector<unsigned> compute_stable_sort_permutation_using_less(const std::vector<T>&v){
	return compute_stable_sort_permutation_using_comparator(v, [](const T&l, const T&r){return l < r;});
}

template<class T>
std::vector<unsigned> compute_inverse_sort_permutation_using_less(const std::vector<T>&v){
	return invert_permutation(compute_sort_permutation_using_less(v));
}

template<class T>
std::vector<unsigned> compute_inverse_stable_sort_permutation_using_less(const std::vector<T>&v){
	return invert_permutation(compute_stable_sort_permutation_using_less(v));
}

template<class T>
std::vector<T>stable_sort_using_less(const std::vector<T>&v){
	return stable_sort_using_comparator(v, [](const T&l, const T&r){return l < r;});
}

template<class T>
std::vector<T>stable_sort_using_less(std::vector<T>&&v){
	return stable_sort_using_comparator(std::move(v), [](const T&l, const T&r){return l < r;});
}

template<class T>
std::vector<T>sort_using_less(const std::vector<T>&v){
	return sort_using_comparator(v, [](const T&l, const T&r){return l < r;});
}

template<class T>
std::vector<T>sort_using_less(std::vector<T>&&v){
	return sort_using_comparator(std::move(v), [](const T&l, const T&r){return l < r;});
}

template<class T>
bool is_sorted_using_less(const std::vector<T>&v){
	return is_sorted_using_comparator(v, [](const T&l, const T&r){return l < r;});
}

} // RoutingKit

#endif
