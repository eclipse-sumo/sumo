#include <routingkit/customizable_contraction_hierarchy.h>
#include <routingkit/contraction_hierarchy.h>
#include <routingkit/sort.h>
#include <routingkit/constants.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/permutation.h>
#include <routingkit/filter.h>
#include <routingkit/graph_util.h>
#include <routingkit/id_mapper.h>
#include <routingkit/timer.h>

#include "emulate_gcc_builtin.h"

#include <vector>
#include <assert.h>
#include <algorithm>
#include <stdexcept>
#ifdef _OPENMP
#include <omp.h>
#endif

namespace RoutingKit{

namespace{
	template<class OnNewArc>
	unsigned compute_chordal_supergraph(unsigned node_count, const std::vector<unsigned>&tail, const std::vector<unsigned>&head, const OnNewArc&on_new_arc){
		std::vector<std::vector<unsigned>> nodes(node_count);
		for(unsigned i = 0; i < tail.size(); ++i){
			if(tail[i] < head[i]) {
				nodes[tail[i]].push_back(head[i]);
			}
		}

		for(unsigned n = 0; n < node_count; ++n){
			std::sort(nodes[n].begin(), nodes[n].end());
			auto it = std::unique(nodes[n].begin(), nodes[n].end());
			nodes[n].resize(std::distance(nodes[n].begin(), it));
		}

		size_t max_upward_degree = 0;
		for(unsigned n = 0; n < node_count; ++n){
			if(nodes[n].size() == 0){ continue; }
			const unsigned lowest_neighbor = nodes[n][0];

			std::vector<unsigned> merged(nodes[n].size() + nodes[lowest_neighbor].size() - 1);
			std::merge(++nodes[n].begin(), nodes[n].end(), nodes[lowest_neighbor].begin(), nodes[lowest_neighbor].end(), merged.begin());
			auto it = std::unique(merged.begin(), merged.end());
			merged.resize(std::distance(merged.begin(), it));
			nodes[lowest_neighbor] = std::move(merged);

			for(unsigned neighbor : nodes[n]){
				on_new_arc(n, neighbor);
			}
			max_to(max_upward_degree, nodes[n].size());
		}
		return max_upward_degree;
	}

	// Let {x,y,z} be a triangle with ranks x<y<z. The bottom arc of this triangle is x,y and the mid arc is x,z and the top arc is y,z.
	// This triangle is a lower triangle of its top arc, i.e., y,z. x is the bottom node, y is the top node, and z is the top node.

	// Callback has signature
	// f(bottom_arc, mid_arc, top_arc, bottom_node, mid_node, top_node)
	// and should return false to abort the enumeration, and true to continue

	// Return value forall_*_triangle_of_arc is false if and only if the enumeration was aborted by the callback.

	template<class F>
	bool forall_upper_triangles_of_arc(const CustomizableContractionHierarchy&cch, unsigned x, unsigned y, unsigned xy, const F&f){
		unsigned x_up_arc = xy+1;
		unsigned x_up_arc_end = cch.up_first_out[x+1];

		unsigned y_up_arc = cch.up_first_out[y];
		unsigned y_up_arc_end = cch.up_first_out[y+1];

		while(x_up_arc != x_up_arc_end && y_up_arc != y_up_arc_end){
			if(cch.up_head[x_up_arc] < cch.up_head[y_up_arc]){
				++x_up_arc;
			}else if(cch.up_head[x_up_arc] > cch.up_head[y_up_arc]){
				++y_up_arc;
			}else{
				unsigned z = cch.up_head[x_up_arc];
				if(!f(xy, x_up_arc, y_up_arc, x, y, z))
					return false;
				++x_up_arc;
				++y_up_arc;
			}
		}
		return true;
	}

	template<class F>
	bool forall_upper_triangles_of_arc(const CustomizableContractionHierarchy&cch, unsigned xy, const F&f){
		return forall_upper_triangles_of_arc(cch, cch.up_tail[xy], cch.up_head[xy], xy, f);
	}

	template<class F>
	bool forall_intermediate_triangles_of_arc(const CustomizableContractionHierarchy&cch, unsigned x, unsigned y, unsigned xy, const F&f){
		unsigned x_up_arc = cch.up_first_out[x];
		unsigned x_up_arc_end = xy;

		unsigned y_down_arc = cch.down_first_out[y];
		unsigned y_down_arc_end = cch.down_first_out[y+1];

		while(x_up_arc != x_up_arc_end && y_down_arc != y_down_arc_end){
			if(cch.up_head[x_up_arc] < cch.down_head[y_down_arc]){
				++x_up_arc;
			}else if(cch.up_head[x_up_arc] > cch.down_head[y_down_arc]){
				++y_down_arc;
			}else{
				unsigned z = cch.up_head[x_up_arc];
				if(!f(x_up_arc, xy, cch.down_to_up[y_down_arc], x, z, y))
					return false;
				++x_up_arc;
				++y_down_arc;
			}
		}
		return true;
	}

	template<class F>
	bool forall_intermediate_triangles_of_arc(const CustomizableContractionHierarchy&cch, unsigned xy, const F&f){
		return forall_intermediate_triangles_of_arc(cch, cch.up_tail[xy], cch.up_head[xy], xy, f);
	}

	template<class F>
	bool forall_lower_triangles_of_arc(const CustomizableContractionHierarchy&cch, unsigned x, unsigned y, unsigned xy, const F&f){
		unsigned x_down_arc = cch.down_first_out[x];
		unsigned x_down_arc_end = cch.down_first_out[x+1];

		unsigned y_down_arc = cch.down_first_out[y];
		unsigned y_down_arc_end = cch.down_first_out[y+1];

		while(x_down_arc != x_down_arc_end && y_down_arc != y_down_arc_end){
			if(cch.down_head[x_down_arc] < cch.down_head[y_down_arc]){
				++x_down_arc;
			}else if(cch.down_head[x_down_arc] > cch.down_head[y_down_arc]){
				++y_down_arc;
			}else{
				unsigned z = cch.down_head[x_down_arc];
				if(!f(cch.down_to_up[x_down_arc], cch.down_to_up[y_down_arc], xy, z, x, y))
					return false;
				++x_down_arc;
				++y_down_arc;
			}
		}
		return true;
	}

	template<class F>
	bool forall_lower_triangles_of_arc(const CustomizableContractionHierarchy&cch, unsigned xy, const F&f){
		return forall_lower_triangles_of_arc(cch, cch.up_tail[xy], cch.up_head[xy], xy, f);
	}

	#ifndef NDEBUG
	struct TriangleVerifier{

		TriangleVerifier(){}
		explicit TriangleVerifier(const CustomizableContractionHierarchy&cch):cch(&cch){}

		bool operator()(
			unsigned bottom_arc, unsigned mid_arc, unsigned top_arc,
			unsigned bottom_node, unsigned mid_node, unsigned top_node
		) const {
			assert(bottom_node != mid_node);
			assert(bottom_node != top_node);
			assert(mid_node != top_node);

			assert(bottom_arc != mid_arc);
			assert(bottom_arc != top_arc);
			assert(mid_arc != top_arc);

			assert(bottom_node < mid_node);
			assert(mid_node < top_node);

			assert(bottom_arc < mid_arc);
			assert(mid_arc < top_arc);

			assert(cch->up_tail[bottom_arc] == bottom_node);
			assert(cch->up_head[bottom_arc] == mid_node);

			assert(cch->up_tail[mid_arc] == bottom_node);
			assert(cch->up_head[mid_arc] == top_node);

			assert(cch->up_tail[top_arc] == mid_node);
			assert(cch->up_head[top_arc] == top_node);

			return true;
		}

