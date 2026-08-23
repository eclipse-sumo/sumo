#include <routingkit/bit_vector.h>

#include "emulate_gcc_builtin.h"

#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <new>

#ifndef ROUTING_KIT_NO_ALIGNED_ALLOC
#ifdef _MSC_VER
#include <malloc.h>
// Let us hope that this workaround does not break when MS finally implements the C11 standard function aligned_alloc from <stdlib.h>...
// If it does break then please open an issue with the information of upto what values of _MSC_VER the workaround is needed.
#define aligned_alloc(alignment, size) _aligned_malloc(size, alignment)
#define aligned_free(ptr) _aligned_free(ptr)
#else
#define aligned_free(ptr) free(ptr)
#endif
#endif

namespace RoutingKit{

// Not all compilers support aligned_alloc such as GCC 4.6. If you have such a compiler then define ROUTING_KIT_NO_ALIGNED_ALLOC

#ifdef ROUTING_KIT_NO_ALIGNED_ALLOC
void*aligned_alloc(uint8_t alignment, uint64_t size){
	uint64_t potentially_unaligned_buffer = (uint64_t)malloc(size+alignment);
	uint64_t aligned_buffer = ((potentially_unaligned_buffer + alignment)/alignment) * alignment;
	uint8_t*buffer = (uint8_t*)aligned_buffer;
	*(buffer-1) = aligned_buffer - potentially_unaligned_buffer;
	return buffer;
}

void aligned_free(void* ptr){
	if(ptr){
		uint8_t*buffer = (uint8_t*)ptr;
		uint8_t shift = *(buffer-1);
		buffer -= shift;
		free(buffer);
	}
}
#endif


namespace {
	#ifndef ROUTING_KIT_NO_GCC_EXTENSIONS
	typedef uint64_t v8_uint64_t __attribute__((vector_size(64)));
	#else
	struct v8_uint64_t{
		v8_uint64_t(){}
		
		v8_uint64_t(unsigned x){
			for(unsigned i=0; i<8; ++i)
				v[i] = x;
		}

		uint64_t v[8];
		void operator^=(v8_uint64_t o){
			for(unsigned i=0; i<8; ++i)
				v[i] ^= o.v[i];
		}

		void operator&=(v8_uint64_t o){
			for(unsigned i=0; i<8; ++i)
				v[i] &= o.v[i];
		}

		void operator|=(v8_uint64_t o){
			for(unsigned i=0; i<8; ++i)
				v[i] |= o.v[i];
		}

		v8_uint64_t operator~()const{
			v8_uint64_t r;
			for(unsigned i=0; i<8; ++i)
				r.v[i] = ~v[i];
			return r;
		}

		uint64_t&operator[](uint64_t i){return v[i];};
		const uint64_t&operator[](uint64_t i)const{return v[i];};

	};

	v8_uint64_t operator^(v8_uint64_t l, v8_uint64_t r){
		l ^= r;
		return l;
	}

	v8_uint64_t operator&(v8_uint64_t l, v8_uint64_t r){
		l &= r;
		return l;
	}

	v8_uint64_t operator|(v8_uint64_t l, v8_uint64_t r){
		l |= r;
		return l;
	}
	#endif


	uint64_t get_v8_uint64_count(uint64_t bit_count){
		return (bit_count+511)/512;
	}

	uint64_t get_uint64_count(uint64_t bit_count){
		return get_v8_uint64_count(bit_count)*8;
	}

	uint64_t get_uint8_count(uint64_t bit_count){
		return get_v8_uint64_count(bit_count)*64;
	}

	v8_uint64_t get_padding_mask(uint64_t size){
		v8_uint64_t u = {0};

		uint64_t x = size % 512;
		uint8_t i = 0;
		while(x >= 64){
			u[i++] = ~0ull;
			x -= 64;
		}
		if(x != 0)
			u[i] = (1ull<<x)-1;

		return u;
	}

	bool is_any_bit_set(const v8_uint64_t*x){
		v8_uint64_t u = *x;

		u[0] |= u[1];
		u[2] |= u[3];
		u[4] |= u[5];
		u[6] |= u[7];

		u[0] |= u[4];
		u[2] |= u[6];
		
		u[0] |= u[2];
		
		return u[0] != 0;
	}

