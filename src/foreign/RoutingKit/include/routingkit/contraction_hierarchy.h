#ifndef ROUTING_KIT_CONTRACTION_HIERARCHY_H
#define ROUTING_KIT_CONTRACTION_HIERARCHY_H

#include <routingkit/id_queue.h>
#include <routingkit/timestamp_flag.h>
#include <routingkit/bit_vector.h>
#include <routingkit/permutation.h>

#include <vector>
#include <string>
#include <functional>
#include <cassert>
#include <type_traits>
#include <limits.h>

namespace RoutingKit{

class ContractionHierarchy{
public:
	static const unsigned default_max_pop_count = 500;

	static ContractionHierarchy build(
		unsigned node_count, std::vector<unsigned>tail, std::vector<unsigned>head, std::vector<unsigned>weight,
		const std::function<void(std::string)>&log_message = std::function<void(std::string)>(), unsigned max_pop_count = default_max_pop_count
	);

	static ContractionHierarchy build_given_rank(
		std::vector<unsigned>rank,
		std::vector<unsigned>tail, std::vector<unsigned>head, std::vector<unsigned>weight,
		const std::function<void(std::string)>&log_message = std::function<void(std::string)>(), unsigned max_pop_count = default_max_pop_count
	);

	static ContractionHierarchy build_given_order(
		std::vector<unsigned>order,
		std::vector<unsigned>tail, std::vector<unsigned>head, std::vector<unsigned>weight,
		const std::function<void(std::string)>&log_message = std::function<void(std::string)>(), unsigned max_pop_count = default_max_pop_count
	);

	static ContractionHierarchy read(std::function<void(char*, unsigned long long)>data_source);
	static ContractionHierarchy read(std::function<void(char*, unsigned long long)>data_source, unsigned long long file_size);
	static ContractionHierarchy read(std::istream&in);
	static ContractionHierarchy read(std::istream&in, unsigned long long file_size);
	static ContractionHierarchy load_file(const std::string&file_name);

	void write(std::function<void(const char*, unsigned long long)>data_sink) const;
	void write(std::ostream&out) const;
	void save_file(const std::string&file_name) const;

	unsigned node_count()const{
		return rank.size();
	}

	struct Side{
		std::vector<unsigned>first_out;
		std::vector<unsigned>head;
		std::vector<unsigned>weight;

		BitVector is_shortcut_an_original_arc;
		std::vector<unsigned>shortcut_first_arc; // contains input arc ID if not shortcut
		std::vector<unsigned>shortcut_second_arc;// contains input tail node ID if not shortcut
	};

	std::vector<unsigned>rank, order;
	Side forward, backward;
};

void check_contraction_hierarchy_for_errors(const ContractionHierarchy&ch);

template<class Weight>
struct ContractionHierarchyExtraWeight{

	ContractionHierarchyExtraWeight(){}

	template<class InputWeightContainer, class LinkFunction>
	ContractionHierarchyExtraWeight(const ContractionHierarchy&ch, const InputWeightContainer&extra_weight, const LinkFunction&link){ reset(ch, extra_weight, link); }

	template<class InputWeightContainer, class LinkFunction>
	ContractionHierarchyExtraWeight& reset(const ContractionHierarchy&ch, const InputWeightContainer&extra_weight, const LinkFunction&link);

	std::vector<Weight>forward_weight, backward_weight;
};

namespace detail{
	template<class T>
	using ReturnTypeWhenPassedIntOf = typename std::remove_const<typename std::remove_reference<decltype(std::declval<T>()(1))>::type>::type;

	template<class T>
	using ValueTypeOfContainer = typename std::remove_const<typename std::remove_reference<decltype(std::declval<T>()[1])>::type>::type;

	template<class T>
	struct GetExtraWeightTypeHelper{
		typedef ValueTypeOfContainer<T> type;
	};

	template<class T>
	struct GetExtraWeightTypeHelper<ContractionHierarchyExtraWeight<T>>{
		typedef T type;
	};

	template<class T>
	using GetExtraWeightType = typename GetExtraWeightTypeHelper<T>::type;
}

class ContractionHierarchyQuery{
public:
	ContractionHierarchyQuery():ch(0){}
	explicit ContractionHierarchyQuery(const ContractionHierarchy&ch);

	ContractionHierarchyQuery&reset();
	ContractionHierarchyQuery&reset(const ContractionHierarchy&ch);

