#ifndef ROUTING_KIT_ID_MAPPER_H
#define ROUTING_KIT_ID_MAPPER_H

#include <routingkit/constants.h>

#include <stdint.h>
#include <assert.h>
#include <stdexcept>
#include <vector>
#include <algorithm>

namespace RoutingKit{

class LocalIDMapper{
public:
	LocalIDMapper():bit_count_(0){}
	template<class Vec>
	explicit LocalIDMapper(const Vec&vec):
		LocalIDMapper(vec.size(), vec.data()){}
	LocalIDMapper(uint64_t bit_count_, const uint64_t*bits_);

	uint64_t global_id_count()const{
		return bit_count_;
	}

	uint64_t local_id_count()const{
		if(!rank_.empty())
			return rank_.back();
		else
			return 0;
	}

	bool is_global_id_mapped(uint64_t global_id) const{
		assert(global_id < global_id_count());
		return (bits_[global_id / 64] & (1ull << (global_id % 64))) != 0;
	}

	uint64_t to_local(uint64_t global_id) const;
	uint64_t to_local(uint64_t global_id, uint64_t invalid) const;

	uint64_t memory_overhead_in_bits()const{return rank_.size()*64;}
protected:
	const uint64_t*bits_;
	uint64_t bit_count_;
	std::vector<uint64_t>rank_;
};

class IDMapper : public LocalIDMapper{
public:
	IDMapper(){}
	template<class Vec>
	explicit IDMapper(const Vec&vec):
		IDMapper(vec.size(), vec.data()){}
	IDMapper(uint64_t bit_count_, const uint64_t*bits_);

	uint64_t to_global(uint64_t local_id) const;

	uint64_t memory_overhead_in_bits()const{return LocalIDMapper::memory_overhead_in_bits() + select_.size()*64;}
private:
	std::vector<uint64_t>select_;
};

} // RoutingKit

#endif
