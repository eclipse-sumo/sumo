#ifndef ROUTING_KIT_BIT_VECTOR_H
#define ROUTING_KIT_BIT_VECTOR_H

#include <stdint.h>
#include <assert.h>
#include <utility>

namespace RoutingKit{

class BitVector{
public:
	struct Uninitialized{};
	static constexpr Uninitialized uninitialized = {};

	BitVector();
	BitVector(uint64_t size, Uninitialized);
	explicit BitVector(uint64_t size, bool init_value = false);
	~BitVector();
	BitVector(const BitVector&);
	BitVector(BitVector&&);

	BitVector&operator=(BitVector);
	void swap(BitVector&);
	
	bool empty()const { return size() == 0; }
	uint64_t size()const { return size_; }
	
	void resize(uint64_t size, Uninitialized);
	void resize(uint64_t size, bool init_value = false);

	void make_large_enough_for(uint64_t x, Uninitialized);
	void make_large_enough_for(uint64_t x, bool init_value = false);

	bool is_set(uint64_t x)const{
		assert(x < size_ && "argument out of bounds");
		uint64_t a = x/64;
	    uint64_t b = x%64;
		uint64_t d = data_[a];
		return d & (1ull << b);
	}

	void set(uint64_t x){
		assert(x < size_ && "argument out of bounds");
		uint64_t a = x/64;
	    uint64_t b = x%64;
		uint64_t d = data_[a];
		d |= (1ull << b);
		data_[a] = d;
	}

	void set_if(uint64_t x, bool value){
		assert(x < size_ && "argument out of bounds");
		uint64_t a = x/64;
	    uint64_t b = x%64;
		uint64_t d = data_[a];
		d |= ((uint64_t)value << b);
		data_[a] = d;
	}

	void set(uint64_t x, bool value){
		assert(x < size_ && "argument out of bounds");
		uint64_t a = x/64;
	    uint64_t b = x%64;
		uint64_t d = data_[a];
		d &= ~(1ull << b);
		d |= ((uint64_t)value << b);
		data_[a] = d;
	}

	void reset(uint64_t x){
		assert(x < size_ && "argument out of bounds");
		uint64_t a = x/64;
	    uint64_t b = x%64;
		uint64_t d = data_[a];
		d &= ~(1ull << b);
		data_[a] = d;
	}

	void toggle(uint64_t x){
		assert(x < size_ && "argument out of bounds");
		uint64_t a = x/64;
	    uint64_t b = x%64;
		uint64_t d = data_[a];
		d ^= (1ull << b);
		data_[a] = d;
	}

	void set_all();
	void set_all(bool value);
	void reset_all();

	bool are_all_set()const;
	bool is_any_set()const;

	uint64_t population_count() const;

	uint64_t count_true() const {return population_count();}
	uint64_t count_false() const {return size()-population_count();}


	BitVector&operator|=(const BitVector&);
	BitVector&operator^=(const BitVector&);
	BitVector&operator&=(const BitVector&);

	void inplace_not();
	BitVector operator~() const { BitVector v = *this; v.inplace_not(); return v; }

	friend bool operator==(const BitVector&l, const BitVector&r);
	friend bool operator<(const BitVector&l, const BitVector&r);

	uint64_t*data(){return data_;}
	const uint64_t*data()const{return data_;}

	uint64_t uint512_count()const{ return (size_+511) / 512; }

	void reset_all_padding_bits();
private:
	uint64_t*data_;
	uint64_t size_;
};

template<class F>
inline BitVector make_bit_vector(uint64_t size, const F&f){
	BitVector v(size, BitVector::uninitialized);
	for(uint64_t x=0; x<size; ++x)
		v.set(x, f(x));
	return v;
}

// I envy the day where C++ finally gets rid of this error prone boilerplate.

inline BitVector operator|(BitVector&&l, BitVector&&r) { l |= r; return std::move(l); }
inline BitVector operator|(BitVector&&l, const BitVector&r) { l |= r; return std::move(l); }
inline BitVector operator|(const BitVector&l, BitVector&&r) { r |= l; return std::move(r); }
inline BitVector operator|(const BitVector&l, const BitVector&r) { BitVector x = l; x |= r; return x; }

inline BitVector operator&(BitVector&&l, BitVector&&r) { l &= r; return std::move(l); }
inline BitVector operator&(BitVector&&l, const BitVector&r) { l &= r; return std::move(l); }
inline BitVector operator&(const BitVector&l, BitVector&&r) { r &= l; return std::move(r); }
inline BitVector operator&(const BitVector&l, const BitVector&r) { BitVector x = l; x &= r; return x; }

inline BitVector operator^(BitVector&&l, BitVector&&r) { l ^= r; return std::move(l); }
inline BitVector operator^(BitVector&&l, const BitVector&r) { l ^= r; return std::move(l); }
inline BitVector operator^(const BitVector&l, BitVector&&r) { r ^= l; return std::move(r); }
inline BitVector operator^(const BitVector&l, const BitVector&r) { BitVector x = l; x ^= r; return x; }

inline bool operator!=(const BitVector&l, const BitVector&r){ return !(l == r); }
inline bool operator>(const BitVector&l, const BitVector&r){ return r < l; }
inline bool operator<=(const BitVector&l, const BitVector&r){ return !(l > r); }
inline bool operator>=(const BitVector&l, const BitVector&r){ return !(l < r); }

} // namespace RoutingKit

#endif