	ContractionHierarchyQuery&add_source(unsigned s, unsigned dist_to_s = 0);
	ContractionHierarchyQuery&add_target(unsigned t, unsigned dist_to_t = 0);

	ContractionHierarchyQuery&run();

	unsigned get_used_source();
	unsigned get_used_target();

	unsigned get_distance();
	std::vector<unsigned>get_node_path();
	std::vector<unsigned>get_arc_path();

	template<class ExtraWeight, class LinkFunction>
	detail::GetExtraWeightType<ExtraWeight> get_extra_weight_distance(const ExtraWeight&extra_weight, const LinkFunction&link);

	ContractionHierarchyQuery& reset_source();
	ContractionHierarchyQuery& pin_targets(const std::vector<unsigned>&);
	unsigned get_pinned_target_count();
	ContractionHierarchyQuery& run_to_pinned_targets();

	ContractionHierarchyQuery& get_distances_to_targets(unsigned*dist);
	std::vector<unsigned> get_distances_to_targets();

	ContractionHierarchyQuery& reset_target();
	ContractionHierarchyQuery& pin_sources(const std::vector<unsigned>&);
	unsigned get_pinned_source_count();
	ContractionHierarchyQuery& run_to_pinned_sources();

	ContractionHierarchyQuery& get_distances_to_sources(unsigned*dist);
	std::vector<unsigned> get_distances_to_sources();

	// TODO: Mirror these functions in CCH

	ContractionHierarchyQuery& get_used_sources_to_targets(unsigned*dist);
	std::vector<unsigned> get_used_sources_to_targets();

	ContractionHierarchyQuery& get_used_targets_to_sources(unsigned*dist);
	std::vector<unsigned> get_used_targets_to_sources();

	// The get_extra_weight_distances function follow a pattern.
	// The usage pattern is
	//
	//    get_extra_weight_distances_to_[target|source](extra_weight, link, [, tmp, [dist]])
	//
	// There get_extra_weight_distances_to_target is used if targets were pinned, whereas
	// get_extra_weight_distances_to_source is for pinned sources.
	//
	// There are four additional parameters. Their meaning are:
	//
	// * extra_weight: The extra weight according to which the path length should be computed.
	//   This can be a ContractionHierarchyExtraWeight<T> or a container<T>. The former is faster.
	//   container<T> is a placeholder for anything that has an operator[] that provides read-only
	//   access to the arc weight. For every arc a, the expression extra_weight[a] should
	//   give the extra weight. Using ContractionHierarchyExtraWeight<T> is faster.
	//   Typical types for container<T> are const vector<T> and const T*.
	//
	// * The extra weight does not have to be a scalar value. It can be an arbitrary structure.
	//   However, the algorithm needs to know how to concatenate the weights of two arcs.
	//   The link parameter is a functor that tells it how to do it.
	//   For travel time, the link parameter is the addition. If some of your
	//   values are inf_weight, normal addition can run into overflow problems. RoutingKit therefore
	//   provides the SaturatedWeightAddition functor that correctly handles overflows for unsigned and int weights.
	//
	//   The link function will never be provided a default constructed object, i.e.,
	//   it does not exploit that link(T(),foo) == foo.
	//
	// * tmp is a container<T>. Its size must be at least node_count. The content of
	//   the first node_count elements in undefined after this function is completed.
	//   If the parameter is omitted, a temporary buffer is allocated. If the function
	//   is called multiple times, it can be faster to only allocate one buffer.
	//
	// * dist is a container<T>. Its size is the number of pinned sources or targets.
	//   the output is written to it. If it is omited, a vector<T> is allocated and
	//   returned by the function.
	//
	//   If the source and the target nodes are equal, the extra weight length is
	//   default constructed T. If there is no path, the extra weight length is also
	//   default constructed T. For integers default constructed means 0.
	//
	// If both tmp and dist are present, the function is guarenteed to not allocate or free any memory.
	// If tmp and dist are present, and link is guarenteed to not throw, the function is guarenteed to not throw.

