#pragma once
#include "param.h"
#include "../common/common.h"
#include "../common/hash.h"

using namespace std;

/*
	all_level_array:
	end_level[level_number] = start_level[level_number - 1]
	             ↓
	[e, e, e, e, e, e, e, e, e, ... e, 0, 0, 0]
	 ↑                                 ↑
	start_level[level_number]     end_level[0]
*/

class LightPart
{
	uint32_t random_value;
	int is_level_zero_sorted_;
	int level_number;
	int min_level_length;
	elem_t **start_level, **end_level;
	elem_t *all_level_array;
	int max_array_length_total;
	double level_mult[MAX_POSSIBLE_LEVEL_NUMBER];
	int max_level_length_reverse[MAX_POSSIBLE_LEVEL_NUMBER];
    
public:

	void prepare_config_top_level_length(int memory){
		int target_array_length = memory / sizeof(elem_t);
		int l = 0, r = TOTAL_ELEMENTS;
		while (l < r){
			int mid = (l + r) >> 1;
			int current_length = calculate_array_length(mid);
			if (current_length > target_array_length) r = mid;
			else l = mid + 1;
		}
		config.top_level_length = l;
	}

	LightPart(int _tot_memory_in_bytes = -1)
	{
		if (_tot_memory_in_bytes > 0){
			prepare_config_top_level_length(_tot_memory_in_bytes);
		}
		srand(time(NULL));
		random_value = rand();
		is_level_zero_sorted_ = 0;
		level_number = config.level_number;

		level_mult[0] = 1;
		for (int i = 1; i < MAX_POSSIBLE_LEVEL_NUMBER; i++){
			level_mult[i] = level_mult[i-1] * config.c;
		}
		
		max_array_length_total = calculate_array_length();
		all_level_array = new elem_t[max_array_length_total];
		start_level = new elem_t*[MAX_POSSIBLE_LEVEL_NUMBER];
		end_level = new elem_t*[MAX_POSSIBLE_LEVEL_NUMBER];

		for (int i = 0; i < level_number; i++){
			start_level[i] = end_level[i] = all_level_array;
		}
	}
	~LightPart()
	{
		delete[] all_level_array;
		delete[] start_level;
		delete[] end_level;
	}

	int calculate_array_length(int length = config.top_level_length, double c = config.c){
		max_level_length_reverse[0] = length;
		for (int i = 1; i < MAX_POSSIBLE_LEVEL_NUMBER; i++)
			max_level_length_reverse[i] = max(MIN_LEVEL_LENGTH, (int)(max_level_length_reverse[i-1] / config.c));
		// return int((double) length * c / (c - 1)) + MIN_LEVEL_LENGTH * MAX_POSSIBLE_LEVEL_NUMBER;
		int used_level = 1;
		while (true){
			int capacity = 0, array_length = 0;
			for (int i = 0; i < used_level; i++){
				capacity += max_level_length_reverse[used_level - 1 - i] << i;
				array_length += max_level_length_reverse[used_level - 1 - i];
			}
			if (capacity >= TOTAL_ELEMENTS) return array_length;
			used_level++;
		}
		return -1;
	}

	// void clear()
	// {
	// 	is_level_zero_sorted_ = 0;
	// 	level_number = 0;
	// 	min_level_length = 0;
	// 	memset(start_level, 0, sizeof(start_level));
	// 	memset(end_level, 0, sizeof(end_level));
	// elem_t **start_level, **end_level;
	// elem_t *all_level_array;

	// }

	bool level_x_is_full(int x){
		return (end_level[x] - start_level[x]) >= max_level_length_reverse[level_number - 1 - x];
	}

	int random_bit(){
		random_value = Hash(random_value, 1024);
		return random_value & 1;
	}

	elem_t randomly_halve_down(elem_t* start, elem_t* &end, bool odd_flag){
		int offset = random_bit();
		int length = end - start;
		elem_t result = 0;
		if (odd_flag){
			length--;
			result = start[length];
		}
		for (int i = 0; i < length; i += 2){
			start[i / 2] = start[i + offset];
		}
		end = start + length / 2;
		return result;
	}

	void expand_new_level(){
		start_level[level_number] = end_level[level_number] = all_level_array;
		++level_number;
		assert(level_number < MAX_POSSIBLE_LEVEL_NUMBER);
	}

