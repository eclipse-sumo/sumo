#include <routingkit/nested_dissection.h>
#include <routingkit/constants.h>
#include <routingkit/graph_util.h>
#include <routingkit/permutation.h>
#include <routingkit/inverse_vector.h>
#include <routingkit/filter.h>
#include <routingkit/id_mapper.h>
#include <routingkit/bit_vector.h>
#include <routingkit/timer.h>

#include <assert.h>

namespace RoutingKit{

void assert_fragment_is_valid(const GraphFragment&fragment){
	#ifndef NDEBUG
//	unsigned node_count = fragment.node_count();
//	unsigned arc_count = fragment.arc_count();

//	assert(fragment.tail.size() == arc_count);
//	assert(fragment.head.size() == arc_count);
//	assert(fragment.back_arc.size() == arc_count);
//	assert(fragment.global_node_id.size() == node_count);

//	if(arc_count != 0){
//		assert(max_element_of(fragment.tail) < node_count);
//		assert(max_element_of(fragment.head) < node_count);
//	}

//	assert(invert_inverse_vector(fragment.first_out) == fragment.tail);

//	for(unsigned a=0; a<arc_count; ++a){
//		assert(fragment.back_arc[a] < arc_count);
//		assert(fragment.back_arc[fragment.back_arc[a]] == a);
//		assert(fragment.tail[a] == fragment.head[fragment.back_arc[a]]);
//		assert(fragment.head[a] == fragment.tail[fragment.back_arc[a]]);
//	}
	#endif
}

GraphFragment make_graph_fragment(unsigned node_count, const std::vector<unsigned>&tail, const std::vector<unsigned>&head){
	unsigned arc_count = head.size();

	GraphFragment fragment;

	unsigned non_loop_arc_count = 0;
	for(unsigned i=0; i<arc_count; ++i)
		if(tail[i] != head[i])
			++non_loop_arc_count;
		
	fragment.tail.resize(2*non_loop_arc_count);
	fragment.head.resize(2*non_loop_arc_count);
	fragment.back_arc.resize(2*non_loop_arc_count);

	{
		unsigned j = 0;
		for(unsigned i=0; i<arc_count; ++i){
			if(tail[i] != head[i]){
				fragment.tail[j] = tail[i];
				fragment.tail[j+non_loop_arc_count] = head[i];
				fragment.head[j] = head[i];
				fragment.head[j+non_loop_arc_count] = tail[i];
				fragment.back_arc[j] = j+non_loop_arc_count;
				fragment.back_arc[j+non_loop_arc_count] = j;
				++j;
			}
		}
	}

	{
		auto p = compute_inverse_sort_permutation_first_by_tail_then_by_head_and_apply_sort_to_tail(node_count, fragment.tail, fragment.head);
		fragment.head = apply_inverse_permutation(p, fragment.head);
		fragment.back_arc = apply_inverse_permutation(p, fragment.back_arc);
		inplace_apply_permutation_to_elements_of(p, fragment.back_arc);
		fragment.first_out = invert_vector(fragment.tail, node_count);
	}

	fragment.global_node_id = identity_permutation(node_count);

	assert_fragment_is_valid(fragment);

	return fragment;
}

BlockingFlow::BlockingFlow(
	const GraphFragment&fragment, BitVector is_source_, BitVector is_target_
):
	fragment(&fragment), is_source(std::move(is_source_)), is_target(std::move(is_target_)),