		const CustomizableContractionHierarchy*cch;
	};
	#endif
}

CustomizableContractionHierarchy::CustomizableContractionHierarchy(
	std::vector<unsigned>arg_order,
	std::vector<unsigned>input_tail,
	std::vector<unsigned>input_head,
	std::function<void(const std::string&)>log_message,
	bool filter_always_inf_arcs
):
	order(std::move(arg_order))
{
	unsigned node_count = order.size();
	unsigned input_arc_count = input_tail.size();

	long long timer = 0;

	if(log_message){
		log_message("Building CCH");
		log_message("Input graph has "+std::to_string(node_count) + " nodes and "+std::to_string(input_arc_count)+" arcs");
	}

	rank = invert_permutation(order);

	#ifndef NDEBUG
	std::vector<unsigned> raw_input_tail = input_tail;
	std::vector<unsigned> raw_input_head = input_head;
	#endif


	if(log_message){
		log_message("Start reordering nodes according to order");
		timer = -get_micro_time();
	}

	input_tail = apply_permutation_to_elements_of(rank, std::move(input_tail));
	input_head = apply_permutation_to_elements_of(rank, std::move(input_head));

	if(log_message){
		timer += get_micro_time();
		log_message("Finished reordering nodes, needed "+std::to_string(timer)+"musec");
	}

	std::vector<unsigned>input_arc_id;

	if(log_message){
		log_message("Start sorting arcs");
		timer = -get_micro_time();
	}

	{
		input_arc_id = compute_sort_permutation_first_by_tail_then_by_head_and_apply_sort_to_tail(node_count, input_tail, input_head);
		input_head = apply_permutation(input_arc_id, input_head);
	}

	if(log_message){
		timer += get_micro_time();
		log_message("Finished sorting arcs, needed "+std::to_string(timer)+"musec");
	}

	#ifndef NDEBUG
	for(unsigned input_arc=0; input_arc<input_arc_count; ++input_arc){
		assert(rank[raw_input_head[input_arc_id[input_arc]]] == input_head[input_arc]);
		assert(rank[raw_input_tail[input_arc_id[input_arc]]] == input_tail[input_arc]);
	}
	#endif

	// Compute up graph

	if(log_message){
		log_message("Start building chordal supergraph");
		timer = -get_micro_time();
	}
	{
		// Make graph symmetric & remove multi edges
		std::vector<unsigned>symmetric_tail(2*input_arc_count);
		std::vector<unsigned>symmetric_head(2*input_arc_count);
		std::copy(input_tail.begin(), input_tail.end(), symmetric_tail.begin());
		std::copy(input_head.begin(), input_head.end(), symmetric_head.begin());
		std::copy(input_tail.begin(), input_tail.end(), symmetric_head.begin()+input_arc_count);
		std::copy(input_head.begin(), input_head.end(), symmetric_tail.begin()+input_arc_count);

		{
			auto p = compute_inverse_sort_permutation_first_by_tail_then_by_head_and_apply_sort_to_tail(node_count, symmetric_tail, symmetric_head);
			symmetric_head = apply_inverse_permutation(p, std::move(symmetric_head));
		}

		BitVector filter(2*input_arc_count, BitVector::uninitialized);
		if(input_arc_count != 0)
			filter.set(0, symmetric_tail[0] != symmetric_head[0]);
		for(unsigned i=1; i<2*input_arc_count; ++i)
			filter.set(i, (symmetric_head[i] != symmetric_head[i-1] || symmetric_tail[i] != symmetric_tail[i-1]) && (symmetric_tail[i] != symmetric_head[i]));

		inplace_keep_element_of_vector_if(filter, symmetric_tail);
		inplace_keep_element_of_vector_if(filter, symmetric_head);

		unsigned upper_treewidth_bound = compute_chordal_supergraph(
			node_count, symmetric_tail, symmetric_head,
			[&](unsigned x, unsigned y){
				if(up_tail.size() == invalid_id){
					if(log_message)
						log_message("CCH Construction aborted because chordal supergraph contains 2^32 or more arcs");
					throw std::runtime_error("CCH must contain at most 2^32-1 arcs");
				}
				up_tail.push_back(x);
				up_head.push_back(y);
			}
		);

		if(log_message){
			log_message("The treewidth of the input graph is bounded by "+std::to_string(upper_treewidth_bound));
		}

		up_head.shrink_to_fit();
		up_tail.shrink_to_fit();

		{
			auto p = compute_inverse_sort_permutation_first_by_tail_then_by_head_and_apply_sort_to_tail(node_count, up_tail, up_head);
			up_head = apply_inverse_permutation(p, up_head);
		}

		up_first_out = invert_vector(up_tail, node_count);
	}

	unsigned cch_arc_count = up_tail.size();

	if(log_message){
		timer += get_micro_time();
		log_message("Finished building chordal supergraph, needed "+std::to_string(timer)+"musec");
		log_message("Chordal supergraph contains "+std::to_string(cch_arc_count)+" arcs");
	}

	// Compute input -> ch mapping

	if(log_message){
		log_message("Start computing mapping from input arcs to CCH arcs");
		timer = -get_micro_time();
	}

	if(cch_arc_count == 0){
		input_arc_to_cch_arc.resize(input_arc_count, invalid_id);
		is_input_arc_upward.resize(input_arc_count, false);
	}else{
		input_arc_to_cch_arc.resize(input_arc_count);
		is_input_arc_upward.resize(input_arc_count, false);

		{
			unsigned cch_up_arc = 0;
			for(unsigned input_arc=0; input_arc<input_arc_count; ++input_arc){
				if(input_tail[input_arc] < input_head[input_arc]){
					while(input_tail[input_arc] != up_tail[cch_up_arc] || input_head[input_arc] != up_head[cch_up_arc]){
						assert(cch_up_arc < cch_arc_count);
						++cch_up_arc;
					}
					assert(input_tail[input_arc] == up_tail[cch_up_arc]);
					assert(input_head[input_arc] == up_head[cch_up_arc]);
					input_arc_to_cch_arc[input_arc_id[input_arc]] = cch_up_arc;
					is_input_arc_upward.set(input_arc_id[input_arc]);
				}else if(input_tail[input_arc] == input_head[input_arc]){
					input_arc_to_cch_arc[input_arc_id[input_arc]] = invalid_id; // input arc is a loop
				}
			}
		}

		{
			std::swap(input_tail, input_head);
			auto p = compute_inverse_sort_permutation_first_by_tail_then_by_head_and_apply_sort_to_tail(node_count, input_tail, input_head);
			input_head = apply_inverse_permutation(p, input_head);
			input_arc_id = apply_inverse_permutation(p, input_arc_id);
		}

		{
			unsigned cch_up_arc = 0;
			for(unsigned input_arc=0; input_arc<input_arc_count; ++input_arc){
				if(input_tail[input_arc] < input_head[input_arc]){
					while(input_tail[input_arc] != up_tail[cch_up_arc] || input_head[input_arc] != up_head[cch_up_arc]){
						assert(cch_up_arc < cch_arc_count);
						++cch_up_arc;
					}
					input_arc_to_cch_arc[input_arc_id[input_arc]] = cch_up_arc;
				}
			}
		}
	}

	if(log_message){
		timer += get_micro_time();
		log_message("Finished computing mapping, needed "+std::to_string(timer)+"musec");
	}

	if(log_message){
		log_message("Start computing elimination tree");
		timer = -get_micro_time();
	}

	// Compute elimination tree
	{
		elimination_tree_parent.resize(node_count);
		for(unsigned x=0; x<node_count; ++x){
			if(up_first_out[x] != up_first_out[x+1])
				elimination_tree_parent[x] = up_head[up_first_out[x]];
			else
				elimination_tree_parent[x] = invalid_id;
		}

		#ifndef NDEBUG
		for(unsigned x=0; x<node_count-1; ++x){
			if(elimination_tree_parent[x] != invalid_id){
				assert(x < elimination_tree_parent[x]);
				for(unsigned xy=up_first_out[x]; xy<up_first_out[x+1]; ++xy){
					unsigned y = up_head[xy];
					assert(elimination_tree_parent[x] <= y);
				}
			}else{
				assert(up_first_out[x] == up_first_out[x+1]);
			}
		}
		#endif
	}


	if(log_message){
		timer += get_micro_time();
		log_message("Finished computing elimination tree, needed "+std::to_string(timer)+"musec");
	}

	if(log_message){
		std::vector<unsigned>
			nodes_in_search_space(node_count, 0),
			arcs_in_search_space(node_count, 0);

		for(unsigned x=node_count-1; x!=(unsigned)-1; --x){
			if(elimination_tree_parent[x] != invalid_id){
				nodes_in_search_space[x] = 1 + nodes_in_search_space[elimination_tree_parent[x]];
				arcs_in_search_space[x] = up_first_out[x+1] - up_first_out[x] + arcs_in_search_space[elimination_tree_parent[x]];
			}else{
				nodes_in_search_space[x] = 1;
				arcs_in_search_space[x] = 0;
			}
		}

		unsigned max_nodes_in_search_space = 0;
		unsigned max_arcs_in_search_space = 0;
		unsigned long long nodes_in_search_space_sum = 0;
		unsigned long long arcs_in_search_space_sum = 0;

		for(unsigned x=0; x<node_count; ++x){
			max_to(max_nodes_in_search_space, nodes_in_search_space[x]);
			max_to(max_arcs_in_search_space, arcs_in_search_space[x]);
			nodes_in_search_space_sum += nodes_in_search_space[x];
			arcs_in_search_space_sum += arcs_in_search_space[x];
		}
		log_message("The average number of nodes in a search space is "+std::to_string(nodes_in_search_space_sum/node_count));
		log_message("The maximum number of nodes in a search space is "+std::to_string(max_nodes_in_search_space));
		log_message("The average number of arcs in a search space is "+std::to_string(arcs_in_search_space_sum/node_count));
		log_message("The maximum number of arcs in a search space is "+std::to_string(max_arcs_in_search_space));
	}

	// filter up graph
	if(!filter_always_inf_arcs)
		log_message("Not filtering upward arcs");
	else
	{
		if(log_message){
			log_message("Start filtering upward arcs");
			timer = -get_micro_time();
		}

		// assert that these have not yet been generated because they would be invalidated by this step
		assert(down_head.empty());
		assert(does_cch_arc_have_extra_input_arc.empty());

		BitVector
			can_forward_weight_be_non_inf(cch_arc_count, false),
			can_backward_weight_be_non_inf(cch_arc_count, false);

		for(unsigned input_arc=0; input_arc<input_arc_count; ++input_arc){
			unsigned cch_arc=input_arc_to_cch_arc[input_arc];
			if(cch_arc != invalid_id){
				if(is_input_arc_upward.is_set(input_arc))
					can_forward_weight_be_non_inf.set(cch_arc);
				else
					can_backward_weight_be_non_inf.set(cch_arc);
			}
		}


		unsigned long long triangle_count = 0;

		for(unsigned a=0; a<cch_arc_count; ++a){
			forall_upper_triangles_of_arc(
				*this, a,
				[&](
					unsigned bottom_arc, unsigned mid_arc, unsigned top_arc,
					unsigned bottom_node, unsigned mid_node, unsigned top_node
				){
					(void) bottom_node; (void) mid_node; (void) top_node;

					// TODO: Micro-optimizing this code could decrease 1% to 10% of the whole CCH build process.

					if(!can_forward_weight_be_non_inf.is_set(top_arc)){
						if(can_backward_weight_be_non_inf.is_set(bottom_arc) && can_forward_weight_be_non_inf.is_set(mid_arc))
							can_forward_weight_be_non_inf.set(top_arc);
					}

					if(!can_backward_weight_be_non_inf.is_set(top_arc)){
						if(can_forward_weight_be_non_inf.is_set(bottom_arc) && can_backward_weight_be_non_inf.is_set(mid_arc))
							can_backward_weight_be_non_inf.set(top_arc);
					}

					++triangle_count;
					return true;
				}
			);
		}

		BitVector must_keep_arc = can_forward_weight_be_non_inf | can_backward_weight_be_non_inf;

		up_head = keep_element_of_vector_if(must_keep_arc, std::move(up_head));
		up_tail = keep_element_of_vector_if(must_keep_arc, std::move(up_tail));
		up_first_out = invert_vector(up_tail, node_count);

		LocalIDMapper map(must_keep_arc);
		for(auto&x:input_arc_to_cch_arc)
			if(x != invalid_id)
				x = map.to_local(x);

		if(log_message){
			timer += get_micro_time();
			log_message("Finished filtering upward arcs, needed "+std::to_string(timer)+"musec");
			log_message("The number of arcs decreased from "+std::to_string(cch_arc_count)+" to "+std::to_string(map.local_id_count()));
			log_message("The number of triangles before filtering was "+std::to_string(triangle_count)+". (The value after filtering was not determined.)");
		}

		cch_arc_count = map.local_id_count();
	}

	if(log_message){
		log_message("Start computing downward arcs");
		timer = -get_micro_time();
	}

	// compute down graph
	auto down_tail = up_head;
	down_head = up_tail;

	{
		down_to_up = compute_sort_permutation_first_by_tail_then_by_head_and_apply_sort_to_tail(node_count, down_tail, down_head);
		down_head = apply_permutation(down_to_up, std::move(down_head));
	}
	down_first_out = invert_vector(down_tail, node_count);

	if(log_message){
		timer += get_micro_time();
		log_message("Finished computing downward arcs, needed "+std::to_string(timer)+"musec");
	}

	if(log_message){
		log_message("Start computing mapping from CCH arcs to input arcs");
		timer = -get_micro_time();
	}

	// Compute ch mapping -> input
	{
		does_cch_arc_have_input_arc.resize(cch_arc_count, BitVector::uninitialized);
		does_cch_arc_have_input_arc.reset_all();

		for(unsigned i=0; i<input_arc_count; ++i)
			if(input_arc_to_cch_arc[i] != invalid_id)
				does_cch_arc_have_input_arc.set(input_arc_to_cch_arc[i]);

		does_cch_arc_have_input_arc_mapper = LocalIDMapper(does_cch_arc_have_input_arc);

		forward_input_arc_of_cch.resize(does_cch_arc_have_input_arc_mapper.local_id_count());
		backward_input_arc_of_cch.resize(does_cch_arc_have_input_arc_mapper.local_id_count());

		std::fill(forward_input_arc_of_cch.begin(), forward_input_arc_of_cch.end(), invalid_id);
		std::fill(backward_input_arc_of_cch.begin(), backward_input_arc_of_cch.end(), invalid_id);

		does_cch_arc_have_extra_input_arc.resize(cch_arc_count, BitVector::uninitialized);
		does_cch_arc_have_extra_input_arc.reset_all();

		for(unsigned input_arc=0; input_arc<input_arc_count; ++input_arc){
			unsigned cch_arc = input_arc_to_cch_arc[input_arc];
			if(cch_arc != invalid_id){
				unsigned i = does_cch_arc_have_input_arc_mapper.to_local(cch_arc);
				if(is_input_arc_upward.is_set(input_arc)){
					if(forward_input_arc_of_cch[i] == invalid_id){
						forward_input_arc_of_cch[i] = input_arc;
					}else{
						does_cch_arc_have_extra_input_arc.set(cch_arc);

						first_extra_forward_input_arc_of_cch.push_back(cch_arc);
						extra_forward_input_arc_of_cch.push_back(input_arc);
					}
				}else{
					if(backward_input_arc_of_cch[i] == invalid_id){
						backward_input_arc_of_cch[i] = input_arc;
					}else{
						does_cch_arc_have_extra_input_arc.set(cch_arc);

						first_extra_backward_input_arc_of_cch.push_back(cch_arc);
						extra_backward_input_arc_of_cch.push_back(input_arc);
					}
				}
			}
		}

		does_cch_arc_have_extra_input_arc_mapper = LocalIDMapper(does_cch_arc_have_extra_input_arc);

		for(auto&x:first_extra_forward_input_arc_of_cch)
			x = does_cch_arc_have_extra_input_arc_mapper.to_local(x);
		for(auto&x:first_extra_backward_input_arc_of_cch)
			x = does_cch_arc_have_extra_input_arc_mapper.to_local(x);

		{
			auto p = compute_inverse_stable_sort_permutation_using_key(
				first_extra_forward_input_arc_of_cch,
				does_cch_arc_have_extra_input_arc_mapper.local_id_count(),
				[](unsigned x){return x;}
			);
			first_extra_forward_input_arc_of_cch =
				invert_vector(
					apply_inverse_permutation(p, std::move(first_extra_forward_input_arc_of_cch)),
					does_cch_arc_have_extra_input_arc_mapper.local_id_count()
				)
			;
			extra_forward_input_arc_of_cch = apply_inverse_permutation(p, std::move(extra_forward_input_arc_of_cch));
		}


		{
			auto p = compute_inverse_stable_sort_permutation_using_key(
				first_extra_backward_input_arc_of_cch,
				does_cch_arc_have_extra_input_arc_mapper.local_id_count(),
				[](unsigned x){return x;}
			);
			first_extra_backward_input_arc_of_cch =
				invert_vector(
					apply_inverse_permutation(p, std::move(first_extra_backward_input_arc_of_cch)),
					does_cch_arc_have_extra_input_arc_mapper.local_id_count()
				)
			;
			extra_backward_input_arc_of_cch = apply_inverse_permutation(p, std::move(extra_backward_input_arc_of_cch));
		}

	}

	if(log_message){
		timer += get_micro_time();
		log_message("Finished computing mapping, needed "+std::to_string(timer)+"musec");
		log_message(std::to_string(does_cch_arc_have_input_arc_mapper.local_id_count())+" cch arcs have an input arc");
		log_message(std::to_string(does_cch_arc_have_extra_input_arc_mapper.local_id_count())+" cch arcs have two or more input arcs");
	}


//	for(unsigned a=0; a<cch_arc_count; ++a)
//		forall_upper_triangles_of_arc(*this, a, TriangleVerifier(*this));
//	for(unsigned a=0; a<cch_arc_count; ++a)
//		forall_intermediate_triangles_of_arc(*this, a, TriangleVerifier(*this));
//	for(unsigned a=0; a<cch_arc_count; ++a)
//		forall_lower_triangles_of_arc(*this, a, TriangleVerifier(*this));
}

namespace{

