#ifndef ROUTING_KIT_ID_SET_QUEUE_H
#define ROUTING_KIT_ID_SET_QUEUE_H

#include <routingkit/constants.h>
#include <routingkit/min_max.h>
#include <vector>
#include <assert.h>

namespace RoutingKit{

//! This is min queue that can only contain IDs from a given range.
//! IDs are compared by their value. Ids in the queue cannot be
//! duplicated.
class IDSetMinQueue{

private:
	// Internally, the queue is represented using an implicit binary tree
	// For every node in the tree, a bit is stored in data.
	//
	// Suppose that id_count() is 6, then the tree nodes are organized as
	// follows:
	//           1
	//          / |
	//         /   |
	//        /     |
	//       /       |
	//      2         3
	//     / |       / |
	//    /   |     /   |
	//   4     5   6     7
	//  /|    /|  / |
	// 8 9  10 11 12 13
	//
	// Node ids are counted starting from 1 and not from 0. IDs are counted 
	// starting from 0.
	//
	// The lowest level contains 6 bits because we have at most 6 IDs. Each of 
	// these lowest level bits encodes whether an ID is in the queue or not.
	//
	// If the number of IDs is uneven, for example 5, then we add another node
	// to the lowest level to make sure that every node has a sibling. This
	// padding bit will always remain false.
	//
	// The bit of upper level nodes is set, iff, there is a child whose bit is
	// set. If the queue would contain the IDs 1 and 2, then the nodes
	// 9, 10, 4, 5, 2, and 1 would have their bits sets. 
	//
	// Whether a bit is set is stored in data. data[0] is not used. The first
	// meaningful bit is data[1].
	//
	// offset stores the node number of the first node of the lowest level.
	// Because of the way that the tree is constructed, this is always
	// a power of two and it is never smaller than id_count().
	//
	// min_id stores the smallest id in the queue or invalid_id.
	//
	//
	// parent maps a node onto its parent.
	//	
	// other_child turns a node into its sibling.
	// is_larger_child checks whether a node is a right child in the 
	// above diagram.
	// is_smaller_child 


	static const unsigned root = 1;

	//! Maps a node onto its parent. Only works if x != root.
	static unsigned parent(unsigned x){
		return x>>1;
	}
	
	//! Maps a node onto its sibling. Only works if x != root.
	static unsigned other_child(unsigned x){
		return x^1;
	}

	//! True if other_child(x) < x. Only works if x != root.
	static unsigned is_larger_child(unsigned x){
		return x&1;
	}

	//! True if other_child(x) > x. Only works if x != root.
	static unsigned is_smaller_child(unsigned x){
		return !is_larger_child(x);
	}

	//! Returns the node y such that parent(y) == x and y < other_child(y).
	static unsigned smaller_child(unsigned x){
		return x<<1;
	}

	//! Returns the node y such that parent(y) == x and y > other_child(y).
	static unsigned larger_child(unsigned x){
		return (x<<1)|1;
	}

	static unsigned smallest_two_power_no_smaller_than(unsigned x){
		unsigned y = 1;
		while(y < x){
			y <<= 1;
			assert(y != 0);
		}
		return y;
	}

public:
	IDSetMinQueue(){}

	//! The queue may contain IDs from 0 to n-1.
	explicit IDSetMinQueue(unsigned n):
		max_number_of_ids(n),
		min_id(invalid_id),
		offset(smallest_two_power_no_smaller_than(n)),
		data(n+offset+(n&1), false)// +(n&1) add an element if n is odd
	{}

	unsigned id_count() const {
		return max_number_of_ids;
	}

	//! Adds an ID to the queue. If the ID is already in the queue, then
	//! nothing happens.
	void push(unsigned id){
		assert(id < id_count() && "id out of bounds");

		if(min_id == invalid_id)
			min_id = id;
		else		
			min_to(min_id, id);

		unsigned x = id + offset;

		if(data[x])
			return;

		for(;;)
		{
			assert(!data[x]);
			data[x] = true;

			if(x == root)
				break;

			x = parent(x);

			if(data[x])
				break;
		}
	}

	//! Checks whether an ID is in the queue.
	bool contains(unsigned id) const {
		assert(id < id_count() && "id out of bounds");
		return data[offset + id];
	}

	//! Checks whether the queue contains any element.
	bool empty() const {
		return min_id == invalid_id;
	}

	//! Returns the smallest ID in the queue without removing it.
	//! Returns invalid_id if the queue is empty.
	unsigned peek() const {
		return min_id;
	}

	//! Returns the smallest ID in the queue and removes it.
	//! Returns invalid_id if the queue is empty.
	unsigned pop() {
		assert(!empty());

		unsigned ret = min_id;

		unsigned x = min_id + offset;

		for(;;)
		{
			assert(data[x]);
			data[x] = false;

			if(x == root){
				min_id = invalid_id;
				return ret;
			}

			if(is_smaller_child(x)){
				unsigned y = other_child(x);
				assert(y < data.size());
				if(data[y])
					break;
			} else {
				assert(!data[other_child(x)]);
			}

			x = parent(x);
		}

		assert(is_smaller_child(x));
		x = other_child(x);
		assert(data[x]);
		for(;;){
			if(x >= offset)
				break;

			x = smaller_child(x);
			if(!data[x])
				x = other_child(x);
			assert(data[x]);
		}
		min_id = x - offset;
		
		return ret;
	}

	//! Removes all elements from the queue.
	void clear(){
		while(!empty())
			pop();
	}

private:

	unsigned max_number_of_ids;
	unsigned min_id;
	unsigned offset;
	std::vector<bool>data;
};

} // RoutingKit

#endif