	flow_intensity(0),
	is_arc_saturated(fragment.arc_count(), false),
	is_arc_blocked(fragment.arc_count(), BitVector::uninitialized),
	is_finished_flag(false)
{
	#ifndef NDEBUG
	assert_fragment_is_valid(fragment);
	assert(is_source.size() == fragment.node_count());
	assert(is_target.size() == fragment.node_count());
	assert(is_source.population_count() > 0);
	assert(is_target.population_count() > 0);
	for(unsigned x=0; x<fragment.node_count(); ++x)
		assert((!is_target.is_set(x) || !is_source.is_set(x)) && "a source node can not also be a target node");
	#endif
}

namespace{
	bool compute_blocking_flow(const GraphFragment&fragment, const BitVector&is_source, const BitVector&is_target, const BitVector&is_arc_saturated, BitVector&is_arc_blocked){
		BitVector is_on_same_level_or_lower(fragment.node_count(), false);
		BitVector was_node_pushed(fragment.node_count(), false);

		is_arc_blocked.reset_all();

		std::vector<unsigned>queue(fragment.node_count());
		unsigned queue_begin = 0;
		unsigned queue_end = 0;

		for(unsigned x=0; x<fragment.node_count(); ++x)
			if(is_source.is_set(x))
				queue[queue_end++] = x;
		unsigned queue_current_level_end = queue_end;

		bool is_a_target_node_reachable = false;

		while(queue_begin != queue_end){

			for(unsigned i=queue_begin; i<queue_current_level_end; ++i){
				is_on_same_level_or_lower.set(queue[i]);
			}

			for(unsigned i=queue_begin; i<queue_current_level_end; ++i){
				unsigned x = queue[i];
				for(unsigned xy=fragment.first_out[x]; xy<fragment.first_out[x+1]; ++xy){
					if(is_arc_saturated.is_set(xy)){
						is_arc_blocked.set(xy);
					} else {
						unsigned y = fragment.head[xy];
						if(is_on_same_level_or_lower.is_set(y)){
							is_arc_blocked.set(xy);
						} else {
							if(is_target.is_set(y)){
								is_a_target_node_reachable = true;
							} else {						
								if(!was_node_pushed.is_set(y)){
									queue[queue_end++] = y;
									was_node_pushed.set(y);
								}
							}
						}
					}
				}
			}

			queue_begin = queue_current_level_end;
			queue_current_level_end = queue_end;			
		}
		return is_a_target_node_reachable;
	}