	void extract_initial_metric_of_cch_arc(const CustomizableContractionHierarchy&cch, CustomizableContractionHierarchyMetric&metric, unsigned cch_arc){
		if(__builtin_expect(!cch.does_cch_arc_have_input_arc.is_set(cch_arc), true)){
			metric.forward[cch_arc] = inf_weight;
			metric.backward[cch_arc] = inf_weight;
		} else {
			{
				unsigned i = cch.does_cch_arc_have_input_arc_mapper.to_local(cch_arc);
				if(cch.forward_input_arc_of_cch[i] != invalid_id)
					metric.forward[cch_arc] = metric.input_weight[cch.forward_input_arc_of_cch[i]];
				else
					metric.forward[cch_arc] = inf_weight;

				if(cch.backward_input_arc_of_cch[i] != invalid_id)
					metric.backward[cch_arc] = metric.input_weight[cch.backward_input_arc_of_cch[i]];
				else
					metric.backward[cch_arc] = inf_weight;
			}
			if(cch.does_cch_arc_have_extra_input_arc.is_set(cch_arc)){
				unsigned i = cch.does_cch_arc_have_extra_input_arc_mapper.to_local(cch_arc);
				for(unsigned j=cch.first_extra_forward_input_arc_of_cch[i]; j<cch.first_extra_forward_input_arc_of_cch[i+1]; ++j)
					min_to(metric.forward[cch_arc], metric.input_weight[cch.extra_forward_input_arc_of_cch[j]]);
				for(unsigned j=cch.first_extra_backward_input_arc_of_cch[i]; j<cch.first_extra_backward_input_arc_of_cch[i+1]; ++j)
					min_to(metric.backward[cch_arc], metric.input_weight[cch.extra_backward_input_arc_of_cch[j]]);
			}
		}
	}

