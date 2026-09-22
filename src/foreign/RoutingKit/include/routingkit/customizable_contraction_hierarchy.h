#ifndef ROUTING_KIT_CUSTOMIZABLE_CONSTRACTION_HIERARCHY_H
#define ROUTING_KIT_CUSTOMIZABLE_CONSTRACTION_HIERARCHY_H

#include <routingkit/constants.h>
#include <routingkit/id_set_queue.h>
#include <routingkit/bit_vector.h>
#include <routingkit/id_mapper.h>

#include <vector>
#include <string>
#include <functional>

namespace RoutingKit{

class ContractionHierarchy;

struct CustomizableContractionHierarchy{
	CustomizableContractionHierarchy(){}

	CustomizableContractionHierarchy(std::vector<unsigned>order, std::vector<unsigned>tail, std::vector<unsigned>head, std::function<void(const std::string&)>log_message = [](const std::string&){}, bool filter_always_inf_arcs = false);

	unsigned node_count()const{
		return rank.size();
	}

	unsigned input_arc_count() const {
		return input_arc_to_cch_arc.size();
	}

	unsigned cch_arc_count() const {
		return up_head.size();
	}

// private:
	std::vector<unsigned>order;
	std::vector<unsigned>rank;
	
	std::vector<unsigned>elimination_tree_parent;
	
	std::vector<unsigned>up_first_out;
	std::vector<unsigned>up_head;
	std::vector<unsigned>up_tail;
	
	std::vector<unsigned>down_first_out;
	std::vector<unsigned>down_head;
	std::vector<unsigned>down_to_up;
	
	std::vector<unsigned>input_arc_to_cch_arc;
	BitVector is_input_arc_upward;

	BitVector does_cch_arc_have_input_arc;
	LocalIDMapper does_cch_arc_have_input_arc_mapper;

	std::vector<unsigned>forward_input_arc_of_cch;
	std::vector<unsigned>backward_input_arc_of_cch;

	BitVector does_cch_arc_have_extra_input_arc;
	LocalIDMapper does_cch_arc_have_extra_input_arc_mapper;

	std::vector<unsigned>first_extra_forward_input_arc_of_cch;
	std::vector<unsigned>first_extra_backward_input_arc_of_cch;
	
	std::vector<unsigned>extra_forward_input_arc_of_cch;
	std::vector<unsigned>extra_backward_input_arc_of_cch;
};

struct CustomizableContractionHierarchyMetric{
	CustomizableContractionHierarchyMetric(){}
	CustomizableContractionHierarchyMetric(const CustomizableContractionHierarchy&cch, const unsigned*input_weight);
	CustomizableContractionHierarchyMetric(const CustomizableContractionHierarchy&cch, const std::vector<unsigned>&input_weight);

	CustomizableContractionHierarchyMetric& reset(const CustomizableContractionHierarchy&cch, const unsigned*input_weight);
	CustomizableContractionHierarchyMetric& reset(const CustomizableContractionHierarchy&cch, const std::vector<unsigned>&input_weight);

	CustomizableContractionHierarchyMetric& reset(const unsigned*input_weight);
	CustomizableContractionHierarchyMetric& reset(const std::vector<unsigned>&input_weight);

	CustomizableContractionHierarchyMetric& customize();

	ContractionHierarchy build_contraction_hierarchy_using_perfect_witness_search();

// private:
	std::vector<unsigned>forward;
	std::vector<unsigned>backward;
	const CustomizableContractionHierarchy*cch;
	const unsigned*input_weight;

};

struct CustomizableContractionHierarchyParallelization{
	CustomizableContractionHierarchyParallelization(){}
	explicit CustomizableContractionHierarchyParallelization(const CustomizableContractionHierarchy&cch);

	CustomizableContractionHierarchyParallelization& reset(const CustomizableContractionHierarchy&cch){
		*this = CustomizableContractionHierarchyParallelization(cch);
		return *this;
	}


	CustomizableContractionHierarchyParallelization& customize(CustomizableContractionHierarchyMetric&metric);
	CustomizableContractionHierarchyParallelization& customize(CustomizableContractionHierarchyMetric&metric, unsigned thread_count);

// private:
	std::vector<unsigned>first_arc_of_level;
	std::vector<unsigned>arcs_ordered_by_level;
	const CustomizableContractionHierarchy*cch;

};

struct CustomizableContractionHierarchyPartialCustomization{
	CustomizableContractionHierarchyPartialCustomization(){}
	explicit CustomizableContractionHierarchyPartialCustomization(const CustomizableContractionHierarchy&cch);

	CustomizableContractionHierarchyPartialCustomization&reset();
	CustomizableContractionHierarchyPartialCustomization&reset(const CustomizableContractionHierarchy&cch);

	CustomizableContractionHierarchyPartialCustomization&update_arc(unsigned xy);
	CustomizableContractionHierarchyPartialCustomization&customize(CustomizableContractionHierarchyMetric&metric);

// private:
	IDSetMinQueue q;
	const CustomizableContractionHierarchy*cch;
};

struct CustomizableContractionHierarchyQuery{
	CustomizableContractionHierarchyQuery(){}
	explicit CustomizableContractionHierarchyQuery(const CustomizableContractionHierarchyMetric&metric);

	CustomizableContractionHierarchyQuery&reset();
	CustomizableContractionHierarchyQuery&reset(const CustomizableContractionHierarchyMetric&metric);

	CustomizableContractionHierarchyQuery&add_source(unsigned s, unsigned dist_to_s = 0);
	CustomizableContractionHierarchyQuery&add_target(unsigned t, unsigned dist_to_t = 0);

	CustomizableContractionHierarchyQuery&run();

	unsigned get_used_source();
	unsigned get_used_target();

	unsigned get_distance();
	std::vector<unsigned> get_node_path();
	std::vector<unsigned> get_arc_path();

	// One-To-Many
	CustomizableContractionHierarchyQuery& reset_source();
	CustomizableContractionHierarchyQuery& pin_targets(const std::vector<unsigned>&);
	CustomizableContractionHierarchyQuery& run_to_pinned_targets();

	CustomizableContractionHierarchyQuery& get_distances_to_targets(unsigned*dist);
	std::vector<unsigned> get_distances_to_targets();

	// Many-To-One
	CustomizableContractionHierarchyQuery& reset_target();
	CustomizableContractionHierarchyQuery& pin_sources(const std::vector<unsigned>&);
	CustomizableContractionHierarchyQuery& run_to_pinned_sources();

	CustomizableContractionHierarchyQuery& get_distances_to_sources(unsigned*dist);
	std::vector<unsigned> get_distances_to_sources();

// private:
	std::vector<unsigned>forward_tentative_distance, backward_tentative_distance;
	std::vector<unsigned>source_node;
	std::vector<unsigned>source_elimination_tree_end;
	std::vector<unsigned>target_node;
	std::vector<unsigned>target_elimination_tree_end;

	std::vector<unsigned>forward_predecessor_node, backward_predecessor_node;

	std::vector<bool>in_forward_search_space, in_backward_search_space;
	
	unsigned shortest_path_meeting_node;

	const CustomizableContractionHierarchy*cch;
	const CustomizableContractionHierarchyMetric*metric;
	unsigned state;
};

} // namespace RoutingKit

#endif
