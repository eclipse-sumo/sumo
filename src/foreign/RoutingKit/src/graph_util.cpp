#include <routingkit/graph_util.h>
#include <routingkit/constants.h>
#include <routingkit/sort.h>
#include <routingkit/permutation.h>

#include <assert.h>
#include <algorithm>

namespace RoutingKit{

unsigned find_arc(const std::vector<unsigned>&first_out, const std::vector<unsigned>&head, unsigned x, unsigned y){
	unsigned ret = find_arc_or_return_invalid(first_out, head, x, y);
	assert(ret != invalid_id && "arc does not exist");
	return ret;
}

unsigned find_arc_or_return_invalid(const std::vector<unsigned>&first_out, const std::vector<unsigned>&head, unsigned x, unsigned y){
	assert(x < first_out.size()-1 && "node id out of bounds");
	assert(y < first_out.size()-1 && "node id out of bounds");

	for(unsigned xy = first_out[x]; xy < first_out[x+1]; ++xy)
		if(head[xy] == y)
			return xy;
	return invalid_id;
}

unsigned find_arc_given_sorted_head(const std::vector<unsigned>&first_out, const std::vector<unsigned>&head, unsigned x, unsigned y){
	unsigned ret = find_arc_or_return_invalid_given_sorted_head(first_out, head, x, y);
	assert(ret != invalid_id && "arc does not exist");
	return ret;
}

unsigned find_arc_or_return_invalid_given_sorted_head(const std::vector<unsigned>&first_out, const std::vector<unsigned>&head, unsigned x, unsigned y){
	assert(x < first_out.size()-1 && "node id out of bounds");
	assert(y < first_out.size()-1 && "node id out of bounds");

	auto
		begin = head.begin() + first_out[x],
		end = head.begin() + first_out[x+1];

	assert(std::is_sorted(begin, end) && "heads are not sorted");
	auto
		pos = std::lower_bound(begin, end, y);
	if(pos == end)
		return invalid_id;
	if(*pos != y)
		return invalid_id;
	return pos - head.begin();
}


std::vector<unsigned>convert_node_path_to_arc_path(const std::vector<unsigned>&first_out, const std::vector<unsigned>&head, std::vector<unsigned>path){
	if(!path.empty()){
		for(unsigned i=0; i<path.size()-1; ++i) {
			for(unsigned xy=first_out[path[i]]; ; ++xy) {
				assert(xy<first_out[path[i]+1]);
				if(head[xy] == path[i+1]) {
					path[i] = xy;
					break;
				}
			}
		}
		path.pop_back();
	}
	return path;
}


std::vector<unsigned>convert_arc_path_to_node_path(unsigned source, const std::vector<unsigned>&head, std::vector<unsigned>path){
	if(!path.empty()){
		path.resize(path.size()+1);
		for(unsigned i=path.size()-1; i>1; --i)
			path[i] = head[path[i-1]];
		path[0] = source;
	}
	return path;
}




std::vector<unsigned>compute_inverse_sort_permutation_first_by_left_then_by_right(
	unsigned a_count,
	const std::vector<unsigned>&a,
	unsigned b_count,
	const std::vector<unsigned>&b
){
	auto p = compute_inverse_stable_sort_permutation_using_key(b, b_count, [](unsigned x){return x;});
	auto q = compute_inverse_stable_sort_permutation_using_key(apply_inverse_permutation(p, std::move(a)), a_count, [](unsigned x){return x;});
	assert(is_sorted_using_less(a));
	return chain_permutation_first_left_then_right(q, p);
}

std::vector<unsigned>compute_sort_permutation_first_by_left_then_by_right(
	unsigned a_count,
	const std::vector<unsigned>&a,
	unsigned b_count,
	const std::vector<unsigned>&b
){
	auto p = compute_stable_sort_permutation_using_key(b, b_count, [](unsigned x){return x;});
	auto q = compute_stable_sort_permutation_using_key(apply_permutation(p, std::move(a)), a_count, [](unsigned x){return x;});
	assert(is_sorted_using_less(a));
	return chain_permutation_first_left_then_right(p, q);
}

std::vector<unsigned>compute_sort_permutation_first_by_left_then_by_right_and_apply_sort_to_left(
	unsigned a_count,
	std::vector<unsigned>&a,
	unsigned b_count,
	const std::vector<unsigned>&b
){
	auto p = compute_stable_sort_permutation_using_key(b, b_count, [](unsigned x){return x;});
	a = apply_permutation(p, std::move(a));
	auto q = compute_stable_sort_permutation_using_key(a, a_count, [](unsigned x){return x;});
	a = apply_permutation(q, std::move(a));
	assert(is_sorted_using_less(a));
	return chain_permutation_first_left_then_right(p, q);
}

std::vector<unsigned>compute_inverse_sort_permutation_first_by_left_then_by_right_and_apply_sort_to_left(
	unsigned a_count,
	std::vector<unsigned>&a,
	unsigned b_count,
	const std::vector<unsigned>&b
){
	auto p = compute_inverse_stable_sort_permutation_using_key(b, b_count, [](unsigned x){return x;});
	a = apply_inverse_permutation(p, std::move(a));
	auto q = compute_inverse_stable_sort_permutation_using_key(a, a_count, [](unsigned x){return x;});
	a = apply_inverse_permutation(q, std::move(a));
	assert(is_sorted_using_less(a));
	return chain_permutation_first_left_then_right(q, p);
}

std::vector<unsigned>compute_inverse_sort_permutation_first_by_tail_then_by_head_and_apply_sort_to_tail(
	unsigned node_count,
	std::vector<unsigned>&tail,
	const std::vector<unsigned>&head
){
	return compute_inverse_sort_permutation_first_by_left_then_by_right_and_apply_sort_to_left(node_count, tail, node_count, head);
}

std::vector<unsigned>compute_sort_permutation_first_by_tail_then_by_head_and_apply_sort_to_tail(
	unsigned node_count,
	std::vector<unsigned>&tail,
	const std::vector<unsigned>&head
){
	return compute_sort_permutation_first_by_left_then_by_right_and_apply_sort_to_left(node_count, tail, node_count, head);
}

std::vector<unsigned>compute_inverse_sort_permutation_first_by_tail_then_by_head(
	unsigned node_count,
	const std::vector<unsigned>&tail,
	const std::vector<unsigned>&head
){
	return compute_inverse_sort_permutation_first_by_left_then_by_right(node_count, tail, node_count, head);
}

std::vector<unsigned>compute_sort_permutation_first_by_tail_then_by_head(
	unsigned node_count,
	const std::vector<unsigned>&tail,
	const std::vector<unsigned>&head
){
	return compute_sort_permutation_first_by_left_then_by_right(node_count, tail, node_count, head);
}

} // RoutingKit