	unsigned augment_all_non_blocked_path(const GraphFragment&fragment, const BitVector&is_source, const BitVector&is_target, BitVector&is_arc_saturated, BitVector&is_arc_blocked){
		std::vector<unsigned>current_path_node(fragment.node_count());
		std::vector<unsigned>current_path_arc(fragment.node_count());

		auto find_first_non_block_outgoing_arc_of_node = [&](unsigned x){
			for(unsigned xy=fragment.first_out[x]; xy<fragment.first_out[x+1]; ++xy)
				if(!is_arc_blocked.is_set(xy))
					return xy;
			return invalid_id;
		};

		unsigned augmented_path_count = 0;

		for(unsigned s=0; s<fragment.node_count(); ++s){
			if(is_source.is_set(s)){
				current_path_node[0] = s;
				current_path_arc[0] = s;
				unsigned current_path_arc_count = 0;
				for(;;){
					unsigned x = current_path_node[current_path_arc_count];
					unsigned xy = find_first_non_block_outgoing_arc_of_node(x);
					if(xy == invalid_id){
						if(current_path_arc_count == 0)
							break;
						--current_path_arc_count;
						is_arc_blocked.set(current_path_arc[current_path_arc_count]);
					} else {
						auto y = fragment.head[xy];
						current_path_arc[current_path_arc_count] = xy;
						++current_path_arc_count;
						current_path_node[current_path_arc_count] = y;
						if(is_target.is_set(y)){
							for(unsigned i=0; i<current_path_arc_count; ++i){
								unsigned a = current_path_arc[i];
								assert(!is_arc_saturated.is_set(a));
								is_arc_blocked.set(a);

								unsigned b = fragment.back_arc[a];
								if(is_arc_saturated.is_set(b))
									is_arc_saturated.reset(b);
								else
									is_arc_saturated.set(a);
							}
							current_path_arc_count = 0;
							++augmented_path_count;
						}
					}
				}
			}
		}
		return augmented_path_count;
	}
}


CutSide BlockingFlow::get_source_cut(){
	assert(is_finished_flag);

	CutSide side;

	side.is_node_on_side.resize(fragment->node_count(), false); 		
	side.cut_size = flow_intensity;
	side.node_on_side_count = 0;

	unsigned stack_end = 0;
	std::vector<unsigned>stack(fragment->node_count());
	for(unsigned s=0; s<fragment->node_count(); ++s)
		if(is_source.is_set(s)){
			stack[stack_end++] = s;
			side.is_node_on_side.set(s);
			++side.node_on_side_count;
		}

	while(stack_end != 0){
		unsigned x = stack[--stack_end];
		for(unsigned xy=fragment->first_out[x]; xy<fragment->first_out[x+1]; ++xy){
			if(!is_arc_saturated.is_set(xy)){
				unsigned y = fragment->head[xy];
				if(!side.is_node_on_side.is_set(y)){
					stack[stack_end++] = y;
					side.is_node_on_side.set(y);
					++side.node_on_side_count;
				}
			}
		}
	}

	return side; // NVRO
}

CutSide BlockingFlow::get_target_cut(){
	assert(is_finished_flag);

	CutSide side;

	side.is_node_on_side.resize(fragment->node_count(), false); 		
	side.cut_size = flow_intensity;
	side.node_on_side_count = 0;

	unsigned stack_end = 0;
	std::vector<unsigned>stack(fragment->node_count());
	for(unsigned t=0; t<fragment->node_count(); ++t)
		if(is_target.is_set(t)){
			stack[stack_end++] = t;
			side.is_node_on_side.set(t);
			++side.node_on_side_count;
		}

	
	while(stack_end != 0){
		unsigned x = stack[--stack_end];
		for(unsigned xy=fragment->first_out[x]; xy<fragment->first_out[x+1]; ++xy){
			if(!is_arc_saturated.is_set(fragment->back_arc[xy])){
				unsigned y = fragment->head[xy];
				if(!side.is_node_on_side.is_set(y)){
					stack[stack_end++] = y;
					side.is_node_on_side.set(y);
					++side.node_on_side_count;
				}
			}
		}
	}

	return side; // NVRO
}

CutSide BlockingFlow::get_balanced_cut(){
	assert(is_finished_flag);

	unsigned node_count = fragment->node_count();
	unsigned arc_count = fragment->arc_count();

	
	BitVector is_source_reachable = is_source;
	BitVector is_target_reachable = is_target;

	unsigned source_reachable_count = 0;
	unsigned target_reachable_count = 0;
		
	std::vector<unsigned>stack(node_count);
	unsigned stack_end = 0;

	std::vector<unsigned>potential_source_piercing_node(arc_count); // arc_count is no typo, nodes may be multiple times in the vector
	unsigned potential_source_piercing_node_end = 0;
	
	std::vector<unsigned>potential_target_piercing_node(arc_count);
	unsigned potential_target_piercing_node_end = 0;
	
	auto enlarge_source_side = [&]{
		while(stack_end != 0){
			++source_reachable_count;
			unsigned x = stack[--stack_end];
			for(unsigned xy = fragment->first_out[x]; xy < fragment->first_out[x+1]; ++xy){
				unsigned y = fragment->head[xy];
				if(is_arc_saturated.is_set(xy)){
					potential_source_piercing_node[potential_source_piercing_node_end++] = y;
				} else {
					if(!is_source_reachable.is_set(y)){
						is_source_reachable.set(y);
						stack[stack_end++] = y;
					}
				}
			
			}
		}
		assert(source_reachable_count == is_source_reachable.population_count());
	};

	auto enlarge_target_side = [&]{
		while(stack_end != 0){
			++target_reachable_count;
			unsigned x = stack[--stack_end];
			for(unsigned xy = fragment->first_out[x]; xy < fragment->first_out[x+1]; ++xy){
				unsigned y = fragment->head[xy];
				if(is_arc_saturated.is_set(fragment->back_arc[xy])){
					potential_target_piercing_node[potential_target_piercing_node_end++] = y;
				} else {
					if(!is_target_reachable.is_set(y)){
						stack[stack_end++] = y;
						is_target_reachable.set(y);
					}
				}
			
			}
		}
		assert(target_reachable_count == is_target_reachable.population_count());
	};

	auto add_source_nodes_to_stack = [&]{
		for(unsigned x=0; x<node_count; ++x)
			if(is_source.is_set(x))
				stack[stack_end++] = x;
	};

	auto add_target_nodes_to_stack = [&]{
		for(unsigned x=0; x<node_count; ++x)
			if(is_target.is_set(x))
				stack[stack_end++] = x;
	};

	add_source_nodes_to_stack();
	enlarge_source_side();

	add_target_nodes_to_stack();
	enlarge_target_side();

	CutSide side;

	for(;;){
		if(source_reachable_count <= target_reachable_count){
			unsigned pierce_node = invalid_id;
			while(pierce_node == invalid_id){
				if(potential_source_piercing_node_end == 0){
					side.is_node_on_side = std::move(is_source_reachable);
					side.cut_size = flow_intensity;
					side.node_on_side_count = source_reachable_count;
					return side; // NVRO
				} else {
					unsigned y = potential_source_piercing_node[--potential_source_piercing_node_end];
					if(!is_source_reachable.is_set(y) && !is_target_reachable.is_set(y))
						pierce_node = y;
				}
			}
			is_source_reachable.set(pierce_node);
			stack[stack_end++] = pierce_node;
			enlarge_source_side();
		}else{
			unsigned pierce_node = invalid_id;
			while(pierce_node == invalid_id){
				if(potential_target_piercing_node_end == 0){
					side.is_node_on_side = std::move(is_target_reachable);
					side.cut_size = flow_intensity;
					side.node_on_side_count = target_reachable_count;
					return side; // NVRO
				} else {
					unsigned y = potential_target_piercing_node[--potential_target_piercing_node_end];
					if(!is_source_reachable.is_set(y) && !is_target_reachable.is_set(y))
						pierce_node = y;
				}
			}
			is_target_reachable.set(pierce_node);
			stack[stack_end++] = pierce_node;
			enlarge_target_side();
		}
	}

	return side; // NVRO
}

void BlockingFlow::advance(){
	if(!is_finished_flag && compute_blocking_flow(*fragment, is_source, is_target, is_arc_saturated, is_arc_blocked)){
		flow_intensity += augment_all_non_blocked_path(*fragment, is_source, is_target, is_arc_saturated, is_arc_blocked);
		is_finished_flag = false;
	}else{
		is_finished_flag = true;
	}
}



namespace{
	template<class GetKey>
	BitVector mark_first_n_elements(unsigned n, unsigned total_element_count, const GetKey&sort_key){
		auto v = identity_permutation(total_element_count);
		auto
			begin = v.begin(),
			mid = v.begin() + n,
			end = v.end();

		std::nth_element(begin, mid, end, [&](unsigned l, unsigned r){return sort_key(l) < sort_key(r);});
		
		BitVector r(total_element_count, false);
		while(begin != mid){
			r.set(*begin);
			++begin;
		}
		return r;
	}

