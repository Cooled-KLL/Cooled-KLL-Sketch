#pragma once
#include "../common/common.h"
#include "../common/sketch.h"
#include "tdigest.hpp"

class Tdigest : public quantile_sketch
{
public:
    int tot_memory_in_bytes;
    sketch::TDigest *tdigest;


    Tdigest(int _tot_memory_in_bytes)
    :tot_memory_in_bytes(_tot_memory_in_bytes){
        int sketch_cap_ = TOTAL_ELEMENTS;
        int delta_ = tot_memory_in_bytes / sizeof(sketch::Centroid);
        delta_ = min(delta_, 200);
        tdigest = new sketch::TDigest(sketch_cap_, delta_);
    }

    ~Tdigest() override {
        delete tdigest;
    }

    void insert(elem_t e) override {
        tdigest->append(e);
    }

    void multi_query_find_e_by_w(vector<elem_t> &result, vector<double> &w) override {
        result.clear();
        tdigest->multi_query_find_e_by_w(result, w);
    }

    void multi_query_find_w_by_e(vector<double> &result, vector<elem_t> &e) override {
        result.clear();
        tdigest->multi_query_find_w_by_e(result, e);
    }

    int memory() override {
        return tdigest->memory();
    }
};