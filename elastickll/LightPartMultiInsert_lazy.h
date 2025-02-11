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

/*
	lazy compress
	init top_level_length = +inf
	0. current memory > total memory
	1. check each level limit, compress if exceed
	2. no compression, change top_level_length to activate compress
*/

class LightPart
{
	uint32_t random_value;
	// int is_level_zero_sorted_;
    int is_all_level_sorted_;
	int level_number;
	int min_level_length;
	// elem_t **start_level, **end_level;
	// elem_t *all_level_array;
	int max_array_length_total;
	double level_mult[MAX_POSSIBLE_LEVEL_NUMBER];
	int max_level_length_reverse[MAX_POSSIBLE_LEVEL_NUMBER];
    vector<vector<elem_t> > arr;
	int insert_distribution[50];

	int current_counter, max_counter, current_top_level_length;
	int max_counter_level[MAX_POSSIBLE_LEVEL_NUMBER];

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
		current_counter = 0;
		max_counter = _tot_memory_in_bytes / sizeof(elem_t);
		current_top_level_length = max_counter;

		memset(insert_distribution, 0, sizeof(insert_distribution));
		srand(time(NULL));
		random_value = rand();
		// is_level_zero_sorted_ = 0;
        is_all_level_sorted_ = 0;
		// level_number = config.level_number;
		level_number = 1;
		max_counter_level[0] = current_top_level_length;

		level_mult[0] = 1;
		for (int i = 1; i < MAX_POSSIBLE_LEVEL_NUMBER; i++){
			level_mult[i] = level_mult[i-1] * config.c;
		}