	struct SourceTargetResult{
		BitVector is_source, is_target;
	};

	template<class GetKey>
	SourceTargetResult select_source_and_target(unsigned n, unsigned node_count, const GetKey&sort_key){
		assert(n <= node_count/2);
		auto v = identity_permutation(node_count);
		auto
			begin = v.begin(),
			source_end = v.begin() + n,
			target_begin = v.begin() + node_count - n,
			end = v.end();

		std::nth_element(begin, source_end, end, [&](unsigned l, unsigned r){return sort_key(l) < sort_key(r);});
		std::nth_element(source_end, target_begin, end, [&](unsigned l, unsigned r){return sort_key(l) < sort_key(r);});
		
		SourceTargetResult ret;
		ret.is_source.resize(node_count);
		ret.is_source.reset_all();

		while(begin != source_end){
			ret.is_source.set(*begin);
			++begin;
		}

		ret.is_target.resize(node_count);
		ret.is_target.reset_all();

		while(target_begin != end){
			ret.is_target.set(*target_begin);
			++target_begin;
		}
		
		return ret; // NVRO
	}
}

void pick_smaller_side(CutSide&c){
	unsigned node_count = c.is_node_on_side.size();
	if(c.node_on_side_count >= (node_count+1)/2){
		c.node_on_side_count = node_count - c.node_on_side_count;
		c.is_node_on_side.inplace_not();
	}
}

CutSide inertial_flow(
	const GraphFragment&g,
	unsigned min_balance,
	const std::vector<float>&latitude, const std::vector<float>&longitude,
	const std::function<void(const std::string&)>&log_message
){
	assert_fragment_is_valid(g);

	long long last_report = 0;
	long long start_time = 0;
	bool first_report = true;

	if(log_message){
		start_time = get_micro_time();
		last_report = start_time;
	}

	const unsigned node_count = g.node_count();

	unsigned side_size = (node_count*min_balance)/100;
	if(side_size == 0)
		side_size = 1;

	auto horizontal_source_target = select_source_and_target(side_size, node_count, [&](unsigned x){return latitude[g.global_node_id[x]];});
	BlockingFlow horizontal_cutter(
		g,
		std::move(horizontal_source_target.is_source),
		std::move(horizontal_source_target.is_target)
	);

	auto vertical_source_target = select_source_and_target(side_size, node_count, [&](unsigned x){return longitude[g.global_node_id[x]];});
	BlockingFlow vertical_cutter(
		g,
		std::move(vertical_source_target.is_source),
		std::move(vertical_source_target.is_target)
	);

	auto main_diagonal_source_target = select_source_and_target(side_size, node_count, [&](unsigned x){return latitude[g.global_node_id[x]]+longitude[g.global_node_id[x]];});
	BlockingFlow main_diagonal_cutter(
		g,
		std::move(main_diagonal_source_target.is_source),
		std::move(main_diagonal_source_target.is_target)
	);

	auto next_diagonal_source_target = select_source_and_target(side_size, node_count, [&](unsigned x){return latitude[g.global_node_id[x]]-longitude[g.global_node_id[x]];});
	BlockingFlow next_diagonal_cutter(
		g,
		std::move(next_diagonal_source_target.is_source),
		std::move(next_diagonal_source_target.is_target)
	);


	auto get_next_cutter = [&]()->BlockingFlow&{
		if(
			horizontal_cutter.get_current_flow_intensity() <= vertical_cutter.get_current_flow_intensity() &&
			horizontal_cutter.get_current_flow_intensity() <= main_diagonal_cutter.get_current_flow_intensity() &&
			horizontal_cutter.get_current_flow_intensity() <= next_diagonal_cutter.get_current_flow_intensity()
		){
			return horizontal_cutter;
		}else if(
			vertical_cutter.get_current_flow_intensity() <= main_diagonal_cutter.get_current_flow_intensity() &&
			vertical_cutter.get_current_flow_intensity() <= next_diagonal_cutter.get_current_flow_intensity()
		){
			return vertical_cutter;
		}else if(
			main_diagonal_cutter.get_current_flow_intensity() <= next_diagonal_cutter.get_current_flow_intensity()
		){
			return main_diagonal_cutter;
		}else{
			return next_diagonal_cutter;
		}
	};

	for(;;){
		auto&c = get_next_cutter();
		if(!c.is_finished()){

			if(log_message){
				long long now = get_micro_time();
				if(now - last_report > 1000000){
					if(first_report){
						first_report = false;	
						log_message("Start running Inertial Flow with imbalance "+std::to_string(min_balance)+"% on graph with "+std::to_string(g.node_count()) +" nodes and "+std::to_string(g.arc_count())+" arcs.");
					}
					last_report = now;
					log_message("Smallest cutter has reached a cut of "+std::to_string(c.get_current_flow_intensity())+" arcs.");
				}
			}

			c.advance();
		}else{
			auto cut = c.get_balanced_cut();
			if(log_message){
				if(!first_report){
					log_message("Inertial Flow is finished and needed "+std::to_string(get_micro_time()-start_time)+"musec. The cut has "+std::to_string(cut.cut_size)+" arcs and the smaller side has "+std::to_string(cut.node_on_side_count)+" nodes.");
				}
			}
			return cut; // NRVO
		}
	}
}

CutSide inertial_flow(
	const GraphFragment&g,
	const std::vector<float>&latitude, const std::vector<float>&longitude,
	const std::function<void(const std::string&)>&log_message
){
	assert_fragment_is_valid(g);

	auto c25 = inertial_flow(g, 25, latitude, longitude, log_message);
	auto c33 = inertial_flow(g, 33, latitude, longitude, log_message);
	auto c40 = inertial_flow(g, 40, latitude, longitude, log_message);

	if(
		static_cast<unsigned long long>(c25.cut_size) * static_cast<unsigned long long>(c33.node_on_side_count) < static_cast<unsigned long long>(c33.cut_size) * static_cast<unsigned long long>(c25.node_on_side_count) &&
		static_cast<unsigned long long>(c25.cut_size) * static_cast<unsigned long long>(c40.node_on_side_count) < static_cast<unsigned long long>(c40.cut_size) * static_cast<unsigned long long>(c25.node_on_side_count)
	)
		return c25; // NVRO
	else if(
		static_cast<unsigned long long>(c33.cut_size) * static_cast<unsigned long long>(c40.node_on_side_count) < static_cast<unsigned long long>(c40.cut_size) * static_cast<unsigned long long>(c33.node_on_side_count)
	)
		return c33; // NVRO
	else
		return c40; // NVRO

}


std::vector<GraphFragment>decompose_graph_fragment_into_connected_components(GraphFragment fragment){
	assert_fragment_is_valid(fragment);

	unsigned node_count = fragment.node_count();
	unsigned arc_count = fragment.arc_count();

	unsigned component_count = 0;
	std::vector<unsigned>component(node_count, invalid_id);
	{
		std::vector<unsigned>inv_pseudo_preorder(node_count);
		{
			std::vector<unsigned>stack(node_count);
			unsigned pos = 0;
			for(unsigned r=0; r<node_count; ++r){
				if(component[r] == invalid_id){
					component[r] = component_count;
					unsigned stack_end = 1;
					stack[0] = r;
					while(stack_end != 0){
						unsigned x = stack[--stack_end];
						inv_pseudo_preorder[x] = pos++;
						for(unsigned xy=fragment.first_out[x]; xy<fragment.first_out[x+1]; ++xy){
							unsigned y=fragment.head[xy];	
							if(component[y] == invalid_id){
								stack[stack_end++] = y;
								component[y] = component_count;
							}
						}		
					}
					++component_count;
				}
			}
		}

		inplace_apply_permutation_to_elements_of(inv_pseudo_preorder, fragment.tail);
		inplace_apply_permutation_to_elements_of(inv_pseudo_preorder, fragment.head);
		fragment.global_node_id = apply_inverse_permutation(inv_pseudo_preorder, fragment.global_node_id);
		component = apply_inverse_permutation(inv_pseudo_preorder, component);
	}

	{
		auto p = compute_inverse_sort_permutation_first_by_tail_then_by_head_and_apply_sort_to_tail(node_count, fragment.tail, fragment.head);
		fragment.head = apply_inverse_permutation(p, fragment.head);
		fragment.back_arc = apply_inverse_permutation(p, fragment.back_arc);
		inplace_apply_permutation_to_elements_of(p, fragment.back_arc);
		fragment.first_out = invert_vector(fragment.tail, node_count);
	}

	assert_fragment_is_valid(fragment);

	std::vector<GraphFragment>part_list;

	auto generate_part = [&](unsigned component_node_begin, unsigned component_node_end, unsigned component_arc_begin, unsigned component_arc_end){
		GraphFragment part;

		unsigned part_node_count = component_node_end - component_node_begin;
		unsigned part_arc_count = component_arc_end - component_arc_begin;
		(void)part_node_count; (void)part_arc_count;


		part.tail = std::vector<unsigned>(fragment.tail.begin()+component_arc_begin, fragment.tail.begin()+component_arc_end);
		for(auto&x:part.tail)
			x -= component_node_begin;

		if(part_arc_count != 0)
			assert(max_element_of(part.tail) < part_node_count);
		
		part.head = std::vector<unsigned>(fragment.head.begin()+component_arc_begin, fragment.head.begin()+component_arc_end);
		for(auto&x:part.head)
			x -= component_node_begin;

		if(part_arc_count != 0)
			assert(max_element_of(part.head) < part_node_count);
			
		part.back_arc = std::vector<unsigned>(fragment.back_arc.begin()+component_arc_begin, fragment.back_arc.begin()+component_arc_end);
		for(auto&x:part.back_arc)
			x -= component_arc_begin;

		if(part_arc_count != 0)
			assert(max_element_of(part.back_arc) < part_arc_count);
		
		part.global_node_id = std::vector<unsigned>(fragment.global_node_id.begin()+component_node_begin, fragment.global_node_id.begin()+component_node_end);

		part.first_out = invert_vector(part.tail, part_node_count);

		assert_fragment_is_valid(part);

		part_list.push_back(std::move(part));
	};

	unsigned component_node_begin = 0;
	unsigned component_arc_begin = 0;
	unsigned component_node_end = 1;
	unsigned component_arc_end = 0;
	
	unsigned current_component = 0;
	while(component_node_end<node_count){
		if(component[component_node_end] != current_component){
			while(component_arc_end != arc_count && component[fragment.tail[component_arc_end]] == current_component){
				assert(component[fragment.tail[component_arc_end]] == component[fragment.head[component_arc_end]]);
				++component_arc_end;
			}
			generate_part(component_node_begin, component_node_end, component_arc_begin, component_arc_end);
			component_node_begin = component_node_end;
			component_arc_begin = component_arc_end;
			current_component = component[component_node_end];
		}
		++component_node_end;
	}
	generate_part(component_node_begin, component_node_end, component_arc_begin, arc_count);
	
	return part_list; // NVRO
}

BitVector derive_separator_from_cut(const GraphFragment&fragment, const BitVector&cut){
	assert_fragment_is_valid(fragment);

	bool small_side = cut.population_count() <= fragment.node_count()/2;
	BitVector is_separator_node(fragment.node_count(), false);

	for(unsigned xy=0; xy<fragment.arc_count(); ++xy){
		unsigned x = fragment.tail[xy], y = fragment.head[xy];
		if(cut.is_set(x) == small_side && cut.is_set(y) != small_side)
			is_separator_node.set(y);
	}
	return is_separator_node; // NVRO
}

SeparatorDecomposition compute_separator_decomposition(
	GraphFragment fragment, const std::function<BitVector(const GraphFragment&)>&compute_separator,
	const std::function<void(const std::string&)>&log_message
){
	assert_fragment_is_valid(fragment);

	long long timer = 0;

	SeparatorDecomposition decomp;
	decomp.order.resize(fragment.node_count());
	
	if(fragment.node_count() == 1){
		decomp.tree.push_back({0, 0, 0, 1});
		decomp.order = std::move(fragment.global_node_id);
	}else{

		unsigned pred = 0;
		unsigned order_begin = 0, order_end = fragment.node_count();

		decomp.tree.push_back({0, 0, 0, order_end});

		if(log_message){
			timer = -get_micro_time();
			log_message("Start decomposing top-level graph");
		}
		auto part_list = decompose_graph_fragment_into_connected_components(std::move(fragment));
		fragment = GraphFragment(); // release memory
		if(log_message){
			timer += get_micro_time();
			log_message("Finished decomposing top-level graph, needed "+std::to_string(timer)+"musec and found "+std::to_string(part_list.size())+" connected components");
		}

		for(auto&part:part_list){
			assert(part.node_count() != 0);
			if(part.node_count() == 1){
				decomp.order[--order_end] = part.global_node_id[0];
			}else{
				if(log_message && part.node_count() > 1000){
					log_message("Computing decomposition for top level component with "+std::to_string(part.node_count())+" nodes");
					timer = -get_micro_time();
					log_message("Start computing top level separator");
				}
				auto is_separator_node = compute_separator(part);
				if(log_message && part.node_count() > 1000){
					timer += get_micro_time();
					log_message("Finished computing top level separator, its size is "+std::to_string(is_separator_node.population_count())+" nodes needed "+std::to_string(timer)+"musec");
				}

				BitVector f = make_bit_vector(
					part.arc_count(),
					[&](unsigned a){
						return !is_separator_node.is_set(part.tail[a]) && !is_separator_node.is_set(part.head[a]);
					}
				);
			
				inplace_keep_element_of_vector_if(f, part.tail);
				inplace_keep_element_of_vector_if(f, part.head);
				inplace_keep_element_of_vector_if(f, part.back_arc);
			
				{
					LocalIDMapper map(f);
					for(auto&x:part.back_arc)
						x = map.to_local(x);
				}

				part.first_out = invert_vector(part.tail, part.node_count());

				assert_fragment_is_valid(part);


				const unsigned part_node_count = part.node_count(); // Capture before move
				if(log_message && part_node_count > 1000){
					timer = -get_micro_time();
					log_message("Start computing remaining separator decomposition using recursion");
				}
				auto sub_decomp = compute_separator_decomposition(std::move(part), compute_separator);
				if(log_message && part_node_count > 1000){
					timer += get_micro_time();
					log_message("Finished recursion, needed "+std::to_string(timer)+"musec");
				}

				for(auto&node:sub_decomp.tree){
					if(node.left_child != 0)
						node.left_child += decomp.tree.size();
					if(node.right_sibling != 0)
						node.right_sibling += decomp.tree.size();
					node.first_separator_vertex += order_begin;
					node.last_separator_vertex += order_begin;
				}
				if(pred == 0)
					decomp.tree[pred].left_child = decomp.tree.size();
				else
					decomp.tree[pred].right_sibling = decomp.tree.size();
				pred = decomp.tree.size();
				decomp.tree.insert(decomp.tree.end(), sub_decomp.tree.begin(), sub_decomp.tree.end());
				std::copy(sub_decomp.order.begin(), sub_decomp.order.end(), decomp.order.begin() + order_begin);
				order_begin += sub_decomp.order.size();
			}
		}
		decomp.tree[0].first_separator_vertex = order_begin;
	}

	return decomp; // NVRO
}

std::vector<unsigned>compute_nested_node_dissection_order(
	GraphFragment fragment, const std::function<BitVector(const GraphFragment&)>&compute_separator,
	const std::function<void(const std::string&)>&log_message
){
	return compute_separator_decomposition(std::move(fragment), compute_separator, log_message).order;
}

std::vector<unsigned>compute_nested_node_dissection_order_using_inertial_flow(
	unsigned node_count, const std::vector<unsigned>&tail, const std::vector<unsigned>&head,
	const std::vector<float>&latitude, const std::vector<float>&longitude,
	const std::function<void(const std::string&)>&log_message
){
	long long timer = 0;

	if(log_message){
		timer = -get_micro_time();
		log_message("Start making graph fragment");
	}
	auto g = make_graph_fragment(node_count, tail, head);
	if(log_message){
		timer += get_micro_time();
		log_message("Finished making graph fragment, needed "+std::to_string(timer)+"musec");
	}

	auto compute_cut = [&](const GraphFragment&fragment)->BitVector{
		auto c = inertial_flow(fragment, latitude, longitude, log_message);
		pick_smaller_side(c);
		return std::move(c.is_node_on_side);
	};

	auto compute_separator = [&](const GraphFragment&fragment)->BitVector{
		return derive_separator_from_cut(fragment, compute_cut(fragment));
	};

	return compute_nested_node_dissection_order(std::move(g), compute_separator, log_message);
}

} // RoutingKit