	void extract_initial_metric(const CustomizableContractionHierarchy&cch, CustomizableContractionHierarchyMetric&metric){
		for(unsigned cch_arc=0; cch_arc<cch.cch_arc_count(); ++cch_arc){
			extract_initial_metric_of_cch_arc(cch, metric, cch_arc);
		}
	}

	struct LowerTriangleRelaxer{

		LowerTriangleRelaxer(){}
		explicit LowerTriangleRelaxer(CustomizableContractionHierarchyMetric&metric):metric(&metric){}

		bool operator()(
			unsigned bottom_arc, unsigned mid_arc, unsigned top_arc,
			unsigned bottom_node, unsigned mid_node, unsigned top_node
		) const {
			(void)bottom_node;
			(void)mid_node;
			(void)top_node;
			min_to(metric->forward[top_arc],  metric->backward[bottom_arc] + metric->forward[mid_arc] );
			min_to(metric->backward[top_arc], metric->forward[bottom_arc]  + metric->backward[mid_arc]);
			return true;
		}

		CustomizableContractionHierarchyMetric*metric;
	};

	#ifndef NDEBUG
	struct LowerTriangleInequalityVerifier{

		LowerTriangleInequalityVerifier(){}
		explicit LowerTriangleInequalityVerifier(CustomizableContractionHierarchyMetric&metric):metric(&metric){}

		bool operator()(
			unsigned bottom_arc, unsigned mid_arc, unsigned top_arc,
			unsigned bottom_node, unsigned mid_node, unsigned top_node
		) const {
			(void)bottom_node;
			(void)mid_node;
			(void)top_node;
			assert(metric->forward[top_arc] <= metric->backward[bottom_arc] + metric->forward[mid_arc]);
			assert(metric->backward[top_arc] <= metric->forward[bottom_arc] + metric->backward[mid_arc]);
			return true;
		}

		CustomizableContractionHierarchyMetric*metric;
	};
	#endif
}

CustomizableContractionHierarchyMetric::CustomizableContractionHierarchyMetric(const CustomizableContractionHierarchy&cch, const std::vector<unsigned>&input_weight):
	forward(cch.cch_arc_count()), backward(cch.cch_arc_count()), cch(&cch), input_weight(&input_weight[0]){
}

CustomizableContractionHierarchyMetric::CustomizableContractionHierarchyMetric(const CustomizableContractionHierarchy&cch, const unsigned*input_weight):
	forward(cch.cch_arc_count()), backward(cch.cch_arc_count()), cch(&cch), input_weight(input_weight){
}

CustomizableContractionHierarchyMetric& CustomizableContractionHierarchyMetric::reset(const CustomizableContractionHierarchy&cch, const std::vector<unsigned>&input_weight){
	assert(input_weight.size() == cch.input_arc_count() && "Input weight vector has the wrong size");
	reset(cch, &input_weight[0]);
	return *this;
}

CustomizableContractionHierarchyMetric& CustomizableContractionHierarchyMetric::reset(const std::vector<unsigned>&input_weight){
	assert(cch && "Need to be attached to a CCH");
	assert(input_weight.size() == cch->input_arc_count() && "Input weight vector has the wrong size");
	reset(&input_weight[0]);
	return *this;
}

CustomizableContractionHierarchyMetric& CustomizableContractionHierarchyMetric::reset(const CustomizableContractionHierarchy&cch_, const unsigned*input_weight_){
	assert(input_weight_ != nullptr && "Input weight pointer must not be null");
	if(cch_.cch_arc_count() != forward.size()){
		*this = CustomizableContractionHierarchyMetric(cch_, input_weight_);
	}else{
		cch = &cch_;
		input_weight = input_weight_;
	}
	return *this;
}

CustomizableContractionHierarchyMetric& CustomizableContractionHierarchyMetric::reset(const unsigned*input_weight_){
	assert(input_weight_ != nullptr && "Input weight pointer must not be null");
	input_weight = input_weight_;
	return *this;
}

CustomizableContractionHierarchyMetric& CustomizableContractionHierarchyMetric::customize(){
	assert(input_weight != nullptr && "Metric must be connected to a weight vector");

	extract_initial_metric(*cch, *this);

	std::vector<unsigned> arc_id_cache(cch->node_count());

	for(unsigned x=0; x<cch->node_count(); ++x){
		const unsigned xz_up_end = cch->up_first_out[x+1];
		for(unsigned xz_up = cch->up_first_out[x]; xz_up < xz_up_end; ++xz_up){
			arc_id_cache[cch->up_head[xz_up]] = xz_up;
		}

		const unsigned xy_down_end = cch->down_first_out[x+1];
		for(unsigned xy_down = cch->down_first_out[x]; xy_down < xy_down_end; ++xy_down){
			const unsigned yx_up = cch->down_to_up[xy_down];
			const unsigned y = cch->down_head[xy_down];
			const unsigned yz_up_end_reversed = cch->up_first_out[y];
			for(unsigned yz_up_reversed = cch->up_first_out[y+1]; yz_up_reversed > yz_up_end_reversed; --yz_up_reversed){
				const unsigned yz_up = yz_up_reversed-1;
				const unsigned z = cch->up_head[yz_up];
				if (z <= x) { break; }
				LowerTriangleRelaxer(*this)(yx_up, yz_up, arc_id_cache[z], y, x, z);
			}
		}
	}

	#ifndef NDEBUG
	for(unsigned a=0; a<cch->cch_arc_count(); ++a)
		forall_upper_triangles_of_arc(*cch, a, LowerTriangleInequalityVerifier(*this));
	#endif
	return *this;
}

namespace{
	template<class T>
	void atomic_min_to(T&x, const T&y){
		 T z = x;
		 while(y < z && !__sync_bool_compare_and_swap(&x, z, y))
		         z = x;
	}

	struct AtomicLowerTriangleRelaxer{

		AtomicLowerTriangleRelaxer(){}
		explicit AtomicLowerTriangleRelaxer(CustomizableContractionHierarchyMetric&metric):metric(&metric){}

		bool operator()(
			unsigned bottom_arc, unsigned mid_arc, unsigned top_arc,
			unsigned bottom_node, unsigned mid_node, unsigned top_node
		) const {
			(void)bottom_node;
			(void)mid_node;
			(void)top_node;
			atomic_min_to(metric->forward[top_arc],  metric->backward[bottom_arc] + metric->forward[mid_arc] );
			atomic_min_to(metric->backward[top_arc], metric->forward[bottom_arc]  + metric->backward[mid_arc]);
			return true;
		}