		// max_array_length_total = calculate_array_length();
        arr.clear();
		for (int i = 0; i < level_number; i++){
            vector<elem_t> row_i;
            arr.push_back(row_i);
        }
	}
	~LightPart()
	{
		// delete[] all_level_array;
		// delete[] start_level;
		// delete[] end_level;
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
		// return (end_level[x] - start_level[x]) >= max_level_length_reverse[x];
        return arr[x].size() >= max_level_length_reverse[level_number - 1 - x];
	}

	int random_bit(){
		random_value = Hash(random_value, 1024);
		return random_value & 1;
	}

	elem_t randomly_halve_down(int current_level, bool odd_flag){
	#ifdef DEBUG_FLAG
		if (odd_flag)
			assert(arr[current_level].size() % 2 == 1);
		else
			assert(arr[current_level].size() % 2 == 0);
	#endif
        vector<elem_t> tmp;
		int offset = random_bit();
		int length = arr[current_level].size() - (odd_flag ? 1 : 0);
		elem_t result = (odd_flag ? arr[current_level][length] : 0);
		for (int i = 0; i < length; i += 2){
			tmp.push_back(arr[current_level][i + offset]);
		}
        arr[current_level].clear();
        for (int i = 0; i < tmp.size(); i++)
            arr[current_level].push_back(tmp[i]);
		return result;
	}

	void expand_new_level(){
		// start_level[level_number] = end_level[level_number] = all_level_array;
        vector<elem_t> new_level;
        arr.push_back(new_level);
		for (int i = level_number; i > 0; i--)
			max_counter_level[i] = max_counter_level[i - 1];
		max_counter_level[0] = max(int(max_counter_level[1] / level_mult[1]), MIN_LEVEL_LENGTH);
		++level_number;
	#ifdef DEBUG_FLAG
		assert(level_number < MAX_POSSIBLE_LEVEL_NUMBER);
	#endif
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

    void merge_sort(int current_level_1, int current_level_2){
        for (int i = 0; i < arr[current_level_2].size(); i++)
            arr[current_level_1].push_back(arr[current_level_2][i]);
        sort(arr[current_level_1].begin(), arr[current_level_1].end());
    }

	void compress_up(int current_level = 0){
		current_counter -= arr[current_level].size() / 2;
        sort(arr[current_level].begin(), arr[current_level].end());
		while (true){
			bool odd_flag = (arr[current_level].size() % 2 == 1);
			elem_t rest_e = randomly_halve_down(current_level, odd_flag);
			if (current_level + 1 == level_number)
				expand_new_level();
            merge_sort(current_level + 1, current_level);
            arr[current_level].clear();
			if (odd_flag) arr[current_level].push_back(rest_e);
            // if (!level_x_is_full(current_level + 1)) break;
			++current_level;
			// lazy compress
			break;
		}
	}

/* insertion */
	void insert_to_level(elem_t e, uint32_t level_id){
	#ifdef DEBUG_FLAG
		insert_distribution[level_id]++;
	#endif
		// if (level_x_is_full(level_id)){
		// 	compress_up(level_id);
		// }
        is_all_level_sorted_ = 0;
		while (level_number <= level_id)
			expand_new_level();
        arr[level_id].push_back(e);
		current_counter += 1;
		if (current_counter <= max_counter) return;
		// need compression
		int adjust_top_level_length = 0;
		for (int i = 0; i < level_number; i++){
			// if (max_counter_level[i] > MIN_LEVEL_LENGTH){
			if (arr[i].size() > MIN_LEVEL_LENGTH){
				int compress_length = (arr[i].size() - 1) * level_mult[level_number - i - 1];
				adjust_top_level_length = max(adjust_top_level_length, compress_length);
			}
			if (arr[i].size() > max_counter_level[i]){
				// compress
				compress_up(i);
				return;
			}
		}
		// resize
		current_top_level_length = adjust_top_level_length;
		for (int i = 0; i < level_number; i++){
			max_counter_level[i] = int(current_top_level_length / level_mult[level_number - i - 1]);
			max_counter_level[i] = max(max_counter_level[i], MIN_LEVEL_LENGTH);
		}
		// compress
		for (int i = 0; i < level_number; i++){
			if (arr[i].size() > max_counter_level[i]){
				compress_up(i);
				return;
			}
		}
		printf("adjust wrong\n");
		assert(false);
	}

	// void insert_single(elem_t e){
	// 	if (level_x_is_full(0)){
	// 		compress_up();
	// 	}
	// 	// end_level[0][0] = e;
	// 	// end_level[0]++;
    //     arr[0].push_back(e);
	// 	// is_level_zero_sorted_ = 0;
    //     is_all_level_sorted_ = 0;
	// }

	void insert(elem_t key, int f = 1)
	{
	#ifdef DEBUG_FLAG
		int old_total_items = check_insert_number() + f;
	#endif
		// for (int i = 0; i < f; i++)
		// 	insert_single(key);
        for (int i = level_number - 1; i >= 0 && f > 0; i--){
            int m = 1 << i;
            while (f >= m){
                insert_to_level(key, i);
                f -= m;
            }
        }
	#ifdef DEBUG_FLAG
		int new_total_items = check_insert_number();
		assert(old_total_items == new_total_items);
	#endif
	}



/* quantile */
	uint32_t find_quantile_by_element(elem_t key) 
	{
		uint32_t result = 0;
		// if (!is_level_zero_sorted_){
		// 	sort(start_level[0], end_level[0]); // slow, if insert\query\insert\query  todo
		// 	is_level_zero_sorted_ = 1;
		// }
        if (!is_all_level_sorted_){
            for (int i = 0; i < level_number; i++)
                sort(arr[i].begin(), arr[i].end());
            is_all_level_sorted_ = 1;
        }
		for (int i = 0, mult = 1; i < level_number; i++, mult *= 2){
			// elem_t *it = lower_bound(start_level[i], end_level[i], key);
			vector<elem_t>::iterator it = lower_bound(arr[i].begin(), arr[i].end(), key);
			// result += mult * (it - start_level[i]);
			result += mult * (it - arr[i].begin());
		}
		return result;
    }

	void prepare_all_element(vector<WeightedElement> &all_element)
	{
		for (int i = 0, mult = 1; i < level_number; i++, mult *= 2){
			// for (elem_t *it = start_level[i]; it < end_level[i]; it++){
            for (int j = 0; j < arr[i].size(); j++){
				all_element.push_back((WeightedElement){
					// *it,
                    arr[i][j],
					mult
				});
			}
		}
    }

	int check_insert_number(){
		int result = 0;
		for (int i = 0, mult = 1; i < level_number; i++, mult *= 2){
			result += mult * arr[i].size();
		}
		return result;
	}

	void find_element_by_quantile(WeightedElement* &current_element) 
	{
		for (int i = 0, mult = 1; i < level_number; i++, mult *= 2){
			// for (elem_t *it = start_level[i]; it < end_level[i]; it++){
            for (int j = 0; j < arr[i].size(); j++){
				*current_element = (WeightedElement){
					// *it,
                    arr[i][j],
					mult
				};
				++current_element;
			}
		}
    }

	void dump_distribution(){
		cout << "level number = " << level_number << endl;
		for (int i = 0; i < level_number; i++){
			cout << "level " << i << " : " 
				// << arr[i].size() << "/" << max_level_length_reverse[level_number - 1 - i] << endl;
				 << arr[i].size() << "/" << max_counter_level[i] << endl;
		}
		cout << "insert distribution : " << endl;
		for (int i = 0; i < level_number; i++)
			cout << "level " << i << " : " << insert_distribution[i] << endl;
	}


/* other measurement task */
    int get_memory_usage() { 
	#ifdef DEBUG_FLAG
		std::cout << "kll insert: " << check_insert_number() << endl;
	#endif
		// return max_array_length_total * sizeof(elem_t); 
		// return max_counter * sizeof(elem_t);
		return current_counter * sizeof(elem_t);
	}
	int max_possible_item(){
		int result = 0;
		for (int i = 0; i < level_number; i++)
			result += max_level_length_reverse[i];
		return result;
	}
	int get_max_array_length_total(){
		return max_array_length_total;
	}
	int get_max_possible_level(int length = config.top_level_length, double c = config.c){
		max_level_length_reverse[0] = length;
		for (int i = 1; i < MAX_POSSIBLE_LEVEL_NUMBER; i++)
			max_level_length_reverse[i] = max(MIN_LEVEL_LENGTH, (int)(max_level_length_reverse[i-1] / config.c));
		int used_level = 1;
		while (true){
			int capacity = 0, array_length = 0;
			for (int i = 0; i < used_level; i++){
				capacity += max_level_length_reverse[used_level - 1 - i] << i;
				array_length += max_level_length_reverse[used_level - 1 - i];
			}
			if (capacity >= TOTAL_ELEMENTS) return used_level;
			used_level++;
		}
		return -1;
	}
};
