#include <routingkit/id_mapper.h>

#include "emulate_gcc_builtin.h"
#include "bit_select.h"

namespace RoutingKit{

namespace{
	const int select_bits = 8;

}

LocalIDMapper::LocalIDMapper(uint64_t bit_count, const uint64_t*bits):
	bits_(bits),
	bit_count_(bit_count),
	rank_((bit_count_+511)/512 + 1){

	uint64_t i = 0;
	uint64_t s = 0;
		
	for(uint64_t j = 0; j<rank_.size()-1; ++j){
		rank_[j] = s;
		uint64_t
			s0 = __builtin_popcountll(bits_[i+0]),
			s1 = __builtin_popcountll(bits_[i+1]),
			s2 = __builtin_popcountll(bits_[i+2]),
			s3 = __builtin_popcountll(bits_[i+3]),
			s4 = __builtin_popcountll(bits_[i+4]),
			s5 = __builtin_popcountll(bits_[i+5]),
			s6 = __builtin_popcountll(bits_[i+6]),
			s7 = __builtin_popcountll(bits_[i+7]);

		i += 8;

		s0 += s4; s1 += s5; s2 += s6; s3 += s7;

		s0 += s2; s1 += s3;

		s0 += s1;

		s += s0;

	}
	rank_.back() = s;
}

IDMapper::IDMapper(uint64_t bit_count, const uint64_t*bits):
	LocalIDMapper(bit_count, bits){
	uint32_t select_query = 0, select_value = 0;
	select_.resize((local_id_count() + ((1<<select_bits)-1)) / (1<<select_bits));
	for(uint64_t block=0; block < rank_.size()-1; ++block){
		while(rank_[block] <= select_value && select_value < rank_[block+1]){
			select_[select_query] = block;
			++select_query;
			select_value = select_query << select_bits;
		}
	}
}

uint64_t LocalIDMapper::to_local(uint64_t global_id, uint64_t invalid) const {
	if(global_id >= global_id_count())
		return invalid;

	uint8_t uint64_offset = global_id % 64;
	uint64_t uint64_index = global_id / 64;


	if(__builtin_expect((bits_[uint64_index] & (1ull<<uint64_offset))==0, true)){
		return invalid;
	}

	uint64_t uint512_index = global_id / 512;

	uint64_t local_id = rank_[uint512_index];

	for(uint64_t i=uint512_index*8; i<uint64_index; ++i)
		local_id += __builtin_popcountll(bits_[i]);

	local_id += __builtin_popcountll(bits_[uint64_index] & ((1ull<<uint64_offset)-1));

	assert(local_id < local_id_count());

	return local_id;
}

uint64_t LocalIDMapper::to_local(uint64_t global_id) const {
	assert(global_id < global_id_count() && "global id is out of bounds");

	uint8_t uint64_offset = global_id % 64;
	uint64_t uint64_index = global_id / 64;

	assert((bits_[uint64_index] & (1ull<<uint64_offset))!=0 && "global id is not mapped");

	uint64_t uint512_index = global_id / 512;

	uint64_t local_id = rank_[uint512_index];

	for(uint64_t i=uint512_index*8; i<uint64_index; ++i)
		local_id += __builtin_popcountll(bits_[i]);

	local_id += __builtin_popcountll(bits_[uint64_index] & ((1ull<<uint64_offset)-1));

	assert(local_id < local_id_count());

	return local_id;
}

uint64_t IDMapper::to_global(uint64_t local_id) const {
	assert(local_id < local_id_count());

	uint64_t start_uint512 = select_[local_id >> select_bits];

	assert(rank_[start_uint512] <= local_id);

	uint64_t global_id = bit_select(
		(bit_count_+511)/512-start_uint512,
		rank_.data() + start_uint512,
		bits_ + 8*start_uint512,
		local_id
	) + 512*start_uint512;

	assert(is_global_id_mapped(global_id));
	assert(to_local(global_id) == local_id);

	return global_id;
}

} // RoutingKit