		CustomizableContractionHierarchyMetric*metric;
	};
}

CustomizableContractionHierarchyParallelization::CustomizableContractionHierarchyParallelization(const CustomizableContractionHierarchy&cch){
	const unsigned node_count = cch.node_count();

	std::vector<unsigned>node_level(node_count);
	unsigned level_count;
	{
		std::vector<unsigned>lock(node_count);
		std::vector<unsigned>zero_lock_list(node_count);
		unsigned zero_lock_count = 0;
		for(unsigned x=0; x<node_count; ++x){
			lock[x] = cch.down_first_out[x+1] - cch.down_first_out[x];
			if(lock[x] == 0){
				zero_lock_list[zero_lock_count] = x;
				++zero_lock_count;
			}
		}

		level_count = 0;
		std::vector<unsigned>next_zero_lock_list(node_count);
		while(zero_lock_count != 0){
			unsigned next_zero_lock_count = 0;
			for(unsigned i=0; i<zero_lock_count; ++i){
				unsigned x = zero_lock_list[i];
				node_level[x] = level_count;
				for(unsigned xy=cch.up_first_out[x]; xy < cch.up_first_out[x+1]; ++xy){
					unsigned y = cch.up_head[xy];
					--lock[y];
					if(lock[y] == 0){
						next_zero_lock_list[next_zero_lock_count] = y;
						++next_zero_lock_count;
					}
				}
			}
			++level_count;
			std::swap(next_zero_lock_list, zero_lock_list);
			zero_lock_count = next_zero_lock_count;
		}
	}

	const unsigned arc_count = cch.cch_arc_count();

	std::vector<unsigned>arc_level(arc_count);
	arcs_ordered_by_level.resize(arc_count);

	auto p = compute_stable_sort_permutation_using_key(node_level, level_count, [&](unsigned x){ return x; });

	unsigned j = 0;
	for(unsigned i=0; i<node_count; ++i){
		unsigned x = p[i];
		for(unsigned xy=cch.up_first_out[x]; xy<cch.up_first_out[x+1]; ++xy){
			arc_level[j] = node_level[x];
			arcs_ordered_by_level[j] = xy;
			++j;
		}
	}

	first_arc_of_level = invert_vector(arc_level, level_count);

	this->cch = &cch;
}


CustomizableContractionHierarchyParallelization& CustomizableContractionHierarchyParallelization::customize(CustomizableContractionHierarchyMetric&metric) {
	// If OpenMP is enabled, it is used to parallelize the code. If OpenMP is disabled, the code will compile but will run sequentially.
	#ifdef _OPENMP
	customize(metric, omp_get_num_procs());
	#else
	customize(metric, 1);
	#endif
	return *this;
}

CustomizableContractionHierarchyParallelization& CustomizableContractionHierarchyParallelization::customize(CustomizableContractionHierarchyMetric&metric, unsigned thread_count) {
	assert(cch == metric.cch);
	assert(thread_count != 0);
	assert(metric.input_weight != nullptr && "Metric must be connected to a weight vector");

	if(thread_count == 1){
		metric.customize();
	} else {
		#ifdef _OPENMP
		#pragma omp parallel num_threads(thread_count)
		#endif
		{
			#ifdef _OPENMP
			#pragma omp for
			#endif
			for(unsigned cch_arc=0; cch_arc<cch->cch_arc_count(); ++cch_arc){
				extract_initial_metric_of_cch_arc(*cch, metric, cch_arc);
			}

			for(unsigned l=0; l<first_arc_of_level.size()-1; ++l){
				#ifdef _OPENMP
				#pragma omp for schedule(dynamic,256)
				#endif
				for(unsigned i=first_arc_of_level[l]; i<first_arc_of_level[l+1]; ++i){
					forall_upper_triangles_of_arc(*cch, arcs_ordered_by_level[i], AtomicLowerTriangleRelaxer(metric));
				}
			}
		}



		#ifndef NDEBUG
		for(unsigned a=0; a<cch->cch_arc_count(); ++a)
			forall_upper_triangles_of_arc(*cch, a, LowerTriangleInequalityVerifier(metric));
		#endif
	}
	return *this;
}

CustomizableContractionHierarchyPartialCustomization::CustomizableContractionHierarchyPartialCustomization(const CustomizableContractionHierarchy&cch_):
	q(cch_.cch_arc_count()),
	cch(&cch_){
}

CustomizableContractionHierarchyPartialCustomization&CustomizableContractionHierarchyPartialCustomization::reset(){
	q.clear();
	return *this;
}

CustomizableContractionHierarchyPartialCustomization&CustomizableContractionHierarchyPartialCustomization::reset(const CustomizableContractionHierarchy&cch_){
	if(cch_.cch_arc_count() != q.id_count()){
		*this = CustomizableContractionHierarchyPartialCustomization(cch_);
	} else {
		q.clear();
		cch = &cch_;
	}
	return *this;
}

CustomizableContractionHierarchyPartialCustomization&CustomizableContractionHierarchyPartialCustomization::update_arc(unsigned xy){
	xy = cch->input_arc_to_cch_arc[xy];
	if(xy != invalid_id)
		q.push(xy);
	return *this;
}

CustomizableContractionHierarchyPartialCustomization&CustomizableContractionHierarchyPartialCustomization::customize(CustomizableContractionHierarchyMetric&metric){
	assert(cch == metric.cch);
	assert(metric.input_weight != nullptr && "Metric must be connected to a weight vector");

	while(!q.empty()){
		unsigned xy = q.pop();

		unsigned old_forward = metric.forward[xy];
		unsigned old_backward = metric.backward[xy];

		metric.forward[xy] = inf_weight;
		metric.backward[xy] = inf_weight;

		extract_initial_metric_of_cch_arc(*cch, metric, xy);

		forall_lower_triangles_of_arc(
			*cch, xy,
			LowerTriangleRelaxer(metric)
		);

		unsigned new_forward = metric.forward[xy];
		unsigned new_backward = metric.backward[xy];

		if(old_forward != new_forward || old_backward != new_backward){
			forall_intermediate_triangles_of_arc(
				*cch, xy,
				[&](
					unsigned bottom_arc, unsigned mid_arc, unsigned top_arc,
					unsigned bottom_node, unsigned mid_node, unsigned top_node
				){
					assert(mid_arc == xy);
					if(
						metric.backward[bottom_arc] + old_forward == metric.forward[top_arc] ||
						metric.forward[bottom_arc] + old_backward == metric.backward[top_arc] ||
						metric.backward[bottom_arc] + new_forward < metric.forward[top_arc] ||
						metric.forward[bottom_arc] + new_backward < metric.backward[top_arc]
					){
						q.push(top_arc);
					}
					return true;
				}
			);
			forall_upper_triangles_of_arc(
				*cch, xy,
				[&](
					unsigned bottom_arc, unsigned mid_arc, unsigned top_arc,
					unsigned bottom_node, unsigned mid_node, unsigned top_node
				){
					assert(bottom_arc == xy);
					if(
						metric.forward[mid_arc] + old_backward == metric.forward[top_arc] ||
						metric.backward[mid_arc] + old_forward == metric.backward[top_arc] ||
						metric.forward[mid_arc] + new_backward < metric.forward[top_arc] ||
						metric.backward[mid_arc] + new_forward < metric.backward[top_arc]
					){
						q.push(top_arc);
					}
					return true;
				}
			);
		}
	}
	#ifndef NDEBUG
	for(unsigned a=0; a<cch->cch_arc_count(); ++a)
		forall_upper_triangles_of_arc(*cch, a, LowerTriangleInequalityVerifier(metric));
	#endif
	return *this;
}

namespace{
	const unsigned query_state_initialized = 0;
	const unsigned query_state_run = 1;
	const unsigned query_state_source_pinned = 2;
	const unsigned query_state_source_run = 3;
	const unsigned query_state_target_pinned = 4;
	const unsigned query_state_target_run = 5;
}

namespace{

	template<class F>
	void forall_ancestors(const std::vector<unsigned>&parent, unsigned x, unsigned stop_at, const F&f){
		assert(x < parent.size());
		assert(stop_at < parent.size() || stop_at == invalid_id);

		while(x != stop_at){
			assert(x < parent.size() && "stop_at is not an ancestor of x");
			if(!f(x))
				return;
			assert(parent[x] > x);
			x = parent[x];
		}
	}

	template<class F>
	void forall_ancestors(const std::vector<unsigned>&parent, unsigned x, const F&f){
		forall_ancestors(parent, x, invalid_id, f);
	}


	void reset_source_list(
		const std::vector<unsigned>&elimination_tree_parent,
		std::vector<unsigned>&source_node, std::vector<unsigned>&source_elimination_tree_end,
		std::vector<bool>&in_forward_search_space, std::vector<unsigned>&forward_tentative_distance
	){
		for(unsigned i=0; i<source_node.size(); ++i){
			forall_ancestors(
				elimination_tree_parent,
				source_node[i], source_elimination_tree_end[i],
				[&](unsigned x){
					in_forward_search_space[x] = false;
					forward_tentative_distance[x] = inf_weight;
					return true;
				}
			);
		}
		source_node.clear();
		source_elimination_tree_end.clear();
	}
}

CustomizableContractionHierarchyQuery::CustomizableContractionHierarchyQuery(const CustomizableContractionHierarchyMetric&metric):
	forward_tentative_distance(metric.cch->node_count(), inf_weight),
	backward_tentative_distance(metric.cch->node_count(), inf_weight),
	forward_predecessor_node(metric.cch->node_count()),
	backward_predecessor_node(metric.cch->node_count()),
	in_forward_search_space(metric.cch->node_count(), false),
	in_backward_search_space(metric.cch->node_count(), false),
	shortest_path_meeting_node(invalid_id),
	cch(metric.cch),
	metric(&metric),
	state(query_state_initialized){}

namespace{
	void internal_add_source(
		unsigned external_s, unsigned dist_to_s,
		const std::vector<unsigned>&rank,
		const std::vector<unsigned>&elimination_tree_parent,
		std::vector<unsigned>&forward_tentative_distance,
		std::vector<unsigned>&forward_predecessor_node,
		std::vector<bool>&in_forward_search_space,
		std::vector<unsigned>&source_node,
		std::vector<unsigned>&source_elimination_tree_end
	){
		unsigned s = rank[external_s];

		if(forward_tentative_distance[s] == inf_weight){

			source_node.push_back(s);
			forward_tentative_distance[s] = dist_to_s;
			forward_predecessor_node[s] = invalid_id;

			forall_ancestors(
				elimination_tree_parent, s,
				[&](unsigned x){
					if(!in_forward_search_space[x]){
						in_forward_search_space[x] = true;
						return true;
					} else {
						source_elimination_tree_end.push_back(x);
						return false;
					}
				}
			);
			if(source_elimination_tree_end.size() != source_node.size())
				source_elimination_tree_end.push_back(invalid_id);
		}else{
			min_to(forward_tentative_distance[s], dist_to_s);
		}
	}
}




CustomizableContractionHierarchyQuery& CustomizableContractionHierarchyQuery::add_source(unsigned external_s, unsigned dist_to_s){
	assert(external_s < cch->node_count());
	assert(state == query_state_initialized || state == query_state_target_pinned);
	internal_add_source(
		external_s, dist_to_s,
		cch->rank,
		cch->elimination_tree_parent,
		forward_tentative_distance,
		forward_predecessor_node,
		in_forward_search_space,
		source_node,
		source_elimination_tree_end
	);
	return *this;
}

CustomizableContractionHierarchyQuery& CustomizableContractionHierarchyQuery::add_target(unsigned external_t, unsigned dist_to_t){
	assert(external_t < cch->node_count());
	assert(state == query_state_initialized || state == query_state_source_pinned);
	internal_add_source(
		external_t, dist_to_t,
		cch->rank,
		cch->elimination_tree_parent,
		backward_tentative_distance,
		backward_predecessor_node,
		in_backward_search_space,
		target_node,
		target_elimination_tree_end
	);
	return *this;
}

namespace{
	template<class SetPred>
	void relax_outgoing_arcs(
		const std::vector<unsigned>&first_out,
		const std::vector<unsigned>&head,
		const std::vector<unsigned>&weight,
		std::vector<unsigned>&tentative_distance,
		const SetPred&set_node_predecessor,
		unsigned x
	){

		for(unsigned xy=first_out[x]; xy<first_out[x+1]; ++xy){
			unsigned y=head[xy];
			if(tentative_distance[x] + weight[xy] < tentative_distance[y]){
				tentative_distance[y] = tentative_distance[x] + weight[xy];
				set_node_predecessor(y, x);
			}
		}
	}

