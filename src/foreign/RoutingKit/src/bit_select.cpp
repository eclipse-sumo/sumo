#include "bit_select.h"
#include "emulate_gcc_builtin.h"
#include <assert.h>


namespace RoutingKit{

uint32_t uint64_bit_select(uint64_t word, uint32_t n) {
	assert(n < __builtin_popcountll(word) && "n is out of bounds");

	uint32_t r = 0;
	assert(n < 64);
	{
		uint32_t lower_ones = __builtin_popcount(static_cast<uint32_t>(word));
		if(lower_ones <= n){
			word >>= 32;
			n -= lower_ones;
			r += 32;
		}
	}
	assert(n < 32);
	{
		uint32_t lower_ones = __builtin_popcount(static_cast<uint32_t>(word)&0xFFFF);
		if(lower_ones <= n){
			word >>= 16;
			n -= lower_ones;
			r += 16;
		}
	}
	assert(n < 16);
	{
		uint32_t lower_ones = __builtin_popcount(static_cast<uint32_t>(word)&0xFF);
		if(lower_ones <= n){
			word >>= 8;
			n -= lower_ones;
			r += 8;
		}
	}	
	assert(n < 8);
	while(n > 0){
		// reset lowest bit
		word &= word - 1;
		--n;
	}
	// __builtin_ffsl returns the 1-based offset of lowest bit, or 0 if no bit is set
	uint32_t x = __builtin_ffsll(word);
	assert(x != 0);
	return r+x-1;
}

uint32_t uint512_bit_select(const uint64_t*block, uint32_t n) {
	#ifndef NDEBUG
	uint32_t i = 0;
	#endif

	uint32_t r = 0;
	for(;;){
		uint8_t lower_ones = __builtin_popcountll(*block);
		if(lower_ones <= n){
			++block;
			#ifndef NDEBUG
			assert(i < 8);
			++i;
			#endif
			n -= lower_ones;
			r += 64;
		}else{
			return r+uint64_bit_select(*block, n);
		}
	}
}



uint64_t bit_select(uint64_t uint512_count, const uint64_t*uint512_rank, const uint64_t*data, uint64_t n){
	assert(uint512_count != 0);

	uint64_t uint512_skipped = 0;

	while(! (n < uint512_rank[1])){
		uint64_t step = 1;
		while(step < uint512_count && uint512_rank[step] <= n)
			step <<= 1;
		step >>= 1;

		uint512_rank += step;
		data += 8*step;
		uint512_count -= step;

		uint512_skipped += step;
	}

	assert(uint512_rank[0] <= n);

	return uint512_bit_select(data, n-uint512_rank[0])+uint512_skipped*512;

	return 0;
}

} // namespace RoutingKit