	template<class ExtraWeight, class LinkFunction>                                          std::vector<detail::GetExtraWeightType<ExtraWeight>> get_extra_weight_distances_to_targets(const ExtraWeight&extra_weight, const LinkFunction&link);
	template<class ExtraWeight, class LinkFunction, class TmpContainer>                      std::vector<detail::GetExtraWeightType<ExtraWeight>> get_extra_weight_distances_to_targets(const ExtraWeight&extra_weight, const LinkFunction&link, TmpContainer&tmp);
	template<class ExtraWeight, class LinkFunction, class TmpContainer, class DistContainer> ContractionHierarchyQuery&                           get_extra_weight_distances_to_targets(const ExtraWeight&extra_weight, const LinkFunction&link, TmpContainer&tmp, DistContainer&dist);
	template<class ExtraWeight, class LinkFunction>                                          std::vector<detail::GetExtraWeightType<ExtraWeight>> get_extra_weight_distances_to_sources(const ExtraWeight&extra_weight, const LinkFunction&link);
	template<class ExtraWeight, class LinkFunction, class TmpContainer>                      std::vector<detail::GetExtraWeightType<ExtraWeight>> get_extra_weight_distances_to_sources(const ExtraWeight&extra_weight, const LinkFunction&link, TmpContainer&tmp);
	template<class ExtraWeight, class LinkFunction, class TmpContainer, class DistContainer> ContractionHierarchyQuery&                           get_extra_weight_distances_to_sources(const ExtraWeight&extra_weight, const LinkFunction&link, TmpContainer&tmp, DistContainer&dist);

//private:
	const ContractionHierarchy*ch;

	TimestampFlags was_forward_pushed, was_backward_pushed;
	MinIDQueue forward_queue, backward_queue;
	std::vector<unsigned>forward_tentative_distance, backward_tentative_distance;
	std::vector<unsigned>forward_predecessor_node, backward_predecessor_node;
	std::vector<unsigned>forward_predecessor_arc, backward_predecessor_arc;
	unsigned shortest_path_meeting_node;
	unsigned many_to_many_source_or_target_count;

	enum class InternalState:unsigned{
		initialized,
		run,
		source_pinned,
		source_run,
		target_pinned,
		target_run
	}state;
};

struct SaturatedWeightAddition{
	unsigned operator()(unsigned l, unsigned r)const;
	int operator()(int l, int r)const;
};

// ------ Template & inline implementations; no more interface descriptions beyond this line -------

inline
unsigned ContractionHierarchyQuery::get_pinned_target_count(){
	assert(state == ContractionHierarchyQuery::InternalState::target_run || state == ContractionHierarchyQuery::InternalState::target_pinned);
	return many_to_many_source_or_target_count;
}

inline
unsigned ContractionHierarchyQuery::get_pinned_source_count(){
	assert(state == ContractionHierarchyQuery::InternalState::source_run || state == ContractionHierarchyQuery::InternalState::source_pinned);
	return many_to_many_source_or_target_count;
}

template<class ExtraWeight, class LinkFunction>
std::vector<detail::GetExtraWeightType<ExtraWeight>> ContractionHierarchyQuery::get_extra_weight_distances_to_targets(
	const ExtraWeight&extra_weight,
	const LinkFunction&link
){
	std::vector<detail::GetExtraWeightType<ExtraWeight>>tmp(ch->node_count()), dist(get_pinned_target_count());
	get_extra_weight_distances_to_targets(extra_weight, link, tmp, dist);
	return dist; // NVRO
}

template<class ExtraWeight, class LinkFunction, class TmpContainer>
std::vector<detail::GetExtraWeightType<ExtraWeight>> ContractionHierarchyQuery::get_extra_weight_distances_to_targets(
	const ExtraWeight&extra_weight,
	const LinkFunction&link,
	TmpContainer&tmp
){
	std::vector<detail::GetExtraWeightType<ExtraWeight>>dist(get_pinned_target_count());
	get_extra_weight_distances_to_targets(extra_weight, link, tmp, dist);
	return dist; // NVRO
}


template<class ExtraWeight, class LinkFunction>
std::vector<detail::GetExtraWeightType<ExtraWeight>> ContractionHierarchyQuery::get_extra_weight_distances_to_sources(
	const ExtraWeight&extra_weight,
	const LinkFunction&link
){
	std::vector<detail::GetExtraWeightType<ExtraWeight>>tmp(ch->node_count()), dist(get_pinned_source_count());
	get_extra_weight_distances_to_sources(extra_weight, link, tmp, dist);
	return dist; // NVRO
}

template<class ExtraWeight, class LinkFunction, class TmpContainer>
std::vector<detail::GetExtraWeightType<ExtraWeight>> ContractionHierarchyQuery::get_extra_weight_distances_to_sources(
	const ExtraWeight&extra_weight,
	const LinkFunction&link,
	TmpContainer&tmp
){
	std::vector<detail::GetExtraWeightType<ExtraWeight>>dist(get_pinned_source_count());
	get_extra_weight_distances_to_sources(extra_weight, link, tmp, dist);
	return dist; // NVRO
}

inline
unsigned SaturatedWeightAddition::operator()(unsigned l, unsigned r)const{
	assert(l <= inf_weight && "unsigned weight must not be larger than inf_weight");
	assert(r <= inf_weight && "unsigned weight must not be larger than inf_weight");
	if(l >= inf_weight-r)
		return inf_weight;
	else
		return l+r;
}

inline
int SaturatedWeightAddition::operator()(int l, int r)const{
	static_assert(inf_weight == INT_MAX, "this function assumes that inf_weight is INT_MAX");
	if(l > 0){
		if (r > INT_MAX - l){
			return INT_MAX;
		}
	}else if(r < INT_MIN - l){
		return INT_MIN;
	}

    return l + r;
}

namespace detail{
	template<class LinkFunction>
	struct InverseLinkFunction{
		explicit InverseLinkFunction(const LinkFunction&link):link(link){}