	template<class SetPred>
	void relax_incoming_arcs(
		const std::vector<unsigned>&first_out,
		const std::vector<unsigned>&head,
		const std::vector<unsigned>&weight,
		std::vector<unsigned>&tentative_distance,
		const SetPred&set_node_predecessor,
		unsigned x
	){
		for(unsigned xy=first_out[x]; xy<first_out[x+1]; ++xy){
			unsigned y=head[xy];
			if(tentative_distance[y] + weight[xy] < tentative_distance[x]){
				tentative_distance[x] = tentative_distance[y] + weight[xy];
				set_node_predecessor(x, y);
			}
		}
	}
}


CustomizableContractionHierarchyQuery& CustomizableContractionHierarchyQuery::run(){
	assert(state == query_state_initialized);

	for(unsigned i = source_node.size()-1; i!=(unsigned)-1; --i){
		forall_ancestors(
			cch->elimination_tree_parent,
			source_node[i], source_elimination_tree_end[i],
			[&](unsigned x){

				relax_outgoing_arcs(
					cch->up_first_out, cch->up_head, metric->forward,
					forward_tentative_distance, [&](unsigned a, unsigned b){forward_predecessor_node[a] = b;},
					x
				);
				return true;
			}
		);
	}

//	for(unsigned x=0; x<cch->node_count(); ++x){
//		for(unsigned xy=cch->up_first_out[x]; xy < cch->up_first_out[x+1]; ++xy){
//			unsigned y = cch->up_head[xy];
//			assert(forward_tentative_distance[y] <= forward_tentative_distance[x] + metric->forward[xy] && "not all forward arcs relaxed");
//		}
//	}

	shortest_path_meeting_node = invalid_id;
	unsigned shortest_path_length = inf_weight;

	for(unsigned i = target_node.size()-1; i!=(unsigned)-1; --i){
		forall_ancestors(
			cch->elimination_tree_parent,
			target_node[i], target_elimination_tree_end[i],
			[&](unsigned x){
				relax_outgoing_arcs(
					cch->up_first_out, cch->up_head, metric->backward,
					backward_tentative_distance, [&](unsigned a, unsigned b){backward_predecessor_node[a] = b;},
					x
				);
				if(in_forward_search_space[x]){
					unsigned l = forward_tentative_distance[x] + backward_tentative_distance[x];
					if(l < shortest_path_length){
						shortest_path_length = l;
						shortest_path_meeting_node = x;
					}
				}
				return true;
			}
		);
	}

//	for(unsigned x=0; x < cch->node_count(); ++x){
//		unsigned l = forward_tentative_distance[x] + backward_tentative_distance[x];
//		assert(l >= shortest_path_length);
//	}

	state = query_state_run;
	return *this;
}

unsigned CustomizableContractionHierarchyQuery::get_distance(){
	assert(state == query_state_run);
	if(shortest_path_meeting_node == invalid_id)
		return inf_weight;
	else
		return forward_tentative_distance[shortest_path_meeting_node] + backward_tentative_distance[shortest_path_meeting_node];
}

unsigned CustomizableContractionHierarchyQuery::get_used_source(){
	assert(state == query_state_run);
	if(shortest_path_meeting_node == invalid_id) {
		return invalid_id;
	} else {
		unsigned x = shortest_path_meeting_node;
		while(forward_predecessor_node[x] != invalid_id){
			x = forward_predecessor_node[x];
		}
		return cch->order[x];
	}
}

unsigned CustomizableContractionHierarchyQuery::get_used_target(){
	assert(state == query_state_run);
	if(shortest_path_meeting_node == invalid_id) {
		return invalid_id;
	} else {
		unsigned x = shortest_path_meeting_node;
		while(backward_predecessor_node[x] != invalid_id){
			x = backward_predecessor_node[x];
		}
		return cch->order[x];
	}
}

namespace{
	struct TriangleUnpacker{
		TriangleUnpacker(){}
		TriangleUnpacker(unsigned&bottom_node, unsigned&bottom_arc, unsigned&mid_arc):
			bottom_node_(&bottom_node), bottom_arc_(&bottom_arc), mid_arc_(&mid_arc){}
		bool operator()(
			unsigned bottom_arc,  unsigned mid_arc,  unsigned top_arc,
			unsigned bottom_node, unsigned mid_node, unsigned top_node
		) const {
			(void)top_node; (void)mid_node;
			if((*forward)[top_arc] == (*backward)[bottom_arc] + (*forward)[mid_arc]){
				*bottom_node_ = bottom_node;
				*bottom_arc_ = bottom_arc;
				*mid_arc_ = mid_arc;

				return false;
			}else{
				return true;
			}
		}

		const std::vector<unsigned>*forward;
		const std::vector<unsigned>*backward;

		unsigned*bottom_node_, *bottom_arc_, *mid_arc_;
	};

	template<class OnNewSegment>
	void unpack_arc(
		const CustomizableContractionHierarchy&cch, const CustomizableContractionHierarchyMetric&metric,
		bool is_forward,
		unsigned x, unsigned y, unsigned xy,
		const OnNewSegment&on_new_segment
	){
		assert(x == cch.up_tail[xy]);
		assert(y == cch.up_head[xy]);

		unsigned
			bottom_node = invalid_id,
			bottom_arc,
			mid_arc;

		auto unpacker = [&](
			unsigned bottom_arc_,  unsigned mid_arc_,  unsigned top_arc_,
			unsigned bottom_node_, unsigned mid_node_, unsigned top_node_
		){
			(void)top_node_; (void)mid_node_;

			bool fits;
			if(is_forward)
				fits = metric.forward[top_arc_] == metric.backward[bottom_arc_] + metric.forward[mid_arc_];
			else
				fits = metric.backward[top_arc_] == metric.forward[bottom_arc_] + metric.backward[mid_arc_];
			if(fits){
				bottom_node = bottom_node_;
				bottom_arc = bottom_arc_;
				mid_arc = mid_arc_;
				return false;
			}else{
				return true;
			}
		};
		forall_lower_triangles_of_arc(cch, x, y, xy, unpacker);
		if(bottom_node == invalid_id){
			if(is_forward)
				on_new_segment(x, xy, true);
			else
				on_new_segment(y, xy, false);
		} else {
			if(is_forward){
				unpack_arc(cch, metric, false, bottom_node, x, bottom_arc, on_new_segment);
				unpack_arc(cch, metric, true, bottom_node, y, mid_arc, on_new_segment);
			}else{
				unpack_arc(cch, metric, false, bottom_node, y, mid_arc, on_new_segment);
				unpack_arc(cch, metric, true, bottom_node, x, bottom_arc, on_new_segment);
			}
		}
	}

	template<class OnNewSegment>
	unsigned unpack_shortest_path(
		const CustomizableContractionHierarchy&cch, const CustomizableContractionHierarchyMetric&metric, const CustomizableContractionHierarchyQuery&query,
		const OnNewSegment&on_new_segment
	){
		if(query.shortest_path_meeting_node == invalid_id)
			return invalid_id;

		{
			std::vector<unsigned>up_path = {query.shortest_path_meeting_node};

			unsigned x = query.shortest_path_meeting_node;
			while(query.forward_predecessor_node[x] != invalid_id){
				x = query.forward_predecessor_node[x];
				up_path.push_back(x);
			}

			for(unsigned i=up_path.size()-1; i!=0; --i){
				unpack_arc(
					cch, metric,
					true,
					up_path[i], up_path[i-1], find_arc_given_sorted_head(cch.up_first_out, cch.up_head, up_path[i], up_path[i-1]),
					on_new_segment
				);
			}
		}
		{
			unsigned x = query.shortest_path_meeting_node;
			unsigned y = query.backward_predecessor_node[x];
			while(y != invalid_id){
				unpack_arc(
					cch, metric,
					false,
					y, x, find_arc_given_sorted_head(cch.up_first_out, cch.up_head, y, x),
					on_new_segment
				);
				x = y;
				y = query.backward_predecessor_node[y];
			}
			return x;
		}
	}
}

std::vector<unsigned>CustomizableContractionHierarchyQuery::get_node_path(){
	assert(state == query_state_run);
	std::vector<unsigned>path;
	unsigned last = unpack_shortest_path(
		*cch, *metric, *this,
		[&](unsigned cch_node, unsigned cch_arc, bool forward){
			path.push_back(cch->order[cch_node]);
			(void)forward;
			(void)cch_arc;
		}
	);
	if(last != invalid_id)
		path.push_back(cch->order[last]);
	return path; // NVRO
}

namespace{
	// unpack_original_*_arc returns the input arc id corresponding to a cch arc if such an arc
	// exists and has the same length as the cch_arc

