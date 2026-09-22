#ifndef ROUTING_KIT_NESTED_DISSECTION_H
#define ROUTING_KIT_NESTED_DISSECTION_H

#include <routingkit/bit_vector.h>
#include <routingkit/constants.h>

#include <vector>
#include <stdexcept>
#include <functional>
#include <string>

namespace RoutingKit{

struct GraphFragment{
	std::vector<unsigned>global_node_id;
	std::vector<unsigned>first_out;
	std::vector<unsigned>tail;
	std::vector<unsigned>head;
	std::vector<unsigned>back_arc;

	unsigned node_count()const{
		return global_node_id.size();
	}

	unsigned arc_count()const{
		return tail.size();
	}
};

GraphFragment make_graph_fragment(unsigned node_count, const std::vector<unsigned>&tail, const std::vector<unsigned>&head);

std::vector<GraphFragment>decompose_graph_fragment_into_connected_components(GraphFragment fragment);

struct CutSide{
	unsigned node_on_side_count;
	unsigned cut_size;
	BitVector is_node_on_side;
};

void pick_smaller_side(CutSide&cut);

class BlockingFlow{
private:
	const GraphFragment*fragment;
	BitVector is_source;
	BitVector is_target;

	unsigned flow_intensity;
	
	BitVector is_arc_saturated;
	BitVector is_arc_blocked;

	bool is_finished_flag;
public:
	BlockingFlow(){}
	BlockingFlow(const GraphFragment&, BitVector is_source, BitVector is_target);

	CutSide get_source_cut();
	CutSide get_target_cut();
	CutSide get_balanced_cut();

	void advance();

	unsigned get_current_flow_intensity()const{
		return flow_intensity;
	}

	bool is_finished()const{
		return is_finished_flag;
	}
};

CutSide inertial_flow(
	const GraphFragment&fragment,
	unsigned min_balance,
	const std::vector<float>&latitude, const std::vector<float>&longitude,
	const std::function<void(const std::string&)>&log_message = [](const std::string&){}
);

CutSide inertial_flow(
	const GraphFragment&fragment,
	const std::vector<float>&latitude, const std::vector<float>&longitude,
	const std::function<void(const std::string&)>&log_message = [](const std::string&){}
);

BitVector derive_separator_from_cut(const GraphFragment&fragment, const BitVector&cut);

struct SeparatorDecomposition{
  struct Node{
    unsigned left_child;
    unsigned right_sibling;
    unsigned first_separator_vertex;
    unsigned last_separator_vertex;
  };

  std::vector<Node>tree;
  std::vector<unsigned>order;
};

SeparatorDecomposition compute_separator_decomposition(
	GraphFragment fragment,
	const std::function<BitVector(const GraphFragment&)>&compute_separator,
	const std::function<void(const std::string&)>&log_message = [](const std::string&){}
);

std::vector<unsigned>compute_nested_node_dissection_order(
	GraphFragment fragment,
	const std::function<BitVector(const GraphFragment&)>&compute_separator,
	const std::function<void(const std::string&)>&log_message = [](const std::string&){}
);

std::vector<unsigned>compute_nested_node_dissection_order_using_inertial_flow(
	unsigned node_count,
	const std::vector<unsigned>&tail, const std::vector<unsigned>&head,
	const std::vector<float>&latitude, const std::vector<float>&longitude,
	const std::function<void(const std::string&)>&log_message = [](const std::string&){}
);

} // RoutingKit

#endif

