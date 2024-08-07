#pragma once
#include "../common/common.h"
#include "../common/sketch.h"
#include "HeavyPart-noSIMD.h"
#ifdef USE_MULTI_INSERT
#include "LightPartMultiInsert.h"
#else
#include "LightPart.h"
#endif


class SingleElasticKLL
{
    int bucket_num;
    int heavy_mem, light_mem;

    HeavyPart *heavy_part;
    LightPart *light_part;

    int total_insert;
    WeightedElement *element_array;

public:
    SingleElasticKLL(int _tot_memory_in_bytes, double heavy_ratio, double elastic_lambda){
        heavy_mem = _tot_memory_in_bytes * heavy_ratio;
        light_mem = _tot_memory_in_bytes - heavy_mem;
        
        heavy_part = new HeavyPart(heavy_mem, elastic_lambda);
        heavy_mem = heavy_part->get_memory_usage();

        light_part = new LightPart(light_mem);
        light_mem = light_part->get_memory_usage();

        clear();
    }

public:
    ~SingleElasticKLL(){
        delete heavy_part;
        delete light_part;
    }
    void clear()
    {
        // heavy_part->clear();
        // light_part->clear();
        total_insert = 0;
    }

    void insert(elem_t key, int f = 1)
    {
	#ifdef DEBUG_FLAG
		int old_heavy_items = heavy_part->check_insert_number() + f;
    #endif

        total_insert += f;
        // uint8_t swap_key[KEY_LENGTH_4];
        elem_t swap_key;
        uint32_t swap_val = 0;
        int result = heavy_part->insert(key, swap_key, swap_val, f);

	#ifdef DEBUG_FLAG
		int new_heavy_items = heavy_part->check_insert_number();
        int new_light_items = 0;
    #endif

        switch(result)
        {
            case 0:{
	#ifdef DEBUG_FLAG
		        assert(old_heavy_items == new_heavy_items);
                new_light_items = light_part->check_insert_number();
                assert(total_insert == new_light_items + new_heavy_items);
    #endif
                return;
            }
            case 1:{
                if(HIGHEST_BIT_IS_1(swap_val))
                    light_part->insert(swap_key, GetCounterVal(swap_val));
                else
                    light_part->insert(swap_key, swap_val);
	#ifdef DEBUG_FLAG
		        assert(old_heavy_items == new_heavy_items + GetCounterVal(swap_val));
                new_light_items = light_part->check_insert_number();
                assert(total_insert == new_light_items + new_heavy_items);
    #endif
                return;
            }
            case 2:{
                light_part->insert(key, 1);
	#ifdef DEBUG_FLAG
		        assert(f == 1);
		        assert(old_heavy_items == new_heavy_items + 1);
                new_light_items = light_part->check_insert_number();
                assert(total_insert == new_light_items + new_heavy_items);
    #endif
                return;
            }
            default:
                printf("error return value !\n");
                exit(1);
        }
    }

    double find_quantile_by_element(elem_t key)
    {
        uint32_t heavy_result = heavy_part->find_quantile_by_element(key);
        uint32_t light_result = light_part->find_quantile_by_element(key);
        // todo: what ratio?
        // int heavy_ratio = 1;
        // int light_ratio = 1;
        return (heavy_result + light_result) / total_insert;
    }

    elem_t find_element_by_quantile(double w){
        int array_length = heavy_part->max_possible_item() + light_part->max_possible_item();
        element_array = new WeightedElement[array_length];
        WeightedElement *current_element = element_array;
        heavy_part->find_element_by_quantile(current_element);
        light_part->find_element_by_quantile(current_element);
        sort(element_array, current_element); // can be optimized to O(n), todo
        int target_k = total_insert * w;
        for (WeightedElement *it = element_array; it < current_element; it++){
            if (current_element - it == 1){
                delete[] element_array;
                return it->e;
            }
            if (target_k < it->w){
                delete[] element_array;
                return it->e;
            }
            target_k -= it->w;
        }
        assert(false); // should not reach here
        return 0;
    }

