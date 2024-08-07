#pragma once
#include "../common/common.h"
#include "../common/sketch.h"
#include "ddsketch.hpp"

class DD : public quantile_sketch
{
public:
    elem_t min_e, max_e;
    int tot_memory_in_bytes;
    sketch::DDSketch *ddsketch;


    DD(int _tot_memory_in_bytes, elem_t _min_e = 0, elem_t _max_e = 1e9)
    :tot_memory_in_bytes(_tot_memory_in_bytes), min_e(_min_e), max_e(_max_e){
        int sketch_cap_ = TOTAL_ELEMENTS;        
        int counter_number = tot_memory_in_bytes / sizeof(uint32_t) * 1.8;
        double gamma = exp(log2((max_e - min_e + 1) * 2) / counter_number);
        double alpha = 1 - (2 / (1 + gamma));
        int max_index = std::ceil(std::log2(max_e - min_e + 1) / std::log2(gamma));
        ddsketch = new sketch::DDSketch(sketch_cap_, alpha, (max_e - min_e + 1) * 2);
    }

    ~DD() override {
        delete ddsketch;
    }

    void insert(elem_t e) override {
        e = e - min_e + 1;
        ddsketch->append(e);
    }

    void multi_query_find_e_by_w(vector<elem_t> &result, vector<double> &w) override {
        result.clear();
        ddsketch->multi_query_find_e_by_w(result, w);
        for (int i = 0; i < result.size(); i++)
            result[i] = result[i] + min_e - 1;
    }

    void multi_query_find_w_by_e(vector<double> &result, vector<elem_t> &e) override {
        result.clear();
        vector<elem_t> new_e;
        for (int i = 0; i < e.size(); i++)
            new_e.push_back(e[i] - min_e + 1);
        ddsketch->multi_query_find_w_by_e(result, new_e);
    }

    int memory() override {
        return ddsketch->memory();
    }
};