	bool are_all_bits_set(const v8_uint64_t*x){
		v8_uint64_t u = *x;

		u[0] &= u[1];
		u[2] &= u[3];
		u[4] &= u[5];
		u[6] &= u[7];

		u[0] &= u[4];
		u[2] &= u[6];
		
		u[0] &= u[2];
		
		return u[0] == ~0ull;
	}

	#ifndef NDEBUG
	bool are_all_padding_bits_zero(const BitVector&v){
		if(v.size() % 512 == 0){
			return true;
		} else {
			auto x = get_padding_mask(v.size());
			x = ((const v8_uint64_t*)v.data())[v.size()/512] & ~x;
			return !is_any_bit_set(&x);
		}
	}
	#endif
}

BitVector::BitVector():
	data_(nullptr), size_(0){}

BitVector::BitVector(uint64_t size, BitVector::Uninitialized)
{
	if(__builtin_expect(size != 0, true)){
		data_ = (uint64_t*)(aligned_alloc(64, get_uint8_count(size)));
		if(data_ == nullptr)
			throw std::bad_alloc();
		size_ = size;
		reset_all_padding_bits();
	}else{
		data_ = nullptr;
		size_ = 0;
	}

	assert(are_all_padding_bits_zero(*this));
}

BitVector::BitVector(uint64_t size, bool init_value)
{
	if(__builtin_expect(size != 0, true)){
		data_ = (uint64_t*)(aligned_alloc(64, get_uint8_count(size)));
		if(data_ == nullptr)
			throw std::bad_alloc();
		size_ = size;

		v8_uint64_t init_vec = {0};
		assert(init_vec[0] == 0ull);
		if(init_value)
			init_vec = ~init_vec;
		assert(!init_value || init_vec[0] == ~0ull);
		assert(init_value || init_vec[0] == 0ull);
			
		for(
			v8_uint64_t*i = (v8_uint64_t*)data_;
			i<((v8_uint64_t*)data_)+get_v8_uint64_count(size);
			++i
		)
			*i = init_vec;

		if(init_value)
			reset_all_padding_bits();
	}else{
		data_ = nullptr;
		size_ = 0;
	}

	assert(are_all_padding_bits_zero(*this));
}

BitVector::~BitVector(){
	aligned_free(data_);
}

BitVector::BitVector(const BitVector&o):
	data_(static_cast<uint64_t*>(aligned_alloc(64, get_uint8_count(o.size_)))), size_(o.size_){
	if(data_ == nullptr)
		throw std::bad_alloc();

	assert(are_all_padding_bits_zero(o));

	for(
		v8_uint64_t*i = (v8_uint64_t*)data_, *j=(v8_uint64_t*)o.data_;
		i<((v8_uint64_t*)data_)+get_v8_uint64_count(size_);
		++i, ++j
	)
		*i = *j;

	assert(are_all_padding_bits_zero(*this));
}

BitVector::BitVector(BitVector&&o):
	data_(o.data_), size_(o.size_){

	assert(are_all_padding_bits_zero(o));

	o.data_ = nullptr;
	o.size_ = 0;	
}

BitVector&BitVector::operator=(BitVector o){
	assert(are_all_padding_bits_zero(o));

	swap(o);
	return *this;
}

void BitVector::swap(BitVector&o){
	assert(are_all_padding_bits_zero(o));
	assert(are_all_padding_bits_zero(*this));

	uint64_t*x = o.data_;
	o.data_ = data_;
	data_ = x;

	uint64_t y = o.size_;
	o.size_ = size_;
	size_ = y;
}

void BitVector::reset_all_padding_bits(){
	if(size_ % 512 != 0){
		v8_uint64_t* v = (v8_uint64_t*)data_ + size_/512;
		*v &= get_padding_mask(size_);
	}

	assert(are_all_padding_bits_zero(*this));
}
	
void BitVector::resize(uint64_t new_size, BitVector::Uninitialized){
	if(data_ == nullptr){
		*this = BitVector(new_size, uninitialized);
	}else{

		assert(are_all_padding_bits_zero(*this));
		if(get_uint8_count(new_size) == get_uint8_count(size_)){
			size_ = new_size;
			reset_all_padding_bits();
		}else{
			BitVector o(new_size);

			for(
				v8_uint64_t*i = (v8_uint64_t*)data_, *j=(v8_uint64_t*)o.data_;
				i<((v8_uint64_t*)data_)+std::min(get_v8_uint64_count(size_), get_v8_uint64_count(new_size));
				++i, ++j
			){
				*j = *i;
			}
			o.reset_all_padding_bits();
			assert(are_all_padding_bits_zero(o));	
			swap(o);
		}
		assert(are_all_padding_bits_zero(*this));
	}
}

void BitVector::resize(uint64_t new_size, bool value){
	if(data_ == nullptr){
		*this = BitVector(new_size, value);
	}else{
		assert(are_all_padding_bits_zero(*this));
		if(new_size < size_) {
			resize(new_size, uninitialized);
		} else if(get_uint8_count(new_size) == get_uint8_count(size_)){
			if(value == false){
				size_ = new_size;
			} else {
				((v8_uint64_t*)data_)[size_/512] |= get_padding_mask(new_size) & ~get_padding_mask(size_);
				size_ = new_size;
			}
		} else {
			BitVector o(new_size);

			v8_uint64_t
				*i = (v8_uint64_t*)data_,
				*j = (v8_uint64_t*)o.data_;

			while(i < (v8_uint64_t*)data_ + get_v8_uint64_count(size_)){
				*j = *i;
				++i;
				++j;
			}

			if(value){
				*(j-1) |= ~get_padding_mask(size_);

				v8_uint64_t all_one = {0};
				all_one = ~all_one;

				while(j < (v8_uint64_t*)o.data_ + get_v8_uint64_count(o.size_)){
					*j = all_one;
					++j;
				}

				*(j-1) &= get_padding_mask(o.size_);
				assert(are_all_padding_bits_zero(o));
			}else{
				v8_uint64_t all_zero = {0};

				while(j < (v8_uint64_t*)o.data_ + get_v8_uint64_count(o.size_)){
					*j = all_zero;
					++j;
				}
				assert(are_all_padding_bits_zero(o));
			}

			assert(are_all_padding_bits_zero(*this));	
			swap(o);
		}
		assert(are_all_padding_bits_zero(*this));
	}
}

void BitVector::make_large_enough_for(uint64_t x, BitVector::Uninitialized){
	assert(are_all_padding_bits_zero(*this));
	if(size_ <= x) {
		uint64_t n = 1;
		while(n <= x) {
			n <<= 1;
		}
		resize(n, BitVector::uninitialized);
	}
	assert(are_all_padding_bits_zero(*this));
	assert(size_ > x);
}

void BitVector::make_large_enough_for(uint64_t x, bool init_value){
	assert(are_all_padding_bits_zero(*this));
	if(size_ <= x) {
		uint64_t n = 1;
		while(n <= x) {
			n <<= 1;
		}
		assert(n > x);
		resize(n, init_value);
	}
	assert(are_all_padding_bits_zero(*this));
	assert(size_ > x);
}


uint64_t BitVector::population_count() const{
	assert(are_all_padding_bits_zero(*this));
	uint64_t n = 0;
	for(uint64_t*i=data_; i<data_ + get_uint64_count(size_); ++i)
		n += __builtin_popcountll(*i);
	return n;
}

void BitVector::set_all(){
	v8_uint64_t x = {0};
	x = ~x;
	for(v8_uint64_t*i = (v8_uint64_t*)data_; i<((v8_uint64_t*)data_)+get_v8_uint64_count(size_); ++i)
		*i = x;
	reset_all_padding_bits();
}

void BitVector::reset_all(){
	v8_uint64_t x = {0};	
	for(v8_uint64_t*i = (v8_uint64_t*)data_; i<((v8_uint64_t*)data_)+get_v8_uint64_count(size_); ++i)
		*i = x;
	assert(are_all_padding_bits_zero(*this));
}

void BitVector::set_all(bool value){
	v8_uint64_t x = {0};
	if(value)
		x = ~x;
		
	for(v8_uint64_t*i = (v8_uint64_t*)data_; i<((v8_uint64_t*)data_)+get_v8_uint64_count(size_); ++i)
		*i = x;

	if(value)
		reset_all_padding_bits();
	assert(are_all_padding_bits_zero(*this));
}

bool BitVector::are_all_set()const{
	assert(are_all_padding_bits_zero(*this));
	if(__builtin_expect(size_ == 0, false))
		return true;

	v8_uint64_t x = {0};
	x = ~x;

	uint64_t n = get_v8_uint64_count(size_);
	--n;

	v8_uint64_t*i;
	for(i = (v8_uint64_t*)data_; i<((v8_uint64_t*)data_)+n; ++i)
		x &= *i;
	x &= *i | ~get_padding_mask(size_);
	return are_all_bits_set(&x);
}

bool BitVector::is_any_set()const{
	assert(are_all_padding_bits_zero(*this));
	if(__builtin_expect(size_ == 0, false))
		return false;

	v8_uint64_t x = {0};
	
	for(v8_uint64_t*i = (v8_uint64_t*)data_; i<((v8_uint64_t*)data_)+get_v8_uint64_count(size_); ++i)
		x |= *i;

	return is_any_bit_set(&x);
}

BitVector&BitVector::operator|=(const BitVector&o){
	assert(are_all_padding_bits_zero(*this));
	assert(are_all_padding_bits_zero(o));
	assert(size_ == o.size_ && "can only combine bit vectors of same size");
	for(v8_uint64_t*i = (v8_uint64_t*)data_, *j=(v8_uint64_t*)o.data_; i<((v8_uint64_t*)data_)+get_v8_uint64_count(size_); ++i, ++j)
		*i |= *j;
	assert(are_all_padding_bits_zero(*this));
	return *this;
}

BitVector&BitVector::operator^=(const BitVector&o){
	assert(are_all_padding_bits_zero(*this));
	assert(are_all_padding_bits_zero(o));
	assert(size_ == o.size_ && "can only combine bit vectors of same size");
	for(v8_uint64_t*i = (v8_uint64_t*)data_, *j=(v8_uint64_t*)o.data_; i<((v8_uint64_t*)data_)+get_v8_uint64_count(size_); ++i, ++j)
		*i ^= *j;
	assert(are_all_padding_bits_zero(*this));
	return *this;
}

BitVector&BitVector::operator&=(const BitVector&o){
	assert(are_all_padding_bits_zero(*this));
	assert(are_all_padding_bits_zero(o));
	assert(size_ == o.size_ && "can only combine bit vectors of same size");
	for(v8_uint64_t*i = (v8_uint64_t*)data_, *j=(v8_uint64_t*)o.data_; i<((v8_uint64_t*)data_)+get_v8_uint64_count(size_); ++i, ++j)
		*i &= *j;
	assert(are_all_padding_bits_zero(*this));
	return *this;
}

void BitVector::inplace_not(){
	assert(are_all_padding_bits_zero(*this));
	for(v8_uint64_t*i = (v8_uint64_t*)data_; i<((v8_uint64_t*)data_)+get_v8_uint64_count(size_); ++i)
		*i = ~*i;
	reset_all_padding_bits();
	assert(are_all_padding_bits_zero(*this));
}

bool operator==(const BitVector&l, const BitVector&r){
	assert(are_all_padding_bits_zero(l));
	assert(are_all_padding_bits_zero(r));
	if(__builtin_expect(l.size_ != r.size_, false))
		return false;

	v8_uint64_t x = {0};

	for(v8_uint64_t*i = (v8_uint64_t*)l.data_, *j=(v8_uint64_t*)r.data_; i<((v8_uint64_t*)l.data_)+get_v8_uint64_count(l.size_); ++i, ++j){
		x |= (*i ^ *j);
	}
	
	return !is_any_bit_set(&x);
}

bool operator<(const BitVector&l, const BitVector&r){
	assert(are_all_padding_bits_zero(l));
	assert(are_all_padding_bits_zero(r));
	if(__builtin_expect(l.size_ != r.size_, false))
		return l.size_ < r.size_;

	for(uint64_t*i = l.data_, *j = r.data_; i<l.data_+get_uint64_count(l.size_); ++i, ++j)
		if(*i != *j)
			return *i < *j;
	return false;
}

} // namespace RoutingKit
