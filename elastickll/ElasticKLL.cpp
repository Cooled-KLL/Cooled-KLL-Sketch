#pragma once
#include "param.h"
#include "../common/common.h"
#include "../common/sketch.h"
#include "SingleElasticKLL.h"

class ElasticKLL : public quantile_sketch
{
public:
    int tot_memory_in_bytes, total_sketch_number;
    int hash_seed;
    vector<SingleElasticKLL*> sketch_list;


    ElasticKLL(int _tot_memory_in_bytes, double heavy_ratio = 0.1, double elastic_lambda = 16)
    :tot_memory_in_bytes(_tot_memory_in_bytes){
        total_sketch_number = 1;
        sketch_list.clear();
        SingleElasticKLL* t = new SingleElasticKLL(_tot_memory_in_bytes, heavy_ratio, elastic_lambda);
        sketch_list.push_back(t);
        // for (int i = 1; i < total_sketch_number; i++)
        //     sketch_list.push_back(new SingleElasticKLL());
        hash_seed = 123;
    }

    ~ElasticKLL() override {
        for (int i = 0; i < total_sketch_number; i++)
            delete sketch_list[i];
        sketch_list.clear();
    }

    void insert(elem_t e) override {
        // int sketch_id = Hash(id, hash_seed) % total_sketch_number;
        // sketch_list[sketch_id]->insert(e);
        sketch_list[0]->insert(e);
    }

    void multi_query_find_e_by_w(vector<elem_t> &result, vector<double> &w) override {
        result.clear();
        sketch_list[0]->multi_query_find_e_by_w(result, w);
    }

    void multi_query_find_w_by_e(vector<double> &result, vector<elem_t> &e) override {
        result.clear();
        sketch_list[0]->multi_query_find_w_by_e(result, e);
    }

    int memory() override {
        return sketch_list[0]->memory();
    }
    
    void debug_aee(vector<elem_t> &groundtruth) override {
        sketch_list[0]->debug_aee(groundtruth);
    }
};