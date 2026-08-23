#ifndef ROUTING_KIT_EMULATE_GCC_BUILTIN_H
#define ROUTING_KIT_EMULATE_GCC_BUILTIN_H
#ifdef ROUTING_KIT_NO_GCC_EXTENSIONS

#include <mutex>
#include <atomic>

#define __builtin_expect(x, y) (x)

namespace RoutingKit{namespace detail{
	inline
	unsigned emulated_builtin_popcountll(unsigned long long x){
		int n=0;
		for(unsigned long long i=1; i!=0; i<<=1)
			if(x & i)
				++n;
		return n;
	}

	inline
	unsigned emulated_builtin_popcount(unsigned x){
		return emulated_builtin_popcountll(x);
	}

	inline
	unsigned emulated_builtin_ffsll(unsigned long long x){
		int n=1;
		for(unsigned long long i=1; i!=0; i<<=1, ++n)
			if(x & i)
				return n;
		return 0;
	}

	template<class T>
	bool emulated_sync_bool_compare_and_swap(T*var, T comp_value, T new_value){
		static std::mutex m;
		std::lock_guard<std::mutex> lock(m);

		std::atomic_thread_fence(std::memory_order_seq_cst);

		if(*var == comp_value){
			*var = new_value;

			std::atomic_thread_fence(std::memory_order_seq_cst);
			return true;
		}else{
			return false;
		}
	}

}}

#define __builtin_popcount(x) ::RoutingKit::detail::emulated_builtin_popcount(x)

#define __builtin_popcountll(x) ::RoutingKit::detail::emulated_builtin_popcountll(x)

#define __builtin_ffsll(x) ::RoutingKit::detail::emulated_builtin_ffsll(x)

#define __sync_bool_compare_and_swap(x, y, z) ::RoutingKit::detail::emulated_sync_bool_compare_and_swap(x, y, z)

#endif
#endif