    void multi_query_find_w_by_e(vector<double> &result, vector<elem_t> &truth_element){
        vector<WeightedElement> all_element;
        all_element.clear();
        heavy_part->prepare_all_element(all_element);
        light_part->prepare_all_element(all_element);

        double current_prefix_sum = 0;
        sort(all_element.begin(), all_element.end()); 

        vector<WeightedElement>::iterator it = all_element.begin();
        for (int i = 0; i < truth_element.size(); i++){
            elem_t element = truth_element[i];
            while (it != all_element.end() && it->e < element){
                current_prefix_sum += it->w;
                if (current_prefix_sum < 0){
                    current_prefix_sum += 10;
                }
                it++;
                if (it == all_element.end()){
                    break;
                }
            }
            result.push_back(current_prefix_sum / total_insert);
        }

        all_element.clear();

    }

    void debug_aee(vector<elem_t> &groundtruth){
        vector<WeightedElement> all_element;
        all_element.clear();
        heavy_part->prepare_all_element(all_element); int hsize = all_element.size();
        light_part->prepare_all_element(all_element); int lsize = all_element.size() - hsize;
        sort(all_element.begin(), all_element.end()); 

        uint32_t current_id = 0;
        int sampling_distribution[101];
        memset(sampling_distribution, 0, sizeof(sampling_distribution));

        vector<WeightedElement>::iterator it = all_element.begin();
        for (int i = 1; i <= 100; i++){
            int id = int(floor((double)(total_insert - 1) * i / 10000));
            while (it != all_element.end() && current_id + it->w < id){
                current_id += it->w;
                it++;
                if (it == all_element.end()){
                    break;
                    // assert(false);
                    // all_element sum(w) < target_id, why?
                }
            }
            sampling_distribution[i] = it - all_element.begin();
        }
        // sampling_distribution[100] = all_element.size();
        for (int i = 0; i < 100; i++) cout << sampling_distribution[i + 1] - sampling_distribution[i] << ",";
        cout << endl;
        all_element.clear();
    }

    void multi_query_find_e_by_w(vector<elem_t> &result, vector<double> &w){
        vector<WeightedElement> all_element;
        all_element.clear();
        heavy_part->prepare_all_element(all_element); int hsize = all_element.size();
	#ifdef DEBUG_FLAG
        // check insert number
        int total1 = 0;
        for (int i = 0; i < all_element.size(); i++){
            total1 += all_element[i].w;
        }
        // end check
    #endif
        light_part->prepare_all_element(all_element); int lsize = all_element.size() - hsize;
	#ifdef DEBUG_FLAG
        cout << "heavy items and light items = " << hsize << " , " << lsize << endl;
        // heavy_part->dump_distribution();
        light_part->dump_distribution();
        // check insert number
        int total2 = 0;
        for (int i = 0; i < all_element.size(); i++){
            total2 += all_element[i].w;
        }
        assert(total2 == total_insert);
        // end check
    #endif

        uint32_t current_id = 0;
        sort(all_element.begin(), all_element.end()); 

        vector<WeightedElement>::iterator it = all_element.begin();
        for (int i = 0; i < w.size(); i++){
            int id = int(floor((double)(total_insert - 1) * w[i]));
            while (it != all_element.end() && current_id + it->w < id){
                current_id += it->w;
                it++;
                if (it == all_element.end()){
                    break;
                    // assert(false);
                    // all_element sum(w) < target_id, why?
                }
            }
            if (it == all_element.end())
                result.push_back(all_element[all_element.size() - 1].e);
            else
                result.push_back(it->e);
        }

        all_element.clear();

    }

    int memory(){
        // return tot_memory_in_bytes;
        return light_part->get_memory_usage() + heavy_part->get_memory_usage();
    }
};