		template<class L, class R>
		auto operator()(L&&l, R&&r)const
			-> decltype(std::declval<LinkFunction>()(std::forward<R>(r), std::forward<L>(l)))
		{
			return link(std::forward<R>(r), std::forward<L>(l));
		}

		const LinkFunction&link;
	};

	template<class LinkFunction>
	InverseLinkFunction<LinkFunction>inverse_link_function(const LinkFunction&link){
		return InverseLinkFunction<LinkFunction>(link);
	}

	template<class InputWeightContainer, class LinkFunction>
	struct ShortcutWeights{
		typedef typename std::remove_reference<decltype(std::declval<InputWeightContainer>()[0])>::type Weight;

		const InputWeightContainer&input_weight;
		const LinkFunction&link;

		const ContractionHierarchy&ch;

		ShortcutWeights(const InputWeightContainer&input_weight, const LinkFunction&link, const ContractionHierarchy&ch):
			input_weight(input_weight), link(link), ch(ch){}

		Weight get_forward_weight(unsigned a)const{
			assert(a < ch.forward.is_shortcut_an_original_arc.size());
			if(ch.forward.is_shortcut_an_original_arc.is_set(a)){
				return input_weight[ch.forward.shortcut_first_arc[a]];
			}else{
				return link(get_backward_weight(ch.forward.shortcut_first_arc[a]), get_forward_weight(ch.forward.shortcut_second_arc[a]));
			}
		}

		Weight get_backward_weight(unsigned a)const{
			assert(a < ch.backward.is_shortcut_an_original_arc.size());
			if(ch.backward.is_shortcut_an_original_arc.is_set(a)){
				return input_weight[ch.backward.shortcut_first_arc[a]];
			}else{
				return link(get_backward_weight(ch.backward.shortcut_first_arc[a]), get_forward_weight(ch.backward.shortcut_second_arc[a]));
			}
		}
	};

	template<class WeightT, class LinkFunction>
	struct ShortcutWeights<ContractionHierarchyExtraWeight<WeightT>, LinkFunction>{

		typedef WeightT Weight;

		const ContractionHierarchyExtraWeight<WeightT>&extra_weight;

		explicit ShortcutWeights(const ContractionHierarchyExtraWeight<WeightT>&extra_weight, const LinkFunction&, const ContractionHierarchy&):
			extra_weight(extra_weight){}

		const Weight&get_forward_weight(unsigned a)const{
			return extra_weight.forward_weight[a];
		}

		const Weight&get_backward_weight(unsigned a)const{
			return extra_weight.backward_weight[a];
		}
	};

	template<class ExtraWeight, class LinkFunction>
	ShortcutWeights<ExtraWeight, LinkFunction> make_shortcut_weights(const ExtraWeight&extra_weight, const LinkFunction&link, const ContractionHierarchy&ch){
		return ShortcutWeights<ExtraWeight, LinkFunction>{extra_weight, link, ch};
	}