	unsigned unpack_original_forward_arc(
		const CustomizableContractionHierarchy&cch, const CustomizableContractionHierarchyMetric&metric,
		unsigned cch_arc
	){
		if(cch.does_cch_arc_have_input_arc.is_set(cch_arc)){
			unsigned i = cch.does_cch_arc_have_input_arc_mapper.to_local(cch_arc);
			if(cch.forward_input_arc_of_cch[i] != invalid_id){
				unsigned original_arc = cch.forward_input_arc_of_cch[i];
				if(metric.forward[cch_arc] == metric.input_weight[original_arc])
					return original_arc;

				if(cch.does_cch_arc_have_extra_input_arc.is_set(cch_arc)){
					unsigned j = cch.does_cch_arc_have_extra_input_arc_mapper.to_local(cch_arc);
					for(unsigned k = cch.first_extra_forward_input_arc_of_cch[j]; k < cch.first_extra_forward_input_arc_of_cch[j+1]; ++k){
						unsigned original_arc = cch.extra_forward_input_arc_of_cch[k];
						if(metric.forward[cch_arc] == metric.input_weight[original_arc])
							return original_arc;
					}
				}
			}
		}
		return invalid_id;
	}

	unsigned unpack_original_backward_arc(
		const CustomizableContractionHierarchy&cch, const CustomizableContractionHierarchyMetric&metric,
		unsigned cch_arc
	){
		if(cch.does_cch_arc_have_input_arc.is_set(cch_arc)){
			unsigned i = cch.does_cch_arc_have_input_arc_mapper.to_local(cch_arc);
			if(cch.backward_input_arc_of_cch[i] != invalid_id){
				unsigned original_arc = cch.backward_input_arc_of_cch[i];
				if(metric.backward[cch_arc] == metric.input_weight[original_arc])
					return original_arc;
				if(cch.does_cch_arc_have_extra_input_arc.is_set(cch_arc)){
					unsigned j = cch.does_cch_arc_have_extra_input_arc_mapper.to_local(cch_arc);
					for(unsigned k = cch.first_extra_backward_input_arc_of_cch[j]; k < cch.first_extra_backward_input_arc_of_cch[j+1]; ++k){
						unsigned original_arc = cch.extra_backward_input_arc_of_cch[k];
						if(metric.backward[cch_arc] == metric.input_weight[original_arc])
							return original_arc;
					}
				}
			}
		}
		return invalid_id;
	}


