#ifndef _HEAVYPART_H_
#define _HEAVYPART_H_

#include "../common/hash.h"
#include "param.h"
#include <vector>


class HeavyPart
{
public:
	int total_memory;
	int bucket_num;
	Bucket *buckets;
	uint32_t seed;
	double elastic_lambda;

	HeavyPart(int _total_memory, double _elastic_lambda): total_memory(_total_memory), elastic_lambda(_elastic_lambda)
	{
		check_insert_number_result = 0;
		int size_of_bucket = sizeof(Bucket);
		#ifdef HEAVYPART_24BIT_COUNTER
			size_of_bucket = sizeof(Bucket) + (24 - 32) / 8 * COUNTER_PER_BUCKET;
		#endif
		#ifdef HEAVYPART_16BIT_COUNTER
			size_of_bucket = sizeof(Bucket) + (16 - 32) / 8 * COUNTER_PER_BUCKET;
		#endif
		seed = time(0) % 1007;
		bucket_num = total_memory / size_of_bucket;
		buckets = new Bucket[bucket_num];
		total_memory = bucket_num * size_of_bucket;
		clear();
	}
	~HeavyPart(){
		delete[] buckets;
	}

	void clear()
	{
		memset(buckets, 0, sizeof(Bucket) * bucket_num);
	}

	int check_insert_number_result;
	int check_insert_number(){
		// int result = 0;
		// for (int pos = 0; pos < bucket_num; pos++){
		// 	for(int i = 0; i < COUNTER_PER_BUCKET - 1; i++){

		// 	}
		// }
		return check_insert_number_result;
	}
	void update_insert_number(int pos, int val){
		for(int i = 0; i < COUNTER_PER_BUCKET - 1; i++)
		{
			check_insert_number_result += val * GetCounterVal(buckets[pos].val[i]);
		}
	}

/* insertion */
	int insert(elem_t &key, elem_t &swap_key, uint32_t &swap_val, uint32_t f = 1)
	{
		elem_t fp;
		int pos = CalculateFP(key, fp);

	#ifdef DEBUG_FLAG
		update_insert_number(pos, -1);
	#endif

		/* find if there has matched bucket */
		int matched = -1, empty = -1, min_counter = 0;
		uint32_t min_counter_val = GetCounterVal(buckets[pos].val[0]);
		for(int i = 0; i < COUNTER_PER_BUCKET - 1; i++)
		{
			if(buckets[pos].key[i] == fp){
				matched = i;
				break;
			}
			if(buckets[pos].key[i] == 0 && GetCounterVal(buckets[pos].val[i]) == 0 && empty == -1)
				empty = i;
			if(min_counter_val > GetCounterVal(buckets[pos].val[i])){
				min_counter = i;
				min_counter_val = GetCounterVal(buckets[pos].val[i]);
			}
		}

		/* if matched */
		if(matched != -1){
			#ifdef HEAVYPART_16BIT_COUNTER
				#ifdef OVERFLOW_INSERT_1
					if ((uint32_t)GetCounterVal(buckets[pos].val[matched]) + f >= (1<<16)){
						swap_key = buckets[pos].key[matched];
						swap_val = f;

						return 1;
					}
				#else
				#ifdef OVERFLOW_INSERT_ALL
					if ((uint32_t)GetCounterVal(buckets[pos].val[matched]) + f >= (1<<16)){
						swap_key = buckets[pos].key[matched];
						// swap_val = buckets[pos].val[matched];
						swap_val = (1<<16);

						buckets[pos].val[matched] = buckets[pos].val[matched] + f - (1<<16);

						return 1;
					}
				#else
					assert(false);
				#endif
				#endif
			#endif
			buckets[pos].val[matched] += f;
	#ifdef DEBUG_FLAG
			update_insert_number(pos, 1);
	#endif
			return 0;
		}

		/* if there has empty bucket */
		if(empty != -1){
			buckets[pos].key[empty] = fp;
			buckets[pos].val[empty] = f;
	#ifdef DEBUG_FLAG
			update_insert_number(pos, 1);
	#endif
			return 0;
		}

		/* update guard val and comparison */
		uint32_t guard_val = buckets[pos].val[MAX_VALID_COUNTER];
		guard_val = UPDATE_GUARD_VAL(guard_val);

		if(!JUDGE_IF_SWAP(GetCounterVal(min_counter_val), guard_val))
		{
			buckets[pos].val[MAX_VALID_COUNTER] = guard_val;
	#ifdef DEBUG_FLAG
			update_insert_number(pos, 1);
	#endif
			return 2;
		}

	
		swap_key = buckets[pos].key[min_counter];
		swap_val = buckets[pos].val[min_counter];


		buckets[pos].val[MAX_VALID_COUNTER] = 0;


		buckets[pos].key[min_counter] = fp;
		buckets[pos].val[min_counter] = 0x80000001;
	#ifdef DEBUG_FLAG
		assert(f == 1);
	#endif

	#ifdef DEBUG_FLAG
		update_insert_number(pos, 1);
	#endif
		return 1;
	}


/* quantile */
	uint32_t find_quantile_by_element(elem_t key)
	{
		// uint32_t fp;
		// int pos = CalculateFP(key, fp);
		uint32_t sum = 0;
		for (int pos = 0; pos < bucket_num; ++pos){
			for (int i = 0; i < MAX_VALID_COUNTER; ++i)
				if (buckets[pos].key[i] != 0 && buckets[pos].key[i] < key)
					sum += GetCounterVal(buckets[pos].val[i]);
		}

		return sum;
	}

	void prepare_all_element(std::vector<WeightedElement> &all_element){
		for (int pos = 0; pos < bucket_num; ++pos){
			for (int i = 0; i < MAX_VALID_COUNTER; ++i)
				if (buckets[pos].key[i] != 0 || GetCounterVal(buckets[pos].val[i]) != 0){
					all_element.push_back((WeightedElement){
						buckets[pos].key[i],
						(int)GetCounterVal(buckets[pos].val[i])
					});
				}
		}
	}

	void find_element_by_quantile(WeightedElement* &current_element)
	{
		for (int pos = 0; pos < bucket_num; ++pos){
			for (int i = 0; i < MAX_VALID_COUNTER; ++i)
				if (buckets[pos].key[i] != 0){
					*current_element = (WeightedElement){
						buckets[pos].key[i],
						(int)GetCounterVal(buckets[pos].val[i])
					};
					++current_element;
				}
		}
	}


/* interface */
	int get_memory_usage()
	{
		return total_memory;
	}
	int get_bucket_num()
	{
		return bucket_num;
	}
	int max_possible_item(){
		return bucket_num * COUNTER_PER_BUCKET;
	}

private:
	int CalculateFP(elem_t key, elem_t &fp)
	{
		fp = key;
		// return CalculateBucketPos(fp) % bucket_num;
		return Hash(key, seed) % bucket_num;
	}
};

#endif