	template<class ShortcutWeights>
	struct InvertShorcutWeights{
		typedef typename ShortcutWeights::Weight Weight;

		const ShortcutWeights&shortcut_weights;

		explicit InvertShorcutWeights(const ShortcutWeights&shortcut_weights):shortcut_weights(shortcut_weights){}

		auto get_forward_weight(unsigned a)const->decltype(std::declval<ShortcutWeights>().get_backward_weight(a)){
			return shortcut_weights.get_backward_weight(a);
		}

		auto get_backward_weight(unsigned a)const->decltype(std::declval<ShortcutWeights>().get_forward_weight(a)){
			return shortcut_weights.get_forward_weight(a);
		}
	};

	template<class ShortcutWeights>
	InvertShorcutWeights<ShortcutWeights>inverse_shortcut_weights(const ShortcutWeights&shortcut_weights){
		return InvertShorcutWeights<ShortcutWeights>(shortcut_weights);
	}

	template<class GetForwardWeight, class LinkFunction>
	ReturnTypeWhenPassedIntOf<GetForwardWeight> get_extra_weight_up_distance(
		unsigned shortest_path_meeting_node,
		const std::vector<unsigned>&forward_predecessor_node,
		const std::vector<unsigned>&forward_predecessor_arc,
		const GetForwardWeight&get_forward_extra_weight,
		const LinkFunction&link
	){
		using Weight = ReturnTypeWhenPassedIntOf<GetForwardWeight>;

		unsigned x = shortest_path_meeting_node;
		assert(forward_predecessor_node[x] != invalid_id);
		Weight ret = get_forward_extra_weight(forward_predecessor_arc[x]);
		x = forward_predecessor_node[x];

		while(forward_predecessor_node[x] != invalid_id){
			ret = link(get_forward_extra_weight(forward_predecessor_arc[x]), ret);
			x = forward_predecessor_node[x];
		}
		return ret;
	}