	void merge_sort(elem_t *start_arr1, elem_t* &end_arr1, elem_t *start_arr2, elem_t *end_arr2){
		elem_t *tmp_arr = new elem_t[(end_arr1 - start_arr1) + (end_arr2 - start_arr2)];
		elem_t *it1 = start_arr1, *it2 = start_arr2, *tmp_it = tmp_arr;
		while ((it1 != end_arr1) || (it2 != end_arr2)){
			if (it1 == end_arr1){
				while (it2 != end_arr2){
					*tmp_it = *it2;
					++it2;
					++tmp_it;
				}
			} else if (it2 == end_arr2){
				while (it1 != end_arr1){
					*tmp_it = *it1;
					++it1;
					++tmp_it;
				}
			} else if (*it1 < *it2){
				*tmp_it = *it1;
				++it1;
				++tmp_it;
			} else {
				*tmp_it = *it2;
				++it2;
				++tmp_it;
			}
		}
		memcpy(start_arr1, tmp_arr, sizeof(elem_t) * ((end_arr1 - start_arr1) + (end_arr2 - start_arr2)));
		end_arr1 = start_arr1 + ((end_arr1 - start_arr1) + (end_arr2 - start_arr2));
		delete[] tmp_arr;
	}

	void compress_up(){
		if (!is_level_zero_sorted_){
			sort(start_level[0], end_level[0]);
			is_level_zero_sorted_ = 1;
		}
		int current_level = 0;
		while (true){
			bool odd_flag = false;
			if ((end_level[current_level] - start_level[current_level]) % 2 == 1) odd_flag = true;
			elem_t rest_e = randomly_halve_down(start_level[current_level], end_level[current_level], odd_flag);

			if (current_level + 1 == level_number)
				expand_new_level();
			merge_sort(start_level[current_level + 1], end_level[current_level + 1],
				start_level[current_level], end_level[current_level]);
			if (odd_flag){
				start_level[current_level] = end_level[current_level];
				end_level[current_level] = end_level[current_level] + 1;
				start_level[current_level][0] = rest_e;
			} else {
				start_level[current_level] = end_level[current_level];
			}
			if (!level_x_is_full(current_level + 1)){
				while (current_level >= 0){
					if (start_level[current_level] == end_level[current_level]){
						start_level[current_level] = end_level[current_level] = end_level[current_level + 1];
					} else {
						end_level[current_level + 1][0] = start_level[current_level][0];
						start_level[current_level] = end_level[current_level + 1];
						end_level[current_level] = start_level[current_level] + 1;
					}
					--current_level;
				}
				break;
			}
			++current_level;
		}
	}

/* insertion */
	void insert_single(elem_t e){
		if (level_x_is_full(0)){
			compress_up();
		}
		end_level[0][0] = e;
		end_level[0]++;
		is_level_zero_sorted_ = 0;
	}

	void insert(elem_t key, int f = 1)
	{
	#ifdef DEBUG_FLAG
		int old_total_items = check_insert_number() + f;
	#endif
		for (int i = 0; i < f; i++)
			insert_single(key);
	#ifdef DEBUG_FLAG
		int new_total_items = check_insert_number();
		assert(old_total_items == new_total_items);
	#endif
	}



/* quantile */
	uint32_t find_quantile_by_element(elem_t key) 
	{
		uint32_t result = 0;
		if (!is_level_zero_sorted_){
			sort(start_level[0], end_level[0]); // slow, if insert\query\insert\query  todo
			is_level_zero_sorted_ = 1;
		}
		for (int i = 0, mult = 1; i < level_number; i++, mult *= 2){
			elem_t *it = lower_bound(start_level[i], end_level[i], key);
			result += mult * (it - start_level[i]);
		}
		return result;
    }

	void prepare_all_element(vector<WeightedElement> &all_element)
	{
		for (int i = 0, mult = 1; i < level_number; i++, mult *= 2){
			for (elem_t *it = start_level[i]; it < end_level[i]; it++){
				all_element.push_back((WeightedElement){
					*it,
					mult
				});
			}
		}
    }

	int check_insert_number(){
		int result = 0;
		for (int i = 0, mult = 1; i < level_number; i++, mult *= 2){
			result += mult * (end_level[i] - start_level[i]);
		}
		return result;
	}

	void find_element_by_quantile(WeightedElement* &current_element) 
	{
		for (int i = 0, mult = 1; i < level_number; i++, mult *= 2){
			for (elem_t *it = start_level[i]; it < end_level[i]; it++){
				*current_element = (WeightedElement){
					*it,
					mult
				};
				++current_element;
			}
		}
    }


/* other measurement task */
    int get_memory_usage() { return max_array_length_total * sizeof(elem_t); }
	int max_possible_item(){
		int result = 0;
		for (int i = 0; i < level_number; i++)
			result += max_level_length_reverse[i];
		return result;
	}
};