	unsigned unpack_original_arc(
		const CustomizableContractionHierarchy&cch, const CustomizableContractionHierarchyMetric&metric,
		unsigned cch_arc, bool is_forward
	){
		if(is_forward)
			return unpack_original_forward_arc(cch, metric, cch_arc);
		else
			return unpack_original_backward_arc(cch, metric, cch_arc);
	}
}

std::vector<unsigned>CustomizableContractionHierarchyQuery::get_arc_path(){
	assert(state == query_state_run);
	std::vector<unsigned>path;
	unpack_shortest_path(
		*cch, *metric, *this,
		[&](unsigned cch_node, unsigned cch_arc, bool is_forward){

			if(is_forward)
				assert(cch_node == cch->up_tail[cch_arc]);
			else
				assert(cch_node == cch->up_head[cch_arc]);

			(void)cch_node;

			unsigned arc = unpack_original_arc(*cch, *metric, cch_arc, is_forward);
			assert(arc != invalid_id);
			path.push_back(arc);
		}
	);
	return path; // NVRO
}

namespace{
	void internal_pin_targets(
		std::vector<unsigned>&target_node,
		std::vector<unsigned>&target_elimination_tree_end,
		std::vector<bool>&in_backward_search_space,
		const std::vector<unsigned>&elimination_tree_parent,
		const std::vector<unsigned>&rank,
		const std::vector<unsigned>&target_list
	){
		target_node.resize(target_list.size());
		target_elimination_tree_end.resize(target_list.size());

		for(unsigned i=0; i<target_list.size(); ++i){
			target_node[i] = rank[target_list[i]];
			target_elimination_tree_end[i] = invalid_id;
			forall_ancestors(
				elimination_tree_parent, target_node[i],
				[&](unsigned x){
					if(!in_backward_search_space[x]){
						in_backward_search_space[x] = true;
						return true;
					} else {
						target_elimination_tree_end[i] = x;
						return false;
					}
				}
			);
		}
	}
}

CustomizableContractionHierarchyQuery& CustomizableContractionHierarchyQuery::pin_targets(const std::vector<unsigned>&target_list){
	assert(state == query_state_initialized);
	internal_pin_targets(target_node, target_elimination_tree_end, in_backward_search_space, cch->elimination_tree_parent, cch->rank, target_list);
	state = query_state_target_pinned;
	return *this;
}

CustomizableContractionHierarchyQuery& CustomizableContractionHierarchyQuery::pin_sources(const std::vector<unsigned>&source_list){
	assert(state == query_state_initialized);
	internal_pin_targets(source_node, source_elimination_tree_end, in_forward_search_space, cch->elimination_tree_parent, cch->rank, source_list);
	state = query_state_source_pinned;
	return *this;
}

namespace{
	void reset_target_distances(
		const std::vector<unsigned>&elimination_tree_parent,
		const std::vector<unsigned>&target_node,
		const std::vector<unsigned>&target_elimination_tree_end,
		std::vector<unsigned>&forward_tentative_distance
	){
		for(unsigned i = target_node.size()-1; i!=(unsigned)-1; --i){
			forall_ancestors(
				elimination_tree_parent,
				target_node[i], target_elimination_tree_end[i],
				[&](unsigned x){
					forward_tentative_distance[x] = inf_weight;
					return true;
				}
			);
		}
	}
}

CustomizableContractionHierarchyQuery& CustomizableContractionHierarchyQuery::reset_source(){
	assert(state == query_state_target_pinned || state == query_state_target_run);

	reset_source_list(cch->elimination_tree_parent, source_node, source_elimination_tree_end, in_forward_search_space, forward_tentative_distance);
	reset_target_distances(cch->elimination_tree_parent, target_node, target_elimination_tree_end, forward_tentative_distance);

	state = query_state_target_pinned;
	return *this;
}

CustomizableContractionHierarchyQuery& CustomizableContractionHierarchyQuery::reset_target(){
	assert(state == query_state_source_pinned || state == query_state_source_run);

	reset_source_list(cch->elimination_tree_parent, target_node, target_elimination_tree_end, in_backward_search_space, backward_tentative_distance);
	reset_target_distances(cch->elimination_tree_parent, source_node, source_elimination_tree_end, backward_tentative_distance);

	state = query_state_source_pinned;
	return *this;
}

namespace{
	void internal_run_to_pinned_targets(
		const CustomizableContractionHierarchy&cch,
		const std::vector<unsigned>&forward_weight, const std::vector<unsigned>&backward_weight,
		std::vector<unsigned>&forward_tentative_distance, std::vector<unsigned>&backward_tentative_distance,
		const std::vector<unsigned>&source_node, const std::vector<unsigned>&source_elimination_tree_end,
		const std::vector<unsigned>&target_node, const std::vector<unsigned>&target_elimination_tree_end
	){
		for(unsigned i = source_node.size()-1; i!=(unsigned)-1; --i){
			forall_ancestors(
				cch.elimination_tree_parent,
				source_node[i], source_elimination_tree_end[i],
				[&](unsigned x){
					relax_outgoing_arcs(
						cch.up_first_out, cch.up_head, forward_weight,
						forward_tentative_distance, [](unsigned,unsigned){},
						x
					);
					return true;
				}
			);
		}

		auto&stack = backward_tentative_distance; // backward_tentative_distance is currently not used
		unsigned stack_end = 0;
		for(unsigned i = target_node.size()-1; i!=(unsigned)-1; --i){
			forall_ancestors(
				cch.elimination_tree_parent,
				target_node[i], target_elimination_tree_end[i],
				[&](unsigned x){
					stack[stack_end++] = x;
					return true;
				}
			);
		}

		while(stack_end != 0){
			--stack_end;
			unsigned x = stack[stack_end];
			stack[stack_end] = inf_weight;
			relax_incoming_arcs(
				cch.up_first_out, cch.up_head, backward_weight,
				forward_tentative_distance, [](unsigned,unsigned){},
				x
			);
		}
	}
}

CustomizableContractionHierarchyQuery& CustomizableContractionHierarchyQuery::run_to_pinned_targets(){
	assert(state == query_state_target_pinned);
	internal_run_to_pinned_targets(
		*cch,
		metric->forward, metric->backward,
		forward_tentative_distance, backward_tentative_distance,
		source_node, source_elimination_tree_end,
		target_node, target_elimination_tree_end
	);
	state = query_state_target_run;
	return *this;
}

CustomizableContractionHierarchyQuery& CustomizableContractionHierarchyQuery::run_to_pinned_sources(){
	assert(state == query_state_source_pinned);
	internal_run_to_pinned_targets(
		*cch,
		metric->backward, metric->forward,
		backward_tentative_distance, forward_tentative_distance,
		target_node, target_elimination_tree_end,
		source_node, source_elimination_tree_end
	);
	state = query_state_source_run;
	return *this;
}

CustomizableContractionHierarchyQuery& CustomizableContractionHierarchyQuery::get_distances_to_targets(unsigned*dist){
	assert(state == query_state_target_run);
	for(unsigned i=0; i<target_node.size(); ++i)
		dist[i] = forward_tentative_distance[target_node[i]];
	return *this;
}

std::vector<unsigned> CustomizableContractionHierarchyQuery::get_distances_to_targets(){
	assert(state == query_state_target_run);
	std::vector<unsigned>v(target_node.size());
	get_distances_to_targets(&v[0]);
	return v;
}

CustomizableContractionHierarchyQuery& CustomizableContractionHierarchyQuery::get_distances_to_sources(unsigned*dist){
	assert(state == query_state_source_run);
	for(unsigned i=0; i<source_node.size(); ++i)
		dist[i] = backward_tentative_distance[source_node[i]];
	return *this;
}

std::vector<unsigned> CustomizableContractionHierarchyQuery::get_distances_to_sources(){
	assert(state == query_state_source_run);
	std::vector<unsigned>v(source_node.size());
	get_distances_to_sources(&v[0]);
	return v;
}

ContractionHierarchy CustomizableContractionHierarchyMetric::build_contraction_hierarchy_using_perfect_witness_search(){
	customize();

	BitVector
		keep_forward_arc(cch->cch_arc_count(), true),
		keep_backward_arc(cch->cch_arc_count(), true);

	for(unsigned a=0; a<cch->cch_arc_count(); ++a)
		if(forward[a] == inf_weight)
			keep_forward_arc.reset(a);

	for(unsigned a=0; a<cch->cch_arc_count(); ++a)
		if(backward[a] == inf_weight)
			keep_backward_arc.reset(a);

	for(unsigned a=cch->cch_arc_count()-1; a!=(unsigned)-1; --a){
		forall_upper_triangles_of_arc(
			*cch, a,
			[&](
				unsigned bottom_arc, unsigned mid_arc, unsigned top_arc,
				unsigned bottom_node, unsigned mid_node, unsigned top_node
			){
				if(forward[bottom_arc] > forward[mid_arc] + backward[top_arc] ){
					forward[bottom_arc] = forward[mid_arc] + backward[top_arc];
					keep_forward_arc.reset(bottom_arc);
				}

				if(backward[bottom_arc] > backward[mid_arc] + forward[top_arc]){
					backward[bottom_arc] = backward[mid_arc] + forward[top_arc];
					keep_backward_arc.reset(bottom_arc);
				}

				if(forward[mid_arc] > forward[bottom_arc] + forward[top_arc]){
					forward[mid_arc] = forward[bottom_arc] + forward[top_arc];
					keep_forward_arc.reset(mid_arc);
				}

				if(backward[mid_arc] > backward[bottom_arc] + backward[top_arc]){
					backward[mid_arc] = backward[bottom_arc] + backward[top_arc];
					keep_backward_arc.reset(mid_arc);
				}
				return true;
			}
		);
	}

	#ifndef NDEBUG
	for(unsigned a=0; a<cch->cch_arc_count(); ++a)
		forall_upper_triangles_of_arc(
			*cch, a,
			[&](
				unsigned bottom_arc, unsigned mid_arc, unsigned top_arc,
				unsigned bottom_node, unsigned mid_node, unsigned top_node
			){
				(void)bottom_node;
				(void)mid_node;
				(void)top_node;

				assert(forward[top_arc] <= backward[bottom_arc] + forward[mid_arc]);
				assert(backward[top_arc] <= forward[bottom_arc] + backward[mid_arc]);

				assert(forward[bottom_arc] <= forward[mid_arc] + backward[top_arc]);
				assert(backward[bottom_arc] <= backward[mid_arc] + forward[top_arc]);

				assert(forward[mid_arc] <= forward[bottom_arc] + forward[top_arc]);
				assert(backward[mid_arc] <= backward[mid_arc]  + backward[top_arc]);

				return true;
			}
		);
	#endif

	ContractionHierarchy ch;

	ch.rank = cch->rank;
	ch.order = cch->order;

	LocalIDMapper forward_map(keep_forward_arc);
	LocalIDMapper backward_map(keep_backward_arc);

	ch.forward.head = keep_element_of_vector_if(keep_forward_arc, cch->up_head);
	ch.forward.first_out = invert_vector(keep_element_of_vector_if(keep_forward_arc, cch->up_tail), cch->node_count());
	ch.forward.weight = keep_element_of_vector_if(keep_forward_arc, forward);
	ch.forward.is_shortcut_an_original_arc = BitVector(ch.forward.head.size(), false);
	ch.forward.shortcut_first_arc = std::vector<unsigned>(forward_map.local_id_count());
	ch.forward.shortcut_second_arc = std::vector<unsigned>(forward_map.local_id_count());

	ch.backward.head = keep_element_of_vector_if(keep_backward_arc, cch->up_head);
	ch.backward.first_out = invert_vector(keep_element_of_vector_if(keep_backward_arc, cch->up_tail), cch->node_count());
	ch.backward.weight = keep_element_of_vector_if(keep_backward_arc, backward);
	ch.backward.is_shortcut_an_original_arc = BitVector(ch.backward.head.size(), false);
	ch.backward.shortcut_first_arc = std::vector<unsigned>(backward_map.local_id_count());
	ch.backward.shortcut_second_arc = std::vector<unsigned>(backward_map.local_id_count());

	for(unsigned cch_arc=0; cch_arc<cch->cch_arc_count(); ++cch_arc){
		if(keep_forward_arc.is_set(cch_arc)){
			unsigned forward_ch_arc = forward_map.to_local(cch_arc);
			unsigned forward_original_arc = unpack_original_forward_arc(*cch, *this, cch_arc);
			if(forward_original_arc != invalid_id){
				ch.forward.is_shortcut_an_original_arc.set(forward_ch_arc);
				ch.forward.shortcut_first_arc[forward_ch_arc] = forward_original_arc;
				ch.forward.shortcut_second_arc[forward_ch_arc] = cch->order[cch->up_head[cch_arc]];
			}else{
				bool was_forward_not_found = forall_lower_triangles_of_arc(
					*cch, cch_arc,
					[&](
						unsigned bottom_arc, unsigned mid_arc, unsigned top_arc,
						unsigned bottom_node, unsigned mid_node, unsigned top_node
					){
						(void)bottom_node;
						(void)mid_node;
						(void)top_node;

						if(keep_backward_arc.is_set(bottom_arc) && keep_forward_arc.is_set(mid_arc)){
							if(backward[bottom_arc] + forward[mid_arc] == forward[top_arc]){
								ch.forward.shortcut_first_arc[forward_ch_arc] = backward_map.to_local(bottom_arc);
								ch.forward.shortcut_second_arc[forward_ch_arc] = forward_map.to_local(mid_arc);
								return false;
							}
						}
						return true;
					}
				);
				(void) was_forward_not_found;
				assert(!was_forward_not_found);
			}
		}
		if(keep_backward_arc.is_set(cch_arc)){
			unsigned backward_ch_arc = backward_map.to_local(cch_arc);
			unsigned backward_original_arc = unpack_original_backward_arc(*cch, *this, cch_arc);
			if(backward_original_arc != invalid_id){
				ch.backward.is_shortcut_an_original_arc.set(backward_ch_arc);
				ch.backward.shortcut_first_arc[backward_ch_arc] = backward_original_arc;
				ch.backward.shortcut_second_arc[backward_ch_arc] = cch->order[cch->up_tail[cch_arc]];
			}else{
				bool was_backward_not_found = forall_lower_triangles_of_arc(
					*cch, cch_arc,
					[&](
						unsigned bottom_arc, unsigned mid_arc, unsigned top_arc,
						unsigned bottom_node, unsigned mid_node, unsigned top_node
					){
						(void)bottom_node;
						(void)mid_node;
						(void)top_node;

						if(keep_backward_arc.is_set(mid_arc) && keep_forward_arc.is_set(bottom_arc)){
							if(backward[mid_arc] + forward[bottom_arc] == backward[top_arc]){
								ch.backward.shortcut_first_arc[backward_ch_arc] = backward_map.to_local(mid_arc);
								ch.backward.shortcut_second_arc[backward_ch_arc] = forward_map.to_local(bottom_arc);
								return false;
							}
						}
						return true;
					}
				);
				(void) was_backward_not_found;
				assert(!was_backward_not_found);
			}
		}
	}

	check_contraction_hierarchy_for_errors(ch);

	return ch;
}


CustomizableContractionHierarchyQuery& CustomizableContractionHierarchyQuery::reset(){
	if(state == query_state_target_pinned || state == query_state_target_run){
		reset_target_distances(cch->elimination_tree_parent, target_node, target_elimination_tree_end, forward_tentative_distance);
	}else if(state == query_state_source_pinned || state == query_state_source_run){
		reset_target_distances(cch->elimination_tree_parent, source_node, source_elimination_tree_end, backward_tentative_distance);
	}
	reset_source_list(cch->elimination_tree_parent, source_node, source_elimination_tree_end, in_forward_search_space, forward_tentative_distance);
	reset_source_list(cch->elimination_tree_parent, target_node, target_elimination_tree_end, in_backward_search_space, backward_tentative_distance);
	state = query_state_initialized;
	return *this;
}

CustomizableContractionHierarchyQuery& CustomizableContractionHierarchyQuery::reset(const CustomizableContractionHierarchyMetric&metric){
	if(this->cch == metric.cch) {
		this->metric = &metric;
		reset();
	} else {
		*this = CustomizableContractionHierarchyQuery(metric);
	}
	state = query_state_initialized;
	return *this;
}


} // namespace RoutingKit