	template<class ShortcutWeights, class LinkFunction>
	typename ShortcutWeights::Weight internal_get_extra_weight_distance(
		const ShortcutWeights&shortcut_weights,
		const LinkFunction&link,
		unsigned shortest_path_meeting_node,
		const std::vector<unsigned>&forward_predecessor_node,
		const std::vector<unsigned>&forward_predecessor_arc,
		const std::vector<unsigned>&backward_predecessor_node,
		const std::vector<unsigned>&backward_predecessor_arc
	){
		using Weight = typename ShortcutWeights::Weight;

		if(shortest_path_meeting_node == invalid_id)
			return Weight{};


		auto inverted_link = detail::inverse_link_function(link);

		bool has_up_part = forward_predecessor_node[shortest_path_meeting_node] != invalid_id;
		bool has_down_part = backward_predecessor_node[shortest_path_meeting_node] != invalid_id;

		// This if-then-else chain avoids the need for a neutral element with respect to link

		if(!has_up_part && !has_down_part) {
			return Weight{};
		} else if(has_up_part && has_down_part) {
			return link(
				detail::get_extra_weight_up_distance(
					shortest_path_meeting_node,
					forward_predecessor_node,
					forward_predecessor_arc,
					[&](unsigned a)->decltype(shortcut_weights.get_forward_weight(a)){return shortcut_weights.get_forward_weight(a);},
					link
				),
				detail::get_extra_weight_up_distance(
					shortest_path_meeting_node,
					backward_predecessor_node,
					backward_predecessor_arc,
					[&](unsigned a)->decltype(shortcut_weights.get_backward_weight(a)){return shortcut_weights.get_backward_weight(a);},
					inverted_link
				)
			);
		} else if(has_up_part) {
			return detail::get_extra_weight_up_distance(
				shortest_path_meeting_node,
				forward_predecessor_node,
				forward_predecessor_arc,
				[&](unsigned a)->decltype(shortcut_weights.get_forward_weight(a)){return shortcut_weights.get_forward_weight(a);},
				link
			);
		} else {
			return detail::get_extra_weight_up_distance(
				shortest_path_meeting_node,
				backward_predecessor_node,
				backward_predecessor_arc,
				[&](unsigned a)->decltype(shortcut_weights.get_backward_weight(a)){return shortcut_weights.get_backward_weight(a);},
				inverted_link
			);
		}
	}

}

template<class ExtraWeight, class LinkFunction>
detail::GetExtraWeightType<ExtraWeight> ContractionHierarchyQuery::get_extra_weight_distance(
	const ExtraWeight&extra_weight,
	const LinkFunction&link){
	assert(ch && "query object must have an attached CH");
	assert(state == ContractionHierarchyQuery::InternalState::run);

	auto shortcut_weight = detail::make_shortcut_weights(extra_weight, link, *ch);

	return detail::internal_get_extra_weight_distance(
		shortcut_weight, link,
		shortest_path_meeting_node,
		forward_predecessor_node, forward_predecessor_arc,
		backward_predecessor_node, backward_predecessor_arc
	);
}

template<class Weight> template<class InputWeightContainer, class LinkFunction>
ContractionHierarchyExtraWeight<Weight>& ContractionHierarchyExtraWeight<Weight>::reset(const ContractionHierarchy&ch, const InputWeightContainer&input_extra_weight, const LinkFunction&link){
	const unsigned node_count = ch.node_count();

	forward_weight.resize(ch.forward.weight.size());
	backward_weight.resize(ch.backward.weight.size());

	for(unsigned x=0; x<node_count; ++x){
		for(unsigned xy=ch.forward.first_out[x]; xy<ch.forward.first_out[x+1]; ++xy){
			if(ch.forward.is_shortcut_an_original_arc.is_set(xy)){
				forward_weight[xy] = input_extra_weight[ch.forward.shortcut_first_arc[xy]];
			} else {
				forward_weight[xy] = link(
					backward_weight[ch.forward.shortcut_first_arc[xy]],
					forward_weight[ch.forward.shortcut_second_arc[xy]]
				);
			}
		}
		for(unsigned xy=ch.backward.first_out[x]; xy<ch.backward.first_out[x+1]; ++xy){
			if(ch.backward.is_shortcut_an_original_arc.is_set(xy)){
				backward_weight[xy] = input_extra_weight[ch.backward.shortcut_first_arc[xy]];
			} else {
				backward_weight[xy] = link(
					backward_weight[ch.backward.shortcut_first_arc[xy]],
					forward_weight[ch.backward.shortcut_second_arc[xy]]
				);
			}
		}
	}
	return *this;
}


namespace detail{
	template<class LinkFunction, class ExtraWeight, class TmpContainer, class DistContainer>
	void extract_distances_to_targets(
		const std::vector<unsigned>&target_list,
		unsigned target_count,

		const TimestampFlags&has_forward_predecessor,
		const std::vector<unsigned>&forward_predecessor_node,
		const std::vector<unsigned>&predecessor_arc,

		const ExtraWeight&extra_weight,
		const std::vector<unsigned>&forward_first_out,
		const std::vector<unsigned>&forward_head,
		const std::vector<unsigned>&backward_first_out,
		const std::vector<unsigned>&backward_head,

		TmpContainer&source_to_node_distance,
		TimestampFlags&has_source_to_node_distance,

		DistContainer&output,

		std::vector<unsigned>&stack,

		const LinkFunction&link
	){
		using Weight = typename ExtraWeight::Weight;

		has_source_to_node_distance.reset_all();

		unsigned nodes_on_stack_with_forward_precessor_count = 0;
		unsigned stack_size = 0;

		auto push = [&](unsigned x){
			assert(stack_size < stack.size());
			stack[stack_size++] = x;
		};

		auto pop = [&]{
			assert(stack_size != 0);
			return stack[--stack_size];
		};

		auto single_forward_step_expand_distance_to_node = [&](unsigned x){
			assert(!has_source_to_node_distance.is_set(x));
			assert(has_forward_predecessor.is_set(x));
			assert(forward_predecessor_node[x] != invalid_id);

			unsigned a = predecessor_arc[x];
			unsigned p = forward_predecessor_node[x];

			if(has_source_to_node_distance.is_set(p))
				source_to_node_distance[x] = link(source_to_node_distance[p], extra_weight.get_forward_weight(a));
			else
				source_to_node_distance[x] = extra_weight.get_forward_weight(a); // p is source
			has_source_to_node_distance.set(x);
		};

		auto single_backward_step_expand_distance_to_node = [&](unsigned x){
			assert(!has_source_to_node_distance.is_set(x));
			assert(!has_forward_predecessor.is_set(x));

			unsigned a = predecessor_arc[x];
			unsigned p = backward_head[a];

			if(has_source_to_node_distance.is_set(p))
				source_to_node_distance[x] = link(source_to_node_distance[p], extra_weight.get_backward_weight(a));
			else
				source_to_node_distance[x] = extra_weight.get_backward_weight(a); // p is source

			has_source_to_node_distance.set(x);
		};

		auto push_non_reached_nodes = [&](unsigned x){
			assert(!has_source_to_node_distance.is_set(x));
			while(!has_forward_predecessor.is_set(x)){
				push(x);
				unsigned y = backward_head[predecessor_arc[x]];
				assert(y > x);
				x = y;
				if(has_source_to_node_distance.is_set(x))
					return;
			}
			while(forward_predecessor_node[x] != invalid_id){
				assert(has_forward_predecessor.is_set(x));
				push(x);
				++nodes_on_stack_with_forward_precessor_count;
				unsigned y = forward_predecessor_node[x];
				assert(y < x);
				x = y;
				if(has_source_to_node_distance.is_set(x))
					return;
			}
			source_to_node_distance[x] = Weight{}; // x is source node
		};

		auto expand_distances_from_source_to_node = [&](unsigned x){
			if(!has_source_to_node_distance.is_set(x)){
				push_non_reached_nodes(x);
				while(stack_size != 0){
					unsigned y = pop();
					if(nodes_on_stack_with_forward_precessor_count != 0){
						--nodes_on_stack_with_forward_precessor_count;
						single_forward_step_expand_distance_to_node(y);
					}else{
						single_backward_step_expand_distance_to_node(y);
					}
				}
			}
		};

		for(unsigned i=0; i<target_count; ++i){
			unsigned t = target_list[i];
			assert(t != invalid_id);

			if(!has_forward_predecessor.is_set(t) && predecessor_arc[t] == invalid_id){
				output[i] = Weight{}; // t is not reachable
			}else{
				expand_distances_from_source_to_node(t);
				output[i] = source_to_node_distance[t];
			}
		}
	}
}

template<class ExtraWeight, class LinkFunction, class TmpContainer, class DistContainer>
ContractionHierarchyQuery& ContractionHierarchyQuery::get_extra_weight_distances_to_targets(
	const ExtraWeight&extra_weight,
	const LinkFunction&link,
	TmpContainer&tmp,
	DistContainer&dist
){
	assert(state == ContractionHierarchyQuery::InternalState::target_run);

	auto shortcut_weight = detail::make_shortcut_weights(extra_weight, link, *ch);

	detail::extract_distances_to_targets(
		backward_predecessor_node, many_to_many_source_or_target_count,

		was_forward_pushed,
		forward_predecessor_node, forward_predecessor_arc,

		shortcut_weight,
		ch->forward.first_out,
		ch->forward.head,
		ch->backward.first_out,
		ch->backward.head,

		tmp,
		was_backward_pushed,

		dist,

		backward_predecessor_arc,

		link
	);

	return *this;
}

template<class ExtraWeight, class LinkFunction, class TmpContainer, class DistContainer>
ContractionHierarchyQuery& ContractionHierarchyQuery::get_extra_weight_distances_to_sources(
	const ExtraWeight&extra_weight,
	const LinkFunction&link,
	TmpContainer&tmp,
	DistContainer&dist
){
	assert(state == ContractionHierarchyQuery::InternalState::source_run);

	auto inverted_link = detail::inverse_link_function(link);

	auto shortcut_weight = detail::make_shortcut_weights(extra_weight, link, *ch);
	auto inverted_shortcut_weight = detail::inverse_shortcut_weights(shortcut_weight);

	detail::extract_distances_to_targets(
		forward_predecessor_node, many_to_many_source_or_target_count,

		was_backward_pushed,
		backward_predecessor_node, backward_predecessor_arc,

		inverted_shortcut_weight,
		ch->backward.first_out,
		ch->backward.head,
		ch->forward.first_out,
		ch->forward.head,

		tmp,
		was_forward_pushed,

		dist,

		forward_predecessor_arc,

		inverted_link
	);

	return *this;
}

extern template struct ContractionHierarchyExtraWeight<unsigned>;
extern template struct ContractionHierarchyExtraWeight<int>;
extern template ContractionHierarchyQuery& ContractionHierarchyQuery::get_extra_weight_distances_to_targets<std::vector<int>, SaturatedWeightAddition, std::vector<int>, std::vector<int>>(const std::vector<int>&, const SaturatedWeightAddition&, std::vector<int>&, std::vector<int>&);
extern template ContractionHierarchyQuery& ContractionHierarchyQuery::get_extra_weight_distances_to_sources<std::vector<int>, SaturatedWeightAddition, std::vector<int>, std::vector<int>>(const std::vector<int>&, const SaturatedWeightAddition&, std::vector<int>&, std::vector<int>&);
extern template ContractionHierarchyQuery& ContractionHierarchyQuery::get_extra_weight_distances_to_targets<std::vector<unsigned>, SaturatedWeightAddition, std::vector<unsigned>, std::vector<unsigned>>(const std::vector<unsigned>&, const SaturatedWeightAddition&, std::vector<unsigned>&, std::vector<unsigned>&);
extern template ContractionHierarchyQuery& ContractionHierarchyQuery::get_extra_weight_distances_to_sources<std::vector<unsigned>, SaturatedWeightAddition, std::vector<unsigned>, std::vector<unsigned>>(const std::vector<unsigned>&, const SaturatedWeightAddition&, std::vector<unsigned>&, std::vector<unsigned>&);
extern template ContractionHierarchyQuery& ContractionHierarchyQuery::get_extra_weight_distances_to_targets<ContractionHierarchyExtraWeight<int>, SaturatedWeightAddition, std::vector<int>, std::vector<int>>(const ContractionHierarchyExtraWeight<int>&, const SaturatedWeightAddition&, std::vector<int>&, std::vector<int>&);
extern template ContractionHierarchyQuery& ContractionHierarchyQuery::get_extra_weight_distances_to_sources<ContractionHierarchyExtraWeight<int>, SaturatedWeightAddition, std::vector<int>, std::vector<int>>(const ContractionHierarchyExtraWeight<int>&, const SaturatedWeightAddition&, std::vector<int>&, std::vector<int>&);
extern template ContractionHierarchyQuery& ContractionHierarchyQuery::get_extra_weight_distances_to_targets<ContractionHierarchyExtraWeight<unsigned>, SaturatedWeightAddition, std::vector<unsigned>, std::vector<unsigned>>(const ContractionHierarchyExtraWeight<unsigned>&, const SaturatedWeightAddition&, std::vector<unsigned>&, std::vector<unsigned>&);
extern template ContractionHierarchyQuery& ContractionHierarchyQuery::get_extra_weight_distances_to_sources<ContractionHierarchyExtraWeight<unsigned>, SaturatedWeightAddition, std::vector<unsigned>, std::vector<unsigned>>(const ContractionHierarchyExtraWeight<unsigned>&, const SaturatedWeightAddition&, std::vector<unsigned>&, std::vector<unsigned>&);
extern template ContractionHierarchyExtraWeight<unsigned>& ContractionHierarchyExtraWeight<unsigned>::reset<std::vector<unsigned>, SaturatedWeightAddition>(const ContractionHierarchy&ch, const std::vector<unsigned>&, const SaturatedWeightAddition&);
extern template ContractionHierarchyExtraWeight<int>& ContractionHierarchyExtraWeight<int>::reset<std::vector<int>, SaturatedWeightAddition>(const ContractionHierarchy&ch, const std::vector<int>&, const SaturatedWeightAddition&);
extern template unsigned ContractionHierarchyQuery::get_extra_weight_distance<std::vector<unsigned>,SaturatedWeightAddition>(const std::vector<unsigned>&, const SaturatedWeightAddition&);
extern template int ContractionHierarchyQuery::get_extra_weight_distance<std::vector<int>,SaturatedWeightAddition>(const std::vector<int>&, const SaturatedWeightAddition&);
extern template unsigned ContractionHierarchyQuery::get_extra_weight_distance<ContractionHierarchyExtraWeight<unsigned>,SaturatedWeightAddition>(const ContractionHierarchyExtraWeight<unsigned>&, const SaturatedWeightAddition&);
extern template int ContractionHierarchyQuery::get_extra_weight_distance<ContractionHierarchyExtraWeight<int>,SaturatedWeightAddition>(const ContractionHierarchyExtraWeight<int>&, const SaturatedWeightAddition&);

} // namespace RoutingKit

#endif
